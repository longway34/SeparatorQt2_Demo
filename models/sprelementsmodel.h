#ifndef SPRELEMENTSMODEL_H
#define SPRELEMENTSMODEL_H

#include <QObject>
#include "models/isprmodeldata.h"
#include "variables/sprqstringvariable.h"
#include "variables/sprqcolorvariable.h"
#include "variables/sprvariable.h"
#include "variables/sprenumvariable.h"

#include "_types.h"

typedef enum _typeElements{
    typeAllElements, typeUsedElements, typeUnisedElements
} TypeSetElements;

class SPRElementsProperty :public ISPRModelData{
    Q_OBJECT

public:
    SPREnumVariable<EnumElements> *key;
    SPRQStringVariable *sName;
    SPRQStringVariable *fName;
    SPRQColorVariable *color;
    SPRElementsProperty(ISPRModelData *parent):
        ISPRModelData(parent), key(nullptr), sName(nullptr), fName(nullptr), color(nullptr)
    {}
    virtual ~SPRElementsProperty(){
        if(key) delete key;
        if(sName) delete sName;
        if(fName) delete fName;
        if(color) delete color;
    }

    // IModelVariable interface
public slots:
    virtual void onModelChanget(IModelVariable *var);
    void onChangeColor(IModelVariable*, QColor _color);
    void onChangeSName(IModelVariable*, QString _sName);
    void OnChangeFName(IModelVariable*, QString _fName);
};

typedef QMap<EnumElements, SPRElementsProperty*> QMapElementsProperty;

class SPRElementsModel: public ISPRModelData
{

    Q_OBJECT

    QMapElementsProperty allElements;
    QMapElementsProperty elements;
    QMapElementsProperty unisedElements;

public:


    SPRElementsProperty *getElementProperty(EnumElements el, TypeSetElements typeSet= typeUsedElements){
        QMapElementsProperty *source;
        switch(typeSet){
        case typeAllElements:
            source = &allElements;
            break;
        case typeUsedElements:
            source = &elements;
            break;
        case typeUnisedElements:
            source = &unisedElements;
            break;
        }
        if(source->contains(el)){
            return (*source)[el];
        } else {
            return nullptr;
        }
    }

    SPRElementsModel(QDomDocument *doc, ISPRModelData *parent=nullptr)
        :ISPRModelData(doc, parent)
    {
        allElements.clear();

        for(int num=0; num < MAX_SPR_SPECTOR_ELEMENTS; num++){

            QString SName_XPATH = SPR_SETTINGS_SPECTRUM_ZONES_XPATH_PREFIX + QString::number(num+1) + "]";
            SPRElementsProperty *el = new SPRElementsProperty(this);
            EnumElements _key = static_cast<EnumElements>(num);
            QString key_XPATH = SName_XPATH + SPR_SETTINGS_SPECTRUM_ZONES_KEY_XPATH_SUFFIX;
            el->key = new SPREnumVariable<EnumElements>(doc, key_XPATH, _key, this);

            EnumElements kkk = el->key->getData();

            QString fName_XPATH = SName_XPATH + SPR_SETTINGS_SPECTRUM_ZONES_FNAME_XPATH_SUFFIX;
            el->fName = new SPRQStringVariable(doc, fName_XPATH, DEF_SPR_FORMULA_ELEMENTS_PROPERTY[el->key->getData()].name, this);


            QString color_XPATH = SName_XPATH + SPR_SETTINGS_SPECTRUM_ZONES_COLOR_XPATH_SUFFIX;
//            el.color = new SPRQColorVariable(doc, color_XPATH, DEF_SPR_FORMULA_ELEMENTS_PROPERTY[el.key->getData()].color, this);
            el->color = new SPRQColorVariable(doc, color_XPATH, DEF_SPR_FORMULA_ELEMENTS_PROPERTY[kkk].color, this);

            el->sName = new SPRQStringVariable(doc, SName_XPATH, DEF_SPR_FORMULA_ELEMENTS_PROPERTY[el->key->getData()].sname, this);
            bool res;
            QString sname = el->sName->getData();
            int i = el->sName->getData().toInt(&res);
            if(!res){
                elements[el->key->getData()] = el;
            } else {
                unisedElements[el->key->getData()] = el;
            }

            allElements[el->key->getData()] = el;
        }
    }

    void deleteElement(SPRElementsProperty *element);

    SPRElementsProperty *changeElementPropery(EnumElements el, QString _sName, QString _fName="", QColor _color = QColor()){
        SPRElementsProperty *nel = nullptr;
        if(allElements.contains(el)){
            nel = allElements[el];
            nel->key->blockSignals(true); nel->key->setData(el); nel->key->blockSignals(false);
            nel->sName->blockSignals(true); nel->sName->setData(_sName); nel->sName->blockSignals(false);
            nel->fName->blockSignals(true); nel->fName->setData(_fName); nel->fName->blockSignals(false);
            nel->color->blockSignals(true); nel->color->setData(_color); nel->color->blockSignals(false);

            if(elements.contains(el)){
                emit modelChanget(this);
            }
        }
        return nel;
    }

    void addElement(EnumElements elKey, QString _sName, QString _fName, QColor _color = QColor()){
        SPRElementsProperty *el = changeElementPropery(elKey, _sName, _fName, _color);
        if(el){
            if(!elements.contains(elKey)){
                elements[elKey] = el;
                emit doStore();
                emit modelChanget(this);
            }
            if(unisedElements.contains(elKey)){
                unisedElements.remove(elKey);
            }
        }
    }
    ~SPRElementsModel(){
        foreach(SPRElementsProperty *el, allElements.values()){
            if(el) delete el;
        }

//        qDeleteAll(allElements); allElements.clear();
//        qDeleteAll(elements); elements.clear();
//        qDeleteAll(unisedElements); unisedElements.clear();
    }

    SPRQColorVariable *getColorVariable(EnumElements el){
        if(elements.contains(el)){
            return elements[el]->color;
        } else {
            return nullptr;
        }
    }

    void setColor(EnumElements el, QColor color){
        if(elements.contains(el)){
            if(color.isValid()){
                elements[el]->color->setData(color);
            }
        }
    }

    QColor getColor(EnumElements el){
        if(elements.contains(el)){
            return elements[el]->color->getData();
        } else {
            return QColor();
        }
    }
    QString getSName(EnumElements el){
        if(elements.contains(el)){
            return elements[el]->sName->getData();
        } else {
            return "";
        }
    }
    QString getFName(EnumElements el){
        if(elements.contains(el)){
            return elements[el]->fName->getData();
        } else {
            return "";
        }
    }

    QList<EnumElements> getUsedlElementKeys(){
        return elements.keys();
    }
    QMapElementsProperty getUnisedElements(){
        return unisedElements;
    }
    QMapElementsProperty getAllElements() const
    {
        return allElements;
    }
    QMapElementsProperty getElements() const
    {
        return elements;
    }
protected:
//    void setSName(EnumElements el, QString _sName){
//        elements[el]->sName->setData(_sName);
//    }

//    void setFName(EnumElements el, QString _fName){
//        elements[el]->fName->setData(_fName);
//    }
//    void setColor(EnumElements el, QColor _color){
//        elements[el]->color->setData(_color);
//    }

    // IModelVariable interface
public slots:
    virtual void onModelChanget(IModelVariable *var);
};

#endif // SPRELEMENTSMODEL_H
