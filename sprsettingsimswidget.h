#ifndef SPRSETTINGSIMSWIDGET_H
#define SPRSETTINGSIMSWIDGET_H

#include "ui_sprsettingsimswidget.h"
#include "isprwidget.h"
#include "models/sprsettingsimsmodel.h"

#include "isprsettingswidget.h"

#include <qwt_plot_curve.h>
#include <qwt_plot_grid.h>
#include <qwt_plot_histogram.h>
#include <qwt_plot_barchart.h>
#include <qwt_plot_multi_barchart.h>
#include <qwt_plot_textlabel.h>

#include <qwt_plot_zoomer.h>
#include <qwt_legend.h>
#include <qwt_plot_picker.h>
#include <qwt_date_scale_draw.h>

#include "scrollzoomer.h"

class SPRMainModel;
class SPRHistoryModel2;

class SPRSettingsIMSWidget : public QWidget, public ISPRWidget, public ISPRSettingsWidget
{
    Q_OBJECT

    class DrawTitle : public QwtScaleDraw{

        bool withThreads;
        // QwtAbstractScaleDraw interface
    public:
        DrawTitle(bool _allThreads=false);
        virtual QwtText label(double v) const;
    };

    class StaticticBarPicter :public QwtPlotPicker{

        QVector<SPRVariable<qulonglong>*> *data;
        SPRVariable<qulonglong> *limit;
    public:
        StaticticBarPicter(int xaxis, int yaxis, RubberBand rubber, DisplayMode mode, QWidget *canvas, QVector<SPRVariable<qulonglong>*> *_data = nullptr, SPRVariable<qulonglong> *_limit=nullptr):
            QwtPlotPicker(xaxis, yaxis, rubber, mode, canvas), data(_data), limit(_limit)
        {
        }
        void setParams(QVector<SPRVariable<qulonglong>*> *_data, SPRVariable<qulonglong> *_limit);
        // QwtPlotPicker interface
    protected:
        QwtText trackerTextF(const QPointF &p) const;
    };
//    class myBarChart : public QwtPlotMultiBarChart{


//        // QwtPlotMultiBarChart interface
//    protected:
//        QwtScaleMap *myYScaleMap;

//        virtual void drawBar(QPainter *painter, int sampleIndex, int barIndex, const QwtColumnRect &rect) const;

//    public:
//        myBarChart(QwtPlot *plot);
//    };

    SPRSettingsIMSModel *model;
    SPRHistoryModel2 *historyModel;
    SPRMainModel *mainModel;

    QwtPlotPicker *pickter;
    StaticticBarPicter *statisticPicter;
public:
    explicit SPRSettingsIMSWidget(QWidget *parent = nullptr);

    ISPRModelData *getModelData();
    void setIms(SPRVariable<uint> *ims){model->setIms(ims);}

private:
    Ui::SPRSettingsIMSWidget ui;

    QwtPlotCurve *curveRed;
    QwtPlotCurve *curveGreen;

    QwtPlotTextLabel *greenFormula;
    QwtPlotTextLabel *formulasText;

//    QwtPlotHistogram *gist;

    QwtPlotGrid *grid;

    QwtPlotMultiBarChart *bars;
//    myBarChart *bars;

//    QwtPlotZoomer *zoom100;
    QwtLegend *legend;
    // ISPRWidget interface
public:

    ISPRModelData *setModelData(SPRSettingsIMSModel *value);

    void repaintGraphicSetts(double);
public slots:
    virtual void viewChange();
    virtual void viewChange(bool val);
    virtual void widgetsShow();
    void onMouseMoved(QPointF point);
    virtual void onModelChanget(IModelVariable *);
    void onSeparateDataReady();
signals:
    void doShow();

    // ISPRWidget interface
protected:

    // ISPRSettingsWidget interface
public:
    virtual void setMasterMode(bool value);
protected slots:
    void onClearStatisticButtonClicked(bool);
};

#endif // SPRSETTINGSIMSWIDGET_H
