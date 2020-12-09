#ifndef SPRSETTINGSMAINWIDGET_H
#define SPRSETTINGSMAINWIDGET_H

#include "ui_sprsettingsmainwidget.h"
#include "isprwidget.h"
#include "models/sprsettingsmainmodel.h"
#include "isprsettingswidget.h"

class SPRSettingsMainWidget : public QWidget, public ISPRWidget, public ISPRSettingsWidget
{
    Q_OBJECT

    SPRSettingsMainModel *model;

    TCPCommand *getStateCommand;


public:
    explicit SPRSettingsMainWidget(QWidget *parent = nullptr);

private:
    Ui::SPRSettingsMainWidget ui;

    // ISPRWidget interface
public:
    virtual ISPRModelData *setModelData(ISPRModelData *data);
    virtual ISPRModelData *getModelData();

    // ISPRWidget interface
public slots:
    virtual void widgetsShow();
    void viewChange(QColor color);
    void onChangeIgnoreParams(bool);
protected slots:
    virtual void viewChange(int);
    virtual void viewChange();
    virtual void viewChange(bool);
    virtual void onModelChanget(IModelVariable *send);
    void onServerConnectButtomClick(bool);
    void onGetStateCommandComplite(TCPCommand *command);
    void onErrorStateConnectServer(ITCPCommand *command);
    void onChangeSeparatorColor(IModelVariable *, QColor color);
signals:
    void doShow();
//    void changeFileSettinds(QString);
//    void changeFileSpectrum(QString);

    // ISPRWidget interface
protected:

    // ISPRSettingsWidget interface
public:
    virtual void setMasterMode(bool value);
};

#endif // SPRSETTINGSMAINWIDGET_H
