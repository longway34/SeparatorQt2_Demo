#ifndef SPRGRAPHITEM_H
#define SPRGRAPHITEM_H

#include <QObject>

#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_histogram.h>

//#include "models/sprspectrumitemmodel.h"

#include "sprporogsmoved.h"
#include "_types.h"

class IModelVariable;
class SPRSpectrumItemModel;
class SPRElementsProperty;

class SPRGraphZoneInterval : public QwtPlotHistogram{

    // QwtPlotHistogram interface
protected:
    SPRElementsProperty *element;
    QwtText graphLabel;

    virtual void drawColumn(QPainter *_painter, const QwtColumnRect &_rect, const QwtIntervalSample &_sample) const;
public:
    SPRGraphZoneInterval(SPRElementsProperty *_element=nullptr);
};

class SPRGraphItem :public QObject
{
//    QwtPlot *plot;

    Q_OBJECT

//    uint thread;
private:
    SPRSpectrumItemModel *model;
    QPen defZonesPen;
    QPen selZonesPen;


    double modeKoeff;
//    QMap<EnumElements, QwtPlotHistogram*> zones;
    QMap<EnumElements, SPRGraphZoneInterval*> zones;
    QwtPlotCurve *spect;
//    SPRGraphItem(): plot(nullptr), spect(nullptr){}
//    SPRGraphItem(QwtPlotCurve *_spect, QwtPlot *_plot): plot(_plot), spect(_spect) {}
public:


    SPRGraphItem(SPRSpectrumItemModel *_model, double _modeKoeff = 1);

    int getThread();
    void setCursorSelectedZone(QwtPlotItem *item);

    bool findItemKey(QwtPlotItem *item, EnumElements *key);

    void setVisible(QwtPlot *plot, bool visible, bool current, bool showZones = true);

    ~SPRGraphItem();
    SPRSpectrumItemModel *getModelData();
    virtual void setModelData(SPRSpectrumItemModel *_model, double _modeKoeff);

    SPRSpectrumItemModel *getModel();

    double getModeKoeff();
    void setModeKoeff(double value);
    void renameGraph(QString _name = "");

    QMap<EnumElements, SPRGraphZoneInterval *> getZones();
//    QMap<EnumElements, QwtPlotHistogram *> getZones();

    QwtPlotCurve *getSpect();

public slots:
    void onModelChanget(IModelVariable* _model);
protected:
signals:
    void deleteZoneGraph(QwtPlotItem*);
};

#endif // SPRGRAPHITEM_H
