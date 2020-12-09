#ifndef TCPCOMMANDSTARTSETPITATEL_H
#define TCPCOMMANDSTARTSETPITATEL_H

#include <QObject>
#include <tcp/TCPCommandSet.h>
#include "variables/sprvariable.h"

class SPRMainModel;

class TCPCommandStartPitatel :public TCPCommandSet
{
//    bool pitatelWorking;

    SPRMainModel *model;
    SPRVariable<uint16_t> *VEMSBeginCode;

public:
    TCPCommandStartPitatel();
    TCPCommandStartPitatel(ServerConnect *_server, SPRMainModel *_model, TCPTimeOutWidget *_widget = nullptr);

//    void setModelVariable(SPRVariable<uint16_t>* _VEMSBeginCode){
//        VEMSBeginCode = _VEMSBeginCode;
//    }

public slots:
//    void setPercents(uint16_t _persents);

    // TCPCommand interface
public slots:
    virtual void go(TCPCommand *_command);
//    void setPitatelStopFlag(){pitatelWorking = false;}
};



#endif // TCPCOMMANDSTARTSETPITATEL_H
