/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   ServerConnect.cpp
 * Author: longway
 * 
 * Created on 9 февраля 2018 г., 17:06
 */

#include "ServerConnect.h"
#include "tcp/TCPCommand.h"

#include "models/sprseparatorlogsmodel.h"

SPRSeparatorLogsModel *ServerConnect::getLogModel()
{
    return logModel;
}

void ServerConnect::setLogModel(SPRSeparatorLogsModel *value)
{
    logModel = value;
}

SPRVariable<uint> *ServerConnect::getVPort() const
{
    return vPort;
}

void ServerConnect::setVPort(SPRVariable<uint> *value)
{
    vPort = value;
    if(vPort){
        port = vPort->getData();
        connect(vName, SIGNAL(modelChanget(IModelVariable*)), this, SLOT(onChangeModelData(IModelVariable*)));
    }
}

SPRQStringVariable *ServerConnect::getVName() const
{
    return vName;
}

void ServerConnect::setVName(SPRQStringVariable *value)
{
    vName = value;
    if(vName){
        name = vName->getData();
        connect(vName, SIGNAL(modelChanget(IModelVariable*)), this, SLOT(onChangeModelData(IModelVariable*)));
    }
}

bool ServerConnect::getConnected() const
{
    return connected;
}

void ServerConnect::setConnected(bool value)
{
    connected = value;
    if(connected){
        setTimer(ServerTimerType::fastTypeServerTimer);
    } else {
        serverDisconnect();
    }
}

void ServerConnect::stateToDebug()
{
    qDebug() << "********** last Server states ***************";
    foreach(ServerConnectState st, mapStateToDebug.keys()){
        if(isState(st)){
            qDebug() << mapStateToDebug[st];
        }
    }
}

uint32_t ServerConnect::setState(uint32_t _state){
    bool flag = currentState != _state;
    if(flag){
        onServerStateChange(_state);
    }
    return currentState;
}

void ServerConnect::setError(TCPCommand *command){
    QString msg;
    int err = command->getErrors();
    if(err < 0){
        if(logModel){
            msg = QString(tr("Ошибка выполнения команды. Нет ответа..."));
            logModel->addLogMessage(command, tError, msg);
        }
        return;
    }
    lastCommandError = static_cast<ServerCommandState>(uint(command->getErrors()));
    if(lastCommandError != spr_remote_error_ok){
        if(logModel){
            switch(lastCommandError){
            case spr_remote_error_unknown_command:
                msg = QString(tr("Ошибка! Неизвестная команда..."));
                break;
            case spr_remote_error_no_data:
                msg = QString(tr("Ошибка! Не хватает данных..."));
                break;
            case spr_remote_error_no_correct_data:
                msg = QString(tr("Ошибка! Некорректные данные..."));
                break;
            case spr_remote_error_not_ready_get_command:
            case spr_remote_error_not_ready_hardware:
                msg = QString(tr("Ошибка! Несвоевременная подача команды..."));
                break;
            case spr_remote_error_er_ser:
                msg = QString(tr("Ошибка! ER_SER..."));
                break;
            case spr_remote_error_er_rent:
                msg = QString(tr("Ошибка! ER_RENT..."));
                break;
            case spr_remote_error_er_term:
                msg = QString(tr("Ошибка! ER_TERM..."));
                break;
            case spr_remote_error_er_im:
                msg = QString(tr("Ошибка! ER_IM..."));
                break;
            }
            logModel->addLogMessage(command, tError, msg);
        }
    }
}

void ServerConnect::changeRemoteState(QByteArray replay){
    if(replay.size() > 1){
        int _st = replay.at(1);
        uint32_t st;
        switch (_st) {
        case REMOTE_WAIT_C:
            st = currentState & 0xFFFFFF00;
            break;
        case REMOTE_SPK:
            st = (currentState & 0xFFFFFF00) | spr_state_spector_scope;
            break;
        case REMOTE_SEP:
            st = (currentState & 0xFFFFFF00) |
                    spr_state_separated |
                    spr_state_exposition_on |
                    spr_state_rentgen_on |
                    spr_state_separator_on |
                    spr_state_rentgen_on_correct |
                    spr_state_codes_is_stated;
            if(!isState(spr_state_pitatel_stop_manual)){
                st |= ( spr_state_pitatel_on |
                        spr_state_rasklad_on);
            }
            st &= ~(spr_state_separator_wo_rentgen);
            break;
        case REMOTE_RENT:
            st = (currentState & 0xFFFFFF00) | spr_state_rentgen_uknown;
            break;
        case REMOTE_TERM:
            st = (currentState & 0xFFFFFF00) | spr_state_therm_set;
            break;
        case REMOTE_TEST:
            st = (currentState & 0xFFFFFF00) | spr_state_test_ims;
            break;
        case REMOTE_RGU:
            st = (currentState & 0xFFFFFF00) | spr_state_gru_moved;
            break;
        default:
            break;
        }
        setState(st);
    }
}

bool ServerConnect::noErrorsInReplay(QByteArray *_replay)
{
    if(!_replay) _replay = &replay;
    if(_replay->size()>0){
        return _replay->at(0) == '\0';
    } else {
        return true;
    }

}

ServerConnect::ServerConnect(QString _name, uint16_t _port, SPRQStringVariable *_vName, SPRVariable<uint> *_vPort, bool _connectingNow):
    QTcpSocket(),
    logModel(nullptr)
{
    name = _name;
    port = _port;
    setVName(_vName);
    setVPort(_vPort);

    currentState = 0;

//    connect(this, SIGNAL(serverStateChange(uint32_t)), this, SLOT(onServerStateChange(uint32_t)));
    //        connect(this, SIGNAL(readyRead()), this, SLOT(onReadyRead()));
    connect(this, SIGNAL(addData()), this, SLOT(queueComplite()));
    getStateCommand = new TCPCommand(getstate);
    connect(getStateCommand, &TCPCommand::commandComplite, this, &ServerConnect::onMyGetStateCommandComplite);

    //        timerGetStateCommand.setInterval(2000);
    connect(&timerGetStateCommand, SIGNAL(timeout()), this, SLOT(onTimeOutGetState()));
    //        timerGetStateCommand.start();
    setTimer(fastTypeServerTimer);

}


void ServerConnect::onMyGetStateCommandComplite(TCPCommand *_command){
    if(_command == getStateCommand){
        emit serverStateChange(currentState);
    }
}

ServerConnect::~ServerConnect() {
}

int ServerConnect::getLastStateCommand()
{
    if(getStateCommand){
        return getStateCommand->getState();
    } else {
        return -1;
    }
}

void ServerConnect::setTimer(ServerConnect::ServerTimerType _type){
    timerGetStateCommand.stop();
    if(_type == noServerTimer){
        return;
    }
    if(_type == fastTypeServerTimer){
        timerGetStateCommand.setInterval(2000);
    } else if(_type == slowTypeServerTimer){
        timerGetStateCommand.setInterval(20000);
    }
    timerGetStateCommand.start();
}

void ServerConnect::setPort(uint16_t port) {
    this->port = port;
}

uint16_t ServerConnect::getPort() const {
    if(vPort){
        return vPort->getData();
    } else {
        return port;
    }
}

void ServerConnect::setName(QString name) {
    this->name = name;
}

QString ServerConnect::getName() const {
    if(vName){
        return vName->getData();
    } else {
        return name;
    }
}

uint32_t ServerConnect::addStates(QSet<ServerConnectState> _states){
//    blockSignals(true);
    foreach(ServerConnectState st, _states){
        addState(st);
    }
//    blockSignals(false);
//    emit serverStateChange(currentState);
}

uint32_t ServerConnect::addState(ServerConnectState _state){
    if(_state > 0x0FFF){
        uint32_t newState = currentState | _state;
        setState(newState);
    }
    return getState();
}

uint32_t ServerConnect::clearState(ServerConnectState _state){
    setState(currentState & (~((uint32_t)_state)));
    return getState();
}

void ServerConnect::serverReconnect(){
    this->disconnectFromHost();
    clearState(spr_state_rentgen_on);
    clearState(spr_state_separator_on);
    clearState(spr_state_exposition_on);
    clearState(spr_state_rentgen_on_correct);
    clearState(spr_state_separator_wo_rentgen);
    //        clearState(spr_state_rentgen_not_regime);
}

void ServerConnect::serverDisconnect(){
    this->disconnectFromHost();
    clearState(spr_state_rentgen_on);
    clearState(spr_state_separator_on);
    clearState(spr_state_exposition_on);
    clearState(spr_state_rentgen_on_correct);
    clearState(spr_state_separator_wo_rentgen);
    setTimer(noServerTimer);
}

bool ServerConnect::isState(ServerConnectState _state){
    if(currentState & ((uint32_t)_state)){
        return true;
    } else {
        return false;
    }
}

void ServerConnect::addSendCommand(TCPCommand* _sender){
    this->mutex.lock();
    sendData.push_back(_sender);
    this->mutex.unlock();
    
    emit addData();
}

void ServerConnect::clearQueue(){
    mutex.lock();
    sendData.clear();
    readAll();
    flush();
    //    disconnectFromHost();

    mutex.unlock();
}

void ServerConnect::queueComplite(){
    while(!sendData.empty()){
        mutex.lock();
        current = sendData.front();
        sendData.pop_front();
        if(state() != QAbstractSocket::SocketState::ConnectedState){
            QString name = getName(); int port = getPort();
            connectToHost(getName(), getPort());
            if(!waitForConnected(100)){
                mutex.unlock();
                setTimer(fastTypeServerTimer);
                emit serverConnectTimeOutError(current);
//                clearState(SPR_STATE_SERVER_CONNECT | SPR_STATE_RENTGEN_ON | SPR_STATE_SEPATOR_ON | SPR_STATE_EXPOSITION_ON);
                if(!isState(spr_state_separated)){
                   clearState(spr_state_server_connect);
                   clearState(spr_state_rentgen_on);
                   clearState(spr_state_separator_on);
                   clearState(spr_state_exposition_on);
                   clearState(spr_state_rentgen_on_correct);
                   clearState(spr_state_codes_is_stated);
                   clearState(spr_state_separator_wo_rentgen);
//                   clearState(spr_state_rentgen_not_regime);
                   addState(spr_state_error_connect);
                }
               return;
           }
       }
//       while(bytesAvailable()){
//           readAll();
//       }
        write(current->toQueue());
        if(waitForReadyRead(100) || bytesAvailable()){
            onReadyRead();
        } else {
            mutex.unlock();
            emit serverReadWriteTimeOutError(current);
            setTimer(fastTypeServerTimer);
            if(!isState(spr_state_separated)){
                clearState(spr_state_server_connect);
                clearState(spr_state_rentgen_on);
                clearState(spr_state_separator_on);
                clearState(spr_state_exposition_on);
                clearState(spr_state_rentgen_on_correct);
                clearState(spr_state_codes_is_stated);
                clearState(spr_state_separator_wo_rentgen);
//                clearState(spr_state_rentgen_not_regime);
            }
//            clearState(SPR_STATE_SERVER_CONNECT | SPR_STATE_RENTGEN_ON | SPR_STATE_SEPATOR_ON | SPR_STATE_EXPOSITION_ON);
            addState(spr_state_error_connect);
        }
    }
}

void ServerConnect::onChangeModelData(IModelVariable *var)
{
    if(var == this->vName || var == this->vPort){
        this->disconnectFromHost();
    }
}

void ServerConnect::onReadyRead(){
    replay.clear();
    while(bytesAvailable()){
        replay.append(readAll());
    }
    mutex.unlock();

    clearState(spr_state_error_connect);
    addState(spr_state_server_connect);
    setTimer(slowTypeServerTimer);

    EnumCommands com = current->getCommand();

    if(com == getstate){
        changeRemoteState(replay);
        if(current == this->getStateCommand){
            onServerStateChange(currentState);
            emit serverStateChangeOut(currentState);
        }

    } else if(com == initada){
        if(noErrorsInReplay()){
            clearState(spr_state_separator_on);
            clearState(spr_state_rentgen_on);
            clearState(spr_state_rentgen_on_correct);
            clearState(spr_state_rentgen_not_regime);
            clearState(spr_state_exposition_on);
            clearState(spr_state_codes_is_stated);
        } else {
//            clearState(spr_state_separator_on);
        }
    } else if(com == onsep){
        if(noErrorsInReplay()){
            addState(spr_state_separator_on);
            clearState(spr_state_rentgen_on);
            clearState(spr_state_rentgen_on_correct);
            clearState(spr_state_rentgen_not_regime);
            clearState(spr_state_exposition_on);
            clearState(spr_state_codes_is_stated);
        } else {
//            clearState(spr_state_separator_on);
        }
    } else if(com == onren){
        if(noErrorsInReplay()){
            addState(spr_state_rentgen_on);
        } else {
//            clearState(spr_state_rentgen_on);
        }
    } else if(com == setudeu || com == setptdeu){
        if(noErrorsInReplay()){
            addState(spr_state_codes_is_stated);
        } else {
            clearState(spr_state_codes_is_stated);
        }
    } else if(com == expon){
        if(noErrorsInReplay()){
            addState(spr_state_exposition_on);
        } else {
//            clearState(spr_state_exposition_on);
        }
    } else if(com == startpuw){
        if(noErrorsInReplay()){
            addState(spr_state_pitatel_on);
            clearState(spr_state_pitatel_stop_manual);
        } else {
            clearState(spr_state_pitatel_on);
        }

    } else if(com == stoppuw){
        if(noErrorsInReplay()){
            clearState(spr_state_pitatel_on);
            addState(spr_state_pitatel_stop_manual);
        }

    } else if(com == oniw){
        if(noErrorsInReplay()){
            addState(spr_state_rasklad_on);
        } else {
            clearState(spr_state_rasklad_on);
        }

    } else if(com == offiw){
        if(noErrorsInReplay()){
            clearState(spr_state_rasklad_on);
        }
    } else if(com == getrgu2){
        if(noErrorsInReplay()){
            if(replay.size() > 1){
                char p = replay[1];
                if(p == 0){
                    clearState(spr_state_rgu_down_position);
                    clearState(spr_state_rgu_up_position);
                } else if(p == 1){
                    clearState(spr_state_rgu_down_position);
                    addState(spr_state_rgu_up_position);
                } else if(p == 2){
                    addState(spr_state_rgu_down_position);
                    clearState(spr_state_rgu_up_position);
                }
            }
        }
    } else if(com == getren){
        if(noErrorsInReplay()){
            QByteArray res = replay.right(4);
            uint mka=0, mkv=0;
            memcpy(&mkv, res.constData(), 2);
            memcpy(&mka, res.constData()+2, 2);
            if(mkv >= 0x0600 && mka >= 0x0600 && mkv < 0x0700 && mka < 0x0700){
                addState(spr_state_rentgen_on_correct);
                clearState(spr_state_rentgen_not_regime);
            } else {
                clearState(spr_state_rentgen_on_correct);
                addState(spr_state_rentgen_not_regime);
            }
        } else {
            clearState(spr_state_rentgen_on_correct);
        }
    } else if(com == offren){
        if(noErrorsInReplay()){
            clearState(spr_state_rentgen_on);
            clearState(spr_state_rentgen_on_correct);
            clearState((spr_state_exposition_on));
        }
    } else if(com == offsep || com == clearrs){
        if(noErrorsInReplay()){
            clearState(spr_state_separator_on);
            clearState(spr_state_rentgen_on);
            clearState((spr_state_exposition_on));
            clearState(spr_state_rentgen_on_correct);
            clearState(spr_state_separator_wo_rentgen);
        }
    } else if(com == expoff){
        if(noErrorsInReplay()){
            clearState((spr_state_exposition_on));
            clearState(spr_state_rentgen_on_correct);
        }
    }
    current->setReplayData(replay);

    setError(current);
    qDebug() << current->toDebug();
    //    emit commandComlite(current);
}

void ServerConnect::onTimeOutGetState(){
    getStateCommand->send(this);
}

void ServerConnect::onServerStateChange(uint32_t _state)
{

    bool emitNeeded = _state != currentState;
    if(logModel){
        uint32_t bits = currentState ^ _state;
//        emitNeeded = bits != 0 ? true : false;
        uint32_t onBits = _state & bits;
        uint32_t offBits = currentState & bits;

        if((onBits & SPR_STATE_SERVER_CONNECT) == SPR_STATE_SERVER_CONNECT){
            logModel->addLogMessage(nullptr, tMessage, QString(tr("Установлено соединение с сервером %1:%2")).arg(getName()).arg(getPort()));
        }
        if((onBits & SPR_STATE_ERROR_CONNECT) == SPR_STATE_ERROR_CONNECT){
            logModel->addLogMessage(nullptr, tError, QString("Нет соединения с сервером ")+getName()+":"+QString::number(getPort()));
        }
        if((onBits & SPR_STATE_RENTGEN_ON) == SPR_STATE_RENTGEN_ON){
            logModel->addLogMessage(nullptr, tMessage, QString(tr("Рентген включен...")));
        }
        if((offBits & SPR_STATE_RENTGEN_ON) == SPR_STATE_RENTGEN_ON){
            logModel->addLogMessage(nullptr, tMessage, QString(tr("Рентген выключен...")));
        }
        if((onBits & SPR_STATE_SEPARATOR_ON) == SPR_STATE_SEPARATOR_ON){
            logModel->addLogMessage(nullptr, tMessage, QString(tr("Сортировочная машина включена...")));
        }
        if((onBits & SPR_STATE_SEPARATOR_WO_RENTGEN) == SPR_STATE_SEPARATOR_WO_RENTGEN){
            logModel->addLogMessage(nullptr, tMessage, QString(tr("Сортировочная машина включена без питания РА...")));
        }
        if((onBits & SPR_STATE_CODES_IS_STATED) == SPR_STATE_CODES_IS_STATED){
            logModel->addLogMessage(nullptr, tMessage, QString(tr("Коды ДЭУ и CP установлены...")));
        }
        if((offBits & SPR_STATE_SEPARATOR_ON) == SPR_STATE_SEPARATOR_ON){
            logModel->addLogMessage(nullptr, tMessage, QString(tr("Сортировочная машина выключена...")));
        }
        if((onBits & SPR_STATE_RGU_MOVED) == SPR_STATE_RGU_MOVED){
            logModel->addLogMessage(nullptr, tMessage, QString(tr("Движение РГУ...")));
        }
//        if((offBits & SPR_STATE_RGU_MOVED) == SPR_STATE_RGU_MOVED){
//            logModel->addLogMessage(nullptr, tMessage, QString(tr("Движение РГУ прекращено...")));
//        }
        if((onBits & SPR_STATE_RGU_UP_POSITION) == SPR_STATE_RGU_UP_POSITION){
            logModel->addLogMessage(nullptr, tMessage, QString(tr("РГУ в верхнем положении...")));
        }
        if((onBits & SPR_STATE_RGU_DOWN_POSITION) == SPR_STATE_RGU_DOWN_POSITION){
            logModel->addLogMessage(nullptr, tMessage, QString(tr("РГУ в нижнем положении...")));
        }
        if((onBits & SPR_STATE_TEST_IMS) == SPR_STATE_TEST_IMS){
            logModel->addLogMessage(nullptr, tMessage, QString(tr("Тест исполнительных механизмов начат...")));
        }
        if((offBits & SPR_STATE_TEST_IMS) == SPR_STATE_TEST_IMS){
            logModel->addLogMessage(nullptr, tMessage, QString(tr("Тест исполнительных механизмов завершен...")));
        }
        if((onBits & SPR_STATE_SPECTOR_SCOPE) == SPR_STATE_SPECTOR_SCOPE){
            logModel->addLogMessage(nullptr, tMessage, QString(tr("Режим накопления спектра...")));
        }
        if((offBits & SPR_STATE_SPECTOR_SCOPE) == SPR_STATE_SPECTOR_SCOPE){
            logModel->addLogMessage(nullptr, tMessage, QString(tr("Режим накопления спектра завершен...")));
        }
        if((onBits & SPR_STATE_PITATEL_ON) == SPR_STATE_PITATEL_ON){
            logModel->addLogMessage(nullptr, tMessage, QString(tr("Питатель включен...")));
        }
        if((offBits & SPR_STATE_PITATEL_ON) == SPR_STATE_PITATEL_ON){
            logModel->addLogMessage(nullptr, tMessage, QString(tr("Питатель выключен...")));
        }
        if((onBits & SPR_STATE_RASKLAD_ON) == SPR_STATE_RASKLAD_ON){
            logModel->addLogMessage(nullptr, tMessage, QString(tr("Раскладчик включен...")));
        }
        if((offBits & SPR_STATE_RASKLAD_ON) == SPR_STATE_RASKLAD_ON){
            logModel->addLogMessage(nullptr, tMessage, QString(tr("Раскладчик выключен...")));
        }
        if((onBits & SPR_STATE_SEPARATED) == SPR_STATE_SEPARATED){
            logModel->addLogMessage(nullptr, tMessage, QString(tr("Сортировка началась...")));

            QSet<ServerConnectState> addStatesSet = {
                spr_state_separator_on,
                spr_state_rentgen_on,
                spr_state_exposition_on,
                spr_state_rentgen_on_correct,
                spr_state_codes_is_stated};
            if(!isState(spr_state_pitatel_stop_manual)){
                addStatesSet.insert(spr_state_pitatel_on);
                addStatesSet.insert(spr_state_rasklad_on);
            };
            addStates(addStatesSet);
//            addState(spr_state_separator_on);
//            addState(spr_state_rentgen_on);
//            addState(spr_state_rentgen_on_correct);
//            addState(spr_state_rasklad_on);
//            addState(spr_state_pitatel_on);
//            addState(spr_state_exposition_on);
            clearState(spr_state_error_connect);
            clearState(spr_state_rentgen_not_regime);
            clearState(spr_state_separator_wo_rentgen);
        }
        if((offBits & SPR_STATE_SEPARATED) == SPR_STATE_SEPARATED){
            logModel->addLogMessage(nullptr, tMessage, QString(tr("Сортировка закончилась...")));
        }
        if((onBits & SPR_STATE_EXPOSITION_ON) == SPR_STATE_EXPOSITION_ON){
            logModel->addLogMessage(nullptr, tMessage, QString(tr("Экспозиция включена...")));
        }
        if((offBits & SPR_STATE_EXPOSITION_ON) == SPR_STATE_EXPOSITION_ON){
            logModel->addLogMessage(nullptr, tMessage, QString(tr("Экспозиция выключена...")));
        }
        if((onBits & SPR_STATE_RENTGEN_ON_CORRECT) == SPR_STATE_RENTGEN_ON_CORRECT){
            logModel->addLogMessage(nullptr, tMessage, QString(tr("Рентген вышел на нормальный режим...")));
        }

        if((onBits & SPR_STATE_RENTGEN_NOT_REGIME) == SPR_STATE_RENTGEN_NOT_REGIME){
            logModel->addLogMessage(nullptr, tError, QString(tr("Рентген не вышел на нормальный режим или включен...")));
        }

    }
    currentState = _state;
    if(emitNeeded){
        emit serverStateChangeOut(currentState);
    }
}







