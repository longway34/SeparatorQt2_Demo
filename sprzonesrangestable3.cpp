#include "sprzonesrangestable3.h"
#include <QHeaderView>

void SPRZonesRangesTable3::setThreadList(const QList<uint8_t> &value)
{
    bool needShow = false;
    if(threadList != value){
        needShow = true;
    }
    threadList = value;
    if(needShow){
        widgetsShow();
    }
}

void SPRZonesRangesTable3::setThreadList(uint8_t th){
    setThreadList(QList<uint8_t>({th}));
}

SPRZonesRangesTable3::SPRZonesRangesTable3(QWidget *parent):
    QTableWidget (parent),
    zonesModel(nullptr),
    mainModel(nullptr)
{
    verticalHeader()->setVisible(false);
    horizontalHeader()->setVisible(true);

    setSelectionMode(QAbstractItemView::SelectionMode::SingleSelection);
    setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectItems);

    setRowCount(1);
    setColumnCount(MAX_SPR_MAIN_THREADS);

    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    show();
}


uint8_t SPRZonesRangesTable3::getCurrentThread(){
    uint8_t res = 255;

    QModelIndex cIndex = currentIndex();
    if(cIndex.isValid()){
        if(cIndex.column() < threadList.size()){
            res = threadList[cIndex.column()];
        }
    }
    return res;
}



void SPRZonesRangesTable3::widgetsShow()
{
    if(zonesModel && mainModel){
        clear();

//        setSizePolicy(QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum));
        setRowCount(1);
        if(threadList.size() == 0)
            threadList = getAllThreadsListDefault();
        int cols = threadList.size();
        setColumnCount(cols);

        QStringList hHeaders;
        int thCount = 0; bool firstCollumnInserted = false;
        foreach(uint8_t th, threadList){
//            if(th >= mainModel->getThreads()->getData()){
//                continue;
//            }
            hHeaders.append(QString(tr("Ручей %1")).arg(th+1));
            SPRZonesRanges3 *wZone = nullptr;
            QWidget *wColumn = cellWidget(0, thCount);
            if(!wColumn){
                wZone = new SPRZonesRanges3(this);

                if(!firstCollumnInserted){
                    wZone->setFirstCollumnVisible(true);
                    firstCollumnInserted = true;
                } else {
                    wZone->setFirstCollumnVisible(false);
                }

                setCellWidget(0, thCount, wZone);
                connect(wZone, &SPRZonesRanges3::gotFocus, this, &SPRZonesRangesTable3::onGotFocus);
                connect(wZone, &SPRZonesRanges3::updateSize, this, &SPRZonesRangesTable3::onUpdateItemSize);
            }

            wZone->setModelData(zonesModel->items[th]);

            int www = wZone->sizeHint().width();
            horizontalHeader()->resizeSection(thCount, www);

            thCount++;
        }
        setHorizontalHeaderLabels(hHeaders);

//        resizeColumnsToContents();
//        resizeRowsToContents();
//        updateGeometry();

    }

}

ISPRModelData *SPRZonesRangesTable3::setModelData(ISPRModelData *data)
{
    if(data){
        mainModel = data->getMainModel();
        if(mainModel){
            zonesModel = mainModel->getSpectrumZonesTableModel();
            if(threadList.size() == 0){
                threadList = getAllThreadsListDefault();
            }
        }
    }
    if(zonesModel){
        connect(zonesModel, &SPRSpectrumZonesTableModel::modelChanget, this, &SPRZonesRangesTable3::onModelChanget);
    }
    widgetsShow();
}

ISPRModelData *SPRZonesRangesTable3::getModelData()
{
    return zonesModel;
}

QSize SPRZonesRangesTable3::sizeHint() const
{
    QSize res = QTableWidget::sizeHint();
    int h = 0, w = 0;
    for(uint8_t th=0; th < threadList.size(); th++){
        int tww = 0; int thh = 0;
        QWidget *wi = cellWidget(0, th);
        if(wi){
            SPRZonesRanges3 *zw = qobject_cast<SPRZonesRanges3*>(wi);
            if(zw){
                QSize sh = zw->sizeHint();
                tww = sh.width() + verticalHeader()->width();
                thh = sh.height() + horizontalHeader()->height();
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

void SPRZonesRangesTable3::onModelChanget(IModelVariable *var)
{
    if(mainModel){
        if(var == zonesModel->getThreads() || sender() == zonesModel->getThreads()){
            if(threadList.size() > 1){
                threadList = getAllThreadsListDefault();
            }
            widgetsShow();
            emit updateSize();
        }
    }
}

void SPRZonesRangesTable3::onGotFocus(SPRZonesRanges3 *zone, QObject *)
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
                        SPRZonesRanges3 *z = qobject_cast<SPRZonesRanges3*>(w);
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

void SPRZonesRangesTable3::onUpdateItemSize()
{
//    updateGeometry();
//    QSize sss = sizeHint();
//    setMinimumSize(sss);
//    resize(sss);
//    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    resizeEvent(nullptr);
    emit updateSize();
}


void SPRZonesRangesTable3::resizeEvent(QResizeEvent *event)
{
    QSize sss = sizeHint();
    setMinimumSize(sss);
    resize(sss);
    QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    show();
}
