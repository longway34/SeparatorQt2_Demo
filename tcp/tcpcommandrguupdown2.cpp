#include "tcpcommandrguupdown2.h"
#include <QMessageBox>
#include "tcp/ServerConnect.h"

#include "models/sprseparatorlogsmodel.h"

//bool TCPCommandRGUUpDown2::getStarted() const
//{
//    return started;
//}

//void TCPCommandRGUUpDown2::setStarted(bool value)
//{
//    started = value;
//}

TCPCommandRGUUpDown2::TCPCommandRGUUpDown2(SPRMainModel *_model, TCPTimeOutWidget *_towidget, SPRTypeRGUMoved _rguMoved, bool _withQuestion)
    :TCPCommandSet(nullptr, _towidget, {}), model(_model)
{
    command = setRGUUpDown;

    sysErrorCancel = false;

    toTitle = MSG_RGU_MOVED;
    withQuestion = _withQuestion;
    rguMoved = _rguMoved;
    if(rguMoved == UpRGU){
        toMessage = MSG_RGU_MOVED_UP;

//        quiestTitle = MSG_RGU_MOVED_QUIES_TITLE;
        quiestMessage = MSG_RGU_MOVED_UP_QUIEST_MSG;
        errorMsg = tr("Ошибка команды \"Поднять РГУ\"...");
    } else {
        toMessage = MSG_RGU_MOVED_DOWN;
        quiestMessage = MSG_RGU_MOVED_DOWN_QUIEST_MSG;
        errorMsg = tr("Ошибка команды \"Опустить РГУ\"...");
    }
    quiestLogMessage = quiestMessage;
    errorLogMsg = errorMsg;

    if(model){
        QString name = model->getSettingsMainModel()->getNameVariable()->getData() + ": ";
        toTitle = name + toTitle;
        quiestMessage = name + quiestMessage;
        errorMsg = name + errorLogMsg;
    }

    stopRgu = nullptr;
}

TCPCommandRGUUpDown2::~TCPCommandRGUUpDown2()
{
    if(stopRgu) delete stopRgu;
}

//typedef enum{notBegin = 0, isResulted, needMovedCommand, needStopCommand} SPRRGUStatusCommand;


void TCPCommandRGUUpDown2::addWaiting(){
    addCommand(getstate)->
    addCommand(new TCPTimeOutCommandWC(getrgu2, 1000, 100, nullptr, toTitle, toMessage, selfTOWidget));
}

QString statusToStr(SPRRGUStatusCommand _status){
    QString res;
    switch(_status){
    case notBegin: res = "notBegin"; break;
    case commandSend: res = "commandSend"; break;
    case movedContinue: res = "movedContinue"; break;
    case movedEnd: res = "movedEnd"; break;
    }
    return res;
}

void TCPCommandRGUUpDown2::go(TCPCommand *_command){
    if(model){
        bool ignoreRGUError = model->getSettingsMainModel()->ignoreRGUErrors->getData();
        if(!_command){
            clear();

            status = notBegin;
            countTemplate = 0;
            addCommand(getstate)->addCommand(getrgu2);

        } else if(_command->getCommand() == getrgu2){
            QByteArray resAr = findCommands(getrgu2).last()->getResult();
            int res = -1;
            if(resAr.size() > 0){
                res = resAr[0];
            }
            int st = getState();

            if(res < 0){
                emit commandNotComplite(commandSet.last());
                return;
            }
            if(st < 0){
                emit commandNotComplite(this);
                return;
            }

    //        clear(); _command = nullptr;

    //        if(_command->getCommand() == getrgu2){

            if(status == notBegin){
                if(res == rguMoved || rguMoved == noRGU){ // not need work
                    emit commandComplite(this);
                    return;
                } else {
                    int res = 0;
                    if(withQuestion){
                        res = QMessageBox::question(nullptr, MSG_RGU_MOVED_QUIES_TITLE, quiestMessage,
                            tr("Да"), tr("Отмена"), tr("Игнорировать"), 0, 2);
                    }
                    if(res == 1){
                        onCancelCommand(nullptr);
                        return;
                    } else if(res == 2){
                        if(getLogModel()){
                            getLogModel()->addLogMessage(nullptr, tWarning, tr("Игнорирование неправильного положения РГУ при продолжении работы..."));
                        }
                        emit commandComplite(this);
                        return;
                    } else {
                        countTemplate = 4;
                        status = commandSend;

                        char move = rguMoved == UpRGU ? 1 : 2;
                        addCommand(setrgu2)->setSendData(&move,1);
                        addWaiting();
                    }

                }
            } else if(status == commandSend){
                    if(res != 0){
                        if(countTemplate-- < 0){
                            if(getLogModel()){
                                getLogModel()->addLogMessage(nullptr, tError, errorLogMsg);
                            }
                            QMessageBox::critical(nullptr,MSG_RGU_MOVED_QUIES_TITLE, errorMsg);
                            sysErrorCancel = true;
                            onCancelCommand(_command);
                            return;
                        } else {
                            addWaiting();
                        }
                    } else {
    //                    if(status != movedContinue){
                        status = movedContinue;

                        countTemplate = model->getSettingsRentgenModel()->timeMoveRGU->getData();
                        countTemplateErr = countTemplate;

                        if(ignoreRGUError){
                            countTemplate = 10000;
                        }
    //                    }
                        addWaiting();
                    }
            } else if(status == movedContinue){
                if(res != rguMoved){
                    countTemplateErr--;
                    if(countTemplateErr < 0){
                        SPRTypeLogMessage type = ignoreRGUError ? tWarning : tError;
                        if(getLogModel()){
                            int num = model->getSettingsRentgenModel()->timeMoveRGU->getData();
                            errorLogMsg += tr("Превышено ориентировочное время движения %1 сек.").arg(num);
                            if(!ignoreRGUError){
                                errorLogMsg += tr(" Движение РГУ будет прервано...");
                            } else {
                                errorLogMsg += tr(" Проверте установки РГУ (Настройка->Измерительный тракт->Контрольные параметры)");
                            }
                            getLogModel()->addLogMessage(nullptr, type, errorLogMsg);
                        }
                        countTemplateErr = 10000;
                        if(!ignoreRGUError){
                            onCancelCommand(_command);
                            QMessageBox::critical(nullptr,MSG_RGU_MOVED_QUIES_TITLE, errorMsg);
                            sysErrorCancel = true;
                            return;
                        }
                    }

                    if(countTemplate-- < 0){
                        status = movedEnd;
                        countTemplate = 1;
                    } else {
                        addWaiting();
                    }
                } else {
    //                if(status != movedEnd){
                        status = movedEnd;
                        countTemplate = 1;
                        char stop = 0;
                        addCommand(setrgu2)->addSendData(&stop, 1);
    //                }
                    addWaiting();
                }
            } else if(status == movedEnd){
                if(res == rguMoved){
                    if(countTemplate-- <= 0){
                        emit commandComplite(this);
                        return;
                    } else {
                        addWaiting();
                    }
                } else {
                    if(ignoreRGUError){
                        errorMsg += tr(" Игнорируем...");
                    }
                    if(getLogModel()){
                        getLogModel()->addLogMessage(nullptr, tError, errorMsg);
                    }
                    if(ignoreRGUError){
                        emit commandComplite(this);
                        return;
                    } else {
                        sysErrorCancel = true;
                        onCancelCommand(_command);
                        QMessageBox::critical(nullptr,MSG_RGU_MOVED_QUIES_TITLE, errorLogMsg);
                    }
                    return;
                }
            }
    //        qDebug() << "next step... size:" << commandSet.size() << " state:" << st << " gru:" << res << " status:" << statusToStr(status) << "template: "<<countTemplate;
        }
        TCPCommandSet::go(_command);
    }
}

void TCPCommandRGUUpDown2::onCancelCommand(TCPCommand *)
{
    setProcessRun(false);


        char stop = '\0';

        if(stopRgu) delete stopRgu; stopRgu = nullptr;

        stopRgu = new TCPCommandSet();
        stopRgu->addCommand(setrgu2)->addSendData(&stop, 1, setrgu2);
        stopRgu->addCommand(getrgu2);
        stopRgu->addCommand(getstate);

//        stopRgu->setSendData(&stop, 1);
        stopRgu->send(server);

        if(!sysErrorCancel){
            QMessageBox::warning(nullptr, MSG_TCPSET_CANCEL_COMMAND, MSG_TCPSET_CANCEL_COMMAND_MSG);
        }
//    emit cancelCommand(this);
    TCPCommandSet::onCancelCommand(this);
}

SPRSeparatorLogsModel *TCPCommandRGUUpDown2::getLogModel() const
{
    if(model){
        return model->getLogModel();
    } else {
        return nullptr;
    }
}
