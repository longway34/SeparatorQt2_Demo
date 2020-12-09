#include "sprseparatedetailstable.h"
#include "models/sprmainmodel.h"
#include <QMap>
#include <QColor>
#include <QScrollBar>
#include <QHeaderView>

#include <QAbstractItemView>

// ************************** SPRSeparateDetailsTableModel *************************
// *********************************************************************************

SPRThreadList SPRSeparateDetailsTableModel::getThreads() const
{
    return threads;
}

void SPRSeparateDetailsTableModel::setThreads(const SPRThreadList &value)
{
    threads = value;
}

QVariant SPRSeparateDetailsTableModel::data(const QModelIndex &index, int role) const
{
    QVariant value;

    if(role == Qt::TextAlignmentRole){
        value = QVariant(Qt::AlignCenter);
    }
    if(index.row() < mdata.size()){
        if(role == Qt::BackgroundColorRole){
            if(mdata[index.row()]->i_prd_Concentrale > 0){
                if(mdata[index.row()]->i_prd_Tail == 0){
                   return QVariant(QColor("#8CFFA8"));
                } else {
                    return QVariant(QColor(Qt::lightGray));
                }
            }
            return value;
        }
        if(role == Qt::DisplayRole){
            int col = index.column(), row = index.row();
            QString ret;
            switch(col){
            case 0:
                ret = QString::number(row+1);
                break;
            case 1:
                ret = QDateTime::currentDateTime().toString("hh:mm:ss");
                break;
            case 2:
                ret = QString::number(mdata[row]->thread+1);
                break;
            case 3:
                ret = QString::number(mdata[row]->H1, 'f', 3);
                break;
            case 4:
                ret = QString::number(mdata[row]->H2, 'f', 3);
                break;
            case 5:
                ret = QString::number(mdata[row]->H3, 'f', 3);
                break;
            case 6:
                ret = QString::number(mdata[row]->t_scope);
                break;
            case 7:
                {
                    QString str;
                    if(mdata[row]->i_prd_Concentrale > 0){
                        str = QString(tr("концентрат"));
                        if(mdata[row]->i_prd_Concentrale > 1 || mdata[row]->i_prd_Concentrale < mdata[row]->i_prd_All){
                            str += QString(" - %1").arg(mdata[row]->i_prd_Concentrale);
                        }
                    }
                    if(mdata[row]->i_prd_Tail > 0){
                        if(str.size() > 0){ str += "; ";
                            str += QString(tr("хвост - %1")).arg(mdata[row]->i_prd_Tail);
                        } else {
                            str += QString(tr("хвост"));
                            if(mdata[row]->i_prd_Tail > 1){
                                str += QString(" - %1").arg(mdata[row]->i_prd_Tail);
                            }
                        }
                    }
                    ret = str;
                }
                break;
            }
            return QVariant(ret);
        }
    }
    return value;
}

int SPRSeparateDetailsTableModel::getMinStoneTime() const
{
    return minStoneTime;
}

void SPRSeparateDetailsTableModel::setMinStoneTime(int value)
{
    minStoneTime = value;
}

void SPRSeparateDetailsTableModel::startStopScope(bool _startStop){
    startStop = _startStop;
    if(_startStop){
        if(model){
            connect(model, SIGNAL(modelChanget(IModelVariable *)), this, SLOT(onModelChanget(IModelVariable*)));
//            last = nullptr; prev = nullptr;
        }
    } else {
        if(model){
            disconnect(model, SIGNAL(modelChanget(IModelVariable *)), this, SLOT(onModelChanget(IModelVariable*)));
        }
    }
}

bool SPRSeparateDetailsTableModel::isStartingScope()
{
    return startStop;
}

void SPRSeparateDetailsTableModel::setModelData(SPRHistoryModel2 *_model){
    if(model != _model && _model){
        connect(_model, &SPRHistoryModel2::separateDataReady, this, &SPRSeparateDetailsTableModel::onSeparateDataReady);
    }
    model = _model;
}

QVariant SPRSeparateDetailsTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    QVariant ret = QAbstractTableModel::headerData(section, orientation, role);

    QVector<QString> hheaders = {tr("№ п/п"), tr("Время"), tr("Ручей"),
                            tr("H1"), tr("H2"), tr("H3"),
                            tr("размер (мм.)"), tr("концентрат/хвост")};

    if(role == Qt::DisplayRole){
        if(orientation == Qt::Horizontal){
            if(section < hheaders.size()){
                ret = QVariant(hheaders[section]);
            }
        }
    }
    return ret;
}

void SPRSeparateDetailsTableModel::onModelChanget(IModelVariable *send)
{
}

void SPRSeparateDetailsTableModel::onSeparateDataReady()
{
    if(sender() == model){

        QVector<SPRHistoryModel2::histDataOneThread> *diff = model->getDiffRecords();
        QVector<SPRHistoryModel2::histDataOneThread> *last = model->getLastRecords();
        QVector<mData*> vmData;
        for(int th=0; th<diff->size(); th++){
            QDateTime _dt = QDateTime::fromString("2018.01.01", "yyyy.MM.dd");
            if(mdata.size() > 0){
                _dt = mdata.first()->dt;
            }
            if((*diff)[th].isNotStouns()){
                continue;
            }
            qint64 d = llabs((*last)[th].dt.msecsTo(_dt));
            if(d < 1000){
                continue;
            }
            if(diff->at(th).data.wcount < getMinStoneTime()){
                continue;
            }
            if(!threads.contains(diff->at(th).data.thread)){
                continue;
            }

            mData *_m = new mData(last->at(th).data.thread);
            _m->H1 = last->at(th).data.p_tkh1;
            _m->H2 = last->at(th).data.p_tkh2;
            _m->H3 = last->at(th).data.p_tkh3;

            _m->i_prd_Concentrale = diff->at(th).data.i_prd[0];
            _m->i_prd_Tail = diff->at(th).data.i_prd[1];
            _m->i_prd_All = diff->at(th).data.i_prd[2];

            _m->dt = last->at(th).dt;
            _m->t_scope = diff->at(th).data.wcount;

            vmData.append(_m);
        }

        if(vmData.size() > 0){
            beginResetModel();
            for(int row=0; row<vmData.size(); row++){
                mdata.push_front(vmData[row]);
            }
            clearLast();
            endResetModel();
        }
    }
}

void SPRSeparateDetailsTableModel::clear(){
    for(int row=0; row<mdata.size(); row++){
        delete mdata[row];
    }
    beginResetModel();
    mdata.clear();
    endResetModel();
    return;
}

void SPRSeparateDetailsTableModel::clearLast(){
    if(mdata.size() > 240){ // 60 sec X 4 thread
//        beginResetModel();
        while(mdata.size() > 240){
            struct mData *row = mdata.takeLast();
            if(row) delete row;
        }
//        endResetModel();
    }
}

// ************************** SPRSeparateDetailsTable ******************************
// *********************************************************************************


SPRSeparateDetailsTable::SPRSeparateDetailsTable(QWidget *parent)
    :QTableView(parent), ISPRWidget(), myModel(nullptr)
{

//    setModel(new SPRSeparateDetailsTableModel(nullptr));


//    horizontalHeader()->set;
    setSelectionBehavior(QAbstractItemView::SelectRows);
    setSelectionMode(QAbstractItemView::SingleSelection);

    setAutoFillBackground(true);
    horizontalHeader()->setStretchLastSection(true);
    horizontalHeader()->setSizeAdjustPolicy(QHeaderView::AdjustToContents);
    setSizeAdjustPolicy(QAbstractItemView::AdjustToContents);
//    horizontalHeader()->set
    verticalHeader()->setVisible(false);
//    show();
}

SPRSeparateDetailsTable::~SPRSeparateDetailsTable()
{
    if(myModel) delete myModel;
}


void SPRSeparateDetailsTable::widgetsShow()
{
}

void SPRSeparateDetailsTable::startStopScope(bool value)
{
    if(myModel){
        myModel->startStopScope(value);
    }
}

bool SPRSeparateDetailsTable::isStartingScope()
{
    if(myModel){
        return myModel->isStartingScope();
    }
    return false;
}



void SPRSeparateDetailsTable::setMinStoneTime(int _time)
{
   if(myModel){
         myModel->setMinStoneTime(_time);
    }
}

int SPRSeparateDetailsTable::getMinStoneTime()
{
    if(myModel){
        return myModel->getMinStoneTime();
    }
}

void SPRSeparateDetailsTable::setVisibleThreads(SPRThreadList _threads)
{
    if(myModel){
        myModel->setThreads(_threads);
    }
}

SPRThreadList SPRSeparateDetailsTable::getVisibleThreads()
{
    if(myModel){
        return myModel->getThreads();
    }
    return SPRThreadList();
}

ISPRModelData *SPRSeparateDetailsTable::setModelData(SPRMainModel *data)
{
    mainModel = data;
    if(mainModel){
        if(myModel) delete myModel;

        myModel = new SPRSeparateDetailsTableModel(mainModel->getHistoryModel2(), this);
        setModel(myModel);

//        myModel->setThreads(getVisibleThreads());

//        connect(mainModel->getSeparateOutputModel(), SIGNAL(modelChanget(IModelVariable*)), this, SLOT(onModelChanget(IModelVariable*)));
        this->horizontalHeader()->setSizeAdjustPolicy(QHeaderView::AdjustToContents);

        horizontalHeader()->setDefaultAlignment(Qt::AlignHCenter);
//        horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
        this->setSizeAdjustPolicy(QAbstractItemView::AdjustToContents);
        show();
    }
}

ISPRModelData *SPRSeparateDetailsTable::getModelData()
{
    return mainModel;
}

void SPRSeparateDetailsTable::clear()
{
    if(myModel){
        myModel->clear();
    }
}

void SPRSeparateDetailsTable::onModelChanget(IModelVariable *)
{
    if(sender() == mainModel->getSeparateOutputModel()){
//        resizeColumnsToContents();
    }
//    widgetsShow();
}

