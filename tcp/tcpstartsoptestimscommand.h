#ifndef TCPSTARTSOPTESTIMSCOMMAND_H
#define TCPSTARTSOPTESTIMSCOMMAND_H

#include <QObject>
#include "tcp/TCPCommandSet.h"


#include <QMessageBox>

class TCPStartSopTestIMSCommand : public TCPCommandSet
{
    QVector<uint8_t> ims;
    uint16_t delay; // msec.
    uint16_t timeWork; // sec.
    uint16_t freq; // Gz
    int countIms;

    bool running;

    TCPCommand *stopTestCommand;
    TCPCommand *getStateCommand;
public:
    TCPStartSopTestIMSCommand();
    TCPStartSopTestIMSCommand(ServerConnect *_server, TCPTimeOutWidget *_widget);

    void setParams(QVector<uint8_t> _ims, int _delay, int _timeWork, int _freq);

    // TCPCommand interface
    virtual QByteArray getReplayData();
    virtual QByteArray getResult();
    TCPCommand *getStopTestCommand() const;

public slots:
    virtual void go(TCPCommand *_command);
    void stopTest();

    // TCPTimeOutCommand interface
public slots:
    void onCancelCommand(TCPCommand *var);
};



#endif // TCPSTARTSOPTESTIMSCOMMAND_H
