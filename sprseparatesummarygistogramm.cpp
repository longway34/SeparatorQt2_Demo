#include "sprseparatesummarygistogramm.h"
#include "models/sprmainmodel.h"
#include "models/sprhistorymodel2.h"

#include <qwt_plot_layout.h>
#include <qwt_date_scale_draw.h>

SPRSeparateSummaryGistogramm::SPRSeparateSummaryGistogramm(QWidget *parent):
    QwtPlot(parent), model(nullptr), mainModel(nullptr), drawTitle(nullptr), porogCurveCritical(nullptr), porogCurvePermitt(nullptr), chart(nullptr)
{

}

void SPRSeparateSummaryGistogramm::setModelData(SPRMainModel *_MainModel, SPRHistoryModel2 *_histModel, QString _title, QColor _barColor, SPRThreadList _threads, SPRWorkSeparateTypeData _type, bool _woPorogs)
{
    mainModel = _MainModel;
    withOutPorogs = _woPorogs;

    type = _type;
    if(type == percentConcentrate2Input){
        withOutPorogs = true;
    }
    model = _histModel;

    canvas()->setPalette( QColor( "#EEEEEE" ) );

    threads = _threads;

    if(mainModel){
        if(threads.size() == 0){
            threads = mainModel->getThreadsList();
        }
        if(model){
            connect(model, &SPRHistoryModel2::separateDataReady, this, &SPRSeparateSummaryGistogramm::onSeparateDataReady);

            setAxisMaxMinor(QwtPlot::xBottom, 0);
            setAxisMaxMajor(QwtPlot::xBottom, threads.size());

            setAxisAutoScale(QwtPlot::yLeft);
            setAxisLabelAlignment(QwtPlot::xBottom, Qt::AlignCenter);

            chart = new QwtPlotMultiBarChart();
            chart->setAxes(QwtPlot::xBottom, QwtPlot::yLeft);
            chart->setStyle(QwtPlotMultiBarChart::Stacked);
            setAxisScaleDraw(QwtPlot::xBottom, new DrawTitle(threads.size() > 0));

            porogCurvePermitt = new QwtPlotMarker(tr("Допустимый порог загрузки"));
            porogCurveCritical = new QwtPlotMarker(tr("Критичный порог загрузки"));

            QVector<QVector<double>> series(threads.size(), QVector<double>(1,0));

            if(!withOutPorogs){
                QPen pen;

                series = QVector<QVector<double>>(threads.size(), QVector<double>(3,0));

                QwtLegend *leg = new QwtLegend();
                this->insertLegend(leg, QwtPlot::BottomLegend);

                pen.setColor(Qt::red); pen.setStyle(Qt::DashLine); pen.setWidth(2);
                porogCurveCritical->setLinePen(pen);
                porogCurveCritical->setLineStyle( QwtPlotMarker::HLine );

                pen.setColor(Qt::blue);
                porogCurvePermitt->setLinePen(pen);
                porogCurvePermitt->setLineStyle( QwtPlotMarker::HLine );

                porogCurvePermitt->attach(this);
                porogCurveCritical->attach(this);

                chart->setBarTitles({QwtText(tr("нормально")), QwtText(tr("допустимо")), QwtText(tr("опасно"))});
                QColor col[3] = {Qt::gray, Qt::blue, Qt::red};
                if(_barColor.isValid()){
                    col[0] = _barColor;
                }

                for(int i=0; i<3; i++){

                    QwtColumnSymbol *symbol = new QwtColumnSymbol( QwtColumnSymbol::Box );
                    symbol->setLineWidth( 2 );
                    symbol->setFrameStyle( QwtColumnSymbol::Raised );
                    symbol->setPalette( QPalette( col[i] ) );

                    chart->setSymbol( i, symbol );
                }
//                chart->setSamples(series);
            }

            title = QwtText(_title);
            QFont font = parentWidget()->font();
            font.setPointSize(10);
            title.setFont(font);
            setTitle(title);

            chart->setSamples(series);

            QwtText leftTitle;
            if(type == percentConcentrate2Input){
                leftTitle.setText("%");
                setAxisScale(QwtPlot::yLeft, 0, 100, 25);
            } else if(type == countInputInSec || type == countConcentrateInSec){
                leftTitle.setText(QString(tr("кусков/сек.")));
                setAxisScale(QwtPlot::yLeft, 0, 10, 2);
            } else {
                leftTitle.setText("");
            }
            setAxisTitle(QwtPlot::yLeft, leftTitle);

            plotLayout()->setAlignCanvasToScale( QwtPlot::yLeft, false );

            plotLayout()->setCanvasMargin( 0 );
            updateCanvasMargins();

            setAutoReplot(true);

            chart->attach(this);

            drawTitle = new DrawTitle(true, model, type);

            setAxisScaleDraw(QwtPlot::xBottom, drawTitle);
            setAxisScale(QwtPlot::xBottom, 0, model->getLastRecords()->size(), 1);

            onModelChanget(nullptr);
        }
    }
}

void SPRSeparateSummaryGistogramm::onModelChanget(IModelVariable* send){
}

void SPRSeparateSummaryGistogramm::onSeparateDataReady()
{
    if(sender() == model){
        if(model){
            double porog1 = mainModel->getSettingsControlModel()->speedStreamPermiss->getData();
            double porog2 = mainModel->getSettingsControlModel()->speedStreamCritical->getData();

            if(!withOutPorogs){
                porogCurvePermitt->setValue(0, porog1);
                porogCurveCritical->setValue(0, porog2);
            }

            QVector<QVector<double>> series;
            QVector<SPRHistoryModel2::histDataOneThread> *last = model->getLastRecords();
            QVector<SPRHistoryModel2::histDataOneThread> *avr = model->getAvr10mRecords();
            QVector<SPRHistoryModel2::histDataOneThread> *diff = model->getDiffRecords();
            QVector<QVector<double>> for1Thread;
            double maxY = 0;
            if(threads.size() > 0){
                double summ_all = 0;double summ_con = 0;
                foreach(uint8_t th, threads){
                    double val = 0;
                    if(type == percentConcentrate2Input){
                        summ_all += (*last)[th].data.i_prd[2];
                        if((*last)[th].data.i_prd[2] > 1e-6){
                            val = (*last)[th].data.i_prd[0] / (*last)[th].data.i_prd[2] * 100;
                            summ_con += (*last)[th].data.i_prd[0];
                        }
                    } else if(type == countInputInSec){
//                        qint64 zn = llabs()
                        summ_all += (*avr)[th].data.i_prd[2];
                        val = (*avr)[th].data.i_prd[2] * 1000;/*/ (DEF_SPR_HISTOTY_AVR_TIME_MSEC / 1000)*/;
                     }

                    if(withOutPorogs){
                        for1Thread.append(QVector<double>({val}));
//                       if(maxY)
                    } else {
                        double v1 = 0, v2 = 0, v3 = 0;
                        if(maxY < porog2) maxY = porog2;
                        v1 = val;
                        if(val > porog1){ v1 = porog1; v2 = val - porog1; if(maxY < v2) maxY = v2;}
                        if(val > porog2){ v2 = porog2; v3 = val - porog2; if(maxY < v3) maxY = v3;}

                        for1Thread.append(QVector<double>({v1, v2, v3}));
                    }

                }
                double val_all = 0;
                if(type == countInputInSec){
                    val_all = summ_all * 1000;
                } else if(type == percentConcentrate2Input){
                    if(summ_all > 1e-6){
                        val_all = (summ_con / summ_all) * 100;
                    }
                }
                if(withOutPorogs){
                    for1Thread.append(QVector<double>({val_all}));
                } else {
                    double v1 = 0, v2 = 0, v3 = 0;
                    if(maxY < porog2) maxY = porog2;
                    v1 = val_all;
                    if(val_all > porog1){ v1 = porog1; v2 = val_all - porog1; if(maxY < v2) maxY = v2;}
                    if(val_all > porog2){ v2 = porog2; v3 = val_all - porog2; if(maxY < v3) maxY = v3;}

                    for1Thread.append(QVector<double>({v1, v2, v3}));
                }
            } else {
                double rval = 0;
                double sval_all = 0, sval_con = 0;
                for(int th=0; th<avr->size(); th++){
                    double val_all = (*last)[th].data.i_prd[2],
                            val_con = (*last)[th].data.i_prd[0];
                    sval_all += val_all; sval_con += val_con;
                }
                if(type == percentConcentrate2Input){
                    if(sval_all > 1e-6){
                        rval = sval_con / sval_all * 100;
                    }
                } else if(type == countInputInSec){
                    rval = sval_all * 1000;
                }
                for1Thread.append(QVector<double>({rval}));
            }
            series.append(for1Thread);
            chart->setSamples(series);
            if(type == countInputInSec && !withOutPorogs){
                setAxisScale(QwtPlot::yLeft, 0, maxY);
            }
            if(drawTitle){
                drawTitle->redrawTitles();
            }
//            this->replot();
        }
        return;
    }
}

SPRSeparateSummaryGistogramm::DrawTitle::DrawTitle(bool _allThreads, SPRHistoryModel2 *_model, SPRWorkSeparateTypeData _type)
{
    withThreads = _allThreads;
    model = _model;
    type = _type;
}

QwtText SPRSeparateSummaryGistogramm::DrawTitle::label(double v) const
{
    int th = static_cast<int>( v );
    double value;
    QString titleValue = "";
    if(model){
        QVector<SPRHistoryModel2::histDataOneThread> *last = model->getLastRecords();
        QVector<SPRHistoryModel2::histDataOneThread> *avr = model->getAvr10mRecords();

        if(th >= last->size()){
            double summ_all = 0; double summ_con = 0;
            if(type == percentConcentrate2Input){
                for(int t=0; t<last->size(); t++){
                    summ_all += (*last)[t].data.i_prd[2];
                    summ_con += (*last)[t].data.i_prd[0];
                }
                if(summ_all > 1e-6){
                    value = (summ_con / summ_all) * 100;
                } else {
                    value = 0;
                }
                titleValue = tr(" (%1%)").arg(QString::number(value,'f', 2));
            } else if(type == countInputInSec){
                for(int t=0; t<avr->size(); t++){
                    summ_all += (*avr)[t].data.i_prd[2];
                }
                value = summ_all * 1000;
                titleValue = tr(" (%1 к/c)").arg(QString::number(value,'f', 1));
            }

        } else{
            if(type == percentConcentrate2Input){
                if((*last)[th].data.i_prd[2] > 1e-6){
                    value = ((*last)[th].data.i_prd[0] / (*last)[th].data.i_prd[2]) * 100;
                } else {
                    value = 0;
                }
                titleValue = tr(" (%1%)").arg(QString::number(value,'f', 2));
            } else if(type == countInputInSec){
                value = (*avr)[th].data.i_prd[2] * 1000;
                titleValue = tr(" (%1 к/c)").arg(QString::number(value,'f', 1));
            }
        }
    }
    QwtText txt;
    if(withThreads){
        if(model && th < model->getLastRecords()->size()){
            txt = QwtText(tr("%1%2").arg(th+1).arg(titleValue));
        } else {
            txt = QwtText(tr("%1%2").arg("все").arg(titleValue));
        }
    } else {
        txt = QwtText(tr("все ручьи"));
    }
    QFont sys = QFont("Tahoma", 8);
    txt.setFont(sys);
    return txt;
}
