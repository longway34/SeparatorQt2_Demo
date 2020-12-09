#ifndef SPRSTONESIZESCHARTWIDGET_H
#define SPRSTONESIZESCHARTWIDGET_H

#include <QObject>
#include <QWidget>
#include <QtCharts/QChart>
#include <QtCharts/QPieSlice>
#include <QtCharts/QChartView>

#include "models/sprmainmodel.h"
#include "models/sprhistorymodel2.h"
#include "models/sprseparatemodel.h"

QT_CHARTS_BEGIN_NAMESPACE
class QAbstractSeries;
class QPieSlice;
QT_CHARTS_END_NAMESPACE

QT_CHARTS_USE_NAMESPACE

class PercentStoneSizesSlice : public QPieSlice
{
    Q_OBJECT

    qreal myValue;
//    uint8_t thread;
public:
    PercentStoneSizesSlice(/*uint8_t _thread, */qreal value, QString prefix, QAbstractSeries *_parentSeries);

    virtual ~PercentStoneSizesSlice(){}
    QAbstractSeries *parentSeries() const
    {
        return m_parentSeries;
    }


public Q_SLOTS:
    void updateLabel();
    void showHighlight(bool show);
private:
    QAbstractSeries *m_parentSeries;
    QString m_prefix;
};

class StoneChart : public QChart
{
    Q_OBJECT

//    uint8_t currentThread;
    SPRMainModel *model;
    SPRVariable<uint> *threadsVariable;
    QPieSeries *allTreadsSeries;
    QList<QPieSeries*> *thSeries;

    QAbstractSeries *m_currentSeries;
    int m_current_thread;
public:
    explicit StoneChart(SPRMainModel *_model = nullptr, QPieSeries *_all = nullptr, QList<QPieSeries*> *_thSeries = nullptr, QGraphicsItem *parent = nullptr, Qt::WindowFlags wFlags = nullptr);

    void setModelData(SPRMainModel *_model, QPieSeries *_all, QList<QPieSeries*> *_th);
    ~StoneChart(){}
    void changeSeries(QAbstractSeries *series);

    QAbstractSeries *currentSeries() const;

    int currentThread() const;

public Q_SLOTS:
    void handleSliceClicked(QPieSlice *slice);

};

class SPRStoneSizesChartWidget : public QWidget
{
    Q_OBJECT

    struct title_slides_data{
        uint8_t th;
        QString title;
        QColor color;
    };

    StoneChart *chart;
    QPieSeries *allThreadsSeries;

    SPRMainModel *mainModel;
    SPRHistoryModel2 *historyModel;
    SPRSeparateModel *separateModel;

    SPRThreadList threads;


    struct title_slides_data titlesStonesSlides[DEF_SPR_IMS_PARTS_SIZE+1];
    QVector<struct title_slides_data> titlesThreadsSlides;
    QList<QPieSeries*> threadSeries;
    void setTitles();
    void updateLegend();
public:
    QChartView *chartView;
    explicit SPRStoneSizesChartWidget(QWidget *parent = nullptr);

    void setModels(SPRMainModel *_mainModel, SPRHistoryModel2 *_historyModel=nullptr, SPRSeparateModel *_separateModel=nullptr);
    void setSeriesData();
signals:

public slots:
    void onModelChandet(IModelVariable *send);
protected:
    void initSeries();
protected slots:
    void onSeparateDataReady();

    // QWidget interface
protected:
    void resizeEvent(QResizeEvent *event);
};

#endif // SPRSTONESIZESCHARTWIDGET_H
