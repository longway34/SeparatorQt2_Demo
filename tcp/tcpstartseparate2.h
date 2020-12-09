#ifndef TCPSTARTSEPARATE2_H
#define TCPSTARTSEPARATE2_H

#include <QObject>
#include "models/sprmainmodel.h"
#include "tcp/TCPCommandSet.h"
#include "tcp/tcpechologswidget.h"
#include "tcp/tcptimeoutwigget.h"
//#include "tcpgetspectrumsgistogramms.h"
//#include "tcp/tcpseparatego.h"
#include "tcp/tcpseparatego2.h"
#include "tcp/tcpteststopseparate.h"
#include "tcp/tcpgetspectrums2.h"

class TCPStartSeparate2 : public TCPCommandSet
{

    Q_OBJECT

    SPRMainModel *model;
//    TCPGetSpectrumsGistogramms *getBaseSpectrum;
    TCPGetSpectrums2 *getBaseSpectrum;
    TCPSeparateGo2 *separateGo;
    TCPTestStopSeparate *separatorStop;

    int errorSeparateState;

    int countTry;
    int numTry;

public:

    TCPStartSeparate2(SPRMainModel *_model, TCPTimeOutWidget *_toWidget=nullptr, SPRSeparatorLogsModel *_logModel=nullptr);



    // TCPCommand interface
    TCPGetSpectrumsGistogramms *getKSpectrunsCommand();
    TCPGetSpectrumsGistogramms *getHistogrammCommand();
    TCPGetSpectrums2 *getGetBaseSpectrum() const;

    TCPSeparateGo2 *getSeparateGo() const;
    bool isStartStopProcess();

public slots:
    virtual void go(TCPCommand *_command);
    void onGetRenComplite(TCPCommand *command);
    void onBaseSpectrumsReady(TCPCommand *command);
    void onGistorgammsDataReady(TCPCommand *command);
    void onSeparateDataReady(TCPCommand *command);
    void onKSpectrumsDataReady(TCPCommand *command);
    void onGetRenNotComplite(TCPCommand *command);

signals:
    void rentgenNotReady(TCPCommand *command);

    void kSpactrumReady(TCPCommand *command);
    void gistogrammReady(TCPCommand *command);
    void separateDataReady(TCPCommand *command);
};

#endif // TCPSTARTSEPARATE2_H
