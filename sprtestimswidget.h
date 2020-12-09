#ifndef SPRTESTIMSWIDGET_H
#define SPRTESTIMSWIDGET_H

#include "ui_sprtestimswidget.h"
#include "models/sprmainmodel.h"
#include "isprwidget.h"
#include "tcp/TCPCommandSet.h"
#include "tcp/tcptimeoutwigget.h"
#include "tcp/tcpcommandrguup.h"
//#include "tcp/tcpcommandrguupdown.h"

#include "tcp/tcpcommandrguupdown2.h"

#include "tcp/tcpstartsoptestimscommand.h"
#include "tcp/tcpcommandstartpitatel.h"
#include "tcp/tcpcommandstoppitatel.h"
#include "tcp/tcpcommandseparatoroff.h"
#include "tcp/tcpcommandrentgersetup.h"
//#include "tcp/tcpcommandgetspectrums.h"
//#include "tcp/tcpgetspectrumsgistogramms.h"
#include "tcp/tcpstartrudospuskcommand.h"
#include "tcp/tcpstoprudostuskcommand.h"
#include "tcp/tcpgetspectrums2.h"

#include "tcp/tcpteststopseparate.h"
#include "tcp/tcpcommandseparatoronfull.h"

#include "tcp/tcpgetsetthermo.h"
#include "sprsettingsrentgenautosetdialog.h"

class SPRTestIMSWidget : public QWidget, public ISPRWidget
{
    Q_OBJECT

    SPRMainModel *model;

    bool spectrumsOk;
    bool masterMode;

    SPRSettingsRentgenAutoSetDialog *autoSettingDialog;

public:
    explicit SPRTestIMSWidget(QWidget *parent = nullptr);

    bool isSpectrumsOk();
    bool setSpectrumsOk(bool value);

    bool isMasterMode();
    void setMasterMode(bool mode);

//    TCPCommandGetSpectrums *getSpectrumsCommand;
//    TCPGetSpectrumsGistogramms *getSpectrumsCommand;
    TCPGetSpectrums2 *getSpectrumsCommand;

//    TCPCommandSeparatorOnFull *separatorOnCommand;
//    TCPTestStopSeparate *separatorOffCommand;

    TCPCommandSeparatorOnFull *rentgenOnCommand;
    TCPTestStopSeparate *rentgenOffCommand;

    TCPCommandSet *rguReadStateCommand;
    TCPCommandRGUUpDown2 *rguUpCommand;
    TCPCommandRGUUpDown2 *rguDownCommand;
    TCPCommand *rguStop;

//    TCPCommandSet *thermoReadStateCommand;
//    TCPCommandSet *thermoWriteStateCommand;
    TCPGetSetThermo *thermoReadStateCommand;
    TCPGetSetThermo *thermoWriteStateCommand;

    TCPStartSopTestIMSCommand *startTestImsCommand;
//    TCPCommand *stopTestImsCommand;

//    TCPCommandStartPitatel *commandStartPitatel;
//    TCPCommandStopPitatel *commandStopPitatel;
//    TCPCommand *commandChangePersentPitatel;

    TCPCommand *commandStartRasklad;
    TCPCommand *commandStopRasklad;


//    TCPStartRudospuskCommand *commandStartRudostusk;
//    TCPStopRudostuskCommand *commandStopRudospusk;

//    TCPCommandSet *spectrumGetCommand;

    TCPTimeOutWidget *towidget;

    void setThermo();

    void stopRGUNow(){
        rguUpCommand->onCancelCommand(nullptr);
        rguDownCommand->onCancelCommand(nullptr);
//        rguUpCommand->setFinish(true);
    }

private:
    Ui::SPRTestIMSWidget ui;
    QVector<QCheckBox*> vectorIms;
    QVector<QWidget*> vectorSeparatorOnEnabled;
    QVector<QWidget*> vectorSeparatedProcessDisabled;
    // ISPRWidget interface
public:
    ISPRModelData *setModelData(SPRMainModel *_model);

    virtual void widgetsShow();
    virtual ISPRModelData *getModelData();
public slots:
    void onCommandComplite(TCPCommand* _comm);
    void onCommandNotComplite(TCPCommand* _comm);
    void onCancelCommand(TCPCommand *comm);
    void onCommand(bool);
//    void onServerConnectError(ITCPCommand *);
//    void onServerReadWriteError(ITCPCommand *_comm);
    void onServerConnectChangeState(uint32_t _state);

    virtual void onChangeValue(bool _val);
    virtual void onChangeValue(double _val);
    virtual void onModelChanget(IModelVariable *var);

    // ISPRWidget interface
protected:

    // ISPRWidget interface
    void mainConnect(TCPCommand *command);
public:
    virtual void setLogModel(TCPEchoLogsWidget *value);
protected slots:
    void onChangeIntValue(int value);
};

#endif // SPRTESTIMSWIDGET_H
