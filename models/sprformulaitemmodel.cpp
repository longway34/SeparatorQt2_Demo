#include "sprformulaitemmodel.h"

void SPRFormulaItemModel::setMin(SPRVariable<double> *value)
{

    if(value && value != min){
        if(min && property("delete_min").value<bool>()){
            delete min;
        }
        connect(value, &IModelVariable::modelChanget, this, &SPRFormulaItemModel::onModelChanget);
    }
    min = value;
    setProperty("delete_min", QVariant(false));
}

void SPRFormulaItemModel::setMax(SPRVariable<double> *value)
{
    if(value && value != max){
        if(max && property("delete_max").value<bool>()){
            delete max;
        }
        connect(value, &IModelVariable::modelChanget, this, &SPRFormulaItemModel::onModelChanget);
    }
    max = value;
    setProperty("delete_max", QVariant(false));
}

SPRElementsModel *SPRFormulaItemModel::getElements() const
{
    return elements;
}

void SPRFormulaItemModel::setElements(SPRElementsModel *value)
{
    if(value != elements){
        elements = value;
        setProperty("delete_elements", QVariant(false));
    }
}

bool SPRFormulaItemModel::isUsed(EnumElements _el)
{
    QList<SPREnumVariable<EnumElements>*> elementsList = {
        this->ElementDown1, this->ElementDown2, this->ElementDown3, this->ElementDown4,
        this->ElementUp1, this->ElementUp2
    };
//    bool res = false;
    foreach(SPREnumVariable<EnumElements> *el, elementsList){
        if(el->getData() == _el){
            return true;
        }
    }
    return false;
}

SPRFormulaItemModel::SPRFormulaItemModel(QObject *parent):
    ISPRModelData()
{
}

SPRFormulaItemModel::SPRFormulaItemModel(QDomDocument *_doc, SPRElementsModel *_elements, int _index, ISPRModelData *parent):
    ISPRModelData(_doc, parent), elements(nullptr), ElementUp1(nullptr), ElementUp2(nullptr),
        ElementDown1(nullptr), ElementDown2(nullptr), ElementDown3(nullptr), ElementDown4(nullptr),
        MulUp(nullptr), MulDown(nullptr), min(nullptr), max(nullptr)
{
    if(_elements){
        setElements(_elements);
    } else {
        elements = new SPRElementsModel(doc, this);
        setProperty("delete_elements", QVariant(true));
    }

    index = _index;
    QString xpath = "SEPARATOR/SEPARATE_SETUP/SH"+QString::number(index+1)+"[s0]";
    ElementUp1 = new SPREnumVariable<EnumElements>(doc, xpath, DEF_SPR_FORMULA_ELEMENT, this);
    xpath = "SEPARATOR/SEPARATE_SETUP/SH"+QString::number(index+1)+"[s1]";
    ElementUp2 = new SPREnumVariable<EnumElements>(doc, xpath, DEF_SPR_FORMULA_ELEMENT, this);

    xpath = "SEPARATOR/SEPARATE_SETUP/SH"+QString::number(index+1)+"[s2]";
    ElementDown1 = new SPREnumVariable<EnumElements>(doc, xpath, DEF_SPR_FORMULA_ELEMENT, this);
    xpath = "SEPARATOR/SEPARATE_SETUP/SH"+QString::number(index+1)+"[s3]";
    ElementDown2 = new SPREnumVariable<EnumElements>(doc, xpath, DEF_SPR_FORMULA_ELEMENT, this);
    xpath = "SEPARATOR/SEPARATE_SETUP/SH"+QString::number(index+1)+"[s4]";
    ElementDown3 = new SPREnumVariable<EnumElements>(doc, xpath, DEF_SPR_FORMULA_ELEMENT, this);
    xpath = "SEPARATOR/SEPARATE_SETUP/SH"+QString::number(index+1)+"[s5]";
    ElementDown4 = new SPREnumVariable<EnumElements>(doc, xpath, DEF_SPR_FORMULA_ELEMENT, this);

    xpath = "SEPARATOR/SEPARATE_SETUP/KH"+QString::number(index+1)+"[k0]";
    MulUp = new SPRVariable<double>(doc, xpath, DEF_SPR_FORMULA_MUL, this);
    xpath = "SEPARATOR/SEPARATE_SETUP/KH"+QString::number(index+1)+"[k1]";
    MulDown = new SPRVariable<double>(doc, xpath, DEF_SPR_FORMULA_MUL, this);

    max = new SPRVariable<double>(doc, SPR_FORMULA_MAX_XPATH, DEF_SPR_FORMULA_MAX, this);
    setProperty("delete_max", QVariant(true));
    min = new SPRVariable<double>(doc, SPR_FORMULA_MIN_XPATH, DEF_SPR_FORMULA_MIN, this);
    setProperty("delete_min", QVariant(true));
}

SPRFormulaItemModel::~SPRFormulaItemModel()
{
    if(elements && property("delete_elements").value<bool>()){
        delete elements; elements = nullptr;
    }
    if(ElementUp1) delete ElementUp1; ElementUp1 = nullptr;
    if(ElementUp2) delete ElementUp2; ElementUp2 = nullptr;
    if(ElementDown1) delete ElementDown1; ElementDown1 = nullptr;
    if(ElementDown2) delete ElementDown2; ElementDown2 = nullptr;
    if(ElementDown3) delete ElementDown3; ElementDown3 = nullptr;
    if(ElementDown4) delete ElementDown4; ElementDown4 = nullptr;

    if(MulUp) delete MulUp; MulUp = nullptr;
    if(MulDown) delete MulDown; MulDown = nullptr;

    if(min && property("delete_min").toBool()) delete min; min = nullptr;
    if(max && property("delete_max").toBool()) delete max; max = nullptr;
}
