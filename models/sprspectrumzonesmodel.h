#ifndef SPRSPECTRUMZONESMODEL_H
#define SPRSPECTRUMZONESMODEL_H

#include <QObject>
#include <QDomDocument>
#include "models/isprmodeldata.h"
#include "variables/sprvariable.h"
#include "variables/sprqstringvariable.h"
#include "variables/sprqcolorvariable.h"
#include "models/sprelementsmodel.h"

#include "_types.h"

class SpectorRange :public QObject{
public:

    SPRElementsProperty *element;
    SPRVariable<uint> *min;
    SPRVariable<uint> *max;

    SpectorRange():element(nullptr), min(nullptr), max(nullptr){
        element = new SPRElementsProperty(nullptr);
        setProperty("delete_property", QVariant(true));
    }
    SpectorRange(SPRElementsProperty *prop): element(prop), min(nullptr), max(nullptr){
        setProperty("delete_property", QVariant(false));
    }
    virtual ~SpectorRange(){
        if(min) delete min;
        if(max) delete max;
        if(element && property("delete_property").value<bool>()){
            delete element;
        }
    }
};

typedef QMap<EnumElements, SpectorRange*> QMapElementsRanges;

#define SPR_SETTINGS_SPECTRUM_RANGES_XPATH_PREFIX "SEPARATOR/SEPARATE_SETUP/OBL/CHANNEL"
#define SPR_SETTINGS_SPECTRUM_RANGES_XPATH_MIN_AGRUMENT "[Ls]"
#define SPR_SETTINGS_SPECTRUM_RANGES_XPATH_MAX_ARGUMENT "[Rs]"

typedef enum typeZones{
    typeUsedZones, typeUnisedZones, typeAllZones
} TypeUsedZones;

class SPRSpectrumZonesModel : public ISPRModelData
{
    QMapElementsRanges elements;
    SPRElementsModel *elementsProperty;
    uint8_t tIndex;

public:


    QMapElementsRanges getZones(TypeUsedZones typeZones=typeUsedZones);

    SPRSpectrumZonesModel(){}
    SPRSpectrumZonesModel(QDomDocument *doc, uint8_t indexThread, SPRElementsModel *_elProperty, IModelVariable *parent);
    virtual ~SPRSpectrumZonesModel();

    void deleteElement(EnumElements el){
//        if(elements.contains(el)){
//            elements.remove(el);
//        }
        SPRElementsProperty *ep = elementsProperty->getElementProperty(el);
        elementsProperty->deleteElement(ep);
//        emit doStore();
    }
    bool setMinMax(EnumElements el, uint min, uint max){
        if(elements.contains(el)){
            if(min < 256 && max < 256 && min < max){
                elements[el]->min->blockSignals(true);
                    elements[el]->min->setData(min); elements[el]->min->blockSignals(false);
                elements[el]->max->blockSignals(true);
                    elements[el]->max->setData(max); elements[el]->max->blockSignals(false);
                emit modelChanget(this);
                return true;
            } else {
                emit modelChanget(this);
                return false;
            }
        }
    }

    bool setMin(EnumElements el, uint min){
        if(elements.contains(el)){
            uint max = elements[el]->max->getData();
            return setMinMax(el, min, max);
        }
        return false;
    }
    bool setMax(EnumElements el, uint max){
        if(elements.contains(el)){
            uint min = elements[el]->min->getData();
            return setMinMax(el, min, max);
        }
        return false;
    }

    SPRElementsModel *getElementsProperty() const
    {
        return elementsProperty;
    }

    // IModelVariable interface
    QMapElementsRanges getElements() const;

    uint8_t getThreadNumber() const;

    SpectorRange *getElementRange(int index, TypeUsedZones type =typeUsedZones);
    EnumElements getElementKey(int index, bool *ok=nullptr, TypeUsedZones type =typeUsedZones);
public slots:
    virtual void onModelChanget(IModelVariable *send);
};

#endif // SPRSPECTRUMZONESMODEL_H
