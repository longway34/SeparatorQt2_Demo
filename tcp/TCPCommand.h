/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   ITCPCommand.h
 * Author: longway
 *
 * Created on 9 февраля 2018 г., 17:23
 */

#ifndef TCPCOMMAND_H
#define TCPCOMMAND_H

#include <stdint.h>
#include <QDebug>
#include <QObject>
#include "ITCPCommand.h"
//#include "ServerConnect.h"

class ServerConnect;
class TCPCommandSet;

class TCPCommand: public ITCPCommand {
    
    Q_OBJECT
    

protected:
    friend TCPCommandSet;

    QList<TCPCommand*> commandSet;

    TCPCommand *next;
    TCPCommand *getNext(){ return next;}
    TCPCommand *setNext(TCPCommand *c){
        next = c;
        return c;
    }
    TCPCommand *setThisToPrev(TCPCommand *prev){
        if(prev){
            prev->setNext(this);
        }
        return this;
    }

public:
    TCPCommand();
    TCPCommand(EnumCommands _command);
    virtual ~TCPCommand();
    
    virtual TCPCommand *setSendData(QByteArray _sendData, EnumCommands _command=lastcommand);
    virtual TCPCommand *setSendData(void *data, uint len, EnumCommands _command=lastcommand);
    virtual TCPCommand *addSendData(QByteArray sendData, EnumCommands _command=lastcommand);
    virtual TCPCommand *addSendData(void *data, uint len, EnumCommands _command=lastcommand);
    virtual void setVisibleTOWidgets(bool){}

    virtual void clear();

    virtual void setReplayData(QByteArray replayData);

    virtual QByteArray getReplayData() {
        return replayData;
    }
    virtual int getReplayDataSize(){
        return replayData.size();
    }


    QByteArray getSendData() {
        return sendData;
    }
    
    virtual void send(ServerConnect *server);
    
    virtual void setTimeOut(int){
    }
    
    virtual bool noErrors(){
        if(replayData.size()>0){
            return replayData[0] == '\0';
        } else {
            return true;
        }
    }
    virtual int getErrors(){
        if(replayData.size()>0){
            return int(replayData[0]);
        }
        return -1;
    }
    virtual int getState(){
        if(replayData.size()>1){
            return int(replayData[1]);
        }
        return -1;
    }
    virtual QByteArray getResult(){
        QByteArray res;
        if(replayData.size() > 1){
            for(int i=1; i< replayData.size(); i++)
            res += replayData[i];
        }
        return res;
    }

    virtual EnumCommands getDataType(){
        return command;
    }
    virtual QList<TCPCommand*> findCommands(EnumCommands _command=lastcommand);

public slots:
    virtual void go(TCPCommand *_command = NULL){}
    virtual void onCommandNotComplite(TCPCommand*_command);

private:

    
signals:
    void commandComplite(TCPCommand*);
    void commandNotComplite(TCPCommand*);
    void cancelCommand(TCPCommand*);

    // ITCPCommand interface
public:
    virtual bool isCommandSet();
    virtual bool isRentgenReady(QByteArray result, uint16_t *mkv=nullptr, uint16_t *mka=nullptr, uint8_t *err=nullptr);
};

#endif /* TCPCOMMAND_H */

