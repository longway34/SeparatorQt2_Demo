#include "sprpitatelcontrolwidget.h"
#include "models/sprseparatorlogsmodel.h"

#include <QBoxLayout>

void SPRPitatelControlWidget::widgetsShow()
{
}

void SPRPitatelControlWidget::onModelChanget(IModelVariable *send)
{
    if(send == mainModel->getSettingsControlModel()->VEMSMaxCode){
        double val = static_cast<double>(mainModel->getSettingsControlModel()->VEMSMaxCode->getData());
        double max = mainModel->getSettingsControlModel()->VEMSMaxCode->getData();
        ui.thPitatelPercents->setScale(0, val);
//        ui.spPitatelPercents->setMaximum(val);
        if(val >= max){
            ui.bPitatelPlus->setEnabled(false);
        } else {
            ui.bPitatelPlus->setEnabled(true);
        }
        return;
    }
    if(send == mainModel->getSettingsControlModel()->VEMSLevelLess){
        double val = static_cast<double>(mainModel->getSettingsControlModel()->VEMSLevelLess->getData());
        ui.thPitatelPercents->setScaleStepSize(val);
        return;
    }
    if(send == mainModel->getSettingsControlModel()->getVEMSBeginCodeVariable()){
        blockSignals(true);
        uint16_t val = mainModel->getSettingsControlModel()->getVEMSBeginCode();
        QString tt = tr("Значение производительности питателя %1").arg(val);
        QString tVal = QString::number(val)+" %";
        ui.spPitatelPercents->setText(tVal); ui.spPitatelPercents->setToolTip(tt);
        ui.thPitatelPercents->setValue(val); ui.thPitatelPercents->setToolTip(tt);
        blockSignals(false);
    }
}

void SPRPitatelControlWidget::onCommandComplite(TCPCommand *){
    if(sender() == startPitatelCommand){
        emit pitatelStartComplite(startPitatelCommand);
        return;
    }
    if(sender() == stopPitatelCommand){
        emit pitatelStopComplite(stopPitatelCommand);
        return;
    }
    if(sender() == startRugospusk){
        emit rudospuskStartComplite(startRugospusk);
        return;
    }
    if(sender() == stopRudosrusk){
        emit rudospuskStopComplite(stopRudosrusk);
        return;
    }
    if(sender() == separatorOn){
        emit separatorOnComplite(separatorOn);
        if(!separatorOn->getFullMode()){
            mainModel->getServer()->addState(spr_state_separator_wo_rentgen);
        } else {
            mainModel->getServer()->clearState(spr_state_separator_wo_rentgen);
        }
        return;
    }
    if(sender() == separatorOff){
        emit separatorOffComplite(separatorOff);
        return;
    }
}

void SPRPitatelControlWidget::setVerticalMode(bool onOff){
//    QGroupBox *gbPC = ui.gbPitatelControl;
//    QGroupBox *gbPWC = ui.gbPowerControl;

    QBoxLayout *la = (QBoxLayout*)layout();
    if(onOff){
        la->setDirection(QBoxLayout::TopToBottom);
//        QLayout *la = new QBoxLayout(QBoxLayout::TopToBottom, nullptr);
    } else {
        la->setDirection(QBoxLayout::LeftToRight);
    }
}

void SPRPitatelControlWidget::onServerStateChange(uint32_t){
    if(mainModel){
        ServerConnect *server = mainModel->getServer();
        if(!server->isState(spr_state_server_connect)){
            this->setEnabled(false);
            return;
        } else {
            this->setEnabled(true);
        }

        blockSignals(true);
        if(server->isState(spr_state_separator_on)){
            ui.bStart->setEnabled(true);
//            ui.spPitatelPercents->setEnabled(true);
            ui.bPitatelMinus->setEnabled(true);
            ui.bPitatelPlus->setEnabled(true);
            ui.bSeparatorOn->setEnabled(false);
            ui.bSeparatorOff->setEnabled(true);
        } else {
            ui.bStart->setEnabled(false);
//            ui.spPitatelPercents->setEnabled(false);
            ui.bPitatelMinus->setEnabled(false);
            ui.bPitatelPlus->setEnabled(false);
            ui.bSeparatorOn->setEnabled(true);
            ui.bSeparatorOff->setEnabled(false);
        }

        showStartButton();

        blockSignals(false);
    }
}


void SPRPitatelControlWidget::showStartButton(){
    if(mainModel){
        bool started; QString tt; QIcon icon; QString title;
        if(rudospuskFullMode){
            if(mainModel->getServer()->isState(spr_state_pitatel_on) || mainModel->getServer()->isState(spr_state_rasklad_on)){
                started = true;
                tt = tr("Остановка рудоспуска");
                icon = iconStop;
                title = tr("Стоп");
            } else {
                started = false;
                tt = tr("Запуск рудоспуска");
                icon = iconStart;
                title = tr("Старт");
            }
        } else {
//mainModel->getServer()->stateToDebug();
            if(mainModel->getServer()->isState(spr_state_pitatel_on)){
                started = true;
                tt = tr("Остановка питателя");
                icon = iconStop;
                title = tr("Стоп");
            } else {
                started = false;
                tt = tr("Запуск питателя");
                icon = iconStart;
                title = tr("Старт");
            }
        }
        ui.bStart->setToolTip(tt);
        QVariant prop; prop.setValue<bool>(!started);
        ui.bStart->setProperty("start", prop);
        ui.bStart->setIcon(icon);
        ui.bStart->setText(title);
    }
}

void SPRPitatelControlWidget::separatorOnOff(bool onOff, bool _toVisible, bool _fullMode){
    if(mainModel){
        ServerConnect *server = mainModel->getServer();
        if(server && server->isState(spr_state_server_connect)){
            if(onOff){
                if(!server->isState(spr_state_separator_wo_rentgen)){
                    if(separatorOn) delete separatorOn;
                    separatorOn = new TCPCommandSeparatorOnFull(server, mainModel, nullptr, _fullMode);
                    connect(separatorOn, &TCPCommandSeparatorOnFull::commandComplite, this, &SPRPitatelControlWidget::onCommandComplite);

                    separatorOn->setVisibleTOWidgets(_toVisible);
                    separatorOn->send(server);
                } else {
                    if(_fullMode){
                        if(mainModel->getLogModel()){
                            mainModel->getLogModel()->addLogMessage(nullptr, tWarning, "Машина включена в режиме \"без питания РА\"...");
                        }
                    }
                }
            } else {
                if(separatorOff) delete separatorOff;
                separatorOff = new TCPCommandSeparatorOff(nullptr, nullptr);
                connect(separatorOff, &TCPTestStopSeparate::commandComplite, this, &SPRPitatelControlWidget::onCommandComplite);

                separatorOff->setVisibleTOWidgets(_toVisible);
                separatorOff->send(server);
            }
        }
    }
}

void SPRPitatelControlWidget::onCommandButtomClickBool(bool){
    if(mainModel){
        ServerConnect *server = mainModel->getServer();
        if(sender() == ui.bStart){
            if(rudospuskFullMode){
                if(ui.bStart->property("start").value<bool>()){
                    if(startRugospusk) delete startRugospusk;
                    startRugospusk = new TCPStartRudospuskCommand(server, mainModel, toWidget);
                    connect(startRugospusk, &TCPTestStopSeparate::commandComplite, this, &SPRPitatelControlWidget::onCommandComplite);

                    startRugospusk->send(server);
                } else {
                    if(stopRudosrusk) delete stopRudosrusk;
                    stopRudosrusk = new TCPStopRudostuskCommand(server, toWidget);
                    connect(stopRudosrusk, &TCPTestStopSeparate::commandComplite, this, &SPRPitatelControlWidget::onCommandComplite);

                    stopRudosrusk->send(server);
                }
            } else {
                if(ui.bStart->property("start").value<bool>()){
                    if(startPitatelCommand) delete startPitatelCommand;
                    startPitatelCommand = new TCPCommandStartPitatel(server,mainModel,toWidget);
                    connect(startPitatelCommand, &TCPTestStopSeparate::commandComplite, this, &SPRPitatelControlWidget::onCommandComplite);

                    startPitatelCommand->send(server);
                } else {
                    if(stopPitatelCommand) delete stopPitatelCommand;
                    stopPitatelCommand = new TCPCommandStopPitatel(server,toWidget);
                    connect(stopPitatelCommand, &TCPTestStopSeparate::commandComplite, this, &SPRPitatelControlWidget::onCommandComplite);

                    stopPitatelCommand->send(server);
                }
            }
        }
        if(sender() == ui.bSeparatorOn){
            if(!onModeDialog){
                onModeDialog = new SPRSeparatorOnModeDialog(nullptr);
            }
            int res = onModeDialog->exec();
            bool fullMode = false;
            if(res == QDialog::Accepted){
                if(onModeDialog->getReturnMode() == onWithRA){
                    fullMode = true;
                } else {
                    fullMode = false;
                }
                separatorOnOff(true, true, fullMode);
            }
//            if(separatorOn) delete separatorOn;
//            separatorOn = new TCPCommandSeparatorOnFull(server, mainModel, nullptr, rentgenMode);
//            connect(separatorOn, &TCPTestStopSeparate::commandComplite, this, &SPRPitatelControlWidget::onCommandComplite);

//            separatorOn->send(server);
            return;
        }
        if(sender() == ui.bSeparatorOff){
//            if(separatorOff) delete separatorOff;
//            separatorOff = new TCPTestStopSeparate(nullptr, mainModel);
//            connect(separatorOff, &TCPTestStopSeparate::commandComplite, this, &SPRPitatelControlWidget::onCommandComplite);

//            separatorOff->send(server);
            separatorOnOff(false, true, false);
            return;
        }
        if(sender() == ui.bPitatelPlus || sender() == ui.bPitatelMinus || !sender()){
            blockSignals(true);
            double val = mainModel->getSettingsControlModel()->getVEMSBeginCode();
            if(sender() == ui.bPitatelPlus){
                val += 1;
                val = val > mainModel->getSettingsControlModel()->VEMSMaxCode->getData() ? mainModel->getSettingsControlModel()->VEMSMaxCode->getData() : val;
            } else {
                val -= 1;
                val = val < 0 ? 0 : val;
            }
            ui.thPitatelPercents->setValue(val);
            ui.spPitatelPercents->setText(QString::number(val)+" %");
            QString tt = tr("значение производительности питателя %1%").arg(val);
            ui.thPitatelPercents->setToolTip(tt);
            ui.spPitatelPercents->setToolTip(tt);

            mainModel->getSettingsControlModel()->setVEMSBeginCode(val);
            uint16_t val16 = mainModel->getSettingsControlModel()->getVEMSBeginCodeVariable()->getData();

            if(setPitatelCommand) delete setPitatelCommand;
            setPitatelCommand = new TCPCommand(setpuw);
            setPitatelCommand->setSendData(&val16, sizeof(val16));
            setPitatelCommand->send(server);
            blockSignals(false);
            return;
        }
    }
}

void SPRPitatelControlWidget::onCommandButtomClickInt(int){
    if(mainModel){
        if(sender() == ui.spPitatelPercents){
            ui.spPitatelPercents->blockSignals(true);
//            mainModel->getSettingsControlModel()->getVEMSBeginCodeVariable()->setData(ui.spPitatelPercents->value());
            ui.spPitatelPercents->blockSignals(false);
        }
    }
}

void SPRPitatelControlWidget::setLogModel(TCPEchoLogsWidget *value){
//    ISPRWidget::setLogModel(value);
//    if(separatorOn){
//        separatorOn->setLogWidget(value);
//    }
//    if(separatorOff){
//        separatorOff->setLogWidget(value);
//    }
}

void SPRPitatelControlWidget::setSeparatorControlVisible(bool value)
{
    separatorControlVisible = value;
}

SPRPitatelControlWidget::SPRPitatelControlWidget(QWidget *parent) :
    QWidget(parent), mainModel(nullptr),
    startPitatelCommand(nullptr),
    stopPitatelCommand(nullptr),
    setPitatelCommand(nullptr),
    startRugospusk(nullptr),
    stopRudosrusk(nullptr),
    separatorOn(nullptr),
    separatorOff(nullptr),
    toWidget(nullptr),
    onModeDialog(nullptr),
    rudospuskFullMode(true),
    separatorControlVisible(false),
    separatorRentgenOn(false),
    isPitatelStopManual(false)

{
    ui.setupUi(this);
    toWidget = new TCPTimeOutWidget(nullptr);

    iconStart = QIcon(":/icons/icons/b_Start.png");
    iconStop = QIcon(":/icons/icons/b_Stop_Active.png");

//    iconSeparatorOn = QIcon(":/icons/icons/Separate_tab_no_colors_icon.png");
//    iconSeparatorOff = QIcon(":/icons/icons/Separate_Close_tab_no_colors_icon.png");

//    iconSeparatorOnFullMode = QIcon(":/icons/icons/Separate_tab_icon.png");
//    iconSeparatorOffFullMode = QIcon(":/icons/icons/Separate_Close_tab_icon.png");

    ui.bStart->setEnabled(false);
    ui.bStart->setCheckable(false);

    QObject::connect(ui.bPitatelMinus, &QAbstractButton::clicked, this, &SPRPitatelControlWidget::onCommandButtomClickBool);
    QObject::connect(ui.bPitatelPlus, &QAbstractButton::clicked, this, &SPRPitatelControlWidget::onCommandButtomClickBool);
    QObject::connect(ui.bStart, &QAbstractButton::clicked, this, &SPRPitatelControlWidget::onCommandButtomClickBool);
    QObject::connect(ui.bSeparatorOn, &QAbstractButton::clicked, this, &SPRPitatelControlWidget::onCommandButtomClickBool);
    QObject::connect(ui.bSeparatorOff, &QAbstractButton::clicked, this, &SPRPitatelControlWidget::onCommandButtomClickBool);

    connect(ui.spPitatelPercents, SIGNAL(valueChanged(int)), this, SLOT(onCommandButtomClickInt(int)));

    setFullMode(rudospuskFullMode, separatorControlVisible);
}

void SPRPitatelControlWidget::setFullMode(bool valFulControl, bool valSeparatorControlVisibleMode)
{
    rudospuskFullMode = valFulControl;
    separatorControlVisible = valSeparatorControlVisibleMode;
    if(rudospuskFullMode){
        this->ui.gbPitatelControl->setTitle(tr("Управление рудоспуском"));
//        ui.bStart->setToolTip(tr("Запуск/остановка рудоспуска"));

//        if(rentgenMode){
//            ui.bSeparatorOn->setIcon(iconSeparatorOnFullMode);
//            ui.bSeparatorOff->setIcon(iconSeparatorOffFullMode);
//            ui.bSeparatorOn->setToolTip(tr("Включить сортировочную машину и питание РА"));
//        } else {
//            ui.bSeparatorOn->setIcon(iconSeparatorOn);
//            ui.bSeparatorOff->setIcon(iconSeparatorOff);
//            ui.bSeparatorOn->setToolTip(tr("Включить сортировочную машину без питания РА"));
//        }

//        ui.bSeparatorOff->setToolTip(tr("Выключить сортировочную машину"));
    } else {
        this->ui.gbPitatelControl->setTitle(tr("Управление питателем"));
//        ui.bStart->setToolTip(tr("Запуск/остановка питателя"));

//        ui.bSeparatorOn->setIcon(iconSeparatorOn);
//        ui.bSeparatorOn->setToolTip(tr("Включить сепаратор без питания РА"));

//        ui.bSeparatorOff->setIcon(iconSeparatorOff);
//        ui.bSeparatorOff->setToolTip(tr("Выключить сортировочную машину"));
    }

    if(separatorControlVisible){
        ui.gbPowerControl->setVisible(true);
    } else {
        ui.gbPowerControl->setVisible(false);
    }
    showStartButton();

//    if(separatorOn){
//        separatorOn->setFullMode(separatorControlVisible);
//    }
}

TCPCommandStartPitatel *SPRPitatelControlWidget::getStartPitatelCommand() const
{
    return startPitatelCommand;
}

TCPCommandStopPitatel *SPRPitatelControlWidget::getStopPitatelCommand() const
{
    return stopPitatelCommand;
}

TCPStartRudospuskCommand *SPRPitatelControlWidget::getStartRugospusk() const
{
    return startRugospusk;
}

TCPStopRudostuskCommand *SPRPitatelControlWidget::getStopRudospusk() const
{
    return stopRudosrusk;
}

TCPCommandSeparatorOnFull *SPRPitatelControlWidget::getSeparatorOn() const
{
    return separatorOn;
}

TCPCommandSeparatorOff *SPRPitatelControlWidget::getSeparatorOff() const
{
    return separatorOff;
}

bool SPRPitatelControlWidget::isStartStopProcess()
{
    QList<TCPCommandSet*> commands = {
        startPitatelCommand, stopPitatelCommand,
        startRugospusk, stopRudosrusk,
        separatorOn, separatorOff
    };
    foreach(TCPCommandSet *c, commands){
        if(c && c->isProcessRun()){
            return true;
        }
    }
    return false;
}



ISPRModelData *SPRPitatelControlWidget::setModelData(ISPRModelData *_mainModel)
{
    mainModel = _mainModel->getMainModel();
    if(mainModel){

        connect(mainModel->getServer(), &ServerConnect::serverStateChangeOut, this, &SPRPitatelControlWidget::onServerStateChange);

        blockSignals(true);
        uint16_t val = mainModel->getSettingsControlModel()->getVEMSBeginCode();
        QString tt = tr("значение производительности питателя %1%").arg(val);
        ui.thPitatelPercents->setValue(val); ui.thPitatelPercents->setToolTip(tt);
        ui.spPitatelPercents->setText(QString::number(val)+" %"); ui.spPitatelPercents->setToolTip(tt);

        ui.thPitatelPercents->setScale(0, mainModel->getSettingsControlModel()->VEMSMaxCode->getData());
//        ui.spPitatelPercents->setMaximum(mainModel->getSettingsControlModel()->VEMSMaxCode->getData());

        ui.bStart->setCheckable(false);
        showStartButton();

        //            ui.bPitatelPause->installEventFilter(this);

        connect(mainModel, SIGNAL(modelChanget(IModelVariable *)), this, SLOT(onModelChanget(IModelVariable *)));
        onServerStateChange(0);
        blockSignals(false);
    }
    return mainModel;
}


ISPRModelData *SPRPitatelControlWidget::getModelData()
{
    return mainModel;
}

