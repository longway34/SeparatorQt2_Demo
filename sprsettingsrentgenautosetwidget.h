#ifndef SPRSETTINGSRENTGENAUTOSETWIDGET_H
#define SPRSETTINGSRENTGENAUTOSETWIDGET_H

#include "ui_sprsettingsrentgenautosetwidget.h"
#include "models/sprsettingsrentgenmodel.h"
#include "tcp/tcpteststopseparate.h"
#include "tcp/tcpcommandseparatoroff.h"

#include "sprsettingsrentgenautosetdialog.h"
#include "isprwidget.h"

#include "isprsettingswidget.h"

class SPRSettingsRentgenAutosetWidget : public QWidget, public ISPRWidget, public ISPRSettingsWidget
{
    Q_OBJECT

    SPRSettingsRentgenModel *model;

    QButtonGroup *bgTypeSetting;

    SPRSettingsRentgenAutoSetDialog *autoSettingDialog;

    TCPCommandSeparatorOff *stopRentgen;
public:
    explicit SPRSettingsRentgenAutosetWidget(QWidget *parent = nullptr);

    ISPRModelData *setModelData(SPRSettingsRentgenModel *value);

private:
    Ui::SPRSettingsRentgenAutosetWidget ui;

public:
    virtual ISPRModelData *getModelData();

public slots:
    virtual void widgetsShow();

    // ISPRWidget interface
protected slots:
//    virtual void viewChange(QTableWidget *, int, int);
    virtual void viewChange(bool value);
    virtual void viewChange(int value);
    virtual void onModelChanget(IModelVariable *send);
    void onClickRentgenOff(bool);


    // ISPRWidget interface
    void onCommandComplite(TCPCommand *_command);
    void onCommandNotComplite(TCPCommand *_command);
    void onServerStateChanget(uint32_t);
protected:
    QList<uint8_t> getSelectedThreads();

    // ISPRSettingsWidget interface
public:
    virtual void setMasterMode(bool value);

    // ISPRWidget interface
public:
    void setStateSeparate(bool on_off);
};

#endif // SPRSETTINGSRENTGENAUTOSETWIDGET_H
