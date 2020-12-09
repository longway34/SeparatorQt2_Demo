#include "sprsettingsformulamodel.h"


SPREnumVariable<TypeConditions> *SPRSettingsFormulaModel::getConditions() const
{
    return conditions;
}

SPRElementsModel *SPRSettingsFormulaModel::getElements() const
{
    return elements;
}

void SPRSettingsFormulaModel::setElements(SPRElementsModel *value)
{
    if(value != elements){
        if(elements && property("delete_elements").value<bool>()){
            delete elements;
        }
    }
    elements = value;
    setProperty("delete_elements", QVariant(false));
}

bool SPRSettingsFormulaModel::isUsed(EnumElements _el, int *numFormula)
{
    int itemsNum = 1;
    if(numFormula) *numFormula = -1;
    if(conditions->getData() == H2) itemsNum = 2;
    if(conditions->getData() == H3) itemsNum = 3;

    for(int i=0; i < itemsNum; i++){
        if(itemsModel[i]->isUsed(_el)){
            if(numFormula) *numFormula = i;
            return  true;
        }
    }
    return false;
}

void SPRSettingsFormulaModel::onModelChanget(IModelVariable *send)
{
    emit modelChanget(send);
}

void SPRSettingsFormulaModel::setCondition(SPREnumVariable<TypeConditions> *value)
{
    if(value){
        if(QVariant(property("delete_conditions")).value<bool>()){
            delete conditions;
            setProperty("delete_conditions", QVariant(false));
        }
        if(value != conditions){
            connect(value, &IModelVariable::modelChanget, this, &SPRSettingsFormulaModel::onModelChanget);
        }
        conditions = value;
    }
}
