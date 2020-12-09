#ifndef TCPCOMMANDSTOPPITATEL_H
#define TCPCOMMANDSTOPPITATEL_H

#include <QObject>
#include "tcp/TCPCommandSet.h"

class TCPCommandStopPitatel : public TCPCommandSet
{
public:
    TCPCommandStopPitatel();
    TCPCommandStopPitatel(ServerConnect *_server, TCPTimeOutWidget *_widget = nullptr);

    // TCPCommand interface
public slots:
    virtual void go(TCPCommand *_command);
};



#endif // TCPCOMMANDSTOPPITATEL_H
