/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   ITCPCommand.h
 * Author: longway
 *
 * Created on 10 февраля 2018 г., 16:47
 */

#ifndef ITCPCOMMAND_H
#define ITCPCOMMAND_H
#include <QObject>
#include <QByteArray>
#include <QString>
#include <QMap>

typedef enum _tcp_commands: unsigned char{
            getstate    = 0x0A,
            testim      = 0x0B,
            initada     = 0x0C,
            getver      = 0x0D,
            stoptest    = 0x0F,
            setspk      = 0x10,
            getspk      = 0x11,
            setsepar    = 0x20,
            getsepar    = 0x21,
            startsep    = 0x22,
            stopsep     = 0x23,
            getkspk     = 0x24,
            getgist     = 0x25,
            startpuw    = 0x27,
            setpuw      = 0x28,
            stoppuw     = 0x29,
            setudeu     = 0x30,
            setren      = 0x31,
            getren      = 0x32,
            onren       = 0x33,
            offren      = 0x34,
            setrgu      = 0x35,
            settemp     = 0x37,
            gettemp     = 0x38,
            expon       = 0x40,
            expoff      = 0x41,
            onsep       = 0x50,
            offsep      = 0x51,
            oniw        = 0x52,
            offiw       = 0x53,
            onosw       = 0x54,
            offosw      = 0x55,
            setrgu2     = 0x56,
            getrgu2     = 0x57,
            setim       = 0x58,
            setptdeu    = 0x63,
            setm50      = 0x65,
            getm50      = 0x66,
            getblk      = 0x67,
            setlog      = 0x68,
                    
            clearrs       = 0x6F,
            setupuser     = 0x70,
            setupsmena    = 0x71,
            masterModeOn  = 0x72,
            masterModeOff = 0x73,

            lastcommand         = 0x7D,
            timeoutcommand      = 0x7E,
            nocommand           = 0x7F,

            setRentgenOn        = 0x80,
            setRGUUpDown        = 0x81,
            setStartPitatel     = 0x82,
            setStopPitatel      = 0x83,
            setGetSpectrumsGistorfamms  =0x84,
            setStartStopIMS    =0x85,
            setStartRudospusk   =0x86,
            setStartStopSeparate = 0x87,
            setTestStartSeparate = 0x88,
            setGetSpectrums      = 0x89,
            setRGUUp             = 0x8A,
            setSeparatorOn       = 0x8B,
            setStopRudospuk      = 0x8C,
            setGetRentgenParams  = 0x8D,
            setTestStopSeparate  = 0x8E,
            setSeparateGo        = 0x8F,
            setSeparatorOnFull     = 0x90,
            setSeparatorOff       = 0x91

} EnumCommands;

const QList<EnumCommands> surrogateCommands = {
    lastcommand, timeoutcommand, nocommand, clearrs,

    setRentgenOn, setRGUUpDown, setStartPitatel, setStopPitatel,
    setGetSpectrumsGistorfamms,
    setStartStopIMS, setStartRudospusk,
    setStartStopSeparate, setTestStartSeparate, setGetSpectrums, setRGUUp,
    setSeparatorOn, setStopRudospuk, setGetRentgenParams, setTestStopSeparate,
    setSeparateGo, setSeparatorOnFull, setSeparatorOff
};

const QList<EnumCommands> remoteCommands = {
    getstate, testim, initada, getver, stoptest, setspk, getspk, setsepar, getsepar,  startsep, stopsep,
    getkspk, getgist, startpuw, setpuw, stoppuw, setudeu, setren, getren, onren, offren, setrgu, settemp,
    gettemp, expon, expoff, onsep, offsep, oniw, offiw, onosw, offosw, setrgu2, getrgu2, setim, setptdeu,
    setm50, getm50, getblk, setlog

};

typedef QMap<EnumCommands, QString> TCPPseudoName;

typedef QMap<EnumCommands, QString>  TCPCommandMessages;

const TCPCommandMessages mapTCPCommandMessages = {
    {testim, QObject::tr("Старт теста ИМ")},
    {stoptest, QObject::tr("Стоп теста ИМ")},
    {startsep, QObject::tr("Начало сортировки")},
    {stopsep, QObject::tr("Завершение сортировки")},
    {onsep, QObject::tr("Включение сортировочной мащины")},
    {offsep, QObject::tr("Выключение сортировочной мащины")},
    {onren, QObject::tr("Включение рентгена")},
    {offren, QObject::tr("Выключение рентгена")},
    {startpuw, QObject::tr("Включение питателя")},
    {stoppuw, QObject::tr("Выключение патателя")},
    {oniw, QObject::tr("Выключение раскладчика")},
    {offiw, QObject::tr("Выключение раскладчика")},
    {expon, QObject::tr("Включение экспозиции")},
    {expoff, QObject::tr("Выключение экспозиции")},
    {setupuser, QObject::tr("Начало работы пользователя")},
    {setupsmena, QObject::tr("Начало смены")},
    {masterModeOn, QObject::tr("Переход в режим МАСТЕРА")},
    {masterModeOff, QObject::tr("Переход в режим ОПЕРАТОР")},
    {getspk, QObject::tr("Получение спектра")}
};

const TCPPseudoName mapTCPPseudoNames = {
    {getstate    , "getstate"},
    {testim      , "testim"},
    {initada     , "initada"},
    {getver      , "getver"},
    {stoptest    , "stoptest"},
    {setspk      , "setspk"},
    {getspk      , "getspk"},
    {setsepar    , "setsepar"},
    {getsepar    , "getsepar"},
    {startsep    , "startsep"},
    {stopsep     , "stopsep"},
    {getkspk     , "getkspk"},
    {getgist     , "getgist"},
    {startpuw    , "startpuw"},
    {setpuw      , "setpuw"},
    {stoppuw     , "stoppuw"},
    {setudeu     , "setudeu"},
    {setren      , "setren"},
    {getren      , "getren"},
    {onren       , "onren"},
    {offren      , "offren"},
    {setrgu      , "setrgu"},
    {settemp     , "settemp"},
    {gettemp     , "gettemp"},
    {expon       , "expon"},
    {expoff      , "expoff"},
    {onsep       , "onsep"},
    {offsep      , "offsep"},
    {oniw        , "oniw"},
    {offiw       , "offiw"},
    {onosw       , "onosw"},
    {offosw      , "offosw"},
    {setrgu2     , "setrgu2"},
    {getrgu2     , "getrgu2"},
    {setim       , "setim"},
    {setptdeu    , "setptdeu"},
    {setm50      , "setm50"},
    {getm50      , "getm50"},
    {getblk      , "getblk"},
    {setlog      , "setlog"},


    {lastcommand         ,"lastcommand"},
    {timeoutcommand      ,"timeoutcommand"},
    {nocommand           ,"nocommand"},

    {setRentgenOn        ,"setRentgenOn"},
    {setRGUUpDown        ,"setRentgenOn"},
    {setStartPitatel     ,"setStartPitatel"},
    {setStopPitatel      ,"setStopPitatel"},
    {setGetSpectrumsGistorfamms  ,"setGetSpectrumsGistorfamms"},
    {setStartStopIMS    ,"setStartStopIMS"},
    {setStartRudospusk   ,"setStartRudospusk"},
    {setStartStopSeparate ,"setStartStopSeparate"},
    {setTestStartSeparate ,"setTestStartSeparate"},
    {setGetSpectrums      ,"setGetSpectrums"},
    {setRGUUp             ,"setRGUUp"},
    {setSeparatorOn       ,"setSeparatorOn"},
    {setSeparatorOff      ,"setSeparatorOff"},
    {setStopRudospuk      ,"setStopRudospuk"},
    {setGetRentgenParams  ,"setGetRentgenParams"},
    {setTestStopSeparate  ,"setTestStopSeparate"},
    {setSeparateGo        ,"setSeparateGo"},
    {setSeparatorOnFull   ,"setSeparatorOnFull"}

};


class TCPEchoLogsWidget;

class ITCPCommand :public QObject{
    
    Q_OBJECT
protected:
    EnumCommands command;
//    int num;
    QString name;
    QByteArray sendData;
    QByteArray replayData;
    
    TCPEchoLogsWidget *logWidget;

public:
    ITCPCommand();
    virtual ~ITCPCommand();

    virtual QByteArray toQueue(){
        QByteArray ret;
        ret.append((char*)&command, sizeof(command)).append(sendData);
        return ret;
    }
    
    QString toString(QByteArray buf){
        QString rep;
        rep = QString(buf.toHex(':'));
        return rep;
    }
    
    QString getPseudoName(){
        if(mapTCPPseudoNames.contains(command)){
            return mapTCPPseudoNames[command];
        } else {
            if(isCommandSet()){
                return "CommandSet";
            } else {
                return "Command";
            }
        }
    }
//    void setNum(int num) {
//        this->num = num;
//    }

//    int getNum() const {
//        return num;
//    }

    virtual void setCommand(EnumCommands command) {
        this->command = command;
        setObjectName(getPseudoName());
    }

    virtual QString toDebug(){
        QString res = QString("com: %1(%2)").arg(getPseudoName()).arg(QString::number((uchar)command,16)) + "; addr:"/* + QString::number((unsigned long)this, 16)*/;
        if(sendData.length() > 16){
            res += QString(" send: len %1bytes").arg(sendData.length());
        } else if(sendData.length() == 1){
            res += QString(" send: %1").arg(QString::number(sendData[0], 16));
        } else {
            res += QString(" send: %1").arg(QString(sendData.toHex(':')));
        }
//        res += sendData.length() > 16 ?  QString(" send: len %1bytes").arg(sendData.length()) : QString(" send: %1").arg(QString(sendData.toHex(':')));
        if(replayData.length() > 16){
            res += QString(" recv: len %1bytes").arg(replayData.length());
        } else if(replayData.length() == 1){
            res += QString(" recv: %1").arg(QString::number(replayData[0], 16));
        } else {
            res += QString(" recv: %1").arg(QString(replayData.toHex(':')));
        }

        return res;
    }

    virtual EnumCommands getCommand() {
        return command;
    }
    virtual void setReplayData(QByteArray replayData)=0;
    virtual QByteArray getReplayData() = 0;
//    virtual void addSendData(QByteArray sendData, EnumCommands)=0;
    virtual QByteArray getSendData()=0;
    virtual int getErrors() = 0;
    virtual bool noErrors() = 0;
    virtual bool isCommandSet() = 0;
    virtual TCPEchoLogsWidget *getLogWidget() const;
    virtual void setLogWidget(TCPEchoLogsWidget *value);

    QString getName() const;
    void setName(const QString &value);

    virtual void setProcessRun(bool value){}

private:

};

#endif /* ITCPCOMMAND_H */

