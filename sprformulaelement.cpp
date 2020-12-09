#include "sprformulaelement.h"


void SPRFormulaElement::setElement(SPREnumVariable<EnumElements> *variable)
{
    if(element){
        disconnect(element, &IModelVariable::modelChanget, this, &SPRFormulaElement::onModelChanget);
    }
    element = variable;
    if(element){
        connect(element, &IModelVariable::modelChanget, this, &SPRFormulaElement::onModelChanget);
    }

}

SPRFormulaElement::SPRFormulaElement(QWidget *parent):
    QLabel(parent), ISPRWidget(),
    elements(nullptr),
    element(nullptr)
{
}

SPRFormulaElement::~SPRFormulaElement()
{
    if(elements && property("delete_elements").value<bool>()){
        delete elements;
    }
}

void SPRFormulaElement::setElementsModel(SPRElementsModel *_elements)
{
}

void SPRFormulaElement::onChangeCurrentElement(bool)
{
    EnumElements el = (EnumElements)((QAction*)sender())->data().toInt();
    if(element){
        element->setData(el);
    }
    setCurrentElement(el);
}

void SPRFormulaElement::setCurrentElement(EnumElements _element)
{
    if(elements){
        if(elements->getElements().contains(_element)){
            this->setText(elements->getElementProperty(_element, typeUsedElements)->sName->getData());
            this->setToolTip(tr("%1 (%2) Для смены области спектра нажмите правую кнопку мыши...").
                             arg(elements->getElementProperty(_element, typeUsedElements)->sName->getData()).
                             arg(elements->getElementProperty(_element, typeUsedElements)->fName->getData()));
        } else {
            this->setText("??");
        }
    } else {
        this->setText("??");
    }
}


void SPRFormulaElement::setActionsElement(){
    if(elements){
        qDeleteAll(actions());
        actions().clear();
        foreach(EnumElements el, elements->getElements().keys()){
            SPRElementsProperty *prop = elements->getElementProperty(el, typeUsedElements);

            QAction *act = new QAction(QString(prop->fName->getData().toUtf8()+"("+prop->sName->getData().toUtf8()+")"), this);
            act->setData(el);

            this->addAction(act);
            connect(act, SIGNAL(triggered(bool)), this, SLOT(onChangeCurrentElement(bool)));
        }
    }
}

void SPRFormulaElement::widgetsShow()
{
    if(elements){

            setActionsElement();


            if(element){
                EnumElements el = element->getData();
                if(!elements->getElements().keys().contains(el)){
                    element->blockSignals(true); element->setData(Ns); element->blockSignals(false);
                }
                setCurrentElement(element->getData());

            }
        }
    this->setContextMenuPolicy(Qt::ActionsContextMenu);
}

ISPRModelData *SPRFormulaElement::setModelData(ISPRModelData *data)
{
    if(data){
        if(elements != data){
            elements = (SPRElementsModel*)data;
            connect(elements, &SPRElementsModel::modelChanget, this, &SPRFormulaElement::onModelChanget);
            widgetsShow();
        }
    }
}

ISPRModelData *SPRFormulaElement::getModelData()
{
    return elements;
}

void SPRFormulaElement::onModelChanget(IModelVariable *variable)
{
    if(variable == element){
        setCurrentElement(element->getData());
        return;
    }
    if(sender() == elements || variable == elements){
        setActionsElement();
    }
}
