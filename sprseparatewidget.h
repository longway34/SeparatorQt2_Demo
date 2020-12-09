#ifndef SPRSEPARATEWIDGET_H
#define SPRSEPARATEWIDGET_H

#include "ui_sprseparatewidget.h"
#include "isprwidget.h"

#include "models/sprmainmodel.h"
#include "isprwidget.h"
#include "sprsettingsporogswidget.h"
#include "models/sprhistorymodel.h"

#include "tcp/tcpstartseparate2.h"
#include "tcp/tcpteststopseparate.h"

#include "sprseparatecontrolwidget.h"
#include "sprporogswidget2.h"


class SPRSeparateWidget : public QWidget, public ISPRWidget
{
    Q_OBJECT

    SPRMainModel *model;
    SPRHistoryModel *history;
    SPRPorogsWidget2 *wPorogs;
    SPRSeparatorLogsModel *logModel;

    SPRSpectrumListItemsModel *kSpectrumModel;

//    TCPStartSeparate2 *startSeparate;
//    TCPTestStopSeparate *stopSeparate;

    QDateTime lastKSpect;
    QDateTime lastGist;
    QDateTime lastGetSpect;

//    TCPGetSpectrumsGistogramms *forKSpertCommands;
//    TCPGetSpectrumsGistogramms *forHistCommands;

    TCPCommand *setSeparate;

//    TCPCommandSeparatorOnFull *separatorOn;
    TCPTimeOutWidget *toWidget;

public:
    Ui::SPRSeparateWidget ui;
    explicit SPRSeparateWidget(QWidget *parent = nullptr);

private:

    // ISPRWidget interface
public:
    virtual ISPRModelData *setModelData(ISPRModelData *data);
    virtual ISPRModelData *getModelData();

    SPRPitatelControlWidget *getPitagelControl();
    SPRSeparateControlWidget *getSeparateControl();
public slots:
    virtual void onModelChanget(IModelVariable *);
    virtual void widgetsShow();

    void onChangePororsCommand(bool);
protected:

    // ISPRWidget interface
    void onSpectrumReady(TCPGetSpectrumsGistogramms *_command);
    void setGraphicTitle();
public:
    virtual void setLogModel(SPRSeparatorLogsModel *value);
protected slots:
    void onCommandComplite(TCPCommand *command);
    void onSeparateCommandComplite(TCPCommand *command);
    void onCommandButtomClick(bool);
    void onServerStateChange(uint32_t);
    void onDblClickMouseEvent();

    // ISPRWidget interface
public:
    virtual void storeWidgetsStates(QString groupId, QSettings *mainSettings);
    virtual void restoreWidgetsStates(QString groupId, QSettings *mainSettings);
    bool isStartStopProcess();
};

#endif // SPRSEPARATEWIDGET_H
