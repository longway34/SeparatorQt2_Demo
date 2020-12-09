/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   TCPCommanSet.h
 * Author: longway
 *
 * Created on 10 февраля 2018 г., 18:32
 */

#ifndef TCPCOMMANSET_H
#define TCPCOMMANSET_H

#define MSG_TIME_OUT_SET_DEU               (QString(tr("Включение рентгена")))
#define MSG_TIME_OUT_SET_DEU_MSG(X)        (QString(tr("Установка кодов ДЭУ...(%1 сек.)")).arg( (X) ))

#define MSG_TIME_OUT_SET_CP                (QString(tr("Включение рентгена")))
#define MSG_TIME_OUT_SET_CP_MSG(X)         (QString(tr("Установка кодов ЦП...(%1 сек.)")).arg( (X) ))

#define MSG_TIME_OUT_REN_ON                (QString(tr("Включение рентгена")))
#define MSG_TIME_OUT_REN_ON_MSG(X)         (QString(tr("Прогрев рентгена...(%1 сек.)")).arg( (X) ))

#define MSG_TIME_OUT_REN_OFF                (QString(tr("Выключение рентгена")))
#define MSG_TIME_OUT_REN_OFF_MSG(X)         (QString(tr("Выключение рентгена...(%1 сек.)")).arg( (X) ))

#define MSG_TIME_OUT_EXP_ON                (QString(tr("Включение рентгена")))
#define MSG_TIME_OUT_EXP_ON_MSG(X)         (QString(tr("Установка экспозиции ...(%1 сек.)")).arg( (X) ))

#define MSG_TIME_OUT_SET_SPK               (QString(tr("Накопление спектра")))
#define MSG_TIME_OUT_SET_SPK_MSG(X)        (QString(tr("Накопление спектра ...(%1 сек.)")).arg( (X) ))

#define MSG_TIME_OUT_ON_SEPAR               (QString(tr("Старт сортировки")))
#define MSG_TIME_OUT_ON_SEPAR_MSG(X)        (QString(tr("Старт сортировки...(%1 сек.)")).arg( (X) ))

#define MSG_TIME_OUT_ON_PITATEL               (QString(tr("Включение питателя")))
#define MSG_TIME_OUT_ON_PITAPEL_MSG(X)        (QString(tr("Включение питателя...(%1 сек.)")).arg( (X) ))

#define MSG_TIME_OUT_OFF_PITATEL               (QString(tr("Выключение питателя")))
#define MSG_TIME_OUT_OFF_PITAPEL_MSG(X)        (QString(tr("Выключение питателя...(%1 сек.)")).arg( (X) ))

#define MSG_TIME_OUT_ON_RASKLAD               (QString(tr("Включение раскладчика")))
#define MSG_TIME_OUT_ON_RASKLAD_MSG(X)        (QString(tr("Включение раскладчика...(%1 сек.)")).arg( (X) ))

#define MSG_TIME_OUT_OFF_RASKLAD               (QString(tr("Выключение раскладчика")))
#define MSG_TIME_OUT_OFF_RASKLAD_MSG(X)        (QString(tr("Выключение раскладчика...(%1 сек.)")).arg( (X) ))

#define MSG_TIME_OUT_OFF_SEPAR               (QString(tr("Остановка сортировки")))
#define MSG_TIME_OUT_OFF_SEPAR_MSG(X)        (QString(tr("Остановка сортировочной машины...(%1 сек.)")).arg( (X) ))

#define MSG_TCPSET_CANCEL_COMMAND             (QString(tr("Команда прервана")))
#define MSG_TCPSET_CANCEL_COMMAND_MSG             (QString(tr("Команда прервана пользователем...")))

#define MSG_TCPSET_RENTGEN_NOT_READY                    (QString(tr("Включение рентгена")))
#define MSG_TCPSET_RENTGEN_NOT_READY_MSG(X, Y)         (QString(tr("Рентген не вышел на рабочий режим (0x%1,0x%2)").arg(X).arg(Y)))

#define MSG_NO_OFF_SEPARATE                 (QString(tr("Включение рентгена")))
#define MSG_NO_OFF_SEPARATE_MSG             (QString(tr("Включение рентгена не удалось из-за внутренней ошибки БРС\nНеобходимо выключить и включить питание сортировочной машины...")))

#define MSG_RGU_MOVED                    (QString(tr("Движение РГУ...")))
#define MSG_RGU_MOVED_UP                 (QString(tr("Идет поднятие РГУ. Подождите...")))
#define MSG_RGU_MOVED_DOWN               (QString(tr("Идет опускание РГУ. Подождите...")))

#define MSG_RGU_MOVED_UP_QUIEST_MSG      (QString(tr("Для продолжения работы нужно,\nчтобы РГУ находился в верхнем положении\n\nВы хотите поднять РГУ?")))
#define MSG_RGU_MOVED_DOWN_QUIEST_MSG    (QString(tr("Для продолжения работы нужно,\nчтобы РГУ находился в нижнем положении\n\nВы хотите опустить РГУ?")))
#define MSG_RGU_MOVED_QUIES_TITLE        (QString(tr("Внимание!")))

#define MSG_BLK_NOT_CLOSED              (QString(tr("Открыта защитная дверь...")))
#define MSG_BLK_NOT_CLOSED_MSG          (QString(tr("Открыта защитная дверь...\nВключение рентгена прервано...")))

#include "tcp/TCPTimeOutCommand.h"
#include <QVector>
//#include "tcp/tcpclearstates.h"

class ServerConnect;
class TCPClearStates;
class TCPTimeOutWidget;
class SPRSeparatorLogsModel;

typedef enum:uint8_t {noRGU = 0, UpRGU = 1, DownRGU=2} SPRTypeRGUMoved;

class TCPCommandSet : public TCPTimeOutCommand
{

    Q_OBJECT


    TCPClearStates *clearCommand;
    SPRSeparatorLogsModel *logModel;

    TCPCommand *currentCommand;
protected:
//    QVector<TCPCommand*> commandSet;

    ServerConnect *server;
    bool processRun;

public:
    TCPCommandSet();
//    TCPCommandSet(TCPTimeOutWigget *_widget): TCPTimeOutCommand(nocommand, 0, 1, _widget, "", ""){}
    TCPCommandSet(ServerConnect *_server, TCPTimeOutWidget *_widget=nullptr, QList<TCPCommand *> _vcomm={});
//    TCPCommandSet(ServerConnect *_server, TCPTimeOutWidget *_widget=nullptr, QList<EnumCommands> _vcomm={});
    TCPTimeOutWidget *getTimeOutWidget(){
        return widget;
    }
    virtual ~TCPCommandSet();
    
    TCPCommandSet *addCommand(TCPCommand* _comm);
    TCPCommandSet *addCommand(QList<TCPCommand*> _vcomm);
    
    TCPCommandSet *addCommand(QList<EnumCommands> _ecomm);

    TCPCommandSet *addCommand(EnumCommands ecommand);

    virtual void sendClearCommand();

    virtual void send(ServerConnect *_server);
    virtual int getErrors();

    virtual TCPCommand *setSendData(QByteArray _sendData, EnumCommands _command = lastcommand);
    virtual TCPCommand *setSendData(void *data, int len, EnumCommands _command = lastcommand);

    virtual TCPCommand *addSendData(QByteArray sendData, EnumCommands _command = lastcommand);

    virtual TCPCommand *addSendData(void *data, uint len, EnumCommands _command = lastcommand);

    virtual int getState();
//    virtual int getState(EnumCommands _comm);
    virtual QByteArray getReplayData(){
        return this->getReplayData(lastcommand);
    }
    virtual QByteArray getReplayData(EnumCommands _comm);
    virtual EnumCommands getCommand();
    virtual QByteArray getResult();
    virtual QByteArray getResult(EnumCommands _comm);
    TCPCommand* findCommand(TCPCommand *_command);

    virtual bool isCommamdCompare(TCPCommand *_command){
        return true;
    }
protected:
    
    virtual void go(TCPCommand *_command = nullptr);
protected slots:
    virtual void onCommandNotComplite(TCPCommand*_command);
    virtual void onCommandComplite(TCPCommand *_command);

    // ITCPCommand interface
public:
    virtual void setLogWidget(TCPEchoLogsWidget *value);

    // ITCPCommand interface
public:
    virtual bool noErrors();

    // ITCPCommand interface
public:
    virtual bool isCommandSet();

    // TCPTimeOutCommand interface
    virtual void setProcessRun(bool value);

    virtual bool isProcessRun() const;

    virtual SPRSeparatorLogsModel *getLogModel() const;
    virtual void setLogModel(SPRSeparatorLogsModel *value);

public slots:
    void onCancelCommand(TCPCommand *send);

    // TCPTimeOutCommand interface
public:
    virtual void setVisibleTOWidgets(bool yes_no);
    TCPCommand *getCurrentCommand() const;
};

#endif /* TCPCOMMANSET_H */

