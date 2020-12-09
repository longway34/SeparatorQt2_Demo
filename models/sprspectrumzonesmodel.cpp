#include "sprspectrumzonesmodel.h"

QMapElementsRanges SPRSpectrumZonesModel::getElements() const
{
    return elements;
}

EnumElements SPRSpectrumZonesModel::getElementKey(int index, bool *ok, TypeUsedZones type){
    EnumElements res = Ns;
    if(ok) *ok = false;
    QMapElementsProperty input;
    switch (type) {
    case typeUsedZones:
        input = elementsProperty->getElements();
        break;
    case typeUnisedZones:
        input = elementsProperty->getUnisedElements();
        break;
    case typeAllZones:
        input = elementsProperty->getAllElements();
    default:
        break;
    }
    if(index < input.keys().size() && index >= 0){
        res = input.keys().at(index);
        if(ok) *ok = true;
    }
    return res;
}

SpectorRange *SPRSpectrumZonesModel::getElementRange(int index, TypeUsedZones type)
{
    SpectorRange *res = nullptr;
    bool _ok = false;
    EnumElements el = getElementKey(index, &_ok, type);
    if(_ok){
        if(elements.contains(el)){
            res = elements[el];
        }
    }
    return res;
}


uint8_t SPRSpectrumZonesModel::getThreadNumber() const
{
    return tIndex;
}

QMapElementsRanges SPRSpectrumZonesModel::getZones(TypeUsedZones typeZones){
    QMapElementsRanges res;
    QMapElementsProperty input;
    switch (typeZones) {
    case typeUsedZones:
        input = elementsProperty->getElements();
        break;
    case typeUnisedZones:
        input = elementsProperty->getUnisedElements();
        break;
    case typeAllZones:
        input = elementsProperty->getAllElements();
    default:
        break;
    }
    foreach (EnumElements el, input.keys()) {
        res[el] = elements[el];
    }
    return res;
}

SPRSpectrumZonesModel::SPRSpectrumZonesModel(QDomDocument *doc, uint8_t indexThread, SPRElementsModel *_elProperty, IModelVariable *parent):
    ISPRModelData(doc, parent), elementsProperty(nullptr)
{
    tIndex = indexThread;
    if(!_elProperty){
        elementsProperty = new SPRElementsModel(doc, this);
        setProperty("delete_elProperty", QVariant(true));
    } else {
        if(elementsProperty != _elProperty && _elProperty){
            connect(_elProperty, &SPRElementsProperty::modelChanget, this, &SPRSpectrumZonesModel::onModelChanget);
        }
        elementsProperty = _elProperty;
        setProperty("delete_elProperty", QVariant(false));
    }

    foreach (EnumElements element, getZones(typeAllZones).keys()) {
        SpectorRange *range = new SpectorRange(elementsProperty->getElementProperty(element));
        QString xpath = SPR_SETTINGS_SPECTRUM_RANGES_XPATH_PREFIX +
                QString("[@INDEX=")+QString::number(tIndex)+"]"+
                "/OblName[#"+QString::number(static_cast<int>(element))+"]" +
                SPR_SETTINGS_SPECTRUM_RANGES_XPATH_MAX_ARGUMENT;
        int def = (DEF_SPR_FORMULA_ELEMENTS_PROPERTY)[element].max;
        range->max = new SPRVariable<uint>(doc, xpath, def, this);

        xpath = SPR_SETTINGS_SPECTRUM_RANGES_XPATH_PREFIX +
                QString("[@INDEX=")+QString::number(tIndex)+"]"+
                "/OblName[#"+QString::number(static_cast<int>(element))+"]" +
                SPR_SETTINGS_SPECTRUM_RANGES_XPATH_MIN_AGRUMENT;
        def = (DEF_SPR_FORMULA_ELEMENTS_PROPERTY)[element].min;
        range->min = new SPRVariable<uint>(doc, xpath, def, this);

        elements[element] = range;
    }

}

SPRSpectrumZonesModel::~SPRSpectrumZonesModel()
{
    foreach (EnumElements elem, elements.keys()) {
        delete elements[elem]->max;
        delete elements[elem]->min;
    }
    elements.clear();
    if(elementsProperty && property("delete_elProperty").value<bool>()){
        delete elementsProperty;
    }
}


void SPRSpectrumZonesModel::onModelChanget(IModelVariable *send)
{
    emit modelChanget(send);
}
