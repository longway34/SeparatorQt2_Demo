/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   TCPTimeOutCommand.h
 * Author: longway
 *
 * Created on 10 февраля 2018 г., 15:06
 */

#ifndef TCPTIMEOUTCOMMAND_H
#define TCPTIMEOUTCOMMAND_H

#include "TCPCommand.h"
#include "tcptimeoutwigget.h"
#include <QTimer>

//#include "tcp/tcpclearstates.h"

typedef enum {
    noTOWidget, selfTOWidget, extTOWidget
} TCPTypeTOWidget;

class TCPTimeOutCommand : public TCPCommand{
    
    Q_OBJECT

protected:
    TCPTimeOutWidget *widget;
//    QTimer timer;
    
    uint parts;
    uint countParts;
    uint timeout;
    uint intervals;
    
    QString title;
    QString message;
    
    TCPTypeTOWidget typeTOWidget;

    bool visible;
public:
    TCPTimeOutCommand();
    TCPTimeOutCommand(EnumCommands _command, uint _timeout_in_msec = 0, uint parts = 1, TCPTimeOutWidget *_widget = nullptr, QString _title = "", QString _message = "", TCPTypeTOWidget _typeTOWidget = selfTOWidget);
    TCPTimeOutCommand(ServerConnect *_server, EnumCommands _command, uint _timeout_in_msec, uint _parts = 1);
    virtual ~TCPTimeOutCommand();
    virtual void setReplayData(QByteArray replayData);

    void setWidgetTitleMessage(QString _title, QString _msg){
         title = _title;
         message = _msg;
    }

    void setTimeOutWidget(TCPTimeOutWidget *_widget, TCPTypeTOWidget _typeTOWidget = selfTOWidget);
    TCPTimeOutWidget *getTimeOutWidget();

    virtual void setTimeOut(uint _timeout_in_msec);
    virtual void send(ServerConnect *server);

    virtual void setVisibleTOWidgets(bool yes_no);
    virtual bool getVisibleTOWidgets();
public slots:
    virtual void onTimeOut();
    virtual void onCancelCommand(TCPCommand*);
};

class TCPTimeOutCommandWC: public TCPTimeOutCommand{

    Q_OBJECT

    QWidget *hw;
    QHBoxLayout *hl;
    QSpacerItem *hs;
    QPushButton *bCancel;
public:
    TCPTimeOutCommandWC(): hw(nullptr), hl(nullptr), hs(nullptr), bCancel(nullptr){}
    TCPTimeOutCommandWC(EnumCommands _commands, uint _timeout_in_msec = 0, uint parts = 1, TCPTimeOutWidget *_widget = nullptr, QString _title = "", QString _message = "", TCPTypeTOWidget _typeTOWidget = extTOWidget);
    ~TCPTimeOutCommandWC(){
    }
public slots:
    virtual void onClickCancelButtom(bool);
signals:
};

#endif /* TCPTIMEOUTCOMMAND_H */

