#include "sprtestimswidget.h"
#include "tcp/TCPTimeOutCommand.h"
#include <QMessageBox>
#include <QDataStream>

#include "scrollzoomer.h"

SPRTestIMSWidget::SPRTestIMSWidget(QWidget *parent) :
    QWidget(parent), model(nullptr),
    getSpectrumsCommand(nullptr),
    rentgenOnCommand(nullptr), rentgenOffCommand(nullptr),
    rguReadStateCommand(nullptr),
    rguUpCommand(nullptr), rguDownCommand(nullptr), rguStop(nullptr),
    thermoReadStateCommand(nullptr), thermoWriteStateCommand(nullptr),
    startTestImsCommand(nullptr),
    commandStartRasklad(nullptr), commandStopRasklad(nullptr),
    spectrumsOk(true), masterMode(false),
    autoSettingDialog(nullptr)


{
    ui.setupUi(this);

    towidget = new TCPTimeOutWidget();

    connect(ui.bRGUReadPosition, SIGNAL(clicked(bool)), this, SLOT(onCommand(bool)));
    connect(ui.bRGUUp, SIGNAL(clicked(bool)), this, SLOT(onCommand(bool)));
    connect(ui.bRGUDown, SIGNAL(clicked(bool)), this, SLOT(onCommand(bool)));

    connect(ui.bThermoGet, SIGNAL(clicked(bool)), this, SLOT(onCommand(bool)));
    connect(ui.bThermoSet, SIGNAL(clicked(bool)), this, SLOT(onCommand(bool)));

    connect(ui.bGetSpectrum, SIGNAL(clicked(bool)), this, SLOT(onCommand(bool)));

    connect(ui.cbZonesShow, SIGNAL(toggled(bool)), this, SLOT(onChangeValue(bool)));
    ui.wSpectrumWidget->setAllCurrent(true);
    ui.wSpectrumWidget->setWithLegend(true);
    ui.wSpectrumWidget->setZonesShow(ui.cbZonesShow->isChecked());


    connect(ui.bIMSStart, SIGNAL(clicked(bool)), this, SLOT(onCommand(bool)));
    connect(ui.bIMSStop, SIGNAL(clicked(bool)), this, SLOT(onCommand(bool)));


    connect(ui.bRaskladStart, SIGNAL(clicked(bool)), this, SLOT(onCommand(bool)));
    connect(ui.bRaskladStop, SIGNAL(clicked(bool)), this, SLOT(onCommand(bool)));


    vectorIms = {ui.cbIMSThread0, ui.cbIMSThread1, ui.cbIMSThread2, ui.cbIMSThread3, ui.cbIMSThread4, ui.cbIMSThread5, ui.cbIMSThread6, ui.cbIMSThread7};

    vectorSeparatorOnEnabled = {ui.bIMSStart,
                                ui.bRGUDown, ui.bRGUUp,
                                ui.bThermoGet,
                                ui.bThermoSet,
                                ui.bRaskladStart,
                                ui.bRaskladStop
                               };
    vectorSeparatedProcessDisabled = {
        ui.bGetSpectrum,
        ui.bIMSStart,
        ui.bIMSStop,
        ui.bRGUDown,
        ui.bRGUUp,
        ui.bStartAutoSet,

        ui.bThermoSet,
        ui.bThermoGet
    };


    rguReadStateCommand = new TCPCommandSet(nullptr, towidget,
        {new TCPCommand(getrgu2),
         new TCPCommand(getstate)
                                            });
    connect(rguReadStateCommand, SIGNAL(commandComplite(TCPCommand*)), this, SLOT(onCommandComplite(TCPCommand*)));

    rguUpCommand = new TCPCommandRGUUpDown2(nullptr, towidget, UpRGU);
    connect(rguUpCommand, SIGNAL(commandComplite(TCPCommand*)), this, SLOT(onCommandComplite(TCPCommand*)));

    rguDownCommand = new TCPCommandRGUUpDown2(nullptr, towidget, DownRGU);
    connect(rguDownCommand, SIGNAL(commandComplite(TCPCommand*)), this, SLOT(onCommandComplite(TCPCommand*)));

    rguStop = new TCPCommand(setrgu2);
    uint8_t st = 0;
    rguStop->setSendData(&st, 1);


    startTestImsCommand = new TCPStartSopTestIMSCommand(nullptr, towidget);
    connect(startTestImsCommand, SIGNAL(commandComplite(TCPCommand*)), this, SLOT(onCommandComplite(TCPCommand*)));

    commandStartRasklad = new TCPCommand(oniw);
    connect(commandStartRasklad, SIGNAL(commandComplite(TCPCommand*)), this, SLOT(onCommandComplite(TCPCommand*)));

    commandStopRasklad = new TCPCommand(offiw);
    connect(commandStopRasklad, SIGNAL(commandComplite(TCPCommand*)), this, SLOT(onCommandComplite(TCPCommand*)));

    ui.wPitatelControl->setFullMode(false, false);
    ui.wPitatelControl->setVerticalMode(true);

    ui.wRudospuskControl->setFullMode(true, true);

    connect(ui.wPitatelControl, &SPRPitatelControlWidget::pitatelStartComplite, this, &SPRTestIMSWidget::onCommandComplite);
    connect(ui.wPitatelControl, &SPRPitatelControlWidget::pitatelStopComplite, this, &SPRTestIMSWidget::onCommandComplite);
    connect(ui.wPitatelControl, &SPRPitatelControlWidget::rudospuskStartComplite, this, &SPRTestIMSWidget::onCommandComplite);
    connect(ui.wPitatelControl, &SPRPitatelControlWidget::rudospuskStopComplite, this, &SPRTestIMSWidget::onCommandComplite);
    connect(ui.wPitatelControl, &SPRPitatelControlWidget::separatorOnComplite, this, &SPRTestIMSWidget::onCommandComplite);
    connect(ui.wPitatelControl, &SPRPitatelControlWidget::separatorOffComplite, this, &SPRTestIMSWidget::onCommandComplite);

    connect(ui.bStartAutoSet, &QPushButton::clicked, this, &SPRTestIMSWidget::onCommand);

    autoSettingDialog = new SPRSettingsRentgenAutoSetDialog();

}

bool SPRTestIMSWidget::isSpectrumsOk()
{
    return spectrumsOk;
}

bool SPRTestIMSWidget::setSpectrumsOk(bool value)
{
    spectrumsOk = value;

    onServerConnectChangeState(0);
}

bool SPRTestIMSWidget::isMasterMode()
{
    return masterMode;
}

void SPRTestIMSWidget::setMasterMode(bool mode)
{
    masterMode = mode;

    onServerConnectChangeState(0);
}

void SPRTestIMSWidget::setThermo(){
    //        TCPCommand *comm = thermoWriteStateCommand->findCommands(settemp).last();
    //        uint16_t temps[2] = {ui.slThermoMin->value(), ui.slThermoMax->value()};

    //        QByteArray ba = QByteArray::fromRawData((char*)temps, sizeof(uint16_t) * 2);
    //        comm->addSendData(ba);
    //        thermoWriteStateCommand->send(model->getServer());

    if(thermoWriteStateCommand) delete thermoWriteStateCommand;
    thermoReadStateCommand = nullptr;

    thermoWriteStateCommand = new TCPGetSetThermo(false, ui.slThermoMin->value(), ui.slThermoMax->value());
    connect(thermoWriteStateCommand, &TCPGetSetThermo::commandComplite, this, &SPRTestIMSWidget::onCommandComplite);
    connect(thermoWriteStateCommand, &TCPGetSetThermo::commandNotComplite, this, &SPRTestIMSWidget::onCommandNotComplite);
    thermoWriteStateCommand->send(model->getServer());
}



void SPRTestIMSWidget::setLogModel(TCPEchoLogsWidget *value)
{
}

void SPRTestIMSWidget::onChangeValue(bool _val){
    if(sender() == ui.cbZonesShow){
        ui.wSpectrumWidget->setZonesShow(_val);
    }
}

void SPRTestIMSWidget::onChangeValue(double _val){
}

void SPRTestIMSWidget::mainConnect(TCPCommand *command){
    if(command){
        connect(command, &TCPCommand::commandComplite, this, &SPRTestIMSWidget::onCommandComplite);
        connect(command, &TCPCommand::commandNotComplite, this, &SPRTestIMSWidget::onCommandNotComplite);
        //        command->setLogWidget(getLogModel());
    }
}

void SPRTestIMSWidget::onCommand(bool){

    if(model){
        if(sender() == ui.bStartAutoSet){
            QList<uint8_t> threads = model->getThreadsList();

            autoSettingDialog->setThreads(threads);
            autoSettingDialog->widgetsShow();
            autoSettingDialog->show();

        }



        if(sender() == ui.bRaskladStart){
            commandStartRasklad->send(model->getServer());
            return;
        }

        if(sender() == ui.bRaskladStop){
            commandStopRasklad->send(model->getServer());
        }

        if(sender() == ui.bRGUReadPosition){
            rguReadStateCommand->send(model->getServer());
            return;
        }
        if(sender() == ui.bRGUUp){
            if(rguUpCommand) delete rguUpCommand;
            rguUpCommand = new TCPCommandRGUUpDown2(model, towidget, UpRGU, false);
            mainConnect(rguUpCommand);

            rguUpCommand->send(model->getServer());
            return;
        }
        if(sender() == ui.bRGUDown){
            if(rguDownCommand) delete rguDownCommand;
            rguDownCommand = new TCPCommandRGUUpDown2(model, towidget, DownRGU, false);
            mainConnect(rguDownCommand);

            rguDownCommand->send(model->getServer());
            return;
        }
        if(sender() == ui.bThermoGet){
            if(thermoReadStateCommand) delete thermoReadStateCommand;
            thermoReadStateCommand = nullptr;

            thermoReadStateCommand = new TCPGetSetThermo();
            connect(thermoReadStateCommand, &TCPGetSetThermo::commandComplite, this, &SPRTestIMSWidget::onCommandComplite);
            connect(thermoReadStateCommand, &TCPGetSetThermo::commandNotComplite, this, &SPRTestIMSWidget::onCommandNotComplite);
            thermoReadStateCommand->send(model->getServer());

            ui.bThermoGet->setEnabled(false);
            ui.bThermoSet->setEnabled(false);

            return;
        }
        if(sender() == ui.bThermoSet){
            setThermo();

            ui.bThermoGet->setEnabled(false);
            ui.bThermoSet->setEnabled(false);

            return;
        }
        if(sender() == ui.bIMSStart){
            QVector<uint8_t> _vch;
            if(startTestImsCommand) delete startTestImsCommand;
            startTestImsCommand = new TCPStartSopTestIMSCommand(model->getServer(), towidget);
            connect(startTestImsCommand, &TCPStartSopTestIMSCommand::commandComplite, this, &SPRTestIMSWidget::onCommandComplite);
            connect(startTestImsCommand, &TCPStartSopTestIMSCommand::commandNotComplite, this, &SPRTestIMSWidget::onCommandNotComplite);

            mainConnect(startTestImsCommand);
            for(uint i=0; i < model->getSettingsMainModel()->getImsVariable()->getData(); i++){
                if(ui.cbIMSAllThreads->isChecked() || vectorIms[i]->isChecked()){
                    _vch.push_back(i);
                }
            }
            startTestImsCommand->setParams(_vch, ui.slIMSDelay->value(), ui.slIMSTime->value(), ui.slIMSFrequency->value());
            startTestImsCommand->send(model->getServer());

            ui.bIMSStart->setEnabled(false);
            return;
        }
        if(sender() == ui.bIMSStop){
            if(startTestImsCommand){
                startTestImsCommand->stopTest();

                ui.bIMSStop->setEnabled(false);
            }
            return;
        }
        if(sender() == ui.bGetSpectrum){
            QList<uint8_t> lth;
            for(int th=0; th<model->getThreads()->getData(); th++) lth << th;
            if(getSpectrumsCommand) delete getSpectrumsCommand;
            getSpectrumsCommand = new TCPGetSpectrums2(model, 5, noRGU, model->getThreadsList(), true, towidget);
            mainConnect(getSpectrumsCommand);

            getSpectrumsCommand->send(model->getServer());
            return;
        }
    }
}

void SPRTestIMSWidget::onCommandComplite(TCPCommand *_comm){
    QString msgTitle;
    QString msgText;

    if(sender() == commandStopRasklad){
        if(!_comm->noErrors()){
            int err = _comm->getErrors();
            msgTitle = tr("Команда выключить раскладчик");
            msgText = tr("Выполнено с ошибкой %1").arg(QString::number(err));
            QMessageBox::information(this, msgTitle, msgText);
        }
        return;
    }

    if(sender() == commandStartRasklad){
        if(!_comm->noErrors()){
            int err = _comm->getErrors();
            msgTitle = tr("Команда включить раскладчик");
            msgText = tr("Выполнено с ошибкой %1").arg(QString::number(err));
            QMessageBox::information(this, msgTitle, msgText);
        }
        return;
    }

    if(sender() == ui.wPitatelControl){
        if(_comm == ui.wPitatelControl->getStartPitatelCommand()){
            QMessageBox::information(this, tr("Команда включить питатель"), tr("Команда включить питатель выполнена успешно..."));
            return;
        }
        if(_comm == ui.wPitatelControl->getStopPitatelCommand()){
            QMessageBox::information(this, tr("Команда выключить питатель"), tr("Команда выключить питатель выполнена успешно..."));
            return;
        }
    }
    if(sender() == ui.wRudospuskControl){
        if(_comm == ui.wRudospuskControl->getStartRugospusk()){
            QMessageBox::information(this, tr("Команда включить рудоспуск"), tr("Команда включить рудоспуск выполнена успешно..."));
            return;
        }
        if(_comm == ui.wRudospuskControl->getStopRudospusk()){
            QMessageBox::information(this, tr("Команда выключить рудоспуск"), tr("Команда выключить рудоспуск выполнена успешно..."));
            return;
        }
    }

    if(sender() == rguReadStateCommand){

        QByteArray result = rguReadStateCommand->getResult(getrgu2);
        if(result.size() > 0){
            switch (result.at(result.size()-1)) {
            case 0:
                ui.lRGUPosition->setText(tr("Положение не определено"));
                break;
            case 1:
                ui.lRGUPosition->setText(tr("Верхнее положение"));
                break;
            case 2:
                ui.lRGUPosition->setText(tr("Нижнее положение"));
                break;
            }
        }
        return;
    }

    if(sender() == rguUpCommand){
        msgTitle = tr("Команда поднять РГУ");
        msgText = tr("Команда поднять РГУ выполнена успешно");
        QMessageBox::information(this, msgTitle, msgText);
        rguReadStateCommand->send(model->getServer());
    }
    if(sender() == rguDownCommand){
        msgTitle = tr("Команда опустить РГУ");
        msgText = tr("Команда опустить РГУ выполнена успешно");
        QMessageBox::information(this, msgTitle, msgText);
    }
    if(sender() == thermoReadStateCommand){
//        QByteArray res = thermoReadStateCommand->getResult(gettemp);
        TCPGetSetThermo::sterms terms = thermoReadStateCommand->getTerms();

        ui.lThermoCurrent->setText(QString::number(terms.termCurrent, 'f', 0));
        ui.slThermoMin->setValue(terms.termLo);
        ui.slThermoMax->setValue(terms.termHi);

            ui.bThermoGet->setEnabled(true);
            ui.bThermoSet->setEnabled(true && isMasterMode());
    }
    if(sender() == thermoWriteStateCommand){
        msgTitle = tr("Команда установки темпиратурного регулятора");
        msgText = tr("Выполнено успешно...");
        QMessageBox::information(this, msgTitle, msgText);

        TCPGetSetThermo::sterms terms = thermoWriteStateCommand->getTerms();
        ui.lThermoCurrent->setText(QString::number(terms.termCurrent, 'f', 0));
        ui.slThermoMin->setValue(terms.termLo);
        ui.slThermoMax->setValue(terms.termHi);

        ui.bThermoGet->setEnabled(true);
        ui.bThermoSet->setEnabled(true && isMasterMode());

        return;

    }
    if(sender() == getSpectrumsCommand){
        QVector<QColor> colors = {Qt::red, Qt::green, Qt::blue, Qt::yellow, Qt::cyan, Qt::magenta};

        SPRSpectrumListItemsModel* mod = dynamic_cast<SPRSpectrumListItemsModel*>(ui.wSpectrumWidget->getModelData());
        if(mod){

            QString _forErr;
            double porogCorr = model->getSettingsControlModel()->correlSpectrumCritical->getData();
            bool findErrorCorrel = false;

            QList<TCPCommand*> lgetspk = getSpectrumsCommand->findCommands(getspk);
            int countErr = 0;
            for(uint8_t ch=0; ch<lgetspk.size(); ch++){
                QByteArray sp = getSpectrumsCommand->getSpectrumData(ch);
                SPRSpectrumItemModel *item = mod->setSpectrumData(ch,sp,spectrumsOnly, 5000, tr("Ручей %1"));
                SPRSpectrumItemModel *b = mod->getBaseItemForChannel(item->getSpectrumThread());

                if(b){
                    double corr = item->getCorrel(b);
                    if(corr < porogCorr){
                        if(countErr == 0){
                            _forErr = _forErr + tr("%1").arg(ch+1);
                        } else {
                            _forErr = _forErr + tr(", %1").arg(ch+1);
                        }
                        countErr++;
                        findErrorCorrel = true;
                    }
                }
            }
            if(findErrorCorrel){
                if(countErr == 1){
                    _forErr = tr("Для ручья %1 ").arg(_forErr);
                } else {
                    _forErr = tr("Для ручьев %1 ").arg(_forErr);
                }
                _forErr = _forErr +tr("обнаружено недопустимо низкое значение корреляции\n"
                                      "Возможно отсутствуют или неправильно получены базовые спектры\n"
                                      "Необходимо заново снять базовые спектры...");
            }
            ui.wSpectrumWidget->setVisibleAll();
            ui.wSpectrumWidget->widgetsShow();
            ui.wSpectrumWidget->setGraphicsMode(viewModeScales);
            ui.wSpectrumWidget->setZonesShow(ui.cbZonesShow->isChecked());

            if(!findErrorCorrel){
                QMessageBox::information(this, tr("Получение спектров"), tr("Спектры получены"));
            } else {
                QMessageBox::warning(this, tr("Ошибки корреляции спектров..."),
                                     _forErr);
            }
            setSpectrumsOk(!findErrorCorrel);
        }

        return;

    }
    if(sender() == startTestImsCommand){
        if(_comm == startTestImsCommand->getStopTestCommand()){
            QMessageBox::information(this, tr("Завершение теста"), tr("Тест остановлен пользователем"));
        } else {
            QMessageBox::information(this, tr("Завершение теста"), tr("Тест выполнен полностью"));
        }
        ui.bIMSStart->setEnabled(true);
        ui.bIMSStop->setEnabled(false);
    }
}

void SPRTestIMSWidget::onCommandNotComplite(TCPCommand *_comm)
{
    if(sender() == getSpectrumsCommand){
        if(_comm->getCommand() == getren){
            uint16_t mka, mkv; uint8_t err;
            getSpectrumsCommand->isRentgenReady(_comm->getReplayData(), &mkv, &mka, &err);
            QMessageBox::warning(nullptr, tr("Ошибка включения рентгена"),
                                 tr("Рентген не вышел на рабочий режим (%1, %2 < 0x600)\n"
                                    "Попробуйте выключить рентген\n"
                                    "И получить спектры еще раз...").
                                 arg(mkv, 0, 16).arg(mka, 0, 16));

        }

    }
    if(sender() == thermoReadStateCommand || sender() == thermoWriteStateCommand){
        ui.bThermoGet->setEnabled(true);
        ui.bThermoSet->setEnabled(true);
    }
}

void SPRTestIMSWidget::onCancelCommand(TCPCommand *comm)
{
    if(sender() == rguDownCommand || sender() == rguUpCommand){
        QMessageBox::warning(nullptr, tr("Работа РГУ"), tr("РГУ остановлено пользователем не дойдя до конечного положения..."));
    }
    if(sender() == thermoReadStateCommand || sender() == thermoWriteStateCommand){
        ui.bThermoGet->setEnabled(true);
        ui.bThermoSet->setEnabled(true && isMasterMode());
    }
    if(sender() == startTestImsCommand){
        ui.bIMSStart->setEnabled(true);
        ui.bIMSStop->setEnabled(false);
    }
}

void SPRTestIMSWidget::onServerConnectChangeState(uint32_t _state){
//    bool enabled = false;
    if(model){
        ServerConnect *server = model->getServer();
        if(!server->isState(spr_state_server_connect) || server->isState(spr_state_separated)){
            this->setEnabled(false);
            return;
        } else {
            this->setEnabled(true);

        }
        if(server->isState(spr_state_test_ims)){
            ui.bIMSStart->setEnabled(false);
            ui.bIMSStop->setEnabled(true);
        } else {
            ui.bIMSStart->setEnabled(true);
            ui.bIMSStop->setEnabled(false);
        }

        if(server->isState(spr_state_separator_on)){
            for(int i=0; i<vectorSeparatorOnEnabled.size(); i++){
                vectorSeparatorOnEnabled[i]->setEnabled(true);
                if(!isMasterMode()){
                    ui.bThermoSet->setEnabled(false);
                }
            }
            for(int i=0; i<vectorSeparatedProcessDisabled.size(); i++){
                vectorSeparatedProcessDisabled[i]->setEnabled(true);
            }

            ui.bStartAutoSet->setEnabled(isMasterMode() && (!isSpectrumsOk()));
            if(!isMasterMode()){
                ui.bThermoSet->setEnabled(false);
            }
            if(server->isState(spr_state_rasklad_on)){
                ui.bRaskladStart->setEnabled(false);
            } else {
                ui.bRaskladStart->setEnabled(true);
            }
        } else {
            for(int i=0; i<vectorSeparatorOnEnabled.size(); i++){
                vectorSeparatorOnEnabled[i]->setEnabled(false);
            }
        }

        if(server->isState(spr_state_separator_wo_rentgen)){
            ui.bGetSpectrum->setEnabled(false);
        } else {
            ui.bGetSpectrum->setEnabled(true);
        }

        ui.bStartAutoSet->setEnabled((isMasterMode()) &&
                                     (!isSpectrumsOk()) &&
                                     (!server->isState(spr_state_separator_wo_rentgen)));

        if(server->isState(spr_state_rgu_up_position)){
            ui.lRGUPosition->setText(tr("Верхнее положение"));
        }
        if(server->isState(spr_state_rgu_down_position)){
            ui.lRGUPosition->setText(tr("Нижнее положение"));
        }
    }
}

ISPRModelData *SPRTestIMSWidget::setModelData(SPRMainModel *_model)
{
    if(_model){
        if(model){
            disconnect(model, SIGNAL(modelChanget(IModelVariable*)), this, SLOT(onModelChanget(IModelVariable*)));
        }
        model = _model;
        connect(model, SIGNAL(modelChanget(IModelVariable*)), this, SLOT(onModelChanget(IModelVariable*)));

        connect(model->getServer(), SIGNAL(serverStateChangeOut(uint32_t)), this, SLOT(onServerConnectChangeState(uint32_t)));


        ui.wSpectrumWidget->setModelData(model->getKSpectrums(), spectrumsOnly, true);
        ui.wSpectrumWidget->setGraphicsMode(viewModeScales);

        QList<uint8_t> lth;
        for(int th=0; th<model->getThreads()->getData(); th++) lth << th;


        uint16_t code = model->getSettingsControlModel()->getVEMSBeginCode();

        ui.wSpectrumWidget->setWithLegend(true);
        ui.wSpectrumWidget->setEnableChangeTypeSet(true);

        SPRZoomerWithCorrel *zoom = new SPRZoomerWithCorrel(model->getKSpectrums(), ui.wSpectrumWidget->ui.canvas->canvas(), ui.wSpectrumWidget->ui.canvas);
        ui.wSpectrumWidget->setZoomer(zoom);
        ui.wSpectrumWidget->setZonesShow(true);

        ui.wPitatelControl->setModelData(model);
        ui.wRudospuskControl->setModelData(model);

        ui.slTimeMoveRGU->setValue(model->getSettingsRentgenModel()->timeMoveRGU->getData());
        ui.lTimeMoveRGU->setValue(model->getSettingsRentgenModel()->timeMoveRGU->getData());
        connect(ui.slTimeMoveRGU, &QSlider::valueChanged, this, &SPRTestIMSWidget::onChangeIntValue);
        connect(ui.lTimeMoveRGU, SIGNAL(valueChanged(int)), this, SLOT(onChangeIntValue(int)));

        autoSettingDialog->setModelData(model->getSettingsRentgenModel());


        widgetsShow();
    }
    return model;
}


void SPRTestIMSWidget::widgetsShow()
{
    if(model){
        blockSignals(true);

        for(int i=0; i<model->getSettingsMainModel()->getImsVariable()->getData(); i++){
            vectorIms[i]->setVisible(true);
        }
        for(int i=model->getSettingsMainModel()->getImsVariable()->getData(); i<vectorIms.size(); i++){
            vectorIms[i]->setVisible(false);
        }
        onServerConnectChangeState(model->getServer()->getState());
        blockSignals(false);
    }

}

ISPRModelData *SPRTestIMSWidget::getModelData()
{
    return model;
}


void SPRTestIMSWidget::onModelChanget(IModelVariable *var)
{
    if(sender() == model->getSettingsMainModel()->getImsVariable()){
        widgetsShow();
    }
    if(sender() == model->getSettingsRentgenModel()->timeMoveRGU
            || var == model->getSettingsRentgenModel()->timeMoveRGU){
        SPRVariable<uint> *var = model->getSettingsRentgenModel()->timeMoveRGU;
        if(var){
            ui.slTimeMoveRGU->blockSignals(true); ui.lTimeMoveRGU->blockSignals(true);

            ui.slTimeMoveRGU->setValue(var->getData());
            ui.lTimeMoveRGU->setValue(var->getData());

            ui.slTimeMoveRGU->blockSignals(false); ui.lTimeMoveRGU->blockSignals(false);
        }


    }
//    widgetsShow();
}

void SPRTestIMSWidget::onChangeIntValue(int value){
    if(model){
        if(sender() == ui.slTimeMoveRGU){
            ui.lTimeMoveRGU->blockSignals(true); ui.lTimeMoveRGU->setValue(value);
            model->getSettingsRentgenModel()->timeMoveRGU->setData(value);
            ui.lTimeMoveRGU->blockSignals(false);
        }
        if(sender() == ui.lTimeMoveRGU){
            ui.slTimeMoveRGU->blockSignals(true); ui.slTimeMoveRGU->setValue(value);
            model->getSettingsRentgenModel()->timeMoveRGU->setData(value);
            ui.slTimeMoveRGU->blockSignals(false);
        }
    }
}

