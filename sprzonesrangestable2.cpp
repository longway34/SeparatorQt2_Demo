#include "sprzonesrangestable2.h"
#include <QHeaderView>

void SPRZonesRangesTable2::setThreadList(const QList<uint8_t> &value)
{
    bool needShow = false;
    if(threadList != value){
        needShow = true;
    }
//    threadList.clear();
//    foreach(uint8_t th, value){
//        threadList.append(th);
//    }
    threadList = value;
    if(needShow){
        widgetsShow();
    }
}

void SPRZonesRangesTable2::setThreadList(uint8_t th){
    setThreadList(QList<uint8_t>({th}));
}

SPRZonesRangesTable2::SPRZonesRangesTable2(QWidget *parent):
    QTableWidget (parent),
    zonesModel(nullptr),
    mainModel(nullptr)
{
    verticalHeader()->setVisible(false);
    horizontalHeader()->setVisible(true);

    setSelectionMode(QAbstractItemView::SelectionMode::SingleSelection);
    setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectItems);
}


uint8_t SPRZonesRangesTable2::getCurrentThread(){
    uint8_t res = 255;

    QModelIndex cIndex = currentIndex();
    if(cIndex.isValid()){
        if(cIndex.column() < threadList.size()){
            res = threadList[cIndex.column()];
        }
    }
    return res;
}



void SPRZonesRangesTable2::widgetsShow()
{
    if(zonesModel && mainModel){
        clear();

//        setSizePolicy(QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum));
        setRowCount(1);
        if(threadList.size() == 0)
            threadList = mainModel->getThreadsList();
        int cols = threadList.size();
        setColumnCount(cols);

        QStringList hHeaders;
        int thCount = 0; bool firstCollumnInserted = false;
        foreach(uint8_t th, threadList){
            hHeaders.append(QString(tr("Ручей %1")).arg(th+1));
            SPRZonesRanges2 *wZone = nullptr;
            QWidget *wColumn = cellWidget(0, thCount);
            if(!wColumn){
                wZone = new SPRZonesRanges2(this);

                if(!firstCollumnInserted){
                    wZone->setFirstCollumnVisible(true);
                    firstCollumnInserted = true;
                } else {
                    wZone->setFirstCollumnVisible(false);
                }

                setCellWidget(0, thCount, wZone);
                connect(wZone, &SPRZonesRanges2::gotFocus, this, &SPRZonesRangesTable2::onGotFocus);
                connect(wZone, &SPRZonesRanges2::updateSize, this, &SPRZonesRangesTable2::onUpdateItemSize);
            }

            wZone->setModelData(zonesModel->items[th]);

            int www = wZone->sizeHint().width();
            horizontalHeader()->resizeSection(thCount, www);

            thCount++;
        }
        setHorizontalHeaderLabels(hHeaders);

//        horizontalHeader()->resizeSections(QHeaderView::Stretch);
        resizeColumnsToContents();
        resizeRowsToContents();
        updateGeometry();
//        setSizePolicy(QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred));

    }

}

ISPRModelData *SPRZonesRangesTable2::setModelData(ISPRModelData *data)
{
    if(data){
        mainModel = data->getMainModel();
        if(mainModel){
            zonesModel = mainModel->getSpectrumZonesTableModel();
            if(threadList.size() == 0){
                threadList = mainModel->getThreadsList();
            }
        }
    }
    if(zonesModel){
        connect(zonesModel->getThreads(), &SPRSpectrumZonesTableModel::modelChanget, this, &SPRZonesRangesTable2::onModelChanget);
    }
    widgetsShow();
}

ISPRModelData *SPRZonesRangesTable2::getModelData()
{
    return zonesModel;
}

QSize SPRZonesRangesTable2::sizeHint() const
{
    QSize res = QTableWidget::sizeHint();
    int h = 0, w = 0;
    for(uint8_t th=0; th < threadList.size(); th++){
        int tww = 0; int thh = 0;
        QWidget *wi = cellWidget(0, th);
        if(wi){
            SPRZonesRanges2 *zw = qobject_cast<SPRZonesRanges2*>(wi);
            if(zw){
                QSize sh = zw->sizeHint();
                tww = sh.width();
                thh = sh.height();
            }
            if(thh > h){
                h = thh;
            }
            w += tww;
        }
    }
    res = QSize(w+5, h);
    return res;
}

void SPRZonesRangesTable2::onModelChanget(IModelVariable *var)
{
    if(var == zonesModel->getThreads()){
        widgetsShow();
    }
}

void SPRZonesRangesTable2::onGotFocus(SPRZonesRanges2 *zone, QObject *)
{
    if(zone){
        uint8_t th = zone->getThread();
        if(threadList.contains(th)){
            int col = threadList.indexOf(th);
            setCurrentCell(0, col);
            emit gotFocus(zone);
            for(int c=0; c<columnCount();c++){
                if(c != col){
                    QWidget *w = cellWidget(0, c);
                    if(w){
                        SPRZonesRanges2 *z = qobject_cast<SPRZonesRanges2*>(w);
                        if(z){
                            z->setCurrentItem(nullptr);
                        }
                    }
                }
            }
            return;
        }
    }
    emit gotFocus(nullptr);
    return;
}

void SPRZonesRangesTable2::onUpdateItemSize()
{
    updateGeometry();
    emit updateSize();
}
