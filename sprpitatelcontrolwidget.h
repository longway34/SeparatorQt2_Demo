#ifndef SPRPITATELCONTROLWIDGET_H
#define SPRPITATELCONTROLWIDGET_H

#include "ui_sprpitatelcontrolwidget.h"
#include "models/sprmainmodel.h"
#include "tcp/tcpcommandstartpitatel.h"
#include "tcp/tcpcommandstoppitatel.h"
#include "tcp/tcpcommandseparatoronfull.h"
#include "tcp/tcpcommandseparatoroff.h"
#include "tcp/tcpteststopseparate.h"
#include "tcp/tcpstartrudospuskcommand.h"
#include "tcp/tcpstoprudostuskcommand.h"
#include "isprwidget.h"
#include "models/sprsettingscontrolmodel.h"

#include "sprseparatoronmodedialog.h"

#include <QAccessibleStateChangeEvent>
#include <QSpinBox>

class SPRPitatelControlWidget : public QWidget, public ISPRWidget
{
    Q_OBJECT

    SPRMainModel *mainModel;

    TCPCommandStartPitatel *startPitatelCommand;
    TCPCommandStopPitatel *stopPitatelCommand;
    TCPCommand *setPitatelCommand;
    TCPStartRudospuskCommand *startRugospusk;
    TCPStopRudostuskCommand *stopRudosrusk;


    TCPCommandSeparatorOnFull *separatorOn;
//    TCPTestStopSeparate *separatorOff;
    TCPCommandSeparatorOff *separatorOff;
    TCPTimeOutWidget *toWidget;

    SPRSeparatorOnModeDialog *onModeDialog;

    bool rudospuskFullMode;
    bool separatorControlVisible;
    bool separatorRentgenOn;
    bool isPitatelStopManual;

    QIcon iconStart;
    QIcon iconStop;
//    QIcon iconSeparatorOn;
//    QIcon iconSeparatorOnFullMode;
//    QIcon iconSeparatorOff;
//    QIcon iconSeparatorOffFullMode;

public:
    explicit SPRPitatelControlWidget(QWidget *parent = nullptr);
    virtual ~SPRPitatelControlWidget(){

    }

    virtual ISPRModelData *setModelData(ISPRModelData *_mainModel);
    bool isStartStopProcess();
private:
    Ui::SPRPitatelControlWidget ui;

    // ISPRWidget interface
public:
    virtual ISPRModelData *getModelData();

public slots:
    virtual void widgetsShow();

protected slots:
    virtual void onModelChanget(IModelVariable *send);

    void onCommandComplite(TCPCommand *);
    void onCommandNotComplite(TCPCommand *){

    }
    void onServerStateChange(uint32_t);
    void onCommandButtomClickBool(bool);

    void onCommandButtomClickInt(int);

    // ISPRWidget interface
public:
    virtual void setLogModel(TCPEchoLogsWidget *value);

public:
    void setFullMode(bool valFulControl, bool valSeparatorControlVisibleMode);

    TCPCommandStartPitatel *getStartPitatelCommand() const;

    TCPCommandStopPitatel *getStopPitatelCommand() const;

    TCPStartRudospuskCommand *getStartRugospusk() const;

    TCPStopRudostuskCommand *getStopRudospusk() const;

    TCPCommandSeparatorOnFull *getSeparatorOn() const;

    TCPCommandSeparatorOff *getSeparatorOff() const;

    void showStartButton();
    void separatorOnOff(bool onOff, bool _toVisible = true, bool _fullMode = true);
    void setSeparatorControlVisible(bool value);

    void setVerticalMode(bool onOff);
signals:
    void pitatelStartComplite(TCPCommand*);
    void pitatelStopComplite(TCPCommand*);
    void rudospuskStartComplite(TCPCommand*);
    void rudospuskStopComplite(TCPCommand*);
    void separatorOnComplite(TCPCommand*);
    void separatorOffComplite(TCPCommand*);
};

#endif // SPRPITATELCONTROLWIDGET_H
