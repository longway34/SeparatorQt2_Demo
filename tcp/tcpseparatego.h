#ifndef TCPSEPARATEGO_H
#define TCPSEPARATEGO_H

#include <QObject>
#include "tcp/TCPCommandSet.h"
#include "models/sprmainmodel.h"
#include "tcp/tcpechologswidget.h"
#include "tcp/tcpgetspectrumsgistogramms.h"
#include <QDateTime>

class TCPSeparateGo : public TCPCommandSet
{
    Q_OBJECT

    SPRMainModel *model;
    TCPEchoLogsWidget *logWidget;
    uint32_t toCount;
    QDateTime dtime;
    QString sdt;
    struct {uint8_t error;
            uint8_t state;} stateResult;

    TCPGetSpectrumsGistogramms *kspectCommand;
    TCPGetSpectrumsGistogramms *histCommand;
    TCPCommand *getseparCommand;
public:
    TCPSeparateGo();
    TCPSeparateGo(SPRMainModel *_model, TCPEchoLogsWidget *log=nullptr);

    SPRMainModel *getModel() const;
    void setModel(SPRMainModel *value);

    // TCPCommand interface
    void setLogWidget(TCPEchoLogsWidget *value);

    TCPGetSpectrumsGistogramms *getKspectCommand() const;

    TCPGetSpectrumsGistogramms *getHistCommand() const;

    TCPCommand *getGetseparCommand() const;

public slots:
    virtual void go(TCPCommand *_command);


signals:
    void separateDataReady(TCPCommand *command);
    void gistogrammsDataReady(TCPCommand *command);
    void kspectrumsDataReady(TCPCommand *command);

    // TCPTimeOutCommand interface
public slots:
//    virtual void onTimeOut();
};





#endif // TCPSEPARATEGO_H
