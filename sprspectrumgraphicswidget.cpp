#include "sprspectrumgraphicswidget.h"

void SPRSpectrumGraphicsWidget::setModelData(SPRSpectrumListItemsModel *value, SPRTypeSpectrumSet _typeSpectrumSet, bool _zonesShow, bool _enableChangeTypeSet)
{
    if(value){
        if(model){
            disconnect(model, SIGNAL(modelChanget(IModelVariable*)), this, SLOT(onModelChanget(IModelVariable*)));
        }
        model = value;
        connect(model, SIGNAL(modelChanget(IModelVariable*)), this, SLOT(onModelChanget(IModelVariable*)));

        elementsModel = model->getZonesTableModel()->getElementsProperty();
        if(elementsModel){
            connect(elementsModel, &SPRElementsModel::modelChanget, this, &SPRSpectrumGraphicsWidget::onModelChanget);
        }

        zonesModel = model->getZonesTableModel();
        if(zonesModel){
            connect(zonesModel, &SPRSpectrumZonesTableModel::modelChanget, this, &SPRSpectrumGraphicsWidget::onModelChanget);
        }
        this->typeSpectrumSet = _typeSpectrumSet;
        this->typeSpectrumSetDef = _typeSpectrumSet;
        setEnableChangeTypeSet(_enableChangeTypeSet);

        spectSource = model->getSpectrumsItemByThread(currentThread, typeSpectrumSet);
        spectrums = &spectSource;

        this->zonesShow = _zonesShow;

        widgetsShow();
    }
}

void SPRSpectrumGraphicsWidget::setVisibleAll(bool _allVisible){
    allVisible = _allVisible;
}

void SPRSpectrumGraphicsWidget::onChangeSelectedCheckedItems(QList<SPRSpectrumItemModel*> _checked, SPRSpectrumItemModel *_current){
    visibleItems.clear();
    foreach(SPRSpectrumItemModel* model, _checked){
        SPRGraphItem *gr = findGraphItemByModel(model);
        if(gr ||allCurrent){
            visibleItems.push_back(gr);
        }
    }
    currentItem = findGraphItemByModel(_current);

    widgetsShow();
}


void SPRSpectrumGraphicsWidget::clearPorogsMovedItems(){
    if(porogsMoved){
        if(spectrums){
            foreach(QwtPlotItem* item, porogsMoved->getMovedItems()){
                if(item->rtti() == QwtPlotItem::Rtti_PlotHistogram){
                    porogsMoved->remoteItem(item);
                }
            }
        }
    }
}

void SPRSpectrumGraphicsWidget::setPorogsMovedItems(){
    if(porogsMoved){
        clearPorogsMovedItems();
        if(currentItem){
            QList<QwtPlotItem*> lpl;
            foreach(QwtPlotItem* pi, currentItem->getZones().values()){
                lpl << pi;
            }
            porogsMoved->addMovedItems(lpl);
        }
    }
}

void SPRSpectrumGraphicsWidget::setAllCurrent(bool value)
{
    allCurrent = value;
}

void SPRSpectrumGraphicsWidget::setZonesShow(bool value)
{
    zonesShow = value;
    widgetsShow();
}

SPRViewGraphicsMode SPRSpectrumGraphicsWidget::getGraphicsMode() const
{
    return graphicsMode;
}

void SPRSpectrumGraphicsWidget::setGraphicsMode(const SPRViewGraphicsMode &value)
{
    graphicsMode = value;
    if(model){
        model->setGraphicMode(value);
    }
}

SPRPorogsMoved *SPRSpectrumGraphicsWidget::getPorogsMoved() const
{
    return porogsMoved;
}

void SPRSpectrumGraphicsWidget::setPorogsMoved(SPRPorogsMoved *value)
{
    porogsMoved = value;
}

bool SPRSpectrumGraphicsWidget::getEnableChangeTypeSet() const
{
    return enableChangeTypeSet;
}

void SPRSpectrumGraphicsWidget::setEnableChangeTypeSet(bool value)
{
    enableChangeTypeSet = value;
    if(enableChangeTypeSet){
        visibleThreads.clear();
        int ths = model != nullptr ? model->getThreads()->getData() : MAX_SPR_MAIN_THREADS;
        visibleThreads.clear();

        for(int i=0; i<ths; i++){
            visibleThreads.push_back(i);
        }
    }

}

bool SPRSpectrumGraphicsWidget::getWithLegend() const
{
    return withLegend;
}

void SPRSpectrumGraphicsWidget::setWithLegend(bool value)
{
    withLegend = value;

    if(legend) delete legend;
    if(withLegend){
        legend = new QwtLegend();
    }
    ui.canvas->insertLegend(legend, QwtPlot::BottomLegend);
}

SPRThreadList SPRSpectrumGraphicsWidget::getVisibleThreads() const
{
    return visibleThreads;
}

void SPRSpectrumGraphicsWidget::setVisibleThreads(const SPRThreadList &value)
{
    visibleThreads = value;
}

int SPRSpectrumGraphicsWidget::getCurrentThread() const
{
    return currentThread;
}

void SPRSpectrumGraphicsWidget::setCurrentThread(int value)
{
    currentThread = value;
}

SPRGraphItem *SPRSpectrumGraphicsWidget::getCurrentItem() const
{
    return currentItem;
}

void SPRSpectrumGraphicsWidget::setCurrentItem(SPRGraphItem *value)
{
    currentItem = value;
}

void SPRSpectrumGraphicsWidget::setZoomer(ScrollZoomer *value)
{
    if(zoomer){
        delete zoomer; zoomer = nullptr;
    }
    if(value){

        zoomer = value;
        if(zoomer){
            zoomer->setCurrent(&currentItem);

            const QColor c( Qt::darkBlue );
            zoomer->setRubberBandPen( c );
            zoomer->setTrackerPen( c );
        }
    }
}

int SPRSpectrumGraphicsWidget::getGraphItemNumber(SPRSpectrumItemModel *_mod){
    int ret = -1;
    for(int index=0; index<spectrums->size(); index++){
        if(_mod == spectrums->at(index)){
            ret = index;
            break;
        }
    }
    return ret;
}

SPRSpectrumGraphicsWidget::SPRSpectrumGraphicsWidget(QWidget *parent) :
    QWidget(parent), model(nullptr), elementsModel(nullptr), zonesModel(nullptr), legend(nullptr), zoomer(nullptr)
{
    ui.setupUi(this);

    forBlockSignalsWidget = findChildren<QWidget*>();

    currentItem = nullptr;
    allCurrent = false;
    allVisible = false;
    zonesShow = true;
    currentThreadIndex = -1;
    currentThread = -1;
    setEnableChangeTypeSet(false);
    withLegend = false;
    typeSpectrumSet = spectrumsOnly;
    typeSpectrumSetDef = spectrumsOnly;

    ui.canvas->setAxisScale(QwtPlot::Axis::xBottom, 0, 256, 10);
    QPalette pal = ui.canvas->palette();
    pal.setBrush(ui.canvas->foregroundRole(), QColor(Qt::white));

    ui.canvas->setPalette(pal);

    ScrollZoomer *zoom = new ScrollZoomer(ui.canvas->canvas(), ui.canvas);

    setZoomer(zoom);

    porogsMoved = new SPRPorogsMoved(ui.canvas);
    connect(porogsMoved, SIGNAL(setSelectItem(QwtPlotItem*,MovedItemPosition)), this, SLOT(onCusorOverSelectItem(QwtPlotItem*, MovedItemPosition)));
    connect(porogsMoved, SIGNAL(changeArgumentValue(QwtPlotItem*,double,MovedItemPosition)), this, SLOT(onChangeSelectedItemValue(QwtPlotItem*,double,MovedItemPosition)));
    connect(porogsMoved, SIGNAL(dblClickMouseEvent()), this, SLOT(onDblClickMouse()));

    for(int th=0; th<MAX_SPR_MAIN_THREADS; th++){
        visibleThreads.push_back(th);
    }

}

void SPRSpectrumGraphicsWidget::onCusorOverSelectItem(QwtPlotItem *item, MovedItemPosition){
    if(currentItem){
        currentItem->setCursorSelectedZone(item);
    }
}

void SPRSpectrumGraphicsWidget::onChangeSelectedItemValue(QwtPlotItem *item, double distance, MovedItemPosition position)
{
    EnumElements el;
    if(currentItem){
        if(currentItem->findItemKey(item, &el)){
            int th = currentItem->getThread();
            SpectorRange *ranges = model->getZonesTableModel()->getElementsRanges(th, el);
            if(ranges){
                if(position == movedLeftSide || position == movedAllSides){
                    SPRVariable<uint> *min = ranges->min;
                    uint value = ranges->min->getData() + distance;
                    min->setData(value);
                }
                if(position == movedRigthSize || position == movedAllSides){
                    SPRVariable<uint> *max = ranges->max;
                    uint value = ranges->max->getData() + distance;
                    max->setData(value);
                }
            }
        }
    } else {
    }

}

void SPRSpectrumGraphicsWidget::onDblClickMouse()
{
    if(model){
        if(enableChangeTypeSet){
            currentThreadIndex++;
            if(currentThreadIndex >= visibleThreads.size()){
                currentThreadIndex = -1;
                currentThread = -1;
                typeSpectrumSet = typeSpectrumSetDef;
                currentItem = nullptr;
            } else {
                currentThread = visibleThreads[currentThreadIndex];
                typeSpectrumSet = spectrumsMixed;
                QList<SPRSpectrumItemModel*> ls = model->getSpectrumsItemByThread(currentThread);
                if(ls.size() > 0){
                    currentItem = ls.first()->getGraphic();
                }
            }
            widgetsShow();
        }
    }
}

void SPRSpectrumGraphicsWidget::hideGraphics(QwtPlotItem *graph){
    if(graph && getCanvas()->itemList().contains(graph)){
        graph->detach();
    }
}

void SPRSpectrumGraphicsWidget::hideGraphics(SPRSpectrumItemModel *item){
    if(item){
        item->getGraphic()->setVisible(getCanvas(), false, false, zonesShow);
    }
}

void SPRSpectrumGraphicsWidget::hideGraphics(QList<SPRSpectrumItemModel *> *items,  bool all){
    if(all){
        items = model->getSpectrumsModel(typeSpectrumSetDef);
    }
    foreach(SPRSpectrumItemModel *item, *items){
        hideGraphics(item);
    }
}

void SPRSpectrumGraphicsWidget::hideGraphics(SPRTypeSpectrumSet type){
    if(model){
        QList<SPRSpectrumItemModel*> *lst = model->getSpectrumsModel(type);
        hideGraphics(lst, false);
    }
}

SPRGraphItem *SPRSpectrumGraphicsWidget::findGraphItemByModel(SPRSpectrumItemModel* model){
    return model->getGraphic();
}

void SPRSpectrumGraphicsWidget::widgetsShow()
{
    if(model){
        blockSignals(true);

        hideGraphics(spectrumsAll);

        spectSource = model->getSpectrumsItemByThread(currentThread, typeSpectrumSet);
        spectrums = &spectSource;
        QList<SPRGraphItem*> graphItems;
        foreach(SPRSpectrumItemModel* model, *spectrums){
                SPRGraphItem *gi = model->getGraphic();
                graphItems << gi;
        }
        foreach(SPRGraphItem *graph, graphItems){
            bool visible = false;
            if(spectrums->contains(graph->getModelData())){
                if(currentItem == graph || allCurrent || allVisible){
                   visible = true;
                }
                if(visibleItems.size() > 0){
                    visible = visibleItems.contains(graph);
                }
            }
            graph->setVisible(ui.canvas, visible, graph==currentItem || allCurrent, zonesShow);
        }
        setPorogsMovedItems();
        blockSignals(false);
        ui.canvas->replot();
    }
}

void SPRSpectrumGraphicsWidget::onModelChanget(IModelVariable *var)
{
    if(model){
        QList<QObject*> mySenders = {model, elementsModel, zonesModel};
        QObject *mvVar = dynamic_cast<QObject*>(var);
        QObject *mvSender = nullptr;
        if(sender()){
            mvSender = sender();
        }

        if(mySenders.contains(mvVar) || mySenders.contains(mvSender)){
            spectSource = model->getSpectrumsItemByThread(-1, spectrumsAll);

            QwtPlotItemList litems = ui.canvas->itemList();
            QwtPlotItemList modelGraphItems;

            foreach(SPRSpectrumItemModel *item, spectSource){
                item->getGraphic()->onModelChanget(item);
                modelGraphItems.append(item->getGraphic()->getSpect());
                foreach(QwtPlotItem* zgr, item->getGraphic()->getZones().values()){
                    modelGraphItems.append(zgr);
                }
            }
            foreach(QwtPlotItem *gr, litems){
                if(!modelGraphItems.contains(gr)){
                    gr->detach();
                }
            }

            if(currentItem){
                if(!spectrums->contains(currentItem->getModelData())){
                    currentItem = nullptr;
                }
            }
            widgetsShow();
        }
    }

}

void SPRSpectrumGraphicsWidget::onChangeGraphicItemsColor(SPRSpectrumItemModel *_item, QColor _color)
{
    if(spectrums){
        SPRGraphItem *graph = findGraphItemByModel(_item);
        if(graph){
            QPen pen = graph->getSpect()->pen();
            pen.setColor(_color);
            graph->getSpect()->setPen(pen);
        }
    }
}
