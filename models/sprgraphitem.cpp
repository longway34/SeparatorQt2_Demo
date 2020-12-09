#include "models/sprgraphitem.h"
#include "models/sprspectrumitemmodel.h"
#include "models/sprelementsmodel.h"

#include <QMap>

SPRSpectrumItemModel *SPRGraphItem::getModel()
{
    return model;
}

double SPRGraphItem::getModeKoeff()
{
    return modeKoeff;
}

void SPRGraphItem::setModeKoeff(double value)
{
    modeKoeff = value;
}

void SPRGraphItem::renameGraph(QString _name){
    if(_name.isEmpty()){
        if(model){
            _name = model->getSpectrumName();
        }
    }
    if(spect){
        spect->setTitle(_name);
    }
    if(model){
        foreach(SPRGraphZoneInterval *zone, zones.values()){
//            foreach(QwtPlotHistogram *zone, zones.values()){
            EnumElements el = zones.key(zone);
            QString _elName = model->getZones()->getElementsProperty()->getSName(el);
            zone->setTitle(_elName);
        }
    }
}

//QMap<EnumElements, QwtPlotHistogram *> SPRGraphItem::getZones()
QMap<EnumElements, SPRGraphZoneInterval *> SPRGraphItem::getZones()
{
//    QMapElementsRanges elementsRanges = model->getZones()->getElements();
//    QSet<EnumElements> needElements = elementsRanges.keys().toSet();
//    QSet<EnumElements> nowElemens = zones.keys().toSet();

//    QSet<EnumElements> toDel = nowElemens;
//    toDel.subtract(needElements);
//    QSet<EnumElements> toAdd = needElements;
//    toAdd.subtract(nowElemens);

//    foreach(EnumElements el, zones.keys()){
//        QwtPlotHistogram *h= zones.take(el);
//        if(h){
//            h->detach();
//            delete h;
//        }
//        zones.remove(el);
//    }
//    foreach(EnumElements el, toAdd){
//        QwtPlotHistogram *h = new QwtPlotHistogram();
//        zones[el] = h;
//        h->setData()

//    }

//    QSet<QString> needElements = {"Ns", "Fe", "Cu", "Mg"};
//    QSet<QString> nowElemens = {"Ns", "Fe", "Mo"};

//    QSet<QString> toDel = nowElemens;
//    toDel.subtract(needElements);
//    QSet<QString> toAdd = needElements;
//    toAdd.subtract(nowElemens);

    return zones;
}

QwtPlotCurve *SPRGraphItem::getSpect()
{
    return spect;
}

SPRGraphItem::SPRGraphItem(SPRSpectrumItemModel *_model, double _modeKoeff): /*plot(_plot), */
    model(nullptr), spect(nullptr) {
//    modeKoeff = 1;
    setModelData(_model, _modeKoeff);
    //        thread = _model->getThread();
    defZonesPen.setWidth(2);
    defZonesPen.setStyle(Qt::SolidLine);

    selZonesPen.setWidth(3);
    selZonesPen.setStyle(Qt::DashLine);

}

int SPRGraphItem::getThread(){
    return model->getThread();
}

void SPRGraphItem::setCursorSelectedZone(QwtPlotItem *item){
    foreach(QwtPlotHistogram *zone, zones.values()){
        QColor color = zone->pen().color();
        if(item == zone){
            selZonesPen.setColor(color);
            zone->setPen(selZonesPen);
        } else {
            defZonesPen.setColor(color);
            zone->setPen(defZonesPen);
        }
    }
}

bool SPRGraphItem::findItemKey(QwtPlotItem *item, EnumElements *key){
    bool res = false;
    if(zones.values().contains((SPRGraphZoneInterval*)item)){
//        if(zones.values().contains((QwtPlotHistogram*)item)){
        res = true;
        if(key){
//            *key = zones.key((QwtPlotHistogram*)item);
            *key = zones.key((SPRGraphZoneInterval*)item);
        }
    }
    return res;
}

void SPRGraphItem::setVisible(QwtPlot *plot, bool visible, bool current, bool showZones){
    foreach (EnumElements el, zones.keys()) {
        if(current && showZones){
            QColor color=model->getZones()->getElementsProperty()->getColor(el);
            color.setAlpha(64);
            defZonesPen.setColor(color);
            zones[el]->setPen(defZonesPen);
            zones[el]->setBrush(QBrush(color));
            if(!plot->itemList().contains(zones[el])){
                zones[el]->attach(plot);
            }
        } else {
            if(plot->itemList().contains(zones[el])){
                zones[el]->detach();
            }
        }
    }
    if(visible){
        QColor cpen = model->getSpectrumColor();
        if(current){
            QPen pen(cpen, 3);
            spect->setPen(pen);
            cpen.setAlpha(64);
            QBrush brush(cpen);
            spect->setBrush(brush);
        } else {
            QPen pen(cpen,1);
            spect->setPen(pen);
            cpen.setAlpha(0);
            spect->setBrush(QBrush(cpen));
        }
        if(!plot->itemList().contains(spect)){
            spect->attach(plot);
        }
    } else {
        if(plot->itemList().contains(spect)){
            spect->detach();
        }
    }
}

SPRGraphItem::~SPRGraphItem(){
//    if(plot){
        foreach (QwtPlotHistogram *zone, zones.values()) {
            //                plot->removeItem(zones[el]);
//            if(plot->itemList().contains(zone)) zone->detach();
            if(zone) delete zone;
        }
//        if(plot->itemList().contains(spect)) spect->detach();
        if(spect) delete spect;
//    }
}

SPRSpectrumItemModel *SPRGraphItem::getModelData()
{
    return model;
}

void SPRGraphItem::setModelData(SPRSpectrumItemModel *_model, double _modeKoeff){
    if(model && model != _model){
        disconnect(model, SIGNAL(modelChanget(IModelVariable*)), this, SLOT(onModelChanget(IModelVariable*)));
    }

    model = _model;

    if(model){
        modeKoeff = _modeKoeff;

        onModelChanget(_model);
        connect(model, SIGNAL(modelChanget(IModelVariable*)), this, SLOT(onModelChanget(IModelVariable*)));
    }
}

void SPRGraphItem::onModelChanget(IModelVariable *_model)
{
//    model = (SPRSpectrumItemModel*)_model;
    if(model){
        SPRSpectrumZonesModel *zonesModel = model->getZones();
        if((model == _model) || (zonesModel == _model)){
            if(!spect){
                spect = new QwtPlotCurve(QString(model->getSpectrumName()));
            }

            QVector<QPointF> samples = model->getSpectrumGraphicsSamples();
            for(int x=0; x<samples.size(); x++){
                double y = samples[x].ry() * modeKoeff;
                samples[x].setY(y);
            }
            spect->setSamples(samples);
            spect->setPen(QPen(model->getSpectrumColor(), 1));
            QList<EnumElements> nowZones = this->zones.keys();
            QList<EnumElements> needZones = model->getZones()->getZones().keys();
            QSet<EnumElements> forDelete = nowZones.toSet();
            forDelete.subtract(needZones.toSet());

//            QSet<QString> needElements = {"Ns", "Fe", "Cu", "Mg"};
//            QSet<QString> nowElemens = {"Ns", "Fe", "Mo"};

//            QSet<QString> toDel = nowElemens;
//            toDel.subtract(needElements);
//            QSet<QString> toAdd = needElements;
//            toAdd.subtract(nowElemens);

            foreach(EnumElements el, forDelete){
                this->zones[el]->detach();
//                if(this->zones[el]) delete this->zones[el];
                this->zones.remove(el);
            }

            foreach (EnumElements el, model->getZones()->getZones().keys()) {
                QString title = model->getElementsModel()->getFName(el);
                if(!zones[el]){
                    zones[el] = new SPRGraphZoneInterval(model->getElementsModel()->getElementProperty(el));
//                    zones[el] = new QwtPlotHistogram(title);
                } else {
                    zones[el]->setTitle(title);
                }
                QVector<QwtIntervalSample> intervals = model->getZonesGaphics()[el];
                for(int i=0; i<intervals.size(); i++){
                    double value = intervals[i].value * modeKoeff;
                    intervals[i].value = value;
                }
                zones[el]->setSamples(intervals);
            }
            renameGraph();
        }
    }
}


void SPRGraphZoneInterval::drawColumn(QPainter *_painter, const QwtColumnRect &_rect, const QwtIntervalSample &_sample) const
{
    QwtPlotHistogram::drawColumn(_painter, _rect, _sample);
    double maxY = this->plot()->axisInterval(yAxis()).maxValue();
    double yPosition = _rect.vInterval.maxValue();
    double xPosition = _rect.hInterval.minValue();

    double yTextPosition = _rect.vInterval.maxValue() - 3 - graphLabel.textSize().height();
    if(yTextPosition < 0){
        yTextPosition = _rect.vInterval.maxValue() - 3;
    }
//    if(_rect.vInterval.maxValue() - 5 - graphLabel.textSize().height())

//    if(maxY - _rect.hInterval.maxValue() < 20){
//        this->
//    }
    QPen oldPen = _painter->pen();
    QPen textPen(element->color->getData());
    QFont oldFont = _painter->font();

    _painter->setFont(QFont("Arial", 10, QFont::Bold));
    _painter->setPen(textPen);
    _painter->drawText(_rect.hInterval.minValue(),
                       yTextPosition,
                       _rect.hInterval.width(),
                       graphLabel.textSize().height(),
                       Qt::AlignHCenter | Qt::AlignVCenter,
                       graphLabel.text());
    _painter->setPen(oldPen);
    _painter->setFont(oldFont);
}

SPRGraphZoneInterval::SPRGraphZoneInterval(SPRElementsProperty *_element): QwtPlotHistogram (), element(nullptr)
{
    if(_element){
        element = _element;
        setTitle(element->fName->getData());
        graphLabel.setText(element->sName->getData());
        graphLabel.setColor(element->color->getData());
        graphLabel.setFont(QFont("Arial", 12, QFont::Bold));
    } else {
        graphLabel.setText("");
    }
}
