#include "sprseparatewidget.h"
#include "models/sprmainmodel.h"

SPRSeparateWidget::SPRSeparateWidget(QWidget *parent) :
    QWidget(parent), history(nullptr), wPorogs(nullptr), kSpectrumModel(nullptr)/*, startSeparate(nullptr), stopSeparate(nullptr), separatorOn(nullptr)*/
{
    ui.setupUi(this);

    toWidget = new TCPTimeOutWidget();
    toWidget->setVisible(false);

//    stopSeparate = new TCPTestStopSeparate(toWidget);
//    connect(stopSeparate, SIGNAL(commandComplite(TCPCommand*)), this, SLOT(onCommandComplite(TCPCommand*)));

    connect(ui.bChangePorogs, SIGNAL(clicked(bool)), this, SLOT(onChangePororsCommand(bool)));

//    connect(ui.bSeparateStart, SIGNAL(clicked(bool)), this, SLOT(onCommandButtomClick(bool)));
//    connect(ui.bSeparateStop, SIGNAL(clicked(bool)), this, SLOT(onCommandButtomClick(bool)));
//    connect(ui.bSeparatorOn, SIGNAL(clicked(bool)), this, SLOT(onCommandButtomClick(bool)));
//    connect(ui.bSeparatorOff, SIGNAL(clicked(bool)), this, SLOT(onCommandButtomClick(bool)));

    ui.gKSpectrums->getCanvas()->setAxisScale(QwtPlot::Axis::xBottom, 0, 256, 25);
//    ui.gKSpectrums->getCanvas()->setAxisMaxMinor(QwtPlot::xBottom, 10);
    connect(ui.gKSpectrums->getPorogsMoved(), SIGNAL(dblClickMouseEvent()), this, SLOT(onDblClickMouseEvent()));
    ui.gKSpectrums->setWithLegend(true);

    ui.spLeftRigth->setStretchFactor(0,1);
    ui.spLeftRigth->setStretchFactor(1,1);
    ui.spLeftRigth->setSizes(QList<int>() << 600 << 500);

    ui.spGraphics->setStretchFactor(0,1);
    ui.spGraphics->setStretchFactor(1,1);
    ui.spGraphics->setSizes(QList<int>() << 6500 << 3500);

    ui.spMaintable->setStretchFactor(1,0);
    ui.spMaintable->setStretchFactor(1,1);
    ui.spMaintable->setSizes(QList<int>() << 6500 << 3500);

//    ui.spGraphic->setSizes(QList<int>()<<50<<20);

}

void SPRSeparateWidget::setGraphicTitle(){
    QString titlePref = QString(tr("Спектры (%1)"));
    int thr = ui.gKSpectrums->getCurrentThread();
    QString titleSuff = thr < 0 ? QString(tr("Все ручьи")) : QString(tr("Ручей %1")).arg(thr+1);
    QString title = titlePref.arg(titleSuff);

    ui.gKSpectrums->getCanvas()->setTitle(title);
}

void SPRSeparateWidget::onDblClickMouseEvent(){
    setGraphicTitle();
    int thr = ui.gKSpectrums->getCurrentThread();
    SPRGraphItem *gr = nullptr;
    if(thr >= 0){
        if(kSpectrumModel){
            QList<SPRSpectrumItemModel*> lst = kSpectrumModel->getSpectrumsItemByThread(thr, spectrumsOnly);
            if(lst.size() > 0){
                 gr = lst.last()->getGraphic();
            }
        }
    }
    ui.gKSpectrums->setCurrentItem(gr);

}

extern TCPCommand *restartsep;

void SPRSeparateWidget::onCommandButtomClick(bool){
    if(model){
    }

}

void SPRSeparateWidget::onServerStateChange(uint32_t){
    if(model){
        if(!model->getServer()->isState(spr_state_server_connect) /*|| model->getServer()->isState(spr_state_separated)*/){
            this->setEnabled(false);
            return;
        } else {
            this->setEnabled(true);
        }
//        if(model->getServer()->isState(spr_state_separated)){
//            if(history){
//                history->clear();
//            }
//        }
    }
}

void SPRSeparateWidget::widgetsShow()
{
}

ISPRModelData *SPRSeparateWidget::setModelData(ISPRModelData *data)
{
    model = data->getMainModel();
    if(model){

        connect(model->getServer(), SIGNAL(serverStateChangeOut(uint32_t)), this, SLOT(onServerStateChange(uint32_t)));

        ui.tSeparateDetail->setModelData(model);
        ui.tSeparateDetail->widgetsShow();

        ui.gGistogramm->setModelData(model);
        ui.gGistogramm->widgetsShow();

        kSpectrumModel = model->getKSpectrums();

        ui.gKSpectrums->setModelData(kSpectrumModel, spectrumsOnly, false, true);
        ui.gKSpectrums->setAllCurrent(true);

        if(wPorogs) delete wPorogs;
        wPorogs = new SPRPorogsWidget2();
        wPorogs->setModelData(model->getSettingsPorogsModel());
        wPorogs->setWidgetMode(forSeparate);
        wPorogs->setWindowTitle(tr("Изменение порогов сортировки"));

        ui.wControlPitatel->setModelData(model);
        ui.wControlPitatel->setFullMode(true, true);
        ui.wControlPitatel->setVerticalMode(true);

        ui.wSeparateControl->setModelData(model, toWidget,  nullptr);

        history = model->getHistoryModel();
        connect(history, &SPRHistoryModel::modelChanget, this, &SPRSeparateWidget::onModelChanget);

        ui.gHistoryIn->init({}, model->getHistoryModel2(), countInputInTime, tr("Подача руды, к./сек. ( %1 )"), /*60*60*2*/ 600);
        ui.gHistoryOut->init({}, model->getHistoryModel2(), percentConcentrate2Input, tr("Выход продукта,% ( %1 )"), /*60*60*2*/ 600);
//        ui.gHistoryIn->init({}, model->getHistoryModel2(), countInputInTime, tr("Подача руды ( %1 )"), /*60*60*2*/ 600);
//        ui.gHistoryOut->init({}, model->getHistoryModel2(), percentConcentrate2Input, tr("Выход продукта,% ( %1 )"), /*60*60*2*/ 600);
        ui.gHistoryOut->setAxisScale(QwtPlot::xBottom, 0, 100, 20);

        SPRThreadList trs = model->getThreadsList();

//        ui.gMainInput->setModelData(model, countInputInSec, false);
//        ui.gMainOutput->setModelData(model, history, percentConcentrate2Input, false);
        ui.gMainInput->setModelData(model, countInputInSec, false);
        ui.gMainOutput->setModelData(model, percentConcentrate2Input, false);

        ui.tMainInfo->init(model, model->getHistoryModel2(), kSpectrumModel);

        ui.gGrandsostav->setModels(model, model->getHistoryModel2(), model->getSeparateModel());

        onServerStateChange(model->getServer()->getState());
    }
}

void SPRSeparateWidget::onCommandComplite(TCPCommand *command){

}

void SPRSeparateWidget::onSpectrumReady(TCPGetSpectrumsGistogramms *_command){
}

void SPRSeparateWidget::onSeparateCommandComplite(TCPCommand* command){
    if(model){
    }
}

void SPRSeparateWidget::onChangePororsCommand(bool){
    if(wPorogs){
        wPorogs->setWindowModality(Qt::WindowModal);
        wPorogs->show();
    }
}

bool SPRSeparateWidget::isStartStopProcess(){
    bool res = ui.wControlPitatel->isStartStopProcess();
    res = res || ui.wSeparateControl->isStartStopProcess();
    return res;
}

ISPRModelData *SPRSeparateWidget::getModelData()
{
    return model;
}

SPRPitatelControlWidget *SPRSeparateWidget::getPitagelControl()
{
    return ui.wControlPitatel;
}

SPRSeparateControlWidget *SPRSeparateWidget::getSeparateControl()
{
    return ui.wSeparateControl;
}

void SPRSeparateWidget::onModelChanget(IModelVariable *)
{
    if(sender() == history){
        QVector<QVector<double>> lastAbrasions = history->getMTableCounts();
    }
}


void SPRSeparateWidget::setLogModel(SPRSeparatorLogsModel *value)
{
    logModel = value;
//    ISPRWidget::setLogModel(value);
    ui.wSeparateControl->setLogModel(logModel);

}


void SPRSeparateWidget::storeWidgetsStates(QString groupId, QSettings *mainSettings)
{
    mainSettings->beginGroup(groupId);
    mainSettings->setValue(objectName()+"."+ui.spGraphics->objectName(), ui.spGraphics->saveState());
    mainSettings->setValue(objectName()+"."+ui.spLeftRigth->objectName(), ui.spLeftRigth->saveState());
    mainSettings->setValue(objectName()+"."+ui.spMaintable->objectName(), ui.spMaintable->saveState());
    mainSettings->endGroup();
    ui.tSeparateDetail->storeWidgetsStates(groupId, mainSettings);
}

void SPRSeparateWidget::restoreWidgetsStates(QString groupId, QSettings *mainSettings)
{
    mainSettings->beginGroup(groupId);
    ui.spGraphics->restoreState(mainSettings->value(objectName()+"."+ui.spGraphics->objectName()).toByteArray());
    ui.spLeftRigth->restoreState(mainSettings->value(objectName()+"."+ui.spLeftRigth->objectName()).toByteArray());
    ui.spMaintable->restoreState(mainSettings->value(objectName()+"."+ui.spMaintable->objectName()).toByteArray());
    mainSettings->endGroup();
    ui.tSeparateDetail->restoreWidgetsStates(groupId, mainSettings);
}
