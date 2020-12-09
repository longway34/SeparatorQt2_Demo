#ifndef SPRSETTINGSRENTGENAUTOSETDIALOG_H
#define SPRSETTINGSRENTGENAUTOSETDIALOG_H

#include "ui_sprsettingrentgenautosetdialog.h"

#include "models/sprmainmodel.h"
#include "tcp/tcpautosetrentgen.h"
#include "tcp/tcpautosetcommandgo.h"
//#include "tcp/tcpcommandseparatoroff.h"
#include "tcp/tcpteststopseparate.h"
#include "tcp/tcpgetspectrums2.h"

#include "tcp/tcpexpositiononoff.h"
#include "tcp/tcpclearstates.h"

#include "firstcollumn2.h"

#include "isprwidget.h"

#include "qwt_legend.h"

class SPRSettingsRentgenAutoSetDialog : public QDialog, public ISPRWidget
{
    Q_OBJECT

    SPRSettingsRentgenModel *rentgenModel;
    SPRMainModel *mainModel;
    SPRSpectrumListItemsModel *spectrums;

    SPRThreadList threads;
//    QVector<uint16_t> codesDEU, codesCP;

    bool useGRU;

    TCPGetSpectrums2 *startCommand;
    TCPAutosetCommandGo *autoSetGo;

    TCPTestStopSeparate *separatorOff;
    TCPExpositionOnOff *expositionOff;
    TCPClearStates *clearStates;

//    uint tempPeakPosition;

    TCPTimeOutWidget *toWidget;

    QwtPlotGrid *grid;
    QwtLegend *legend;


    int countSteps;

    QVector<uint16_t> tempDEU;
    QVector<uint16_t> tempCP;

    QByteArray tempDEU2Command;
    QByteArray tempCP2Command;

    void settingBACodes();

    QVector<bool> deuOK;
    QVector<bool> cpOK;
    QVector<int> controlCount;
    QVector<int> vectorMoveDeu;
    QVector<int> stepDeu;

    double controlDEU, controlCP, controlAll;
public:
    QVector<QPointF> peakData;
    QwtPlotCurve *peakCurve;

    explicit SPRSettingsRentgenAutoSetDialog(QWidget *parent = 0);
    Ui::SPRSettingRentgenAutoSetDialog ui;

    virtual ~SPRSettingsRentgenAutoSetDialog();

    virtual ISPRModelData *getModelData();
    virtual ISPRModelData *setModelData(ISPRModelData *value);

    int currentViewThread;

    QPen defPen, selPen;

    SPRThreadList getThreads();
    void setThreads(SPRThreadList &value);
    void setGraphicTitle();

    bool finish;

    TCPTimeOutWidget *getToWidget() const;
    void setToWidget(TCPTimeOutWidget *value);

public slots:
    virtual void widgetsShow();

    void onCommandComplite(TCPCommand *_command);
    void onClickButtons(bool);
    void onCusorOverSelectItem(QwtPlotItem *item, MovedItemPosition);
    void onChangeSelectedItemValue(QwtPlotItem *item, double distance, MovedItemPosition position);
    void onCommandNotComplite(TCPCommand *send);
    void onCancelCommand(TCPCommand*);
    void onServerStateChange(uint32_t);
    void onChangePeakPosition(int);
protected:
//    bool isDeuCpAllOK(uint8_t th = 255, QVector<bool> *_src = nullptr);
    double prDeuCpOKAll(QVector<bool> *_src = nullptr);
    void process();
private:

    QByteArray codesToByteArray(uint16_t *table);
protected slots:
    virtual void onModelChanget(IModelVariable *var);
    void onDblClickMouseEvent();

};

#endif // SPRSETTINGSRENTGENAUTOSETDIALOG_H
