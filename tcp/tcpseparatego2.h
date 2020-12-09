#ifndef TCPSEPARATEGO2_H
#define TCPSEPARATEGO2_H

#include <QObject>
#include <QWidget>
#include <QTimer>

#include "tcp/TCPCommandSet.h"
#include "tcpgetspectrumsgistogramms.h"
#include "tcp/tcpcommandseparatoroff.h"

#include "tcp/tcpteststopseparate.h"

class SPRMainModel;

class TCPSeparateGo2 : public TCPCommandSet
{
    Q_OBJECT

    QTimer selfTimer;
    qulonglong countTimer;

    TCPGetSpectrumsGistogramms *kspectCommand;
    TCPGetSpectrumsGistogramms *histCommand;
    TCPCommand *getseparCommand;

    TCPCommand *getStateCommand;

//    TCPTestStopSeparate *stopSeparateCommand;
    TCPCommandSeparatorOff *stopSeparator;

    SPRMainModel *model;

//    uint16_t lastState;

public:
    TCPSeparateGo2(SPRMainModel *_model);
    TCPGetSpectrumsGistogramms *getKspectCommand() const;
    TCPGetSpectrumsGistogramms *getHistCommand() const;
    TCPCommand *getGetseparCommand() const;


    void stopSeparate();
    // TCPCommand interface
public slots:
    void go(TCPCommand *_command);

    // TCPTimeOutCommand interface
public slots:
    void onTimeOut();

signals:
    void separateDataReady(TCPCommand *command);
    void gistogrammsDataReady(TCPCommand *command);
    void kspectrumsDataReady(TCPCommand *command);

    // TCPCommandSet interface
protected slots:
    void onCommandComplite(TCPCommand *_command);

    // ITCPCommand interface
public:

    // TCPCommandSet interface
public:
    virtual bool isProcessRun() const;
};

#endif // TCPSEPARATEGO2_H
