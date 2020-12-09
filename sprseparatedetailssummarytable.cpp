#include "sprseparatedetailssummarytable.h"
#include "models/sprhistorymodel2.h"
#include "models/sprmainmodel.h"

#include <QHeaderView>

SPRHistoryModel2 *SPRSeparateDetailSummaryModel::getModelData() const
{
    return separateModel;
}

void SPRSeparateDetailSummaryModel::setVisibleThreads(const SPRThreadList &value)
{
    if(value.size() > 0){
        visibleThreads = value;
    } else {
        visibleThreads = getAllThreadsListDefault();
//        for(int i=0; i<MAX_SPR_MAIN_THREADS; i++){
//            visibleThreads.push_back(i);
//        }
    }
}

void SPRSeparateDetailSummaryModel::startStopScope(bool value)
{
    if(separateModel){
        if(value){
            lastDt = QDateTime::currentDateTime();
            connect(separateModel, SIGNAL(modelChanget(IModelVariable*)), this, SLOT(onModelChanget(IModelVariable*)));
        } else {
            disconnect(separateModel, SIGNAL(modelChanget(IModelVariable*)), this, SLOT(onModelChanget(IModelVariable*)));
        }
    }
}

bool SPRSeparateDetailSummaryModel::isStartStopScope()
{
    return startStop;
}

void SPRSeparateDetailSummaryModel::setMinStoneTime(int value)
{
    minStoneTime = value;
}

int SPRSeparateDetailSummaryModel::getMinStoneTime() const
{
    return minStoneTime;
}

SPRSeparateDetailSummaryModel::SPRSeparateDetailSummaryModel(QObject *parent):
    QAbstractTableModel(parent), separateModel(nullptr), /*lastData(nullptr), prevData(nullptr), */lastDt(QDateTime::currentDateTime())
{

}

void SPRSeparateDetailSummaryModel::setModelData(SPRHistoryModel2 *_model)
{
    separateModel = _model;
    if(separateModel){
        connect(separateModel, &SPRHistoryModel2::separateDataReady, this, &SPRSeparateDetailSummaryModel::onSeparateDataReady);
        SPRMainModel *mm = separateModel->getMainModel();
        if(mm){
            visibleThreads = mm->getThreadsList();
        } else {
            visibleThreads = getAllThreadsListDefault();
        }
//        visibleThreads = separateModel->getWorkThreadList();
    }
}

int SPRSeparateDetailSummaryModel::rowCount(const QModelIndex &) const
{
    return 6;
}

int SPRSeparateDetailSummaryModel::columnCount(const QModelIndex &) const
{
    return 4;
}

QVariant SPRSeparateDetailSummaryModel::data(const QModelIndex &index, int role) const
{
    QVariant res/* = QAbstractTableModel::data(index, role)*/;
    if(role == Qt::TextAlignmentRole){
        res = QVariant(Qt::AlignCenter);
    }
    if(role == Qt::DisplayRole){
        QString sres;
        switch(index.row()){
        case 0: // Разброс
            switch(index.column()){
            case 0:
                sres = myData.H1.getValue(var_min_max);
                break;
            case 1:
                sres = myData.H2.getValue(var_min_max);
                break;
            case 2:
                sres = myData.H3.getValue(var_min_max);
                break;
            case 3:
                sres = myData.time.getValue(var_min_max);
                break;
            }
            break;
        case 1: // min
            switch(index.column()){
            case 0:
                sres = myData.H1.getValue(var_minimum);
                break;
            case 1:
                sres = myData.H2.getValue(var_minimum);
                break;
            case 2:
                sres = myData.H3.getValue(var_minimum);
                break;
            case 3:
                sres = myData.time.getValue(var_minimum);
                break;
            }
            break;
        case 2: // max
            switch(index.column()){
            case 0:
                sres = myData.H1.getValue(var_maximum);
                break;
            case 1:
                sres = myData.H2.getValue(var_maximum);
                break;
            case 2:
                sres = myData.H3.getValue(var_maximum);
                break;
            case 3:
                sres = myData.time.getValue(var_maximum);
                break;
            }
            break;
        case 3: // Среднее
            switch(index.column()){
            case 0:
                sres = myData.H1.getValue(var_average);
                break;
            case 1:
                sres = myData.H2.getValue(var_average);
                break;
            case 2:
                sres = myData.H3.getValue(var_average);
                break;
            case 3:
                sres = myData.time.getValue(var_average);
                break;
            }
            break;
        case 4: // Дельта
            switch(index.column()){
            case 0:
                sres = myData.H1.getValue(var_delta);
                break;
            case 1:
                sres = myData.H2.getValue(var_delta);
                break;
            case 2:
                sres = myData.H3.getValue(var_delta);
                break;
            case 3:
                sres = myData.time.getValue(var_delta);
                break;
            }
            break;
        case 5: // SCO
            switch(index.column()){
            case 0:
                sres = myData.H1.getValue(var_sco);
                break;
            case 1:
                sres = myData.H2.getValue(var_sco);
                break;
            case 2:
                sres = myData.H3.getValue(var_sco);
                break;
            case 3:
                sres = myData.time.getValue(var_sco);
                break;
            }
            break;
        }
        res = QVariant(sres);
    }
    return res;
}

QVariant SPRSeparateDetailSummaryModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    QVariant res = QAbstractTableModel::headerData(section, orientation, role);
    if(role == Qt::DisplayRole){
        if(orientation == Qt::Horizontal){
            QVector<QVariant> hHeaders = {QVariant(QString(tr("Значение H1"))),
                                         QVariant(QString(tr("Значение H2"))),
                                         QVariant(QString(tr("Значение H3"))),
                                         QVariant(QString(tr("Время изм. (мс)")))};
            if(section < hHeaders.size()){
                res = hHeaders[section];
            }
        }
        if(orientation == Qt::Vertical){
            QVector<QVariant> vHeaders = {
                QVariant(QString(tr("Разброс"))),
                QVariant(QString(tr("Минимум"))),
                QVariant(QString(tr("Максимуи"))),
                QVariant(QString(tr("Среднее"))),
                QVariant(QString(tr("Дельта"))),
                QVariant(QString(tr("Ср. кв. откл.")))
            };
            if(section < vHeaders.size()){
                res = vHeaders[section];
            }
        }
    }
    return res;
}

void SPRSeparateDetailSummaryModel::onModelChanget(IModelVariable *variable)
{
}

void SPRSeparateDetailSummaryModel::onSeparateDataReady()
{
    if(sender() == separateModel){

        QVector<SPRHistoryModel2::histDataOneThread> *last = separateModel->getLastRecords();
        QVector<SPRHistoryModel2::histDataOneThread> *diff = separateModel->getDiffRecords();
        QDateTime dt;
        if(last->size()>0){
            dt = (*last)[0].dt;
        }
        if(llabs(lastDt.msecsTo((*last)[0].dt)) < 1000){
            return;
        }
        foreach(uint8_t th, visibleThreads){
            int diff_w_count = static_cast<int>((*diff)[th].data.wcount);
            int stone_count = static_cast<int>((*diff)[th].data.i_prd[2]);
            double p_tkh1 = (*last)[th].data.p_tkh1;
            double p_tkh2 = (*last)[th].data.p_tkh2;
            double p_tkh3 = (*last)[th].data.p_tkh3;
            if(stone_count > 0){
                beginResetModel();
                myData.addData(p_tkh1,
                               p_tkh2,
                               p_tkh3,
                               diff_w_count, stone_count);
                endResetModel();
            }
            lastDt = dt;
        }
    }
}

//void SPRSeparateDetailSummaryModel::onModelChanget(IModelVariable *variable)
//{
//    if(variable == separateModel || sender() == separateModel){
//        qint64 mdt = separateModel->getWorkSeparateLast()->mdt;
//        if(abs(static_cast<int>(lastMdt - mdt)) >= 1000 || lastMdt == 0){
//            lastMdt = mdt;
//            lastData = separateModel->getWorkSeparateLast();
//            prevData = separateModel->getWorkSeparatePrev();
//            foreach(uint8_t th, visibleThreads){
//                int diff_w_count = static_cast<int>(lastData->source.wcount[th] - prevData->source.wcount[th]);
//                int stone_count = static_cast<int>(lastData->source.i_prd[th][SPR_SEPARATE_OUT_INDEX_ALL] - prevData->source.i_prd[th][SPR_SEPARATE_OUT_INDEX_ALL]);
//                if(stone_count > 0){
//                    beginResetModel();
//                    myData.addData(lastData->source.p_tkh1[th], lastData->source.p_tkh2[th], lastData->source.p_tkh3[th], diff_w_count, stone_count);
//                    endResetModel();
//                }
//            }
//        }
//    }
//}

/***
 *
*/
//SPRSeparateDetailSummaryModel *SPRSeparateDetailsSummaryTable::getMyModel() const
//{
//    return myModel;
//}

//void SPRSeparateDetailsSummaryTable::setMyModel(SPRSeparateDetailSummaryModel *value)
//{
//    myModel = value;
//}

SPRSeparateDetailsSummaryTable::SPRSeparateDetailsSummaryTable(QWidget *parent):
    QTableView(parent), ISPRWidget(), myModel(nullptr), separateModel(nullptr)
{
//    if(separateModel){
//        myModel = new SPRSeparateDetailSummaryModel(_model);
//        setModel(myModel);
//        connect(separateModel, SIGNAL(modelChanget(IModelVariable*)), this, SLOT(onModelChanget(IModelVariable*)));
//    }

    setSelectionBehavior(QAbstractItemView::SelectRows);
    setSelectionMode(QAbstractItemView::SingleSelection);

    setAutoFillBackground(true);
//    horizontalHeader()->setStretchLastSection(true);
//    verticalHeader()->setVisible(true);
    horizontalHeader()->setSizeAdjustPolicy(QHeaderView::AdjustToContents);
    setSizeAdjustPolicy(QAbstractItemView::AdjustToContents);
//    show();
}



void SPRSeparateDetailsSummaryTable::widgetsShow()
{
//    show();
}

ISPRModelData *SPRSeparateDetailsSummaryTable::setModelData(ISPRModelData *data)
{
    if(data){
        SPRMainModel *_mod = data->getMainModel();
        if(_mod){
            if(!separateModel){
                separateModel = _mod->getHistoryModel2();
                if(separateModel){
//                    connect(separateModel, SIGNAL(modelChanget(IModelVariable*)), this, SLOT(onModelChanget(IModelVariable*)));
                    if(myModel) delete myModel;
                    myModel = new SPRSeparateDetailSummaryModel(this);
                    myModel->setModelData(separateModel);
                    setModel(myModel);
                }
                this->horizontalHeader()->resizeSections(QHeaderView::Stretch);

                horizontalHeader()->setDefaultAlignment(Qt::AlignHCenter);
                horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
                show();
            }
        }
    }
    return data;
}

ISPRModelData *SPRSeparateDetailsSummaryTable::getModelData()
{
    return separateModel;
}

void SPRSeparateDetailsSummaryTable::setVisibleThreads(const SPRThreadList &value)
{
    if(myModel){
        myModel->setVisibleThreads(value);
    }
}

bool SPRSeparateDetailsSummaryTable::isStartStopScope()
{
    if(myModel){
        return myModel->isStartStopScope();
    }
    return false;
}

void SPRSeparateDetailsSummaryTable::startStopScope(bool value)
{
    if(myModel){
        myModel->startStopScope(value);
    }
}

void SPRSeparateDetailsSummaryTable::setMinStoneTime(int value)
{
    if(myModel){
        myModel->setMinStoneTime(value);
    }
}

void SPRSeparateDetailsSummaryTable::onModelChanget(IModelVariable *)
{
    if(sender() == separateModel){
//        resizeColumnsToContents();
    }
}

