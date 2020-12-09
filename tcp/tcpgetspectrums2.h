#ifndef TCPGETSPECTRUMS2_H
#define TCPGETSPECTRUMS2_H

#include <QObject>
#include <QWidget>

#include "tcp/TCPCommandSet.h"
#include "tcp/tcpexpositiononoff.h"
#include "tcp/tcpcommandseparatoronfull.h"
#include "tcp/tcpcommandrguupdown2.h"
#include "tcp/tcptimeoutwigget.h"
#include "tcp/tcpteststopseparate.h"

#include "models/sprmainmodel.h"

class TCPGetSpectrums2 : public TCPCommandSet
{
    Q_OBJECT


    TCPCommandSeparatorOnFull *separatorOn;
    TCPExpositionOnOff *expositionOn;
    TCPExpositionOnOff *expositionOff;
    TCPTestStopSeparate *separatorStop;
    TCPCommandRGUUpDown2 *rguMovedCommand;

    SPRMainModel *model;

    SPRTypeRGUMoved rguMoved;
    bool offExposition;
    bool onExposition;

    SPRThreadList threads;
    double timeScope_in_sec;

    QByteArray codesDEU, codesCP;

public:
    TCPGetSpectrums2(SPRMainModel *_model, double _timeScope_in_sec, SPRTypeRGUMoved _moved, SPRThreadList ths, bool _offExposition, TCPTimeOutWidget *_toWidget=nullptr);

    SPRThreadList getThreads() const;
    void setThreads(const SPRThreadList &value);
    bool getOffExposition() const;
    void setOffExposition(bool value);
    SPRTypeRGUMoved getRguMoved() const;
    void setRguMoved(const SPRTypeRGUMoved &value);
    SPRMainModel *getModel() const;
    void setModel(SPRMainModel *value);

    // TCPCommand interface
    QByteArray getSpectrumData(int thread);
    QByteArray getCodesDEU() const;
    void setCodesDEU(const QByteArray &value);

    QByteArray getCodesCP() const;
    void setCodesCP(const QByteArray &value);

    void setOnExposition(bool value);

public slots:
    void go(TCPCommand *_command);
    //    void onCommandNotComplite(TCPCommand *_command);

    // TCPTimeOutCommand interface
public slots:
//    void onCancelCommand(TCPCommand *_command);
};

#endif // TCPGETSPECTRUMS2_H
