#ifndef TCPTESTSTARTSEPARATE_H
#define TCPTESTSTARTSEPARATE_H

#include <QObject>
#include "tcp/TCPCommandSet.h"
#include "tcp/tcpechologswidget.h"
#include "models/sprseparatemodel.h"
#include "models/sprmainmodel.h"
#include "tcp/tcpseparatego.h"
#include "tcp/tcpgetspectrumsgistogramms.h"
#include "tcp/tcpcommandseparatoronfull.h"
#include "tcp/tcpcommandseparatoroff.h"
#include "tcp/tcpteststopseparate.h"

class TCPTestStartSeparate : public TCPCommandSet
{
    Q_OBJECT


    SPRSeparateModel *separateModel;
    SPRMainModel *mainModel;
    int countTry;
    int numTry;

    int errorSeparateState;

    TCPSeparateGo *separateGoCommand;
//    TCPCommandSeparatorOff *separatorStop;
    TCPTestStopSeparate *separateStop;
    TCPCommandSeparatorOff *separatorStop;
    TCPGetSpectrumsGistogramms *getBaseSpectrumCommand;
public:
    TCPTestStartSeparate(ServerConnect *_server, SPRMainModel *_model, TCPTimeOutWidget *_widget, TCPEchoLogsWidget *log = nullptr);

    SPRSeparateModel *getSeparateModel() const;
    void setModeDatal(SPRMainModel *value);
    SPRMainModel *getMainModel() const;

    // TCPCommand interface
    TCPSeparateGo *getSeparateGoCommand() const;

    TCPGetSpectrumsGistogramms *getGetBaseSpectrumCommand() const;

    SPRMainModel *getModelData() const;
public slots:
    void go(TCPCommand *_command);

signals:
    void errorCommand(TCPCommand*);
    void rentgenReady(TCPCommand*);
};

#endif // TCPTESTSTARTSEPARATE_H
