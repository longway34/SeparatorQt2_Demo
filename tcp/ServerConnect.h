/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   ServerConnect.h
 * Author: longway
 *
 * Created on 9 февраля 2018 г., 17:06
 */

#ifndef SERVERCONNECT_H
#define SERVERCONNECT_H

#include <QTcpSocket>
#include <QQueue>
#include <QByteArray>
#include <QMutex>
#include "tcp/tcpechologswidget.h"
#include "tcp/ITCPCommand.h"
#include "models/imodelvariable.h"
#include "variables/sprvariable.h"
#include "variables/sprqstringvariable.h"
#include "QTimer"

#define REMOTE_WAIT_C   0
#define REMOTE_SPK      1
#define REMOTE_SEP      2
#define REMOTE_RENT     3
#define REMOTE_TERM     4
#define REMOTE_TEST     5
#define REMOTE_RGU      6

#define SPR_STATE_WAIT_COMMAND          0
#define SPR_STATE_SPECTOR_SCOPE         0x00000001
#define SPR_STATE_SEPARATED             0x00000002
#define SPR_STATE_THERM_SET             0x00000004
#define SPR_STATE_RENTGEN_UNKNOW        0x00000008
#define SPR_STATE_TEST_IMS              0x00000010
#define SPR_STATE_RGU_MOVED             0x00000020

#define SPR_STATE_RGU_UP_POSITION       0x00001000
#define SPR_STATE_RGU_DOWN_POSITION     0x00002000

#define SPR_STATE_CODES_IS_STATED       0x00004000

#define SPR_STATE_SEPARATOR_ON            0x00010000
#define SPR_STATE_RENTGEN_ON            0x00020000
#define SPR_STATE_PITATEL_ON            0x00040000
#define SPR_STATE_EXPOSITION_ON         0x00080000
#define SPR_STATE_RENTGEN_ON_CORRECT    0x00100000
#define SPR_STATE_RASKLAD_ON            0x00200000
#define SPR_STATE_SEPARATOR_WO_RENTGEN  0x00400000
#define SPR_STATE_PITATEL_STOP_MANUAL   0x00800000

#define SPR_STATE_RENTGEN_NOT_REGIME    0x02000000
#define SPR_STATE_ERROR_CONNECT         0x01000000
#define SPR_STATE_SERVER_CONNECT        0x04000000

#define SPR_REMOTE_ERROR_OK                     0x0 // Ok
#define SPR_REMOTE_ERROR_UKNOWN_COMMAND         0x1 // N_KM
#define SPR_REMOTE_ERROR_NO_DATA                0x2 // N_D_KM
#define SPR_REMOTE_ERROR_NO_CORRECT_DATA        0x3 // N_DATE
#define SPR_REMOTE_ERROR_NOT_READY_GET_COMMAND  0x4 // N_EX
#define SPR_REMOTE_ERROR_NOT_READY_HARDWARE     0x5 // NN_EX
#define SPR_REMOTE_ERROR_ER_SER                 0x6 // ER_SER
#define SPR_REMOTE_ERROR_ER_RENT                0x7 // ER_RENT
#define SPR_REMOTE_ERROR_ER_TERM                0x8 // ER_TERM
#define SPR_REMOTE_ERROR_ER_IM                  0x9 // ER_IM
#define SPR_REMOTE_ERROR_TCP                    (-1)

typedef enum server_connect_state :uint32_t {
    spr_state_wait_command          = SPR_STATE_WAIT_COMMAND,
    spr_state_spector_scope         = SPR_STATE_SPECTOR_SCOPE,
    spr_state_separated             = SPR_STATE_SEPARATED,
    spr_state_therm_set             = SPR_STATE_THERM_SET,
    spr_state_rentgen_uknown        = SPR_STATE_RENTGEN_UNKNOW,
    spr_state_test_ims              = SPR_STATE_TEST_IMS,
    spr_state_gru_moved             = SPR_STATE_RGU_MOVED,

    spr_state_rgu_up_position       = SPR_STATE_RGU_UP_POSITION,
    spr_state_rgu_down_position     = SPR_STATE_RGU_DOWN_POSITION,

    spr_state_codes_is_stated       = SPR_STATE_CODES_IS_STATED,

    spr_state_separator_on          = SPR_STATE_SEPARATOR_ON,
    spr_state_rentgen_on            = SPR_STATE_RENTGEN_ON,
    spr_state_pitatel_on            = SPR_STATE_PITATEL_ON,
    spr_state_exposition_on         = SPR_STATE_EXPOSITION_ON,
    spr_state_rentgen_on_correct    = SPR_STATE_RENTGEN_ON_CORRECT,
    spr_state_rasklad_on            = SPR_STATE_RASKLAD_ON,
    spr_state_separator_wo_rentgen  = SPR_STATE_SEPARATOR_WO_RENTGEN,
    spr_state_pitatel_stop_manual   = SPR_STATE_PITATEL_STOP_MANUAL,

    spr_state_rentgen_not_regime    = SPR_STATE_RENTGEN_NOT_REGIME,
    spr_state_error_connect         = SPR_STATE_ERROR_CONNECT,
    spr_state_server_connect        = SPR_STATE_SERVER_CONNECT

} ServerConnectState;

static const QMap<ServerConnectState, QString> mapStateToDebug = {
    {spr_state_wait_command, QObject::tr("spr_state_wait_command")},
    {spr_state_spector_scope, QObject::tr("spr_state_spector_scope")},
    {spr_state_separated, QObject::tr("spr_state_separated")},
    {spr_state_rentgen_uknown, QObject::tr("spr_state_rentgen_uknown")},
    {spr_state_therm_set, QObject::tr("spr_state_therm_set")},
    {spr_state_test_ims, QObject::tr("spr_state_test_ims")},
    {spr_state_gru_moved, QObject::tr("spr_state_gru_moved")},
    {spr_state_rgu_up_position, QObject::tr("spr_state_rgu_up_position")},
    {spr_state_rgu_down_position, QObject::tr("spr_state_rgu_down_position")},
    {spr_state_codes_is_stated, QObject::tr("spr_state_codes_is_stated")},
    {spr_state_separator_on, QObject::tr("spr_state_separator_on")},
    {spr_state_rentgen_on, QObject::tr("spr_state_rentgen_on")},
    {spr_state_pitatel_on, QObject::tr("spr_state_pitatel_on")},
    {spr_state_exposition_on, QObject::tr("spr_state_exposition_on")},
    {spr_state_rentgen_on_correct, QObject::tr("spr_state_rentgen_on_correct")},
    {spr_state_rasklad_on, QObject::tr("spr_state_rasklad_on")},
    {spr_state_rentgen_not_regime, QObject::tr("spr_state_rentgen_not_regime")},
    {spr_state_error_connect, QObject::tr("spr_state_error_connect")},
    {spr_state_server_connect, QObject::tr("spr_state_server_connect")}

};

typedef enum server_command_state :uint{
    spr_remote_error_ok                     = SPR_REMOTE_ERROR_OK,
    spr_remote_error_unknown_command        = SPR_REMOTE_ERROR_UKNOWN_COMMAND,
    spr_remote_error_no_data                = SPR_REMOTE_ERROR_NO_DATA,
    spr_remote_error_no_correct_data        = SPR_REMOTE_ERROR_NO_CORRECT_DATA,
    spr_remote_error_not_ready_get_command  = SPR_REMOTE_ERROR_NOT_READY_GET_COMMAND,
    spr_remote_error_not_ready_hardware     = SPR_REMOTE_ERROR_NOT_READY_HARDWARE,
    spr_remote_error_er_ser                 = SPR_REMOTE_ERROR_ER_SER,
    spr_remote_error_er_rent                = SPR_REMOTE_ERROR_ER_RENT,
    spr_remote_error_er_term                = SPR_REMOTE_ERROR_ER_TERM,
    spr_remote_error_er_im                  = SPR_REMOTE_ERROR_ER_IM
} ServerCommandState;

//#define SPR_STATE_

class TCPCommand;

class ServerConnect: public QTcpSocket{

    Q_OBJECT
    
    QQueue<TCPCommand*> sendData;
    QMutex mutex;
    
    TCPCommand *current;
    TCPCommand *getStateCommand;
    QByteArray replay;
    QTimer timerGetStateCommand;

    SPRSeparatorLogsModel *logModel;

    ServerCommandState lastCommandError;

    uint32_t currentState;
    uint32_t setState(uint32_t _state);

protected:
    QString name;
    uint16_t port;
    SPRVariable<uint> *vPort;
    SPRQStringVariable *vName;

    bool connected;
    void setError(TCPCommand *command);

    void changeRemoteState(QByteArray replay);
    bool noErrorsInReplay(QByteArray *_replay = nullptr);
    uint32_t addStates(QSet<ServerConnectState> _states);
public:
    ServerConnect(QString _name, uint16_t _port, SPRQStringVariable *_vName = nullptr, SPRVariable<uint> *_vPort = nullptr, bool _connectingNow = true);
    virtual ~ServerConnect();
    
    void timerStart(){timerGetStateCommand.start();}
    void timerStop(){timerGetStateCommand.start();}

    typedef enum serverTimerType{
        fastTypeServerTimer, slowTypeServerTimer, noServerTimer
    } ServerTimerType;


    int getLastStateCommand();

    void setTimer(ServerTimerType _type = fastTypeServerTimer);

    void addSendCommand(TCPCommand* _sender);
    void setPort(uint16_t port);
    uint16_t getPort() const;
    void setName(QString name);
    QString getName() const;

    uint32_t getState(){ return currentState;}
    uint32_t addState(ServerConnectState _state);
    uint32_t clearState(ServerConnectState _state);

    void serverReconnect();
    void serverDisconnect();
    bool isState(ServerConnectState _state);

    void clearQueue();
    SPRSeparatorLogsModel *getLogModel();
    void setLogModel(SPRSeparatorLogsModel *value);

    SPRVariable<uint> *getVPort() const;
    void setVPort(SPRVariable<uint> *value);

    SPRQStringVariable *getVName() const;
    void setVName(SPRQStringVariable *value);

    bool getConnected() const;
    void setConnected(bool value);

    void stateToDebug();
public slots:
    void queueComplite();
    void onChangeModelData(IModelVariable *var);

protected slots:    
    void onReadyRead();
    void onTimeOutGetState();
    void onServerStateChange(uint32_t _state);
    void onMyGetStateCommandComplite(TCPCommand *_command);
signals:
    void addData();
    void commandComlite(TCPCommand*);
    void commandError(TCPCommand*, ServerCommandState);
    void serverConnectTimeOutError(TCPCommand*);
    void serverReadWriteTimeOutError(TCPCommand*);
    void serverStateChange(uint32_t);
    void serverStateChangeOut(uint32_t);


};

#endif /* SERVERCONNECT_H */

