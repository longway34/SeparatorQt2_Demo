#ifndef TCPCLEARSTATES_H
#define TCPCLEARSTATES_H

#include <QObject>
#include <QWidget>

#include "tcp/TCPCommand.h"
//#include "tcp/TCPCommandSet.h"

class TCPClearStates : public TCPCommand
{
    Q_OBJECT

public:
    TCPClearStates();

    // TCPCommand interface
public:


    // TCPCommand interface
public:
    virtual void send(ServerConnect *server);
};

#endif // TCPCLEARSTATES_H
