#ifndef SPRSEPARATECONTROLWIDGET_H
#define SPRSEPARATECONTROLWIDGET_H

#include "ui_sprseparatecontrolwidget.h"
#include "models/sprmainmodel.h"
#include "models/sprspectrumlistitemsmodel.h"
#include "tcp/tcpstartseparate2.h"
#include "tcp/tcpteststopseparate.h"
#include "tcp/tcpgetspectrums2.h"


#include "isprwidget.h"

class SPRHistoryModel2;
class SPRKSpectrumSeparateSummary;

class SPRSeparateControlWidget : public QWidget, public ISPRWidget
{
    Q_OBJECT

    SPRMainModel *model;
    SPRHistoryModel2 *histModel;

    SPRSpectrumListItemsModel *kSpectrumModel;

    TCPStartSeparate2 *startSeparate;
    TCPTestStopSeparate *stopSeparate;

    TCPCommandSeparatorOff *stopSeparator;
    TCPCommand *setSeparate;

    TCPTimeOutWidget *toWidget;
//    TCPEchoLogsWidget *logWidget;

//    SPRSeparatorLogsModel *logModel;

    TCPGetSpectrums2 *getBaseSpectrumsCommand;

    QTimer separateTimer;
    QDateTime lastTimerUpdate;

    bool flagSeparateGo;

    int spectWarningLoadAirSend;
    int spectWarningCorrelSend;
    int spectWarningDiffSend;
    int speedStoneErrSend;

//    bool transportWarningSend;

    void clearErrorsCounts();

public:
    Ui::SPRSeparateControlWidget ui;
    explicit SPRSeparateControlWidget(QWidget *parent = nullptr);

    void setModelData(SPRMainModel *_model, TCPTimeOutWidget *_toWidget, SPRSeparatorLogsModel *_logModel);

    SPRSpectrumListItemsModel *getKSpectrumModel() const;

    void setKSpectrumModel(SPRSpectrumListItemsModel *value);

    TCPTimeOutWidget *getTimeOutWidget() const;

private:

    bool isErrorsOnSpectrumReady(QVector<SPRKSpectrumSeparateSummary> *vSpectSummary);
public slots:
    void onServerStateChange(uint32_t);
    void onTCPCommandComplite(TCPCommand *command);
    void onTCPCommandNotComplite(TCPCommand *command);
    void onModelChanget(IModelVariable *);
    void onCommandButtomClick(bool);
    void onTimeOut();
protected:
//    void onSpectrumsReadyParse(TCPGetSpectrumsGistogramms *_command, bool isKSpectrum = true, uint32_t _time_scope_in_msec = 0);

    // ISPRWidget interface
    void separateGo();
public:
    virtual void widgetsShow();
    virtual ISPRModelData *getModelData();
    void separateStartStop(bool onOff, bool toVisible = true);
    bool isStartStopProcess();
protected slots:
    void onKSpectrumReady(TCPCommand *_command);
    void onGistogrammReady(TCPCommand *command);
    void onSeparateDataReady(TCPCommand *command);
};


#endif // SPRSEPARATECONTROLWIDGET_H
