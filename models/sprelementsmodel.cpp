#include "sprelementsmodel.h"

void SPRElementsProperty::onChangeColor(IModelVariable *var, QColor _color)
{
    if(var == this && color){
        this->color->setData(_color);
    }
}

void SPRElementsProperty::onChangeSName(IModelVariable *var, QString _sName)
{
    if(var == this && sName){
        this->sName->setData(_sName);
    }
}

void SPRElementsProperty::OnChangeFName(IModelVariable *var, QString _fName)
{
    if(var == this && fName){
        this->fName->setData(_fName);
    }
}

void SPRElementsModel::deleteElement(SPRElementsProperty *element){
    if(element){
        EnumElements el = element->key->getData();
//        blockSignals(true);
        if(elements.contains(el)){
            elements.remove(el);
            emit modelChanget(this);
        }
        if(unisedElements.contains(el)){
            unisedElements.remove(el);
        }
        unisedElements[el] = element;
        changeElementPropery(el, QString::number(static_cast<int>(el)));
        emit doStore();
//        blockSignals(false);
    }
}

void SPRElementsModel::onModelChanget(IModelVariable *var)
{
    if(isMyChildren(var) || (sender() && isMyChildren(qobject_cast<IModelVariable*>(sender())))){
        emit modelChanget(var);
    }
}

void SPRElementsProperty::onModelChanget(IModelVariable *var)
{
    if(isMyChildren(var) || (sender() && isMyChildren(qobject_cast<IModelVariable*>(sender())))){
        emit modelChanget(var);
    }
}

