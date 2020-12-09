#include "sprsettingsporogsmodel.h"

SPREnumVariable<TypeSelection> *SPRSettingsPorogsModel::getTypeSelection() const
{
    return typeSelection;
}

void SPRSettingsPorogsModel::setSelection(SPREnumVariable<TypeSelection> *value)
{
    setProperty("delete_selections", QVariant(false));
    typeSelection = value;
}

SPRPorogsModel *SPRSettingsPorogsModel::getPorogs() const
{
    return porogs;
}

SPRVariable<uint> *SPRSettingsPorogsModel::getThreads() const
{
    return threads;
}

void SPRSettingsPorogsModel::setThreads(SPRVariable<uint> *value)
{
    if(value){
        threads = value;
        setProperty("delete_threads", QVariant(false));
        if(porogs){
            porogs->setThreads(threads);
        }
    }
}

SPRVariable<double> *SPRSettingsPorogsModel::getPorog(int th, int condNum) const
{
    if(porogs && th < MAX_SPR_MAIN_THREADS && condNum < MAX_SPR_FORMULA_CONDITION){
        return porogs->porogs[th][condNum];
    } else {
        return nullptr;
    }
}

void SPRSettingsPorogsModel::setPorog(uint8_t th, int condNum, double value)
{
    if(porogs && th < MAX_SPR_MAIN_THREADS && condNum < MAX_SPR_FORMULA_CONDITION){
        porogs->porogs[th][condNum]->setData(value);
    }
}

SPRVariable<uint> *SPRSettingsPorogsModel::getTMeteringMinStone() const
{
    return tMeteringMinStone;
}

void SPRSettingsPorogsModel::setTMeteringMinStone(SPRVariable<uint> *value)
{
    tMeteringMinStone = value;
}

SPRVariable<uint> *SPRSettingsPorogsModel::getTMeteringMaxStone() const
{
    return tMeteringMaxStone;
}

void SPRSettingsPorogsModel::setTMeteringMaxStone(SPRVariable<uint> *value)
{
    tMeteringMaxStone = value;
}

SPRPorogsModel *SPRSettingsPorogsModel::getPorogs2() const
{
    return porogs2;
}

SPRSettingsPorogsModel::SPRSettingsPorogsModel(QObject *parent)
{
    typeSelection = nullptr;
    setProperty("delete_selection", QVariant(false));
    forMinStone = nullptr;
    forMaxStone = nullptr;
    xRayCorrection = nullptr;
    porogs = nullptr;
    invertSelection = nullptr;
    porogs2 = nullptr;
    invertSelection2 = nullptr;
    threads = nullptr;
    setProperty("delete_threads", QVariant(false));

    conditions = nullptr;
    setProperty("delete_conditions", QVariant(false));
}

SPRSettingsPorogsModel::SPRSettingsPorogsModel(QDomDocument *_doc, ISPRModelData *parent):
    ISPRModelData(_doc, parent)
{
    typeSelection = new SPREnumVariable<TypeSelection>(doc,SPR_POROGS_TYPE_SELECTION_XPATH, DEF_SPR_POROGS_TYPE_SELECTION, this);
    setProperty("delete_selection", QVariant(false));
    forMinStone = new SPRKPRVariable(doc, SPR_POROGS_FOR_MIN_STONE_XPATH, DEF_SPR_POROGS_FOR_MIN_STONE, this);
    forMaxStone = new SPRKPRVariable(doc, SPR_POROGS_FOR_MAX_STONE_XPATH, DEF_SPR_POROGS_FOR_MAX_STONE, this);
    xRayCorrection = new SPRVariable<double>(doc, SPR_POROGS_X_RAY_CORRECTION_XPATH, DEF_SPR_POROGS_X_RAY_CORRECTION, this);
    conditions = new SPREnumVariable<TypeConditions>(doc, SPR_POROGS_CONDITION_XPATH, DEF_SPR_FORMULA_CONDITION, this);
    threads = new SPRVariable<uint>(doc, SPR_SETTINGS_MAIN_THREADS_XPATH, DEF_SPR_MAIN_THREADS, this);
    setProperty("delete_threads", QVariant(true));
    porogs = new SPRPorogsModel(_doc, 0, this);
    porogs->setConditions(conditions);
    porogs->setThreads(threads);
    invertSelection = new SPRBooleanVariable(doc, SPR_POROGS_INVERT_SELECTION, DEF_SPR_POROGS_INVERT_SELECTION, this);

    porogs2 = new SPRPorogsModel(_doc, 2, parent);
    porogs2->setConditions(conditions);
    porogs2->setThreads(threads);
    invertSelection2 = new SPRBooleanVariable(doc, SPR_POROGS_INVERT_SELECTION2, DEF_SPR_POROGS_INVERT_SELECTION, this);
}

SPRSettingsPorogsModel::~SPRSettingsPorogsModel()
{
    if(typeSelection && QVariant(property("delete_selection")).toBool()) delete typeSelection; typeSelection = nullptr;
    setProperty("delete_selection", QVariant(false));
    
    if(forMinStone) delete forMinStone; forMinStone = nullptr;
    if(forMaxStone) delete forMaxStone; forMaxStone = nullptr;
    if(xRayCorrection) delete xRayCorrection; xRayCorrection = nullptr;

    if(conditions && QVariant(property(("delete_conditions"))).toBool()){
        delete conditions; conditions = nullptr;
        setProperty("delwtw_conditions", QVariant(false));
    }
    if(threads && QVariant(property("delete_threads")).toBool()){
        delete threads; threads = nullptr;
        setProperty("delete_threads", QVariant(false));
    }
    if(porogs)delete porogs; porogs = nullptr;
    if(invertSelection) delete invertSelection; invertSelection = nullptr;
    if(porogs2) delete porogs2; porogs2 = nullptr;
    if(invertSelection2) delete invertSelection2; invertSelection2 = nullptr;

}

SPRBooleanVariable *SPRSettingsPorogsModel::getInvertSelection() const
{
    return invertSelection;
}

void SPRSettingsPorogsModel::onModelChanget(IModelVariable *send)
{
    if(send == getConditions()){

        TypeConditions cond = getConditions()->getData();
        if(cond == H1){
            for(int th=0; th<MAX_SPR_MAIN_THREADS; th++){
                porogs->porogs[th][1]->blockSignals(true); porogs->porogs[th][1]->setData(99); porogs->porogs[th][1]->blockSignals(true);
                porogs->porogs[th][2]->blockSignals(true); porogs->porogs[th][2]->setData(-99); porogs->porogs[th][2]->blockSignals(true);
                porogs->porogs[th][3]->blockSignals(true); porogs->porogs[th][3]->setData(99); porogs->porogs[th][3]->blockSignals(true);
                porogs->porogs[th][4]->blockSignals(true); porogs->porogs[th][4]->setData(-99); porogs->porogs[th][4]->blockSignals(true);
                porogs->porogs[th][5]->blockSignals(true); porogs->porogs[th][5]->setData(99); porogs->porogs[th][5]->blockSignals(true);
            }
        } else if(cond == H2){
            for(int th=0; th<MAX_SPR_MAIN_THREADS; th++){
                porogs->porogs[th][4]->blockSignals(true); porogs->porogs[th][4]->setData(-99); porogs->porogs[th][4]->blockSignals(true);
                porogs->porogs[th][5]->blockSignals(true); porogs->porogs[th][5]->setData(99); porogs->porogs[th][5]->blockSignals(true);
            }
        }
    }
    emit modelChanget(send);
}

SPREnumVariable<TypeConditions> *SPRSettingsPorogsModel::getConditions() const
{
    return conditions;
}

void SPRSettingsPorogsModel::setConditions(SPREnumVariable<TypeConditions> *value)
{
    if(value){
        conditions = value;
        setProperty("delete_conditions", QVariant(false));
        if(porogs){
            porogs->setConditions(conditions);
        }
    }
}
