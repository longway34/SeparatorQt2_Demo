#include "sprsepratemaintable.h"

void SPRSeparateMainTableModel::setHxTable(const QVector<QVector<double> > &value)
{
    beginResetModel();
    hxTable = value;
    endResetModel();
}

void SPRSeparateMainTableModel::setHxTable()
{
    beginResetModel();
    hxTable.resize(columnCount(QModelIndex()));
    for(uint8_t th=0; th<columnCount(QModelIndex()); th++){
        hxTable[th].fill(0, 3);
    }
    endResetModel();
}

void SPRSeparateMainTableModel::setI_prd(const QVector<QVector<double> > &value)
{
    beginResetModel();
    i_prd = value;
    endResetModel();
}
void SPRSeparateMainTableModel::setI_prd(){
    beginResetModel();
    i_prd.resize(columnCount(QModelIndex()));
    for(uint8_t th=0; th<columnCount(QModelIndex()); th++){
        i_prd[th].fill(0, 3);
    }
    endResetModel();
}

void SPRSeparateMainTableModel::setSeparTables(const QVector<QVector<double> > _hxs, const QVector<QVector<double> > _i_prds)
{
    beginResetModel();
    hxTable = _hxs;
    i_prd = _i_prds;
    QVector<double> summ_i; QVector<double> summ_hx_i;
    summ_i.fill(0, 3);
    summ_hx_i.fill(0, 3);
    for(int th=0; th < hxTable.size(); th++){
        for(int num=0; num<hxTable[th].size();num++){
            summ_i[num] = summ_i[num] + i_prd[th][num];
            summ_hx_i[num] = summ_hx_i[num] + i_prd[th][num] * hxTable[th][num];
        }
    }
    for(int num=0; num<summ_hx_i.size(); num++){
        all_i_prd[num] = summ_i[num];
        all_hx_Table[num] = all_i_prd[num] > 1e-6 ? summ_hx_i[num] / all_i_prd[num] : 0;
    }
    endResetModel();
}
void SPRSeparateMainTableModel::setCorrelTable(const QVector<double> &value)
{
    beginResetModel();
    correlTable = value;
    endResetModel();
}
void SPRSeparateMainTableModel::setCorrelTable(){
    beginResetModel();
    correlTable.resize(columnCount(QModelIndex()));
    correlTable.fill(0);
    endResetModel();
}

void SPRSeparateMainTableModel::setPowerTable(const QVector<double> &value){
    beginResetModel();
    powerSpectrums = value;
    endResetModel();
}
void SPRSeparateMainTableModel::setPowerTable(){
    beginResetModel();
    powerSpectrums.resize(columnCount(QModelIndex()));
    powerSpectrums.fill(0);
    endResetModel();
}

void SPRSeparateMainTableModel::setAirTable(const QVector<double> &value)
{
    beginResetModel();
    airSpectrum = value;
    endResetModel();
}

void SPRSeparateMainTableModel::setAirTable()
{
    beginResetModel();
    airSpectrum.resize(columnCount(QModelIndex()));
    airSpectrum.fill(0);
    endResetModel();

}


void SPRSeparateMainTableModel::onModelChanget(IModelVariable *send)
{
//     if(send == this->spectrumsModel || sender() == spectrumsModel){

//         QObject *ob = sender();

//        QVector<double> tCorrel;
//        QVector<double> tAir;
//        QVector<double> tPower;
//        foreach(uint8_t th, mainModel->getThreadsList()){
//            QList<SPRSpectrumItemModel *> lspect = spectrumsModel->getSpectrumsItemByThread(th, spectrumsOnly);
//            QList<SPRSpectrumItemModel *> lbase = spectrumsModel->getSpectrumsItemByThread(th, spectrumBase);

//            if(lspect.size() > 0 && lbase.size() >0){
//                SPRSpectrumItemModel *item = lspect.first();
//                SPRSpectrumItemModel *bItem = lbase.first();
//                double corr = item->getCorrel(bItem);
//                tCorrel.append(corr);
//                double air = 0; uint32_t summ = *item->getSpectrumData()->summ;
//                double time = *item->getSpectrumData()->time_in_sec;
//                double b_time = bItem->getTimeScope();
//                double power = *item->getSpectrumData()->center;
//                double b_power = *bItem->getSpectrumData()->center;

//                if(time > 1e-6){
//                    air = static_cast<double>(summ) / time;
//                    power = power / time;
//                }
//                tAir.append(air);

//                if(b_time > 1e-6){
//                    b_power = b_power / b_time;
//                }
//                double diff = qAbs(power - b_power);
//                double pr_diff = 0;
//                if(b_power > 1e-6){
//                    pr_diff = diff / b_power * 100;
//                }
//                tPower.append(pr_diff);
//            }
//        }
//        setCorrelTable(tCorrel);
//        setAirTable(tAir);
//        setPowerTable(tPower);
//    }
}

SPRHistoryModel2 *SPRSeparateMainTableModel::getHistoryModel() const
{
    return historyModel;
}

void SPRSeparateMainTableModel::setHistoryModel(SPRHistoryModel2 *value)
{
    if(historyModel){
        disconnect(historyModel, &SPRHistoryModel2::separateDataReady, this, &SPRSeparateMainTableModel::onSeparateDataReady);
        disconnect(historyModel, &SPRHistoryModel2::kspectrumDataReady, this, &SPRSeparateMainTableModel::onKSpectrumDataReady);
    }
    historyModel = value;
    if(historyModel){
        connect(historyModel, &SPRHistoryModel2::separateDataReady, this, &SPRSeparateMainTableModel::onSeparateDataReady);
        connect(historyModel, &SPRHistoryModel2::kspectrumDataReady, this, &SPRSeparateMainTableModel::onKSpectrumDataReady);
    }
}


void SPRSeparateMainTableModel::onKSpectrumDataReady(){
    beginResetModel();
    endResetModel();
}

void SPRSeparateMainTableModel::onSeparateDataReady(){
    beginResetModel();
    endResetModel();
}

void SPRSeparateMainTableModel::setSpectrumsModel(SPRSpectrumListItemsModel *value)
{
//    if(spectrumsModel){
//        disconnect(spectrumsModel, &SPRSpectrumListItemsModel::modelChanget, this, &SPRSeparateMainTableModel::onModelChanget);
//    }
//    spectrumsModel = value;
//    if(spectrumsModel){
//        connect(spectrumsModel, &SPRSpectrumListItemsModel::modelChanget, this, &SPRSeparateMainTableModel::onModelChanget);
//    }
//    if(spectrumsModel){
//        SPRMainModel *_main = spectrumsModel->getMainModel();
//        mainModel = _main;
//    }
}

void SPRSeparateMainTableModel::setMainModel(SPRMainModel *value)
{
    if(mainModel){
//        disconnect(mainModel, &SPRMainModel::modelChanget, this, &SPRSeparateMainTableModel::onModelChanget);
    }
    mainModel = value;
    if(mainModel){
        connect(mainModel, &SPRMainModel::modelChanget, this, &SPRSeparateMainTableModel::onModelChanget);

        clearTables();
//        setHistoryModel(SPRMainModel.getHistoryModel());
    }
}

QVector<double> SPRSeparateMainTableModel::getAirTable()
{
    return airSpectrum;
}

SPRSeparateMainTableModel::SPRSeparateMainTableModel(QObject *parent):
    QAbstractTableModel(parent),
    historyModel(nullptr),spectrumsModel(nullptr), mainModel(nullptr)
{
//    historyModel = _hist;
    clearTables();
}

void SPRSeparateMainTableModel::resetData(){
    beginResetModel(); endResetModel();
}

void SPRSeparateMainTableModel::init(SPRMainModel *_mainModel, SPRHistoryModel2 *_historyModel, SPRSpectrumListItemsModel *_spectrumsModel)
{
    setMainModel(_mainModel);
    setHistoryModel(_historyModel);
    setSpectrumsModel(_spectrumsModel);
}

int SPRSeparateMainTableModel::rowCount(const QModelIndex &) const
{
    return 6;
}

int SPRSeparateMainTableModel::columnCount(const QModelIndex &) const
{
    if(mainModel){
        return mainModel->getThreads()->getData() + 1;
    }
    return 0;
}

QVariant SPRSeparateMainTableModel::data(const QModelIndex &index, int role) const
{
    QVariant res;
    if(role == Qt::BackgroundColorRole){
        QVector<SPRKSpectrumSeparateSummary> *kSpectTable = historyModel->getSpectrumSummaryTable();

        if(kSpectTable){
            if(index.column() < kSpectTable->size()){
                if(index.row() == 5){ // correl
                    if(index.column() < correlTable.size()){
                        double corr = kSpectTable->at(index.column()).correl;
                        if(corr < -1){
                            res = QVariant(QColor(Qt::lightGray));
                        } else if(corr < correlPorogCritical->getData()){
                            res = QVariant(QColor(Qt::red));
                        } else if(corr < correlPorogPermiss->getData()){
                            res = QVariant(QColor(Qt::yellow));
                        }
                    }
                }
                if(index.row() == 4){ // power
                        double power = kSpectTable->at(index.column()).diffPower;
                        if(power > spectrumDiffPowerPermiss->getData()){
                            res = QVariant(QColor(Qt::yellow));
                        }
                        if(power > spectrumDiffPowerCritical->getData()){
                            res = QVariant(QColor(Qt::red));
                        }
                }
                if(index.row() == 3){ // air
                    double air = kSpectTable->at(index.column()).airLoad;
                    if(air < spectrumAirLoadingMinCritical->getData() || air > spectrumAirLoadingMaxCritical->getData()){
                        res = QVariant(QColor(Qt::red));
                    } else if( air < spectrumAirLoadingMinPermiss->getData() || air > spectrumAirLoadingMaxPermiss->getData()) {
                        res = QVariant(QColor(Qt::yellow));
                    }
                }
            }
        }
        return res;
    }
    if(role == Qt::ForegroundRole){}
    if(role == Qt::FontRole){}
    if(role == Qt::TextAlignmentRole){
        res = QVariant(Qt::AlignHCenter);
        return res;
    }
    if(role == Qt::DisplayRole){

        QString sres = "--";
        if(index.column() < columnCount(QModelIndex())){
            QVector<SPRHistoryModel2::histDataOneThread> *last = historyModel->getLastRecords();
            if(index.column() == columnCount(QModelIndex()) - 1){ // common vars
                if(index.row() < 3){
                    long sum_stone = 0; double sum_val = 0; double res = 0;
                    for(int th=0; th < last->size(); th++){
                            sum_stone += (*last)[th].data.i_prd[index.row()];
                            sum_val += (*last)[th].data.p_prd[index.row()] * (*last)[th].data.i_prd[index.row()];
                            if(sum_stone > 0){
                                res = sum_val / static_cast<double>(sum_stone);
                            }
                            sres = QString::number(res, 'f', 3);
                    }
                }
            } else {
                if(index.row() < 3){
                    sres = QString::number((*last)[index.column()].data.p_prd[index.row()], 'f', 3);
                } else {

                    QVector<SPRKSpectrumSeparateSummary> *kSpectTable = historyModel->getSpectrumSummaryTable();

                    if(index.row() == 3){ // air

                        sres = QString::number(kSpectTable->at(index.column()).airLoad, 'f', 0);
//                        sres = QString::number(airSpectrum[index.column()], 'f', 0);
                    } else if(index.row() == 4){ // power
                        sres = QString::number(kSpectTable->at(index.column()).diffPower, 'f', 0);
//                        sres = QString::number(powerSpectrums[index.column()], 'f', 0);
                    } else if(index.row() == 5){ // correl
                        sres = QString::number(kSpectTable->at(index.column()).correl, 'f', 2);
//                        sres = QString::number(correlTable[index.column()], 'f', 2);
                    }
                }
            }
        }


        /*


//        if((hxTable.size() > 0)  && (index.row() < hxTable.at(0).size())){// Концентрат, хвосты, средний
//            if(index.column() < hxTable.size()){ // only one channel
//                if(hxTable.at(index.column()).at(index.row()) > 0){ // data is as
//                    sres = QString::number(hxTable.at(index.column()).at(index.row()),'f', 2);
//                }
//            } else if(index.column() == hxTable.size()){ // column "all"
////                double val = 0; int count = 0;
////                foreach(uint8_t th, threads) {
////                    if(hxTable.at(th).at(index.row()) > 0){ // data is as
////                        val += hxTable.at(index.column()).at(index.row()) * i_prd.at(index.column()).at(index.row());
////                        count += i_prd.at(index.column()).at(index.row());
////                    }
////                    if(count > 0){
////                        val = val / count;
////                    }
////                    if(val > 0){
////                        sres = QString::number(val,'f', 2);
////                    }
////                }
//                sres = QString::number(all_hx_Table.at(index.row()), 'f', 2);
//            }
//        } else if(index.row() == 3){ // air
//            if(index.column() < airSpectrum.size()){
//                if(airSpectrum.at(index.column()) >= -1){
//                    sres = QString::number(airSpectrum.at(index.column()), 'f', 0);
//                }
//            }
//        } else if(index.row() == 4){ // power
//            if(index.column() < powerSpectrums.size()){
//                if(powerSpectrums.at(index.column()) >= -1){
//                    sres = QString::number(powerSpectrums.at(index.column()), 'f', 0);
//                }
//            }
//        } else if(index.row() == 5){ // correl
//            if(index.column() < correlTable.size()){
//                if(correlTable.at(index.column()) >= -1){
//                    sres = QString::number(correlTable.at(index.column()), 'f', 2);
//                }
//            }
//        }
*/
        res = QVariant(sres);
        return res;
    }
    return res;
}

QVariant SPRSeparateMainTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    QVariant res = QAbstractTableModel::headerData(section, orientation, role);
    if(role == Qt::DisplayRole){
        if(orientation == Qt::Horizontal){
            res = QVariant(QString(tr("Общий")));
            if(section < columnCount(QModelIndex())-1){
                res = QVariant(QString(tr("Ручей %1")).arg(section + 1));
            }
        }
        if(orientation == Qt::Vertical){
            switch(section){
            case 0:
                res = QVariant(QString(tr("Ср. концентрат")));
                break;
            case 1:
                res = QVariant(QString(tr("Ср. хвосты")));
                break;
            case 2:
                res = QVariant(QString(tr("Средний")));
                break;
            case 3:
                res = QVariant(QString(tr("Загрузка")));
                break;
            case 4:
                res = QVariant(QString(tr("Отклонение")));
                break;
            case 5:
                res = QVariant(QString(tr("Корреляция")));
                break;
            }
        }
    }
    return res;
}

void SPRSeparateMainTableModel::clearTables()
{
//    threads = _threads;
    setHxTable();
    setI_prd();
    setCorrelTable();
    setPowerTable();
    setAirTable();

    all_hx_Table.resize(3);
    all_hx_Table.fill(0);

    all_i_prd.resize(3);
    all_i_prd.fill(0);
}


SPRSeprateMainTable::SPRSeprateMainTable(QWidget *parent):
    QTableView(parent),
    mainModel(nullptr),
    separateModel(nullptr),
    spectrumModel(nullptr)
{
    tableModel = new SPRSeparateMainTableModel(this);
//    tableModel->setHxTable(&hxTableData);

//    this->setModel(tableModel);
}

SPRSeprateMainTable::~SPRSeprateMainTable(){
    if(tableModel) delete tableModel;
}

void SPRSeprateMainTable::init(SPRMainModel *_mainModel, SPRHistoryModel2 *_histModel, SPRSpectrumListItemsModel *_kSpectModel){
    mainModel = _mainModel;
    if(mainModel){
        disconnect(mainModel, &SPRMainModel::modelChanget, this, &SPRSeprateMainTable::onModelChanget);
    }
//    if(separateModel){
//        disconnect(separateModel, &SPRHistoryModel2::separateDataReady, this, &SPRSeprateMainTable:;
//    }
    if(spectrumModel){
        disconnect(spectrumModel, &SPRSpectrumListItemsModel::modelChanget, this, &SPRSeprateMainTable::onModelChanget);
    }

    if(mainModel){
        tableModel->setMainModel(mainModel);
        tableModel->setPorogsVariable();

        this->setModel(tableModel);

        connect(mainModel, SIGNAL(modelChanget(IModelVariable*)), this, SLOT(onModelChanget(IModelVariable*)));
    }
    separateModel = _histModel;
    if(separateModel){
        tableModel->setHistoryModel(_histModel);
    }
    spectrumModel = _kSpectModel;
    if(spectrumModel){
        tableModel->setSpectrumsModel(spectrumModel);
    }


    onModelChanget(nullptr);
}

void SPRSeprateMainTable::onModelChanget(IModelVariable *source){
    if(!source){
        return;
    } else {
        if(source == mainModel->getThreads()){
            if(source == mainModel->getThreads()){
                init(mainModel, separateModel, spectrumModel);
            }

            return;
        }
    }
}



