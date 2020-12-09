#include "sprspectrumzonestablemodel.h"

SPRVariable<uint> *SPRSpectrumZonesTableModel::getThreads() const
{
    return threads;
}

void SPRSpectrumZonesTableModel::setThreads(SPRVariable<uint> *value)
{
    if(value){
        if(QVariant(property("delete_threads")).toBool()){
            if(threads) delete threads;
            setProperty("delete_threads", QVariant(false));
        }
        connect(value, &IModelVariable::modelChanget, this, &SPRSpectrumZonesTableModel::onModelChanget);
        threads = value;
    }
}

QMapElementsRanges SPRSpectrumZonesTableModel::getElementsRanges(uint _tIndex){
    return getItem(_tIndex)->getZones();
}

SpectorRange *SPRSpectrumZonesTableModel::getElementsRanges(int _tIndex, EnumElements _el){
    SpectorRange* ranges = nullptr;
    QMapElementsRanges mranges = getItem(_tIndex)->getZones();
    if(mranges.contains(_el)){
        ranges = mranges[_el];
    }
    return ranges;
}

SPRElementsModel *SPRSpectrumZonesTableModel::getElementsProperty(){
    return elementsProperty;
}

void SPRSpectrumZonesTableModel::setElementsProperty(SPRElementsModel *value)
{
    if(value){
        if(elementsProperty && property("delete_elementsProperty").value<bool>()){
            delete elementsProperty; elementsProperty = nullptr;
        }
    }
    if(elementsProperty){
        disconnect(elementsProperty, &SPRElementsModel::modelChanget, this, &SPRSpectrumZonesTableModel::onModelChanget);
    }
    elementsProperty = value;
    if(elementsProperty){
        connect(elementsProperty, &SPRElementsModel::modelChanget, this, &SPRSpectrumZonesTableModel::onModelChanget);
    }
    setProperty("delete_elementsProperty", QVariant(false));
}

void SPRSpectrumZonesTableModel::setMinMaxValues(EnumElements el, uint _min, uint _max, int tIndex){
    if(tIndex < 0){
        for(int th=0; th<items.size(); th++){
            items[th]->setMinMax(el, _min, _max);
        }
    } else {
        if(tIndex >= items.size()-1){
            tIndex = items.size()-1;
        }
        items[tIndex]->setMinMax(el, _min, _max);
    }
}

SPRSpectrumZonesModel *SPRSpectrumZonesTableModel::getItem(int _tIndex){
    if(_tIndex < 0 || _tIndex >= items.size()){
        _tIndex = 0;
    }
    return items[_tIndex];
}

SPRSpectrumZonesTableModel::SPRSpectrumZonesTableModel(): threads(nullptr), items(), elementsProperty(nullptr)
{

}

SPRSpectrumZonesTableModel::SPRSpectrumZonesTableModel(QDomDocument *_doc, SPRElementsModel *elements, ISPRModelData *parent):
    ISPRModelData(_doc, parent), threads(nullptr), items(), elementsProperty(nullptr)
{

    threads = new SPRVariable<uint>(doc,SPR_SETTINGS_MAIN_THREADS_XPATH, DEF_SPR_MAIN_THREADS, this);
    setProperty("delete_threads", QVariant(true));

    if(elements){
        setElementsProperty(elements);
    } else {
        elementsProperty = new SPRElementsModel(doc, this);
        setProperty("delete_elementsProperty", QVariant(true));
    }

    for(int i=0; i<MAX_SPR_MAIN_THREADS; i++){
        SPRSpectrumZonesModel *it = new SPRSpectrumZonesModel(doc, i, elementsProperty, this);
        connect(it, &SPRSpectrumZonesModel::modelChanget, this, &SPRSpectrumZonesTableModel::onModelChanget);
        items.push_back(it);
    }
}

SPRSpectrumZonesTableModel::~SPRSpectrumZonesTableModel()
{
    if(threads && QVariant(property("delete_threads")).toBool()){
        delete threads; threads = nullptr;
        setProperty("delete_threads", QVariant(false));
    }
    if(elementsProperty && property("delete_elementsProperty").value<bool>()){
        delete elementsProperty;
        setProperty("delete_elementsProperty", QVariant(false));
    }
}


void SPRSpectrumZonesTableModel::onModelChanget(IModelVariable *send)
{
    QList<IModelVariable*> ch = getChildren();
    if(isMyChildren(send) || isMyChildren(qobject_cast<IModelVariable*>(sender()))){
        emit modelChanget(send);
        return;
    }
    if(sender() == elementsProperty || elementsProperty->isMyChildren(qobject_cast<IModelVariable*>(sender())) || send == elementsProperty){
        emit modelChanget(send);
        return;
    }
    if(sender() == threads || send == threads){
        emit modelChanget(send);
        return;
    }
}


void SPRSpectrumZonesTableModel::store()
{
    if(elementsProperty){
        elementsProperty->store();
    }
    emit doStore();
}
