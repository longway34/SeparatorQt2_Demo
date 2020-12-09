#ifndef TCPSTARTRUDOSPUSKCOMMAND_H
#define TCPSTARTRUDOSPUSKCOMMAND_H

#include <QObject>
#include "tcp/TCPCommandSet.h"
#include "tcpcommandstartpitatel.h"

class SPRMainModel;

class TCPStartRudospuskCommand : public TCPCommandSet
{
    SPRMainModel *model;
public:
    TCPStartRudospuskCommand();
    TCPStartRudospuskCommand(ServerConnect *_server, SPRMainModel *_model, TCPTimeOutWidget *_widget);

    // TCPCommand interface
public slots:
    virtual void go(TCPCommand *_command);
};



#endif // TCPSTARTRUDOSPUSKCOMMAND_H
