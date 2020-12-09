#include "sprstonesizeschartwidget.h"

#include <QtCharts/QChartView>
#include <QtCharts/QLegend>
#include <QtCharts/QPieSeries>
#include "_types.h"

#include <QGraphicsLayout>

SPRStoneSizesChartWidget::SPRStoneSizesChartWidget(QWidget *parent) :
    QWidget(parent), allThreadsSeries(nullptr), mainModel(nullptr), historyModel(nullptr), separateModel(nullptr)
{
            chart = new StoneChart();

            chart->setTheme(QChart::ChartThemeLight);
            chart->setAnimationOptions(QChart::AllAnimations);
            chart->legend()->setVisible(true);
//            chart->legend()->setAlignment(Qt::AlignRight);
            chart->legend()->setShowToolTips(true);
            chart->legend()->layout()->setContentsMargins(0, 0, 0, 0);
            chart->setAnimationOptions(QChart::GridAxisAnimations);
            QFont font; font.setFamily("Tahoma"); font.setPixelSize(8);

            chart->legend()->setFont(font);

            allThreadsSeries = new QPieSeries();
            allThreadsSeries->setName(tr("Все ручьи"));


            chartView = new QChartView(chart);
            chartView->setLineWidth(0);
            chartView->setContentsMargins(0,0,0,0);
//            chartView->layout()->setContentsMargins(0,0,0,0);
//            chartView->layout()->setSpacing(0);
            chartView->setRenderHint(QPainter::Antialiasing);
            setLayout(new QBoxLayout(QBoxLayout::TopToBottom));

            setSizePolicy(*new QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred));

            layout()->setMargin(0);
            layout()->setSpacing(0);
            layout()->addWidget(chartView);

            chart->legend()->detachFromChart();


//            QPieSeries *yearSeries = new QPieSeries(this);
//            yearSeries->setName("Sales by year - All");

//            const QStringList months = {
//                "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
//            };
//            const QStringList names = {
//                "Jane", "John", "Axel", "Mary", "Susan", "Bob"
//            };

//            for (const QString &name : names) {
//                QPieSeries *series = new QPieSeries(this);
//                series->setName("Sales by month - " + name);

//                for (const QString &month : months)
//                    *series << new PercentStoneSizesSlice(qrand() % 1000, month, yearSeries);

//                QObject::connect(series, &QPieSeries::clicked, chart, &StoneChart::handleSliceClicked);

//                *yearSeries << new PercentStoneSizesSlice(series->sum(), name, series);
//            }

//            QObject::connect(yearSeries, &QPieSeries::clicked, chart, &StoneChart::handleSliceClicked);

//            chart->changeSeries(yearSeries);

}

void SPRStoneSizesChartWidget::updateLegend(){
    QRectF newRect(
                0,
                chartView->height()-chartView->height() * 0.2,
                chartView->width() - 100,
                chartView->height()
                );
    chart->legend()->setBackgroundVisible(true);
    chart->legend()->setBrush(QBrush(QColor(Qt::lightGray)));
    chart->legend()->setPen(QPen(QColor(Qt::white)));

    chart->legend()->setGeometry(newRect);

    chart->legend()->update();
}

void SPRStoneSizesChartWidget::initSeries(){
    if(!allThreadsSeries){
        allThreadsSeries = new QPieSeries(this);
        allThreadsSeries->setObjectName(QString("Series_all"));
    }

    QList<int> groups = {0, 1, 2, 3, 4};

    allThreadsSeries->clear();

//    for(int gr=0; gr<DEF_SPR_IMS_PARTS_SIZE+1;gr++){
//        foreach(uint8_t th, threads){

//        }
//    }

    for(int gr=0; gr<DEF_SPR_IMS_PARTS_SIZE+1;gr++){
//        QString _prefix = tr("Все каналы (%1)");
        PercentStoneSizesSlice *slice = new PercentStoneSizesSlice(1, titlesStonesSlides[gr].title, allThreadsSeries);
        slice->setColor(titlesStonesSlides[gr].color);
        slice->setObjectName(QString("slice_gr_%1").arg(gr));

        allThreadsSeries->append(slice);
    }

    foreach(uint8_t th, threads){
        QPieSeries *series = new QPieSeries(this);
        series->setObjectName(QString("series_th_%1").arg(th));
        threadSeries << series;
        series->setName(titlesThreadsSlides[th].title);
        foreach(int gr, groups){

            PercentStoneSizesSlice *slice = new PercentStoneSizesSlice(1/* / groups.size()*/, titlesStonesSlides[gr].title, allThreadsSeries);
            slice->setColor(titlesStonesSlides[gr].color);
            slice->setObjectName(QString("slice_th_%1_gr_%2").arg(th).arg(gr));

            *series << slice;
        }

        QObject::connect(series, &QPieSeries::clicked, chart, &StoneChart::handleSliceClicked);
//        PercentStoneSizesSlice *slice = new PercentStoneSizesSlice(1/* / threads.size()*/, titlesThreadsSlides[th].title, series);
//        slice->setColor(titlesThreadsSlides[th].color);
//        slice->setObjectName(QString("slice_th_%1").arg(th));

//        *allThreadsSeries << slice;
    }
    QObject::connect(allThreadsSeries, &QPieSeries::clicked, chart, &StoneChart::handleSliceClicked);

    chart->changeSeries(allThreadsSeries);
    updateLegend();
}

void SPRStoneSizesChartWidget::setSeriesData()
{
    QAbstractSeries *_current = chart->currentSeries();
    QVector<double> toAllThs;
    QVector<QVector<double>> stonesCounts;
    if(historyModel){
        QVector<SPRHistoryModel2::histDataOneThread> *last = historyModel->getLastSessionRecords();
        foreach(uint8_t th, threads){
            QVector<double> vss;
//            toAllThs.append((*last)[th].data.i_prd[2]);
            for(int gr=0;gr<5;gr++){
                vss.append((*last)[th].data.s_rst[gr]);

                if(toAllThs.size() <= gr){
                    toAllThs.append((*last)[th].data.s_rst[gr]);
                } else {
                    toAllThs[gr] += (*last)[th].data.s_rst[gr];
                }
            }
            stonesCounts.append(vss);
        }
    } else {
        toAllThs.fill(0, DEF_SPR_IMS_PARTS_SIZE+1);
    }

//    QVector<QVector<double>> toAllThs = historyModel->getMTableCounts(threads);
    uint8_t countGr = 0;


    foreach(QPieSlice *oneGroupStoneSlice, allThreadsSeries->slices()){
//        oneChannelSlice->setColor(titlesThreadsSlides[countTh].color);
        oneGroupStoneSlice->setValue(toAllThs[countGr]);

        PercentStoneSizesSlice *ps = static_cast<PercentStoneSizesSlice*>(oneGroupStoneSlice);
        if(ps){
            ps->updateLabel();
        }

        countGr++;
    }
    int countTh = 0;
    foreach(QPieSeries *oneChannelSeries, threadSeries){
//        oneChannelSeries->setName(titlesThreadsSlides[countTh].title);
        int groupNum = 0;
        foreach(QPieSlice *oneGroup, oneChannelSeries->slices()){

//            oneGroup->setColor(titlesStonesSlides[groupNum].color);
//            oneGroup->setLabel(titlesStonesSlides[groupNum].title);
            oneGroup->setValue(stonesCounts[countTh][groupNum]);

            PercentStoneSizesSlice *ps = static_cast<PercentStoneSizesSlice*>(oneGroup);
            if(ps){
                qreal per = ps->percentage();
                ps->updateLabel();
            }
            groupNum++;
        }
        countTh++;
    }
    chart->changeSeries(_current);
    updateLegend();
}

void SPRStoneSizesChartWidget::setTitles(){

    titlesThreadsSlides.clear();
    foreach(uint8_t th, threads){
        titlesThreadsSlides.append(title_slides_data({th, QString(tr("Руч.%1\n")).arg(th+1), defThreadsColor[th]}));
    }

    titlesStonesSlides[0] = title_slides_data({0, QString(tr("Ост.\n")), defThreadsColor[0]});

    for(uint8_t i=1; i<DEF_SPR_IMS_PARTS_SIZE+1;i++){
        if(separateModel->isSeparateEmpty()){
            titlesStonesSlides[i] = title_slides_data({i, QString("%1-%2\n").
                arg(mainModel->getSettingsIMSModel()->timesMettering[i-1]->getData()).
                arg(mainModel->getSettingsIMSModel()->timesMettering[i]->getData()),
                                    defThreadsColor[i]});
        } else {
            titlesStonesSlides[i] = title_slides_data({i, QString("%1-%2\n").
                arg(mainModel->getSeparateModel()->getSettingsSeparate()->tiz[i-1]).
                arg(mainModel->getSeparateModel()->getSettingsSeparate()->tiz[i]),
                                    defThreadsColor[i]});
        }
    }
}

void SPRStoneSizesChartWidget::onModelChandet(IModelVariable *send){
}

void SPRStoneSizesChartWidget::onSeparateDataReady(){
    if(sender() == historyModel){
        setSeriesData();
    }
}

void SPRStoneSizesChartWidget::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    if(chart && chartView){
        updateLegend();
    }
}

void SPRStoneSizesChartWidget::setModels(SPRMainModel *_mainModel, SPRHistoryModel2 *_historyModel, SPRSeparateModel *_separateModel)
{
    mainModel = _mainModel;
    if(mainModel){
        threads = mainModel->getThreadsList();
    } else {
        threads = getAllThreadsListDefault();
    }
    if(_historyModel){
        historyModel = _historyModel;
        connect(historyModel, &SPRHistoryModel2::separateDataReady, this, &SPRStoneSizesChartWidget::onSeparateDataReady);
    }
    if(!_separateModel){
        if(mainModel){
            separateModel = mainModel->getSeparateModel();
        }
    } else {
        separateModel = _separateModel;
    }


    setTitles();
    initSeries();

    chart->setModelData(mainModel, allThreadsSeries, &threadSeries);

    chart->changeSeries(allThreadsSeries);
}
/**
 * @brief StoneChart::StoneChart
 * @param _model
 * @param _all
 * @param _thSeries
 * @param parent
 * @param wFlags
 */


StoneChart::StoneChart(SPRMainModel *_model, QPieSeries *_all, QList<QPieSeries *> *_thSeries, QGraphicsItem *parent, Qt::WindowFlags wFlags): QChart(QChart::ChartTypeCartesian, parent, wFlags),
    m_current_thread(-1), m_currentSeries(nullptr), allTreadsSeries(_all), thSeries(_thSeries)
{
    model = _model;
    if(model){
        threadsVariable = model->getThreads();
        m_current_thread = threadsVariable->getData() + 1;
    }

}

void StoneChart::setModelData(SPRMainModel *_model, QPieSeries *_all, QList<QPieSeries *> *_th){
    model = _model;
    if(model){
        threadsVariable = model->getThreads();
        m_current_thread = threadsVariable->getData() + 1;
    }
    allTreadsSeries = _all;
    thSeries = _th;
    m_current_thread = -1;
    m_currentSeries = (QAbstractSeries*)allTreadsSeries;
}

void StoneChart::changeSeries(QAbstractSeries *series)
{
    // NOTE: if the series is owned by the chart it will be deleted
    // here the "window" owns the series...
    if (m_currentSeries)
        removeSeries(m_currentSeries);

    m_currentSeries = series;

    if(m_currentSeries){
        addSeries(series);
        setTitle(series->name());
    }
}

QAbstractSeries *StoneChart::currentSeries() const
{
    return m_currentSeries;
}

int StoneChart::currentThread() const
{
    return m_current_thread;
}

void StoneChart::handleSliceClicked(QPieSlice *slice)
{
    QPieSeries *nextSeries = nullptr;
    m_current_thread++;
    if(m_current_thread >= threadsVariable->getData()){
        m_current_thread = -1;
    };
    if(m_current_thread < 0){
        if(allTreadsSeries){
            nextSeries = allTreadsSeries;
        }
    } else {
        if(thSeries && m_current_thread <  thSeries->size()){
            nextSeries = (*thSeries)[m_current_thread];
        }
    }
    //        PercentStoneSizesSlice *percentStoneSizesSlice = static_cast<PercentStoneSizesSlice *>(slice);
    //        currentThread = percentStoneSizesSlice->getThread();
    if(nextSeries){
        changeSeries((QAbstractSeries*)nextSeries);
    }
}

/**
 * @brief PercentStoneSizesSlice::updateLabel
 */

PercentStoneSizesSlice::PercentStoneSizesSlice(qreal value, QString prefix, QAbstractSeries *_parentSeries)
    : /*thread(_thread),*/
      m_parentSeries(_parentSeries),
      m_prefix(prefix)
{
    setValue(value);
    updateLabel();
    setLabelFont(QFont("Tahoma", 6));
    connect(this, &PercentStoneSizesSlice::percentageChanged, this, &PercentStoneSizesSlice::updateLabel);
    connect(this, &PercentStoneSizesSlice::hovered, this, &PercentStoneSizesSlice::showHighlight);
}

void PercentStoneSizesSlice::updateLabel()
{
//    QString label = "";
    QString label = m_prefix;
//    //        if(this->value() > 1){
//    label += " ";
    label += tr(" %1 (%2%)").arg(QString::number(this->value())).arg(QString::number(this->percentage() * 100, 'f', 2));
//    label += " (";
//    label += QString::number(this->percentage() * 100, 'f', 2);
//    label += "%)";
    //        }
    setLabel(label);
    if(this->percentage() > 0.15){
        this->setLabelPosition(QPieSlice::LabelInsideNormal);
    } else {
        this->setLabelPosition(QPieSlice::LabelOutside);
    }
}

void PercentStoneSizesSlice::showHighlight(bool show)
{
    setLabelVisible(show);
    setExploded(show);
}
