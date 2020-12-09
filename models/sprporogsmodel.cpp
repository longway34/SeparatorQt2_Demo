#include "sprporogsmodel.h"
#include <QString>

void SPRPorogsModel::setThreads(SPRVariable<uint> *value)
{
    if(value){
        if(QVariant(property("delete_threads")).toBool()){
            if(threads) delete threads;
            setProperty("delete_threads", QVariant(false));
        }
        threads = value;
    }
}

void SPRPorogsModel::setConditions(SPREnumVariable<TypeConditions> *value)
{
    if(value != conditions){
        if(QVariant(property("delete_conditions")).toBool()){
           if(conditions) delete conditions;
           setProperty("delete_conditions", QVariant(false));
        }
        if(value){
            connect(value,&IModelVariable::modelChanget, this, &SPRPorogsModel::onModelChanget);
        }
        conditions = value;
    }
}

SPREnumVariable<TypeConditions> *SPRPorogsModel::getConditions() const
{
    return conditions;
}

SPRVariable<uint> *SPRPorogsModel::getThreads() const
{
    return threads;
}

QList<IModelVariable *> SPRPorogsModel::getAllPorogs() const
{
    return allPorogs;
}

SPRPorogsModel::SPRPorogsModel(QObject *parent)
{
    porogs = nullptr;
    conditions = nullptr;
    setProperty("delete_conditions", QVariant(false));
    threads = nullptr;
    setProperty("delete_threads", QVariant(false));
}

SPRPorogsModel::SPRPorogsModel(QDomDocument *_doc, uint _row, ISPRModelData *parent):
    ISPRModelData(_doc, parent), row(_row)
{
    porogs = (SPRVariable<double>***)malloc(sizeof(SPRVariable<double>**) * MAX_SPR_MAIN_THREADS);
    QString indexRow = "";
    if(row > 0){
        indexRow = QString::number(row);
    }
    for(int th=0; th<MAX_SPR_MAIN_THREADS;th++){
        porogs[th] = (SPRVariable<double>**)malloc(sizeof(SPRVariable<double>*) * MAX_SPR_FORMULA_CONDITION);
        QString xpath = "SEPARATOR/SEPARATE_SETUP/Prg/CHANNEL[@INDEX="+QString::number(th)+"]";
        for(uint cond=0; cond < MAX_SPR_FORMULA_CONDITION; cond++){
            QString xxpath = xpath + "[p"+indexRow+QString::number(cond)+"]";
            double defValue;
            if(cond == 0){
                defValue = 1;
            } else if(cond & 1){ // четное сравнение
                defValue = -99;
            } else {
                defValue = 99;
            }
            porogs[th][cond] = new SPRVariable<double>(doc, xxpath, defValue, this);
            porogs[th][cond]->setObjectName(tr("porog_%1_%2").arg(th).arg(cond));
            allPorogs.push_back(porogs[th][cond]);
        }
    }
    conditions = new SPREnumVariable<TypeConditions>(doc, SPR_POROGS_CONDITION_XPATH, DEF_SPR_FORMULA_CONDITION, this);
    setProperty("delete_conditions", QVariant(true));
    threads = new SPRVariable<uint>(doc, SPR_SETTINGS_MAIN_THREADS_XPATH, DEF_SPR_MAIN_RENTGENS, this);
    setProperty("delete_threads", QVariant(true));
}
    SPRPorogsModel::~SPRPorogsModel()
{
    if(porogs){
        for(int th=0; th<MAX_SPR_MAIN_THREADS; th++){
            if(porogs[th]){
                for(int cond=0; cond<MAX_SPR_FORMULA_CONDITION; cond++){
                    if(porogs[th][cond]) delete porogs[th][cond]; porogs[th][cond] = nullptr;
                }
                delete porogs[th]; porogs[th] = nullptr;
            }
        }
        delete porogs; porogs = nullptr;
    }
    if(conditions && QVariant(property("delete_conditions")).toBool()) delete conditions;
    conditions = nullptr;
    if(threads && QVariant(property("delete_threads")).toBool()) delete threads;
    threads = nullptr;
}
