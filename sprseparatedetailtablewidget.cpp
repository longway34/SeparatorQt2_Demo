#include "sprseparatedetailtablewidget.h"

SPRSeparateDetailTableWidget::SPRSeparateDetailTableWidget(QWidget *parent) :
    QWidget(parent), model(nullptr), separateModel(nullptr)
{
    ui.setupUi(this);

    connect(ui.bBeginScope, SIGNAL(clicked(bool)), this, SLOT(onClickButtomCommand(bool)));
    connect(ui.bEndScope, SIGNAL(clicked(bool)), this, SLOT(onClickButtomCommand(bool)));
    connect(ui.bClear, SIGNAL(clicked(bool)), this, SLOT(onClickButtomCommand(bool)));
}

SPRThreadList SPRSeparateDetailTableWidget::getVisbleThreads(){
    SPRThreadList res;
    QList<QCheckBox*> cbs = ui.gbThreads->findChildren<QCheckBox*>();
    for(uint8_t i=0; i<cbs.size(); i++){
        if(cbs[i]->isChecked()){
            if(cbs[i]->property("index").isValid()){
                res.push_back(cbs[i]->property("index").value<int>());
            } else {
                res.push_back(i);
            }
        }
    }
    return res;
}

void SPRSeparateDetailTableWidget::widgetsShow()
{
    if(model){
//        QSpacerItem *spacer = ui.gbThreads->findChild("hsTreads");
        QList<QCheckBox*> cbs = ui.gbThreads->findChildren<QCheckBox*>();
        QList<bool> oldThreadList;

        foreach(QCheckBox *th, cbs){
            if(th->isChecked()) oldThreadList.push_back(true); else oldThreadList.push_back(false);
            ui.gbThreads->layout()->removeWidget(th);
            delete th;
        }
        ui.gbThreads->layout()->removeItem(ui.hsTreads);
        blockSignals(true);
        for(uint8_t i=0; i<model->getThreads()->getData(); i++){
           QCheckBox *th = new QCheckBox(QString(tr("Ручей %1")).arg(QString::number(i+1)));
           th->setObjectName(QString("cb_%1").arg(QString::number(i+1)));
           th->setProperty("index", QVariant(i));
           if(i < oldThreadList.size()){
                th->setChecked(oldThreadList[i]);
           }
           if(model->getServer()->isState(spr_state_separated)){
               th->setEnabled(true);
           } else {
               th->setEnabled(false);
           }
           connect(th, SIGNAL(toggled(bool)), this, SLOT(onChangeThreadList(bool)));
           ui.gbThreads->layout()->addWidget(th);
        }
        blockSignals(true);
        ui.gbThreads->layout()->addItem(ui.hsTreads);

        onChandeServerState(0);
    }

//    ui.tSeparateDetail->widgetsShow();
}

void SPRSeparateDetailTableWidget::onChandeServerState(uint32_t){
    if(model){
        QList<QCheckBox*> cbs = ui.gbThreads->findChildren<QCheckBox*>();
        foreach(QCheckBox *cb, cbs){
            if(model->getServer()->isState(spr_state_separated)){
                cb->setEnabled(true);
            } else {
                cb->setEnabled(false);
            }
        }
        if(model->getServer()->isState(spr_state_separated)){
            if(ui.tSeparateDetail->isStartingScope()){
                ui.bBeginScope->setEnabled(true);
                ui.bEndScope->setEnabled(true);
            } else {
                ui.bBeginScope->setEnabled(true);
                ui.bEndScope->setEnabled(false);
            }
            ui.leCommandTimeInterval->setEnabled(true);
            ui.leMinimumTimeScope->setEnabled(true);
        } else {
            ui.bBeginScope->setEnabled(false);
            ui.bEndScope->setEnabled(false);
            ui.leCommandTimeInterval->setEnabled(false);
            ui.leMinimumTimeScope->setEnabled(false);
        }
    }
}

void SPRSeparateDetailTableWidget::onClickButtomCommand(bool){
    if(sender() == ui.bBeginScope){
        if(model){
            ui.tSeparateDetail->setMinStoneTime(ui.leMinimumTimeScope->value());
            ui.tSeparateDetail->startStopScope(true);
            ui.tSeparateDetail->setVisibleThreads(getVisbleThreads());
//            ui.tSeparateDetail->getMyModel()->setScopeData(0);

            ui.tSeparateDetailSummary->setVisibleThreads(getVisbleThreads());
            ui.tSeparateDetailSummary->setMinStoneTime(ui.leMinimumTimeScope->value());
            ui.tSeparateDetailSummary->startStopScope(true);

            ui.bEndScope->setEnabled(true);
            model->getSettingsControlModel()->tMeassureForData->setData(ui.leCommandTimeInterval->value());
        }
        return;
    }
    if(sender() == ui.bEndScope){
        if(model){
            ui.tSeparateDetail->startStopScope(false);
            ui.tSeparateDetailSummary->startStopScope(false);
            model->getSettingsControlModel()->tMeassureForData->restore();

            ui.bEndScope->setEnabled(false);
        }
        return;
    }
    if(sender() == ui.bClear){
        if(model){
            ui.tSeparateDetail->clear();
            ui.tSeparateDetailSummary->clear();
        }
        return;
    }
    QList<QCheckBox*> cbs = ui.gbThreads->findChildren<QCheckBox*>();
    QCheckBox *cbSender = qobject_cast<QCheckBox*>(sender());
    if(cbSender){
        ui.tSeparateDetail->setVisibleThreads(getVisbleThreads());
        ui.tSeparateDetailSummary->setVisibleThreads(getVisbleThreads());
    }
//    if(cbs.contains())
}

void SPRSeparateDetailTableWidget::onChangeIntValue(int){
    if(sender() == ui.leCommandTimeInterval){
        model->getSettingsControlModel()->tMeassureForData->setData(ui.leCommandTimeInterval->value());
        ui.bBeginScope->setText(tr("Установить период %1 сек.").arg(ui.leCommandTimeInterval->value()));
        ui.bBeginScope->setToolTip(tr("Установить периодичность опроса %1 сек."));
        return;
    }
    if(sender() == ui.leMinimumTimeScope){
        ui.tSeparateDetail->setMinStoneTime(ui.leMinimumTimeScope->value());
        return;
    }
}

void SPRSeparateDetailTableWidget::onChangeThreadList(bool value){
    SPRThreadList visible = getVisbleThreads();
    ui.tSeparateDetail->setVisibleThreads(visible);
    ui.tSeparateDetail->widgetsShow();
}

ISPRModelData *SPRSeparateDetailTableWidget::setModelData(ISPRModelData *data)
{
    model = (SPRMainModel*)data;
    if(model){

        connect(model, SIGNAL(modelChanget(IModelVariable*)), this, SLOT(onModelChanget(IModelVariable*)));
        connect(model->getServer(), SIGNAL(serverStateChange(uint32_t)), this, SLOT(onChandeServerState(uint32_t)));

        connect(model->getHistoryModel2(), &SPRHistoryModel2::separateDataReady, this, &SPRSeparateDetailTableWidget::onSeparateDataReady);

        ui.tSeparateDetail->setModelData(model);
        ui.tSeparateDetail->setVisibleThreads(getVisbleThreads());
        ui.tSeparateDetailSummary->setModelData(model);

        ui.bBeginScope->setText(tr("Установить период %1 сек.").arg(ui.leCommandTimeInterval->value()));
        ui.bBeginScope->setToolTip(tr("Установить периодичность опроса %1 сек."));

        connect(ui.leCommandTimeInterval, SIGNAL(valueChanged(int)), this, SLOT(onChangeIntValue(int)));

        widgetsShow();
    }
}

ISPRModelData *SPRSeparateDetailTableWidget::getModelData()
{
    return model;
}

void SPRSeparateDetailTableWidget::onSeparateDataReady(){
    onChandeServerState(0);
}

void SPRSeparateDetailTableWidget::onModelChanget(IModelVariable *variable)
{
    if(model){
        if(variable == model->getSettingsControlModel()->tMeassureForData){
            blockSignals(true);
            ui.leCommandTimeInterval->setValue(model->getSettingsControlModel()->tMeassureForData->getData());
            blockSignals(false);
        }
        if(variable == model->getSettingsMainModel()->getThreadsVariable()){
            widgetsShow();
        }
//        if(sender() == separateModel){
//            widgetsShow();
//        }
    }
}


void SPRSeparateDetailTableWidget::storeWidgetsStates(QString groupId, QSettings *mainSettings)
{
    mainSettings->beginGroup(groupId);
    mainSettings->setValue(objectName()+"."+ui.spTables->objectName(), ui.spTables->saveState());
    mainSettings->endGroup();

}

void SPRSeparateDetailTableWidget::restoreWidgetsStates(QString groupId, QSettings *mainSettings)
{
    mainSettings->beginGroup(groupId);
    ui.spTables->restoreState(mainSettings->value(objectName()+"."+ui.spTables->objectName()).toByteArray());
    mainSettings->endGroup();
}
