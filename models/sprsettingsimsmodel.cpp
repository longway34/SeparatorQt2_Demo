#include "sprsettingsimsmodel.h"
#include "models/sprmainmodel.h"
#include "models/sprhistorymodel2.h"

SPRVariable<uint> *SPRSettingsIMSModel::getIms() const
{
    return ims;
}

void SPRSettingsIMSModel::setIms(SPRVariable<uint> *value)
{
    if(value){
        if(ims && ims != value){
            if(property("delete_ims").value<bool>()){
                delete  ims;
            }
            connect(value, &IModelVariable::modelChanget, this, &SPRSettingsIMSModel::onModelChanget);
        }
        ims = value;
        setProperty("delete_ims", QVariant(false));

    }
}

SPRSettingsIMSModel::lineFormula SPRSettingsIMSModel::getLineKoeff(double x1, double y1, double x2, double y2){
    // Ax + By + C = 0
    // By = -Ax - C
    // y = -A/Bx -C/B;

    double A = y1 - y2;
    double B = x2 - x1;
    double C = x1 * y2 - x2 * y1;

    struct lineFormula ret;
    ret.k = -A/B;
    ret.b = -C/B;

    return ret;
}

//SPRSettingsIMSModel::getLine

QString SPRSettingsIMSModel::getFormulaString(bool delayDuration){
    QString ret = QString("Y = %1*X + %2");
    if(delayDuration){
        if(kKoeffDelay[0] && bKoeffDelay[0]){
            ret = ret.arg(kKoeffDelay[0]->getData(), 0, 'F',2).arg(bKoeffDelay[0]->getData(), 0, 'f',2);
        } else {
            return "";
        }
    } else {
        if(kKoeffDuration[0] && bKoeffDuration[0]){
            ret = ret.arg(kKoeffDuration[0]->getData(), 0, 'f',2).arg(bKoeffDuration[0]->getData(), 0, 'f',2);
        } else {
            return "";
        }
    }
    return ret;
}

void SPRSettingsIMSModel::setTimesMeassureDelayDuration(uint tmin, uint tmax, uint delayMin, uint delayMax, uint durationMin, uint durationMax){
    if(tmin == 0xFFFF) tmin = tMeteringMinStone->getData();
    if(tmax == 0xFFFF) tmax = tMeteringMaxStone->getData();
    if(delayMin == 0xFFFF) delayMin = tDelayMinStone->getData();
    if(delayMax == 0xFFFF) delayMax = tDelayMaxStone->getData();
    if(durationMin == 0xFFFF) durationMin = tDurationMinStone->getData();
    if(durationMax == 0xFFFF) durationMax = tDurationMaxStone->getData();

    double t_part = (double)(tmax - tmin) / (double)(DEF_SPR_IMS_PARTS_SIZE);
    for(int i=0; i<DEF_SPR_IMS_PARTS_SIZE - 1; i++){
        double d = t_part * (double)(i) + tmin;
        timesMettering[i]->setData(d);
    }
    double d = (double)(tmax * 2);
    timesMettering[DEF_SPR_IMS_PARTS_SIZE]->setData(d);

    struct lineFormula lfDelay = getLineKoeff(tMeteringMinStone->getData(),
                                              tDelayMinStone->getData(),
                                              tMeteringMaxStone->getData(),
                                              tDelayMaxStone->getData());
    struct lineFormula lfDuration = getLineKoeff(tMeteringMinStone->getData(),
                                                 tDurationMinStone->getData(),
                                                 tMeteringMaxStone->getData(),
                                                 tDurationMaxStone->getData());
    for(int i=0; i<DEF_SPR_MAIN_IMS; i++){
        kKoeffDelay[i]->setData(lfDelay.k);
        bKoeffDelay[i]->setData(lfDelay.b);
        kKoeffDuration[i]->setData(lfDuration.k);
        bKoeffDuration[i]->setData(lfDuration.b);
    }
}
SPRVariable<uint> *SPRSettingsIMSModel::getTMeteringMinStone() const
{
    return tMeteringMinStone;
}

SPRVariable<uint> *SPRSettingsIMSModel::getTMeteringMaxStone() const
{
    return tMeteringMaxStone;
}

void SPRSettingsIMSModel::setHistoryModel(SPRHistoryModel2 *_model)
{
    if(_model && _model != history){
        history = _model;
        connect(history, &SPRHistoryModel2::separateDataReady, this, &SPRSettingsIMSModel::onSeparateDataReday);
    }
    dtLastUpdateAbrassions = QDateTime::currentDateTime();
}

void SPRSettingsIMSModel::clearAbrasions(QList<int> threads)
{
    if(threads.size() == 0){
        if(ims){
            for(uint8_t th = 0; th < ims->getData(); th++){
                threads.append(th);
            }
        }
    }
    for(int th=0; th<abrasions.size(); th++){
        if(threads.contains(th)){
            abrasions[th]->blockSignals(true);
            abrasions[th]->setData(0);
            abrasions[th]->blockSignals(false);
        }
    }
    emit modelChanget(this);
}

void SPRSettingsIMSModel::onSeparateDataReday(){
    if(sender() == history){
        QDateTime last = QDateTime::currentDateTime();
        bool beChangetValue = false;
        if(llabs(dtLastUpdateAbrassions.secsTo(last)) > 1){
//            dtLastUpdateAbrassions = last;
            QVector<SPRHistoryModel2::histDataOneThread> *abrasionsValue = history->getDiffRecords();
            for(int i=0; i<abrasions.size(); i++){
                if( i >= abrasionsValue->size()){
                    break;
                }
                qulonglong old = abrasions[i]->getData();
                if(invertSelection && !invertSelection->getData()){
                    old += (*abrasionsValue)[i].data.i_prd[0]; // consentrate
                } else {
                    old += (*abrasionsValue)[i].data.i_prd[1]; // tail
                }
                if(old != abrasions[i]->getData()){
                    abrasions[i]->blockSignals(true);
                    abrasions[i]->setData(old);
                    abrasions[i]->toXml();
                    abrasions[i]->blockSignals(false);
                    beChangetValue = true;
                }
            }
        }
        if(beChangetValue){
            SPRMainModel *mm = getMainModel();
            if(mm){
                QString fName = mm->getFileName();
                if(!fName.isEmpty() && !fName.isNull()){
                    saveAsOperateData(fName);
                }
            }
            emit modelChanget(this);
        }
        dtLastUpdateAbrassions = last;
        return;
    }

}

void SPRSettingsIMSModel::onModelChanget(IModelVariable *send)
{
    ISPRModelData::modelChanget(send);
}

SPRSettingsIMSModel::SPRSettingsIMSModel(){
    ims = nullptr; setProperty("delete_ims", QVariant(false));
    tMeteringMinStone = nullptr;
    tMeteringMaxStone =nullptr;
    tMeteringMaxMaxStone =nullptr;
    tDurationMinStone = nullptr;
    tDurationMaxStone = nullptr;
    tDurationMaxMaxStone = nullptr;
    tDelayMinStone = nullptr;
    tDelayMaxStone = nullptr;
    tDelayMaxMaxStone = nullptr;

    //    kSpeedOreA = nullptr;
    //    kSpeedOreB = nullptr;

    limitAbrasion = nullptr;
    blockImsParam = nullptr;
    blockImsParam2 = nullptr;

    withMaxMaxStone = nullptr;
    history = nullptr;
    invertSelection = nullptr;
}

SPRSettingsIMSModel::SPRSettingsIMSModel(QDomDocument *_doc, ISPRModelData *parent):
    ISPRModelData(_doc, parent), history(nullptr), invertSelection(nullptr)
{
    ims = new SPRVariable<uint>(doc, SPR_SETTINGS_MAIN_IMS_XPATH, DEF_SPR_MAIN_IMS, this);
    setProperty("delete_ims", QVariant(true));

    tMeteringMinStone = new SPRVariable<uint>(doc, SPR_IMS_T_METTERING_MIN_STONE_XPATH, DEF_SPR_IMS_T_METTERING_MIN_STONE, this);
    tMeteringMaxStone = new SPRVariable<uint>(doc, SPR_IMS_T_METTERING_MAX_STONE_XPATH, DEF_SPR_IMS_T_METTERING_MAX_STONE, this);
    tMeteringMaxMaxStone = new SPRVariable<uint>(doc, SPR_IMS_T_METTERING_MAX_MAX_STONE_XPATH, DEF_SPR_IMS_T_METTERING_MAX_NAX_STONE, this);

    for(int i=0; i<DEF_SPR_IMS_PARTS_SIZE+1; i++){
        QString xpath = SPR_IMS_TIMES_METTERING_XPATH_PREFIX + QString::number(i)+"]";
        SPRVariable<uint> *var = new SPRVariable<uint>(doc, xpath, 0, this);
        timesMettering.push_back(var);
    }

    tDelayMinStone = new SPRVariable<uint>(doc, SPR_IMS_T_DELAY_MIN_STONE_XPATH, DEF_SPR_IMS_T_DELAY_MIN_STONE, this);
    tDelayMaxStone = new SPRVariable<uint>(doc, SPR_IMS_T_DELAY_MAX_STONE_XPATH, DEF_SPR_IMS_T_DELAY_MAX_STONE, this);
    tDelayMaxMaxStone = new SPRVariable<uint>(doc, SPR_IMS_T_DELAY_MAX_MAX_STONE_XPATH, DEF_SPR_IMS_T_DELAY_MAX_MAX_STONE, this);

    tDurationMinStone = new SPRVariable<uint>(doc, SPR_IMS_T_DURATION_MIN_STONE_XPATH, DEF_SPR_IMS_T_DURATION_MIN_STONE, this);
    tDurationMaxStone = new SPRVariable<uint>(doc, SPR_IMS_T_DURATION_MAX_STONE_XPATH, DEF_SPR_IMS_T_DURATION_MAX_STONE, this);
    tDurationMaxMaxStone = new SPRVariable<uint>(doc, SPR_IMS_T_DURATION_MAX_MAX_STONE_XPATH, DEF_SPR_IMS_T_DURATION_MAX_MAX_STONE, this);


    blockImsParam = new SPRVariable<uint>(doc, SPR_IMS_BLOCK_PARAM_XPATH, DEF_SPR_IMS_BLOCK_PARAM, this);
    blockImsParam2 = new SPRVariable<uint>(doc, SPR_IMS_BLOCK_PARAM2_XPATH, DEF_SPR_IMS_BLOCK_PARAM, this);
    kSpeedOreA = new SPRVariable<double>(doc, SPR_IMS_SPEAD_ORE_A, DEF_SPR_IMS_SPEAD_ORE_A, this);
    kSpeedOreB = new SPRVariable<double>(doc, SPR_IMS_SPEAD_ORE_B, DEF_SPR_IMS_SPEAD_ORE_B, this);
    limitAbrasion = new SPRVariable<qulonglong>(doc, SPR_IMS_LIMIT_ABRASION_XPATH, DEF_SPR_IMS_LIMIT_ABRASION, this);

    withMaxMaxStone = new SPRBooleanVariable(doc, SPR_IMS_WITH_MAX_STONE, DEF_SPR_IMS_WITH_MAX_STONE, this);

//    struct lineFormula lfDelay = getLineKoeff(timesMettering[0]->getData(), tDelayMinStone->getData(), timesMettering[timesMettering.size() - 1]->getData(), tDelayMaxStone->getData());
//    struct lineFormula lfDuration = getLineKoeff(timesMettering[0]->getData(), tDurationMinStone->getData(), timesMettering[timesMettering.size() - 1]->getData(), tDurationMaxStone->getData());

    for(int i=0; i<MAX_SPR_MAIN_IMS; i++){
        QString xpath = SPR_IMS_ABRASIONS_XPATH_PREFIX+QString::number(i)+"]";
        SPRVariable<qulonglong> *var = new SPRVariable<qulonglong>(doc, xpath, DEF_SPR_IMS_ABRASION, this);
        abrasions.push_back(var);
    }
    for(int i=0; i<DEF_SPR_MAIN_IMS; i++){
        QString xpath = SPR_IMS_K_DELAY_XPATH_PREFIX + QString::number(i)+"]";
        SPRVariable<double> *dvar = new SPRVariable<double>(doc, xpath, 1, this);
        kKoeffDelay.push_back(dvar);

        xpath = SPR_IMS_B_DELAY_XPATH_PREFIX + QString::number(i)+"]";
        dvar = new SPRVariable<double>(doc, xpath, 0, this);
        bKoeffDelay.push_back(dvar);

        xpath = SPR_IMS_K_DURATION_XPATH_PREFIX + QString::number(i)+"]";
        dvar = new SPRVariable<double>(doc, xpath, 1, this);
        kKoeffDuration.push_back(dvar);

        xpath = SPR_IMS_B_DURATION_XPATH_PREFIX + QString::number(i)+"]";
        dvar = new SPRVariable<double>(doc, xpath, 0, this);
        bKoeffDuration.push_back(dvar);

    }
    setTimesMeassureDelayDuration();

    SPRMainModel *mm = parent->getMainModel();
    if(mm){
        history = mm->getHistoryModel2();
        if(history){
            connect(history, &SPRHistoryModel2::separateDataReady, this, &SPRSettingsIMSModel::onSeparateDataReday);
        }

        invertSelection = mm->getSettingsPorogsModel()->getInvertSelection();
    }

}

SPRSettingsIMSModel::~SPRSettingsIMSModel()
{
    if(ims && QVariant(property("delete_ims")).toBool()){
        delete ims; ims = nullptr;
    }

    if(tMeteringMinStone) delete tMeteringMinStone; tMeteringMinStone = nullptr;
    if(tMeteringMaxStone) delete tMeteringMaxStone; tMeteringMaxStone = nullptr;

    if(tDelayMinStone) delete tDelayMinStone; tDelayMinStone = nullptr;
    if(tDelayMaxStone) delete tDelayMaxStone; tDelayMaxStone = nullptr;

    if(tDurationMinStone) delete tDurationMinStone; tDurationMinStone = nullptr;
    if(tDurationMaxStone) delete tDurationMaxStone; tDurationMaxStone = nullptr;

    if(kSpeedOreA) delete kSpeedOreA; kSpeedOreA = nullptr;
    if(kSpeedOreB) delete kSpeedOreB; kSpeedOreB = nullptr;

    if(blockImsParam) delete blockImsParam; blockImsParam = nullptr;
    if(limitAbrasion) delete limitAbrasion; limitAbrasion = nullptr;

    if(withMaxMaxStone) delete withMaxMaxStone; withMaxMaxStone = nullptr;

    for(int i=0; i<abrasions.size(); i++){
        if(abrasions[i]) delete abrasions[i]; abrasions[i] = nullptr;
    }
    for(int i=0; i<timesMettering.size(); i++){
        delete timesMettering[i]; timesMettering[i] = nullptr;
    }
    for(int i=0; i<kKoeffDelay.size(); i++){
        delete kKoeffDelay[i]; kKoeffDelay[i] = nullptr;
    }
    for(int i=0; i<bKoeffDelay.size(); i++){
        delete bKoeffDelay[i]; bKoeffDelay[i] = nullptr;
    }
    for(int i=0; i<kKoeffDuration.size(); i++){
        delete kKoeffDuration[i]; kKoeffDuration[i] = nullptr;
    }
    for(int i=0; i<bKoeffDuration.size(); i++){
        delete bKoeffDuration[i]; bKoeffDuration[i] = nullptr;
    }
}
