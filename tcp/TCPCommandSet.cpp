/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   TCPCommanSet.cpp
 * Author: longway
 * 
 * Created on 10 февраля 2018 г., 18:32
 */

#include "TCPCommandSet.h"
#include "tcp/ServerConnect.h"
#include "tcp/tcpclearstates.h"

#include <QDebug>

void TCPCommandSet::setProcessRun(bool value)
{
    processRun = value;
    if(!value){
        if(server){
            server->timerStart();
        }
        foreach(TCPCommand *comm, commandSet){
            if(comm && comm->isCommandSet()){
                comm->setProcessRun(value);
            }
        }
    } else {
        if(server){
            server->timerStop();
        }
    }
}

bool TCPCommandSet::isProcessRun() const
{
    return processRun;
}

void TCPCommandSet::onCancelCommand(TCPCommand *send)
{
    setProcessRun(false);

    sendClearCommand();

    if(getLogWidget()){
        getLogWidget()->addLogsMessage(MSG_TCPSET_CANCEL_COMMAND);
    }
    emit cancelCommand(send);
}

SPRSeparatorLogsModel *TCPCommandSet::getLogModel() const
{
    return logModel;
}

void TCPCommandSet::setLogModel(SPRSeparatorLogsModel *value)
{
    logModel = value;
}

TCPCommand *TCPCommandSet::getCurrentCommand() const
{
    return currentCommand;
}

TCPCommandSet::TCPCommandSet():TCPTimeOutCommand(){
    server = nullptr;
    command = nocommand;
    logModel = nullptr;
    currentCommand = nullptr;
    processRun = false;
//    setProcessRun(false);
}

TCPCommandSet::TCPCommandSet(ServerConnect *_server, TCPTimeOutWidget *_widget, QList<TCPCommand*> _vcomm): TCPTimeOutCommand(nocommand, 0, 1, _widget, "", ""){
    server = _server;
    replayData = QByteArray(1, '\0');
    logModel = nullptr;
    currentCommand = nullptr;
    processRun = false;
//    setProcessRun(false);
    addCommand(_vcomm);
}


//TCPCommandSet::TCPCommandSet(ServerConnect *_server, TCPTimeOutWidget *_widget, QList<EnumCommands> _vcomm): TCPTimeOutCommand(nocommand, 0, 1, _widget, "", ""){
//    server = _server;
//    replayData = QByteArray(1, '\0');
//    addCommand(_vcomm);
//}

TCPCommandSet::~TCPCommandSet() {
}

TCPCommandSet *TCPCommandSet::addCommand(TCPCommand *_comm){
//    _comm->setNum(commandSet.size());
    if(_comm){
        TCPCommand *prev = nullptr;
        if(commandSet.size() > 0){
            prev = commandSet.last();
            if(prev){
                prev->setNext(_comm);
            }
        }
        if(!commandSet.contains(_comm)){
            connect(_comm, &TCPCommand::commandComplite, this, &TCPCommandSet::onCommandComplite);
            connect(_comm, &TCPCommand::commandNotComplite, this, &TCPCommandSet::onCommandNotComplite);
            connect(_comm, &TCPCommand::cancelCommand, this, &TCPCommandSet::onCancelCommand);
            commandSet.push_back(_comm);
            _comm->setVisibleTOWidgets(this->visible);
        }

    }

    return this;
}

TCPCommandSet *TCPCommandSet::addCommand(QList<TCPCommand *> _vcomm){
    for(int i=0; i<_vcomm.size(); i++){
        addCommand(_vcomm[i]);
    }
    return this;
}

TCPCommandSet *TCPCommandSet::addCommand(QList<EnumCommands> _ecomm){
    foreach(EnumCommands com, _ecomm){
        addCommand(com);
    }
    return this;
}

TCPCommandSet *TCPCommandSet::addCommand(EnumCommands ecommand){
    return addCommand(new TCPCommand(ecommand));
}

void TCPCommandSet::sendClearCommand()
{
    if(server){
        clearCommand = new TCPClearStates();
        clearCommand->send(server);
    }
}

int TCPCommandSet::getErrors(){
    int ret = 0;
    foreach(TCPCommand *comm, commandSet){
        ret |= comm->getErrors();
    }

    return ret;
}

TCPCommand *TCPCommandSet::setSendData(QByteArray _sendData, EnumCommands _command)
{
    return setSendData(_sendData.data(), _sendData.size(), _command);
}

TCPCommand *TCPCommandSet::setSendData(void *data, int len, EnumCommands _command)
{
    TCPCommand *_comm;
    if(_command == lastcommand){
        _comm = commandSet.last();
    } else {
        _comm = findCommands(_command).last();
    }
    if(_comm){
        _comm->setSendData(data, len);
    }
    return this;
}

TCPCommand *TCPCommandSet::addSendData(QByteArray sendData, EnumCommands _command){
    return addSendData((void*)sendData.constData(), sendData.size(), _command);
}

TCPCommand *TCPCommandSet::addSendData(void *data, uint len, EnumCommands _command){
    TCPCommand *_comm;
    if(_command == lastcommand){
        _comm = commandSet.last();
    } else {
        _comm = findCommands(_command).last();
    }
    if(_comm){
        _comm->addSendData(data, len);
    }
    return this;
}

int TCPCommandSet::getState(){
    QList<TCPCommand*> lgs = findCommands(getstate);
    if(lgs.size() > 0){
        return lgs.last()->getState();
    } else {
        if(server){
            return server->getLastStateCommand();
        } else {
            return -1;
        }
    }
}

QByteArray TCPCommandSet::getReplayData(EnumCommands _comm){
    QByteArray ret;
    if(_comm == lastcommand){
        if(commandSet.size() > 0){
            ret = commandSet.last()->getReplayData();
        } else {
            return ret;
        }
    } else {
        QList<TCPCommand *> _vcomm = findCommands(_comm);
        if(_vcomm.size() > 0){
            ret = _vcomm.last()->getReplayData();
        } else {
            return ret;
        }
    }
    return ret;
}

EnumCommands TCPCommandSet::getCommand(){
    if(commandSet.size() > 0){
        return commandSet.last()->getCommand();
    } else {
        return nocommand;
    }
}

QByteArray TCPCommandSet::getResult(){
    QByteArray ret;
    if(commandSet.size() > 0){
        return commandSet.last()->getResult();
    } else {
        return ret;
    }
}

QByteArray TCPCommandSet::getResult(EnumCommands _comm){
    QByteArray ret;
    QList<TCPCommand *> _vcomm = findCommands(_comm);
    if(_vcomm.size()>0){
        ret = _vcomm.last()->getResult();
    }
    return ret;
}

TCPCommand *TCPCommandSet::findCommand(TCPCommand *_command){
    foreach(TCPCommand* comm, commandSet){
        if(comm == _command){
            return comm;
        }
    }
    return nullptr;
}

void TCPCommandSet::send(ServerConnect *_server){
    server = _server;
    if(server){
        TCPCommand *comm = nullptr;
        QWidget *w = nullptr;
        comm = qobject_cast<TCPCommand*>(sender());
        w = qobject_cast<QWidget*>(sender());
        go();
    }
}

void TCPCommandSet::onCommandComplite(TCPCommand *_command)
{
    if(isProcessRun() && commandSet.contains(_command)){
        go(_command);
    }
}


void TCPCommandSet::go(TCPCommand *_command)
{
//    currentCommand = _command;
    if(!_command){
        if(!commandSet.isEmpty()){
            TCPCommand *c = commandSet.first();
            if(c){
                setProcessRun(true);
                currentCommand = c;
                c->send(server);
                return;
            }
        }
    } else {
        if(isProcessRun()){
            if(commandSet.contains(_command)){
                if(isCommamdCompare(_command)){
                    TCPCommand *nc = _command->getNext();
                    if(nc){
                        currentCommand = nc;
                        nc->send(server);
                    } else {
                        setProcessRun(false);
                        currentCommand = nullptr;
                        emit commandComplite(this);
                    }
                } else {
                    setProcessRun(false);
                    currentCommand = nullptr;
                    emit commandNotComplite(_command);
                }
            } else {
                currentCommand = nullptr;
            }
        }
    }
}





void TCPCommandSet::onCommandNotComplite(TCPCommand *_command)
{
    setProcessRun(false);
    emit commandNotComplite(_command);
}


void TCPCommandSet::setLogWidget(TCPEchoLogsWidget *value)
{
    ITCPCommand::setLogWidget(value);
    foreach(TCPCommand *com, commandSet){
//    for(int i=0; i<commandSet.size(); i++){
        com->setLogWidget(value);
    }
}

bool TCPCommandSet::noErrors()
{
    bool res = true;
    foreach(TCPCommand *com, commandSet){
//    for(int i=0; i<commandSet.size(); i++){
        res &= com->noErrors();
    }
    return res;
}


bool TCPCommandSet::isCommandSet()
{
    return true;
}


void TCPCommandSet::setVisibleTOWidgets(bool yes_no)
{
    TCPTimeOutCommand::setVisibleTOWidgets(yes_no);
    foreach(TCPCommand *com, commandSet){
        if(com){
            com->setVisibleTOWidgets(yes_no);
        }
    }
}
