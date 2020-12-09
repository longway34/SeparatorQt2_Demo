#include "sprsettingsimswidget.h"
#include "models/sprmainmodel.h"
#include "models/sprhistorymodel2.h"

#include "qwt_legend.h"
#include "qwt_legend_label.h"

#include <QMessageBox>

#include "qwt_symbol.h"

ISPRModelData *SPRSettingsIMSWidget::getModelData()
{
    return model;
}

ISPRModelData *SPRSettingsIMSWidget::setModelData(SPRSettingsIMSModel *value)
{
    if(value){
        if(model){
            disconnect(model, SIGNAL(modelChanget(IModelVariable*)), this, SLOT(onModelChanget(IModelVariable*)));
        }
        model = value;
        connect(model, SIGNAL(modelChanget(IModelVariable*)), this, SLOT(onModelChanget(IModelVariable*)));

        mainModel = model->getMainModel();
        if(mainModel){
            historyModel = mainModel->getHistoryModel2();
            if(historyModel){
                connect(historyModel, &SPRHistoryModel2::separateDataReady, this, &SPRSettingsIMSWidget::onSeparateDataReady);
            }
        }

//        connect(ui.cbWithMaxMaxStone, SIGNAL(clicked(bool)), this, SLOT(viewChange(bool)));
        connect(ui.leBlockParam, SIGNAL(editingFinished()), this, SLOT(viewChange()));
//        connect(ui.leKoefA, SIGNAL(editingFinished()), this, SLOT(viewChange()));
//        connect(ui.leKoefB, SIGNAL(editingFinished()), this, SLOT(viewChange()));
        connect(ui.leLimitAbrasion, SIGNAL(editingFinished()), this, SLOT(viewChange()));

        QColor colors[] = { QColor(Qt::gray), QColor(Qt::blue)};

        ui.plotStatistuka->setAxisScale(QwtPlot::yLeft, 0, 100);
        ui.plotStatistuka->setAxisTitle(QwtPlot::yLeft, tr("Износ ИМ, %"));
//        ui.plotStatistuka->setAxisScale(QwtPlot::yRight, 0, model->limitAbrasion->getData());
//        ui.plotStatistuka->setAxisTitle(QwtPlot::yRight, tr("Кол-во срабатываний"));
        DrawTitle *scaleDraw = new DrawTitle(true);
        scaleDraw->enableComponent(QwtScaleDraw::Backbone, false);
        scaleDraw->enableComponent(QwtScaleDraw::Ticks, false);

        ui.plotStatistuka->setAxisScaleDraw(QwtPlot::xBottom, scaleDraw);

        ui.plotStatistuka->setAutoReplot(true);

        statisticPicter = new StaticticBarPicter(QwtPlot::xBottom, QwtPlot::yLeft, QwtPlotPicker::NoRubberBand, QwtPlotPicker::AlwaysOn, ui.plotStatistuka->canvas());
        statisticPicter->setParams(&model->abrasions, model->limitAbrasion);

        bars = new QwtPlotMultiBarChart(tr("Износ ИМ, (%)"));

        bars->attach(ui.plotStatistuka);

        connect(ui.bClearStatistic, &QPushButton::clicked, this, &SPRSettingsIMSWidget::onClearStatisticButtonClicked);

        widgetsShow();
    }
    return model;

}

void SPRSettingsIMSWidget::onSeparateDataReady(){

}

void SPRSettingsIMSWidget::onClearStatisticButtonClicked(bool){
    if(model){
        QList<QCheckBox*> chThreads = ui.gbStatistic->findChildren<QCheckBox*>();
        if(chThreads.size() > 0){

            int c = 0;
            QString toQuest;
            foreach(QCheckBox *tch, chThreads){
                if(tch->isChecked()){
                    if(c > 0){
                        toQuest += tr(", ");
                    }
                    toQuest += tr("%1").arg(tch->property("th").value<int>()+1) ;
                    c++;
                }
            }
            if(chThreads.size() > 1){
                toQuest = QString(tr(" ручьев (%1) ")).arg(toQuest);
            } else {
                toQuest = QString(tr(" ручья %1 ")).arg(toQuest);
            }

            int res = QMessageBox::question(nullptr, tr("Сброс статистики"),
                                    tr("Вы действительно хотите очистить статистику работы ИМ %1?")
                                    .arg(toQuest), QMessageBox::StandardButton::Yes, QMessageBox::StandardButton::No);

            if(res == QMessageBox::StandardButton::Yes){
                QList<int> threads;
                foreach(QCheckBox *tch, chThreads){
                    int th = tch->property("th").value<int>();
                    if(th >= 0 && th < model->getIms()->getData()){
                        if(tch->isChecked()){
                            threads.append(th);
                        }
                    }
                }
                model->clearAbrasions(threads);
            }
        }
    }
}

void SPRSettingsIMSWidget::widgetsShow()
{
    if(model){
        foreach(QWidget* w, forBlockSignalsWidget){
            w->blockSignals(true);
        }

        QPushButton *bClear = ui.bClearStatistic;
        QList<QCheckBox*> lcb = ui.wStatistic->findChildren<QCheckBox*>();
        QGridLayout *gridOut = qobject_cast<QGridLayout*>(ui.wStatistic->layout());
        QList<int> oldChacked;
        if(gridOut){
            foreach(QCheckBox *cb, lcb){
                int th = cb->property("th").value<int>();
                if(th >= 0 && th < model->getIms()->getData()){
                    if(cb->isChecked()){
                        oldChacked.append(th);
                    }
                }
                gridOut->removeWidget(cb);
                forBlockSignalsWidget.removeAll(cb);
                delete cb;
            }
            int r = 0; int c = 0;
            for(int i=0; i < model->getIms()->getData();i++){
                r = i / 4; c = i % 4;
                QCheckBox *cb = new QCheckBox(tr("Ручей %1").arg(i+1));
                if(oldChacked.contains(i)){
                    cb->setChecked(true);
                } else {
                    cb->setChecked(false);
                }
                cb->setProperty("th", QVariant(i));
                forBlockSignalsWidget.append(cb);
                gridOut->addWidget(cb, r, c, 1, 1);
//                c++;
            }
            r++;
            gridOut->addWidget(bClear, r, 0, 1, 2);
        }
        repaintGraphicSetts(0);

        for(int row=0; row<ui.tMinMaxParams->rowCount(); row++){
            for(int col=0; col<ui.tMinMaxParams->columnCount();col++){
                QLineEdit *le = qobject_cast<QLineEdit*>(ui.tMinMaxParams->cellWidget(row, col));
                if(le){
                    delete le;
                }
            }
        }


        ui.tMinMaxParams->clear();
        ui.tMinMaxParams->setRowCount(3);
        QStringList vHeaderTitle = {
            tr("Время измерения куска (мс)"),
            tr("Время задержки на срабатывания ИМ (мс)"),
            tr("Длительность импульса ИМ (мс)")
        };
        QStringList hHeaderTitle = {
            tr("Мин. кусок"), tr("Макс. кусок"), tr("Макс. камень")
        };
        ui.tMinMaxParams->setVerticalHeaderLabels(vHeaderTitle);
        ui.tMinMaxParams->verticalHeader()->sectionResizeMode(QHeaderView::ResizeMode::Stretch);
        ui.tMinMaxParams->setColumnCount(3);


        QLineEdit *le = setNumberCell(ui.tMinMaxParams, 0, 0, model->tMeteringMinStone->getData(), 0, 1000, tr("Время измерения для минимального куска в миллисекундах"));
        connect(le, SIGNAL(editingFinished()), this, SLOT(viewChange()));
        le = setNumberCell(ui.tMinMaxParams, 0, 1, model->tMeteringMaxStone->getData(), 0, 1000, tr("Время измерения для максимального куска в миллисекундах"));
        connect(le, SIGNAL(editingFinished()), this, SLOT(viewChange()));
        le = setNumberCell(ui.tMinMaxParams, 0, 2, model->tMeteringMaxMaxStone->getData(), 0, 1000, tr("Время измерения для максимального камня (бута) в миллисекундах"));
        connect(le, SIGNAL(editingFinished()), this, SLOT(viewChange()));

        le = setNumberCell(ui.tMinMaxParams, 1, 0, model->tDelayMinStone->getData(), 0, 1000, tr("Время задержки на срабатывания ИМ для минимального куска в миллисекундах"));
        connect(le, SIGNAL(editingFinished()), this, SLOT(viewChange()));
        le = setNumberCell(ui.tMinMaxParams, 1, 1, model->tDelayMaxStone->getData(), 0, 1000, tr("Время задержки на срабатывания ИМ для максимального куска в миллисекундах"));
        connect(le, SIGNAL(editingFinished()), this, SLOT(viewChange()));
        le = setNumberCell(ui.tMinMaxParams, 1, 2, model->tDelayMaxMaxStone->getData(), 0, 1000, tr("Время задержки на срабатывания ИМ для максимального камня (бута) в миллисекундах"));
        connect(le, SIGNAL(editingFinished()), this, SLOT(viewChange()));

        le = setNumberCell(ui.tMinMaxParams, 2, 0, model->tDurationMinStone->getData(), 0, 1000, tr("Длительность импульса ИМ для минимального куска в миллисекундах"));
        connect(le, SIGNAL(editingFinished()), this, SLOT(viewChange()));
        le = setNumberCell(ui.tMinMaxParams, 2, 1, model->tDurationMaxStone->getData(), 0, 1000, tr("Длительность импульса ИМ для максимального куска в миллисекундах"));
        connect(le, SIGNAL(editingFinished()), this, SLOT(viewChange()));
        le = setNumberCell(ui.tMinMaxParams, 2, 2, model->tDurationMaxMaxStone->getData(), 0, 1000, tr("Длительность импульса ИМ для максимального камня (бута) в миллисекундах"));
        connect(le, SIGNAL(editingFinished()), this, SLOT(viewChange()));
        if(model->withMaxMaxStone->getData()){
            ui.tMinMaxParams->showColumn(2);
        } else {
            ui.tMinMaxParams->hideColumn(2);
        }

        ui.tMinMaxParams->setHorizontalHeaderLabels(hHeaderTitle);
        ui.tMinMaxParams->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::Stretch);

        ui.tMinMaxParams->resizeColumnsToContents();
        ui.tMinMaxParams->resizeRowsToContents();

//        ui.cbWithMaxMaxStone->setChecked(model->withMaxMaxStone->getData());
        ui.leBlockParam->setValue(model->blockImsParam->getData());
//        ui.leKoefA->setValue(model->kSpeedOreA->getData());
//        ui.leKoefB->setValue(model->kSpeedOreB->getData());
        ui.leLimitAbrasion->setValue(model->limitAbrasion->getData());

        foreach(QWidget* w, forBlockSignalsWidget){
            w->blockSignals(false);
        }
    }
}

void SPRSettingsIMSWidget::repaintGraphicSetts(double)
{
    double grAdd = (model->timesMettering[model->timesMettering.size()-1]->getData()/2) * 1.05;
    QVector<QPointF> grDataRed = {
        {0, (double)model->tDelayMinStone->getData()},
        {(double)model->timesMettering[0]->getData(), (double)model->tDelayMinStone->getData()},
        {(double)model->timesMettering[model->timesMettering.size()-1]->getData()/2, (double)model->tDelayMaxStone->getData()},
        {grAdd, (double)model->tDelayMaxStone->getData()}
    };

    QVector<QPointF> grDataGreen = {
        {0, (double)model->tDurationMinStone->getData()},
        {(double)model->timesMettering[0]->getData(),(double)model->tDurationMinStone->getData()},
        {(double)model->timesMettering[model->timesMettering.size()-1]->getData()/2, (double)model->tDurationMaxStone->getData()},
        {grAdd, (double)model->tDurationMaxStone->getData()}
    };

    ui.plotParams->setAxisScale(QwtPlot::xBottom, 0, grAdd);
//    QList<QwtLegendLabel*> legendEntries = static_cast<QwtLegend*>(ui.plotParams->legend())->contentsWidget()->findChildren<QwtLegendLabel*>(QString(), Qt::FindDirectChildrenOnly);

//    legendEntries[0]->setText(tr("Задер. (%1)").arg(model->getFormulaString(true)));
    curveRed->setSamples(grDataRed);
    curveRed->setTitle(tr("Задер. (%1)").arg(model->getFormulaString(true)));
//    QVariant infoRed = ui.plotParams->itemToInfo(curveRed);
//    QwtLegendData lDataRed;
//    QVariant lTitleRed; lTitleRed.setValue<QwtText>(curveRed->title());
//    QIcon lIconRed = infoRed.
    if(model){
      QString str = tr("Задер. (%1)\nДлит. (%2)").arg(model->getFormulaString(true)).arg(model->getFormulaString(false));
      formulasText->setText(QwtText(str));
    }
    ;
//    lDataRed.setValue(QwtLegendData::TitleRole, lTitleRed);
//    lDataRed.setValue(QwtLegendData::IconRole, legendEntries[0]->icon());
//    ui.plotParams->legend()->updateLegend(infoRed, QList<QwtLegendData>({lDataRed}));
//    curveRed->set


    curveGreen->setSamples(grDataGreen);
    curveGreen->setTitle(tr("Длит. (%1)").arg(model->getFormulaString(false)));
//    legendEntries[1]->setText(tr("Длит. (%1)").arg(model->getFormulaString(false)));
//    QVariant infoGreen = ui.plotParams->itemToInfo(curveGreen);
//    QwtLegendData lDataGreen;
//    QVariant lTitleGreen; lTitleGreen.setValue<QwtText>(curveGreen->title());

//    lDataRed.setValue(QwtLegendData::TitleRole, lTitleRed);
//    ui.plotParams->legend()->updateLegend(infoRed, QList<QwtLegendData>({lDataRed}));


//    ui.plotParams->replot();
//    ui.plotParams->legend()->updateLegend();

    QVector<QVector<double>> grDataYellow(model->getIms()->getData(), QVector<double>(1,0));
    ui.plotStatistuka->setAxisScale(QwtPlot::xBottom, 0, model->getIms()->getData()-1, 1.0);

    for(int i=0; i < model->getIms()->getData(); i++) {
        if(model->limitAbrasion->getData() > 0){
            qulonglong d = model->abrasions[i]->getData();
            double r = (static_cast<double>(model->abrasions[i]->getData()) / static_cast<double>(model->limitAbrasion->getData())) * 100.0;
            grDataYellow[i] = QVector<double>({r});
        }
//        grDataYellow[i] = model->limitAbrasion->getData();
        qlonglong v = model->limitAbrasion->getData();
    }
    bars->setSamples(grDataYellow);

//    ui.plotStatistuka->replot();

}


SPRSettingsIMSWidget::SPRSettingsIMSWidget(QWidget *parent) :
    QWidget(parent), model(nullptr), historyModel(nullptr), mainModel(nullptr)
{
    ui.setupUi(this);

    forBlockSignalsWidget = findChildren<QWidget*>();

    model = nullptr;

    curveRed = new QwtPlotCurve(tr("Задер."));
    curveRed->setPen(QColor(Qt::red), 2, Qt::SolidLine);
    curveRed->attach(ui.plotParams);

    QwtText titleRed("Задер.\nДлит.");
    titleRed.setRenderFlags( Qt::AlignHCenter | Qt::AlignTop );

    formulasText = new QwtPlotTextLabel();
    formulasText->setText(titleRed);
//    formulasText->textRect(QRect(QPoint(5,5),QSize(ui.plotParams->canvas()->size())),QSize(ui.plotParams->canvas()->size()));
    formulasText->attach(ui.plotParams);

    curveGreen = new QwtPlotCurve(tr("Длит."));
    curveGreen->setPen(QColor(Qt::green), 2, Qt::SolidLine);
    curveGreen->attach(ui.plotParams);

    grid = new QwtPlotGrid();
    grid->enableXMin(true);
    grid->setMajorPen(QColor(Qt::black), 0.1, Qt::DotLine);
    grid->setMinorPen(QColor(Qt::gray), 0.1, Qt::DotLine);
    grid->attach(ui.plotParams);


    legend = new QwtLegend();
    ui.plotParams->insertLegend(legend, QwtPlot::TopLegend);

    pickter = new QwtPlotPicker(QwtPlot::xBottom, QwtPlot::yLeft,
                                QwtPicker::RectRubberBand,
                                QwtPicker::ActiveOnly, ui.plotParams->canvas());
}



void SPRSettingsIMSWidget::onMouseMoved(QPointF point)
{
    int x = 0;
}

void SPRSettingsIMSWidget::viewChange()
{
    if(sender() == ui.leBlockParam){
        model->blockImsParam->setData(ui.leBlockParam->value());
        return;
    }
    if(sender() == ui.leLimitAbrasion){
        model->limitAbrasion->setData(ui.leLimitAbrasion->value());
        return;
    }
    QObject *tw = sender()->property("tw").value<QObject*>();
    int row = sender()->property("row").toInt();
    int col = sender()->property("col").toInt();

    if(tw == ui.tMinMaxParams){
        QLineEdit *le = (QLineEdit*)sender();
        if(row == 0){
            if(col == 0){
                model->tMeteringMinStone->setData(le->text().toInt());
                if(model->tMeteringMinStone->getData() > model->tMeteringMaxStone->getData()){
                    model->tMeteringMaxStone->setData(model->tMeteringMinStone->getData());
                }
            }
            if(col == 1){
                model->tMeteringMaxStone->setData(le->text().toInt());
                if(model->tMeteringMinStone->getData() > model->tMeteringMaxStone->getData()){
                    model->tDurationMinStone->setData(model->tMeteringMinStone->getData());
                }
            }
            if(col == 2){
                model->tMeteringMaxMaxStone->setData(le->text().toInt());
            }
        }
        if(row == 1){
            if(col == 0){
                model->tDelayMinStone->setData(le->text().toInt());
            }
            if(col == 1){
                model->tDelayMaxStone->setData(le->text().toInt());
            }
            if(col == 2){
                model->tDelayMaxMaxStone->setData(le->text().toInt());
            }
        }
        if(row == 2){
            if(col == 0){
                model->tDurationMinStone->setData(le->text().toInt());
            }
            if(col == 1){
                model->tDurationMaxStone->setData(le->text().toInt());
            }
            if(col == 2){
                model->tDurationMaxMaxStone->setData(le->text().toInt());
            }
        }
        model->setTimesMeassureDelayDuration();
//        emit doShow();

        repaintGraphicSetts(0);
    }
}

void SPRSettingsIMSWidget::viewChange(bool val)
{
}



void SPRSettingsIMSWidget::onModelChanget(IModelVariable *)
{
    if(sender() == model){
        widgetsShow();
        return;
    }

    if(sender() == historyModel){
    }

    QList<QObject*> mySenders = {
    };
//    SPRMainModel *mainModel = model->getMainModel();

}


void SPRSettingsIMSWidget::setMasterMode(bool value)
{
    this->setEnabled(value);
}
/**
 * @brief SPRSettingsIMSWidget::DrawTitle::DrawTitle
 * @param _allThreads
 */
SPRSettingsIMSWidget::DrawTitle::DrawTitle(bool _allThreads)
{
    withThreads = _allThreads;
}

QwtText SPRSettingsIMSWidget::DrawTitle::label(double v) const
{
    int th = static_cast<int>( v );
    QwtText txt;
    if(withThreads){
        txt = QwtText(tr(" Ручей %1 ").arg(th+1));
    } else {
        txt = QwtText(tr("все ручьи"));
    }
    QFont sys = QFont("Tahoma", 8);
    txt.setFont(sys);
    return txt;

}


void SPRSettingsIMSWidget::StaticticBarPicter::setParams(QVector<SPRVariable<qulonglong> *> *_data, SPRVariable<qulonglong> *_limit)
{
    data = _data; limit = _limit;
}

QwtText SPRSettingsIMSWidget::StaticticBarPicter::trackerTextF(const QPointF &p) const
{
    QColor bg( Qt::white );
    bg.setAlpha( 200 );

    int x = round(p.x());
    if(x < 0) x = 0;
    if(data && limit){
        if(data->size() > 0 ){
        if(x >= data->size()) x = data->size()-1;
            qulonglong value = data->at(x)->getData();
            double pr = static_cast<double>(value) / static_cast<double>(limit->getData()) * 100;
            QString tt = tr("Ручей %1 - %2%(%3)").arg(x+1).arg(QString::number(pr, 'f', 2)).arg(value);
            QwtText ret = QwtText(tt);
            ret.setBackgroundBrush(QBrush(bg));
            return ret;
        }
    }
//    qDebug() << "point: " << p << "; x=" << x << "; p1=" << p1 << "; p2=" << p2;

    return QwtPlotPicker::trackerTextF(p);
}
