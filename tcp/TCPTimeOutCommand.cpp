/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   TCPTimeOutCommand.cpp
 * Author: longway
 * 
 * Created on 10 февраля 2018 г., 15:06
 */

#include "TCPTimeOutCommand.h"

#include "QAbstractButton"
#include "QPushButton"

TCPTimeOutCommand::TCPTimeOutCommand(): TCPCommand() {
    parts = 0;
    timeout = 0;
    command = nocommand;
    countParts = 0;
    visible = true;
}

TCPTimeOutCommand::TCPTimeOutCommand(EnumCommands _command, uint _timeout_in_msec, uint _parts, TCPTimeOutWidget *_widget, QString _title, QString _message, TCPTypeTOWidget _typeTOWidget):
    TCPCommand(_command),
    widget(nullptr),
    parts(_parts),
    timeout(_timeout_in_msec),
    title(_title),
    message(_message),
    visible(true)
{
    command = _command;
    if(parts == 0) parts = 1;
    setTimeOutWidget(_widget, _typeTOWidget);

}

TCPTimeOutCommand::TCPTimeOutCommand(ServerConnect *_server, EnumCommands _command, uint _timeout_in_msec, uint _parts):
    TCPCommand(_command),
    widget(nullptr),
    parts(_parts),
    timeout(_timeout_in_msec),
    title(""), message(""),
    typeTOWidget(noTOWidget),
    visible(true)
{
    if(parts == 0) parts = 1;
    setTimeOutWidget(nullptr, noTOWidget);

}

void TCPTimeOutCommand::setTimeOutWidget(TCPTimeOutWidget *_widget, TCPTypeTOWidget _typeTOWidget)
{
    typeTOWidget = _typeTOWidget;

    if(typeTOWidget == selfTOWidget){
        if(!title.isEmpty() || !message.isEmpty()){
            widget = new TCPTimeOutWidget(nullptr);
            setProperty("delete_widget", QVariant(true));
        } else {
            setProperty("delete_widget", QVariant(false));
        }
    } else if(typeTOWidget == extTOWidget){
        widget = _widget;
        setProperty("delete_widget", QVariant(false));
    } else {
        setProperty("delete_widget", QVariant(false));
    }
}

void TCPTimeOutCommand::setReplayData(QByteArray replayData){
    this->replayData = replayData;
    if(!noErrors()){
        emit commandNotComplite(this);
        return;
    }
    if(timeout > 0){
        countParts = 0;
        if(widget && typeTOWidget != noTOWidget){
            intervals = timeout / parts;
            widget->setWindowTitle(title);
            widget->setMessage(message);
            if(visible){
                widget->show();
            }
            widget->onRangeChange(countParts, parts);

        } else {
            intervals = timeout;
            parts = 1;
        }
        QTimer::singleShot(intervals, this, &TCPTimeOutCommand::onTimeOut);
    } else {
        emit commandComplite(this);
    }
}

TCPTimeOutWidget *TCPTimeOutCommand::getTimeOutWidget()
{
    return widget;
}

void TCPTimeOutCommand::send(ServerConnect *server)
{
    if(remoteCommands.contains(command)){
        TCPCommand::send(server);
    } else {
        setReplayData(QByteArray("\0",1));
    }
}

void TCPTimeOutCommand::setVisibleTOWidgets(bool yes_no)
{
    visible = yes_no;
}

bool TCPTimeOutCommand::getVisibleTOWidgets()
{
    return visible;
}

void TCPTimeOutCommand::onTimeOut(){
    countParts++;
    if(widget){
        widget->onRangeChange(countParts, parts);
    }
    if(countParts >= parts){
        if(widget){
            if(typeTOWidget == selfTOWidget){
                widget->close();
            } else if(typeTOWidget == extTOWidget){
                widget->hide();
            }
        }

        emit commandComplite(this);
    } else {
        QTimer::singleShot(intervals , this, &TCPTimeOutCommand::onTimeOut);
    }
}

TCPTimeOutCommand::~TCPTimeOutCommand() {
    if(widget && property("delete_widget").value<bool>()){
        delete widget;
    }
}


void TCPTimeOutCommand::setTimeOut(uint _timeout_in_msec)
{
    timeout = _timeout_in_msec;
    countParts = 0;
}

void TCPTimeOutCommand::onCancelCommand(TCPCommand *)
{
    if(widget && typeTOWidget == selfTOWidget){
        widget->close();
    }

    emit cancelCommand(this);
}

TCPTimeOutCommandWC::TCPTimeOutCommandWC(EnumCommands _commands, uint _timeout_in_msec, uint parts, TCPTimeOutWidget *_widget, QString _title, QString _message, TCPTypeTOWidget _typeTOWidget):
    TCPTimeOutCommand(_commands, _timeout_in_msec, parts, _widget, _title, _message, _typeTOWidget), hw(nullptr), hl(nullptr), hs(nullptr), bCancel(nullptr)
{
    if(widget){
            widget->ui.wCancelButtons->setVisible(true);
            widget->ui.wCancelButtons->setEnabled(true);
            connect(widget->ui.bCancel, &QAbstractButton::clicked, this, &TCPTimeOutCommandWC::onClickCancelButtom);
    }
}

void TCPTimeOutCommandWC::onClickCancelButtom(bool){
    onCancelCommand(this);
}
