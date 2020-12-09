#include "sprcomplexinfowidget.h"

#include "qwt_plot_layout.h"
#include "qwt_scale_draw.h"
#include "qwt_scale_div.h"

#include "mainwindow.h"

SPRComplexInfoWidget::SPRComplexInfoWidget(QWidget *parent) :
    QWidget(parent),
    listWidgetsData(nullptr)
{
    ui.setupUi(this);

    myTimer.setInterval(1000);
    separateIsRunning = false;

    ui.plot->setAxisScale(QwtPlot::yLeft, 0, 100, 25);
    ui.plot->setAxisScale(QwtPlot::xBottom, 0, 1, 1.0);

    ui.plot->updateCanvasMargins();

    chart = new QwtPlotMultiBarChart();
    QwtColumnSymbol *simbol = new QwtColumnSymbol(QwtColumnSymbol::Box);
    simbol->setPalette(QPalette(QColor(Qt::blue)));
    simbol->setFrameStyle(QwtColumnSymbol::Raised);
    chart->setSymbol(0, simbol);
    chart->setSpacing(0);
    chart->setMargin(3);

    QwtScaleDraw *sd = ui.plot->axisScaleDraw(QwtPlot::xBottom);
    sd->enableComponent(QwtScaleDraw::Backbone, false);
    sd->enableComponent(QwtScaleDraw::Ticks, false);
    sd->enableComponent(QwtScaleDraw::Labels, false);

//    chart->setLayoutPolicy( QwtPlotMultiBarChart::AutoAdjustSamples );
    chart->setLayoutPolicy( QwtPlotMultiBarChart::ScaleSampleToCanvas );

    ui.plot->setAutoReplot(true);
    chart->attach(ui.plot);

//    Qwt

    connect(&myTimer, &QTimer::timeout, this, &SPRComplexInfoWidget::onTimeOut);

    connect(ui.bOnAll, &QPushButton::clicked, this, &SPRComplexInfoWidget::onClickCommands);
    connect(ui.bOffAll, &QPushButton::clicked, this, &SPRComplexInfoWidget::onClickCommands);
    connect(ui.bStartAll, &QPushButton::clicked, this, &SPRComplexInfoWidget::onClickCommands);
    connect(ui.bStopAll, &QPushButton::clicked, this, &SPRComplexInfoWidget::onClickCommands);

    myTimer.start(1000);
}

void SPRComplexInfoWidget::upDateData(QList<SPRSeparatorWidgetData *> *_listModels)
{
    bool separateStarted = false;
    bool separatorsOn = false;
    bool serverOn = false;
    bool isProcess = false;

    QVector<QVector<double>> samples;
    double countAll = 0; double countConc = 0;

    if(_listModels){
        listWidgetsData = _listModels;
    }

    if(listWidgetsData){
        foreach(SPRSeparatorWidgetData *wd, (*listWidgetsData)){
    //    foreach(SPRMainModel *mod, (*listWidgetsData)){
            ServerConnect *server = wd->model->getServer();
            if(wd->widget->isStartStopProcess()){
                isProcess = true;
            }
            if(server){
                if(server->isState(spr_state_server_connect)){
                    serverOn = true;
                }
                if(server->isState(spr_state_rentgen_on) &&
                        server->isState(spr_state_codes_is_stated) &&
                        !server->isState(spr_state_separator_wo_rentgen)){
                    separatorsOn = true;
                }
                if(server->isState(spr_state_separated)){
                    separateStarted = true;
                    if(!separateIsRunning){
                        beginSeparatesProcess = QDateTime::currentDateTime();
                        separateIsRunning = true;
                    }

                    SPRHistoryModel2 *hist = wd->model->getHistoryModel2();
                    if(hist){
                        QVector<SPRHistoryModel2::histDataOneThread> *last = hist->getLastRecords();
                        for(int th=0; th<last->size(); th++){
                            countAll += (*last)[th].data.i_prd[2];
                            countConc += (*last)[th].data.i_prd[0];
                        }
                    }
                }
            }
        }
    }

    separateIsRunning = separateStarted;

    if(!serverOn || isProcess){ // not connect
      ui.bOffAll->setEnabled(false);
      ui.bOnAll->setEnabled(false);
      ui.bStartAll->setEnabled(false);
      ui.bStopAll->setEnabled(false);
    } else { // at least one of all server connected
        if(separateIsRunning){ // at least one of all separate started
            ui.bOffAll->setEnabled(false);
            ui.bOnAll->setEnabled(false);
            ui.bStartAll->setEnabled(true);
            ui.bStopAll->setEnabled(true);
        } else { // no separate
            if(!separatorsOn){ // no one separator on
                ui.bOffAll->setEnabled(false);
                ui.bOnAll->setEnabled(true);
                ui.bStartAll->setEnabled(false);
                ui.bStopAll->setEnabled(false);
            } else { // at least one of all separator on
                ui.bOffAll->setEnabled(true);
                ui.bOnAll->setEnabled(true);
                ui.bStartAll->setEnabled(true);
                ui.bStopAll->setEnabled(false);
            }
        }
    }

//    if(separateStarted){
//        if(!myTimer.isActive()){
//            myTimer.start();
//        }
//    } else {
//        if(myTimer.isActive()){
//            myTimer.stop();
//        }
//    }

    if(separateIsRunning){
        int MIN_SEC = 60;
        int HOUR_SEC = (MIN_SEC * 60);

        qint64 ddiff = llabs(beginSeparatesProcess.secsTo(QDateTime::currentDateTime()));
        int h = ddiff / (HOUR_SEC);
        int m = (ddiff % HOUR_SEC) / MIN_SEC;
        int sec = (ddiff & MIN_SEC);
        QTime td = QTime(h,  m,  sec);
        if(ddiff  > 0){
            ui.leTime->setText(td.toString("hh:mm:ss"));
        }

    }

    QVector<double> point;
    if(countAll > 1e-6){
        point.append((countConc / countAll) * 100);

        ui.leCount->setText(QString::number(countAll));
    } else {
        point.append(0);
    }

    samples.append(point);
//    QVector<Qvector<dp
    chart->setSamples(samples);

//        ui.plot->plotLayout()->setAlignCanvasToScale( QwtPlot::yLeft, false );
//        ui.plot->plotLayout()->setAlignCanvasToScale( QwtPlot::xBottom, true );
        ui.plot->plotLayout()->setCanvasMargin( 0 );
        ui.plot->updateCanvasMargins();

        ui.plot->replot();

}

void SPRComplexInfoWidget::onTimeOut()
{
    upDateData(nullptr);
}

void SPRComplexInfoWidget::onClickCommands(bool)
{
    if(sender() == ui.bOnAll){
        foreach(SPRSeparatorWidgetData *wd, (*listWidgetsData)){
            MainTabWidget *w = wd->widget;
            if(w){
                w->getPitatelControl()->separatorOnOff(true, false);
            }
        }
        return;
    }
    if(sender() == ui.bOffAll){
        foreach(SPRSeparatorWidgetData *wd, (*listWidgetsData)){
            MainTabWidget *w = wd->widget;
            if(w){
                w->getPitatelControl()->separatorOnOff(false, false);
            }
        }
        return;
    }
    if(sender() == ui.bStartAll){
        foreach(SPRSeparatorWidgetData *wd, (*listWidgetsData)){
            MainTabWidget *w = wd->widget;
            if(w){
                w->getSeparateControl()->separateStartStop(true, false);
            }
        }
        return;
    }
    if(sender() == ui.bStopAll){
        foreach(SPRSeparatorWidgetData *wd, (*listWidgetsData)){
            MainTabWidget *w = wd->widget;
            if(w){
                w->getSeparateControl()->separateStartStop(false, false);
            }
        }
        return;
    }
}


void SPRComplexInfoWidget::widgetsShow()
{
}

ISPRModelData *SPRComplexInfoWidget::getModelData()
{
}

void SPRComplexInfoWidget::storeWidgetsStates(QString groupId, QSettings *mainSettings)
{
    mainSettings->beginGroup(groupId);
    mainSettings->endGroup();
}

void SPRComplexInfoWidget::restoreWidgetsStates(QString groupId, QSettings *mainSettings)
{
    mainSettings->beginGroup(groupId);
    mainSettings->endGroup();
}

void SPRComplexInfoWidget::onModelChanget(IModelVariable *)
{
}
