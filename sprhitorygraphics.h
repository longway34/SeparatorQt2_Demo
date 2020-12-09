#ifndef SPRHITORYGRAPHICS_H
#define SPRHITORYGRAPHICS_H

#include <QObject>
#include <QWidget>

#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_grid.h>
#include <qwt_legend.h>
#include <qwt_scale_draw.h>

#include "models/sprseparatemodel.h"
#include "models/sprmainmodel.h"

#include "models/sprhistorymodel2.h"

/**
 * @brief The TimeScaleDraw class
 */

#define MAX_SPR_HISTORY_GRAPHIXS_SAMPLES_MSEC    (60 * 60 * 2 *1000)

class TimeScaleDraw: public QwtScaleDraw
{
public:
    TimeScaleDraw(  )
//        baseTime( base )
    {
    }
    virtual QwtText label( double v ) const
    {
        QDateTime time = QDateTime::fromSecsSinceEpoch( static_cast<int>( v ) );
        QwtText txt = QwtText(time.toString("hh:mm"));
        QFont sys = QFont("Tahoma", 8);
        txt.setFont(sys);
        return txt;
    }
private:
//    QTime baseTime;
};
/**
 * @brief The SPRHitoryGraphics class
 */
class SPRHitoryGraphics: public QwtPlot
{
    Q_OBJECT

    class HistoryGraphic {
        uint8_t thread;
        QwtPlotCurve *curve;
        QVector<QPointF> curveData;
        QColor color;
        QString name;


    public:
        HistoryGraphic(uint8_t _th, QString _name, QColor _color): thread(_th), curve(nullptr), name(_name), color(_color){
            curve = new QwtPlotCurve(_name);
//            QPen pen(_color);
            curve->setPen(_color, 2);
        }
        virtual ~HistoryGraphic(){
            if(curve){
                delete curve;
            }
        }
        QwtPlotCurve *getGraphic(){
            return curve;
        }
        uint8_t getThread(){
            return thread;
        }
        void setThread(const uint8_t &value){
            thread = value;
        }
        QVector<QPointF> getGraphicData(){
            return curveData;
        }
        void setGraphicData(const QVector<QPointF> &value){
            curveData = value;
            if(curve){
                curve->setSamples(curveData);
            }
        }
        void setColor(const QColor &value){
            color = value;
        }
        void setName(const QString &value){
            name = value;
        }
        QString getName(){
            return name;
        }
        QColor getColor(){
            return color;
        }
        QVector<QPointF> getCurveData(){
            return curveData;
        }
        void clearOldValues(qint64 _min_time_in_sec){
            bool isRange = false;
            while(!isRange){
                qint64 diff = static_cast<qint64>(fabs(curveData.back().x() - curveData.front().x()));
                if(diff < _min_time_in_sec){
                    break;
                }
                curveData.takeFirst();
            }
         }
        void setCurveDataValue(QPointF &value, qint64 _min_time_in_sec){
//            if(curveData.size()>0){
//                if(value.x() - curveData.back().x() > 100){
//                    curveData.push_back(value);
//                }
//            } else {
                curveData.push_back(value);
//            }
            clearOldValues(_min_time_in_sec);
            curve->setSamples(curveData);
        }
        void setCurveData(QVector<QPointF> &data, qint64 _min_time_in_sec){
            curveData = data;
            clearOldValues(_min_time_in_sec);
            curve->setSamples(curveData);
        }
        QwtPlotCurve *getCurve() const;
    };

public:

    QList<HistoryGraphic *> lstGraphics;
    //    QVector<QVector<QPointF>> curvesData;

    SPRThreadList threads;
    HistoryGraphic *current;
    QString titleFormat;

    SPRHistoryModel2 *model;

    SPRWorkSeparateTypeData type;

    QwtPlotGrid *grid;
    QwtLegend *legend;
    TimeScaleDraw *timeDraw;

    qint64 interval_axis_scale_in_sec;
    qint64 size_parts_in_sec;

    QList<QwtPlotItem *> getGraphics(int thread =-1);

public:
    virtual bool eventFilter(QObject *object, QEvent * event);

    SPRHitoryGraphics(QWidget *parent=nullptr);

    virtual ~SPRHitoryGraphics();

    void init(SPRThreadList _thread, SPRHistoryModel2 *_model, SPRWorkSeparateTypeData _type, QString _titleFormat = "", qint64 _interval_in_sec = 60 * 60 * 2);
    void setThreadsVisible(HistoryGraphic *_current);


    void setGraphicTitle();
public slots:
    void onDblClick();

    void onModelChange(IModelVariable*);
    void onSeparateDataReady();

signals:
    void dblClickMouseEvent();
protected:
    QList<QwtPlotItem *> getGraphics(HistoryGraphic *_current);
};

#endif // SPRHITORYGRAPHICS_H
