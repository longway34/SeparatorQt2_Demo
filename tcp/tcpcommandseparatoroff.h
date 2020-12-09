#ifndef TCPCOMMANDSEPARATORON_H
#define TCPCOMMANDSEPARATORON_H

#include <QObject>
#include "tcp/TCPCommandSet.h"
#include "models/sprsettingsrentgenmodel.h"
#include "tcp/tcpstoprudostuskcommand.h"

class TCPCommandSeparatorOff : public TCPCommandSet
{
    TCPStopRudostuskCommand *stopRudosrusk;

public:
    TCPCommandSeparatorOff();
    TCPCommandSeparatorOff(ServerConnect *_server, TCPTimeOutWidget *_widget);

public slots:
    virtual void go(TCPCommand *_command);
};



#endif // TCPCOMMANDSEPARATORON_H
