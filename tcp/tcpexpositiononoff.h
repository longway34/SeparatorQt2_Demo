#ifndef TCPEXPOSITIONONOFF_H
#define TCPEXPOSITIONONOFF_H

#include <QObject>

#include "tcp/TCPCommandSet.h"
#include "tcp/tcpechologswidget.h"

#include "models/sprmainmodel.h"

class TCPExpositionOnOff : public TCPCommandSet
{
    Q_OBJECT

    SPRMainModel *model;
    bool onOff;
    bool offIfError;

public:
    TCPExpositionOnOff(ServerConnect *_server, bool _onOff, bool _offIfError=true, SPRMainModel *_model = nullptr,  TCPTimeOutWidget *_toWidget = nullptr, TCPEchoLogsWidget *logWidget=nullptr);
    SPRMainModel *getModelData() const;
    void setModelData(SPRMainModel *value);

    // TCPCommand interface
public slots:
    virtual void go(TCPCommand *_command);
protected:
    const QByteArray getDeuCodes(bool deu_cp);
};

#endif // TCPEXPOSITIONONOFF_H
