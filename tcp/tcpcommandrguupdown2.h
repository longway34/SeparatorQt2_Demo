#ifndef TCPCOMMANDRGUUPDOWN2_H
#define TCPCOMMANDRGUUPDOWN2_H

#include <QObject>
#include "tcp/TCPCommandSet.h"
#include "models/sprmainmodel.h"

typedef enum{notBegin = 0, commandSend, movedContinue, movedEnd} SPRRGUStatusCommand;

class TCPCommandRGUUpDown2 : public TCPCommandSet
{
    Q_OBJECT

    SPRTypeRGUMoved rguMoved;
    char sendData;

//    uint8_t rguCommand;
    SPRRGUStatusCommand status;  // 0 - work not begin; 1 - need move


    TCPCommandSet *stopRgu;

//    TCPTimeOutCommandWC *timeoutCommand;

    QString toMessage;
    QString toTitle;
    QString quiestMessage;
    QString quiestLogMessage;
    QString errorMsg;
    QString errorLogMsg;

    int countTemplate;
    int countTemplateErr;

    bool withQuestion;
    bool sysErrorCancel;

    SPRMainModel *model;

//    TCPCommand *setRgu;
//    TCPCommand *stopRgu;

    void addWaiting();
public:
    TCPCommandRGUUpDown2(SPRMainModel *_model, TCPTimeOutWidget *_towidget, SPRTypeRGUMoved _rguMoved, bool _withQuestion = true);
//    TCPCommandRGUUpDown2(ServerConnect *_server, TCPTimeOutWidget *_towidget, SPRTypeRGUMoved _rguMoved);
    ~TCPCommandRGUUpDown2();

    // TCPCommand interface
//    bool getStarted() const;
//    void setStarted(bool value);

public slots:
    virtual void go(TCPCommand *_command);

    // TCPTimeOutCommand interface
public slots:
    void onCancelCommand(TCPCommand *);

    // TCPCommandSet interface
public:
    SPRSeparatorLogsModel *getLogModel() const;
};

#endif // TCPCOMMANDRGUUPDOWN2_H
