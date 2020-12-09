#include "sprsettingscontrolmodel.h"


uint16_t SPRSettingsControlModel::getVEMSBeginCode(){
    return VEMSBeginCode->getData() / 20;
}

void SPRSettingsControlModel::setVEMSBeginCode(uint16_t value){
    VEMSBeginCode->setData(value * 20);
}

SPRVariable<uint16_t> *SPRSettingsControlModel::getVEMSBeginCodeVariable() const
{
    return VEMSBeginCode;
}

void SPRSettingsControlModel::setVEMSBeginCodeVariable(SPRVariable<uint16_t> *value)
{
    VEMSBeginCode = value;
}

SPRSettingsControlModel::SPRSettingsControlModel(QDomDocument *_doc, ISPRModelData *parent):
    ISPRModelData(_doc, parent)
{
    correlSpectrumPermiss = new SPRVariable<double>(doc,SPR_CONTROL_CORREL_SPECTRUM_PERMISS_XPATH, DEF_SPR_CONTROL_CORREL_SPECTRUM_PERMISS, this);
    correlSpectrumCritical = new SPRVariable<double>(doc,SPR_CONTROL_CORREL_SPECTRUM_CRITICAL_XPATH, DEF_SPR_CONTROL_CORREL_SPECTRUM_CRITICAL, this);
    speedStreamPermiss = new SPRVariable<uint>(doc, SPR_CONTROL_SPEED_STREAM_PERMISS_XPATH, DEF_SPR_CONTROL_SPEED_STREAM_PERMISS, this);
    speedStreamCritical = new SPRVariable<uint>(doc, SPR_CONTROL_SPEED_STREAM_CRITICAL_XPATH, DEF_SPR_CONTROL_SPEED_STREAM_CRITICAL, this);
    diffCenterGravityPermiss = new SPRVariable<uint>(doc, SPR_CONTROL_DIFF_CENTER_GRAVITY_PERMISS_XPATH, DEF_SPR_CONTROL_DIFF_CENTER_GRAVITY_PERMISS, this);
    diffCenterGravityCritical = new SPRVariable<uint>(doc, SPR_CONTROL_DIFF_CENTER_GRAVITY_CRITICAL_XPATH, DEF_SPR_CONTROL_DIFF_CENTER_GRAVITY_CRITICAL, this);
    airLoadingMinPermiss = new SPRVariable<uint>(doc, SPR_CONTROL_AIR_LOADING_MIN_PERMISS_XPATH, DEF_SPR_CONTROL_AIR_LOADING_MIN_PERMISS, this);
    airLoadingMaxPermiss = new SPRVariable<uint>(doc, SPR_CONTROL_AIR_LOADING_MAX_PERMISS_XPATH, DEF_SPR_CONTROL_AIR_LOADING_MAX_PERMISS, this);
    airLoadingMinCritical = new SPRVariable<uint>(doc, SPR_CONTROL_AIR_LOADING_MIN_CRITICAL_XPATH, DEF_SPR_CONTROL_AIR_LOADING_MIN_CRITICAL, this);
    airLoadingMaxCritical = new SPRVariable<uint>(doc, SPR_CONTROL_AIR_LOADING_MAX_CRITICAL_XPATH, DEF_SPR_CONTROL_AIR_LOADING_MAX_CRITICAL, this);
    controlArea = new SPREnumVariable<EnumElements>(doc, SPR_CONTROL_CONTROL_AREA_XPATH, DEF_SPR_CONTROL_CONTROL_AREA, this);
    weightAvgStoneTail = new SPRVariable<uint>(doc, SPR_CONTROL_WEIGHT_ARG_STONE_TAIL_XPATH, DEF_SPR_CONTROL_WEIGHT_ARG_STONE_TAIL, this);
    weightAvgStoneConcentrate = new SPRVariable<uint>(doc, SPR_CONTROL_WEIGHT_ARG_STONE_CONCENTRATE_XPATH, DEF_SPR_CONTROL_WEIGHT_ARG_STONE_CONCENTRATE, this);
    tMeassureForData = new SPRVariable<uint>(doc, SPR_CONTROL_T_MEASSURE_FOR_DATA_XPATH, DEF_SPR_CONTROL_T_MEASSURE_FOR_DATA, this);
    tMeassureForSpectrum = new SPRVariable<uint>(doc, SPR_CONTROL_T_MEASSURE_FOR_SPECTRUM_XPATH, DEF_SPR_CONTROL_T_MEASSURE_FOR_SPECTRUM, this);
    tMeassureForHistogramm = new SPRVariable<uint>(doc, SPR_CONTROL_T_MEASSURE_FOR_HISTOGRAMM_XPATH, DEF_SPR_CONTROL_T_MEASSURE_FOR_HISTOGRAMM, this);
    correctOptimalOreStream = new SPRVariable<double>(doc, SPR_CONTROL_CORRECT_OPTIMAL_ORE_STREAM_XPATH, DEF_SPR_CONTROL_CORRECT_OPTIMAL_ORE_STREAM, this);
    autoOreStreamControl = new SPRBooleanVariable(doc, SPR_CONTROL_AUTO_STREAM_CONTROL_XPATH, DEF_SPR_CONTROL_AUTO_STREAM_CONTROL, this);
    VEMSLevelLess = new SPRVariable<uint16_t>(doc, SPR_CONTROL_VEMS_LEVEL_LESS_XPATH, DEF_SPR_CONTROL_VEMS_LEVEL_LESS, this);
    VEMSMaxCode = new SPRVariable<uint16_t>(doc, SPR_CONTROL_VEMS_MAX_CODE_XPATH, DEF_SPR_CONTROL_VEMS_MAX_CODE, this);
    VEMSBeginCode = new SPRVariable<uint16_t>(doc, SPR_CONTROL_VEMS_BEGIN_CODE_XPATH, DEF_SPR_CONTROL_VEMS_BEGIN_CODE, this);
}

SPRSettingsControlModel::~SPRSettingsControlModel()
{
    if(correlSpectrumPermiss) delete correlSpectrumPermiss; correlSpectrumPermiss = nullptr;
    if(correlSpectrumCritical) delete correlSpectrumCritical; correlSpectrumCritical = nullptr;
    if(speedStreamPermiss) delete speedStreamPermiss; speedStreamPermiss = nullptr;
    if(speedStreamCritical) delete speedStreamCritical; speedStreamCritical = nullptr;
    if(diffCenterGravityPermiss) delete diffCenterGravityPermiss; diffCenterGravityPermiss = nullptr;
    if(diffCenterGravityPermiss) delete diffCenterGravityPermiss; diffCenterGravityPermiss = nullptr;
    if(airLoadingMinPermiss) delete airLoadingMinPermiss; airLoadingMinPermiss = nullptr;
    if(airLoadingMaxPermiss) delete airLoadingMaxPermiss; airLoadingMaxPermiss = nullptr;
    if(airLoadingMinCritical) delete airLoadingMinCritical; airLoadingMinCritical = nullptr;
    if(airLoadingMaxCritical) delete airLoadingMaxCritical; airLoadingMaxCritical = nullptr;
    if(controlArea) delete controlArea; controlArea = nullptr;
    if(weightAvgStoneTail) delete weightAvgStoneTail; weightAvgStoneTail = nullptr;
    if(weightAvgStoneConcentrate) delete weightAvgStoneConcentrate; weightAvgStoneConcentrate = nullptr;
    if(tMeassureForData) delete tMeassureForData; tMeassureForData = nullptr;
    if(tMeassureForSpectrum) delete tMeassureForSpectrum; tMeassureForSpectrum = nullptr;
    if(tMeassureForHistogramm) delete tMeassureForHistogramm; tMeassureForHistogramm = nullptr;
    if(correctOptimalOreStream) delete correctOptimalOreStream; correctOptimalOreStream = nullptr;
    if(autoOreStreamControl) delete autoOreStreamControl; autoOreStreamControl = nullptr;
    if(VEMSLevelLess) delete VEMSLevelLess; VEMSLevelLess = nullptr;
    if(VEMSMaxCode) delete VEMSMaxCode; VEMSMaxCode = nullptr;
}
