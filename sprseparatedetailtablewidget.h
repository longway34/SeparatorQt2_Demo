#ifndef SPRSEPARATEDETAILTABLEWIDGET_H
#define SPRSEPARATEDETAILTABLEWIDGET_H

#include "ui_sprseparatedetailtablewidget.h"

#include "models/sprmainmodel.h"
#include "isprwidget.h"

class SPRSeparateDetailTableWidget : public QWidget, public ISPRWidget
{
    Q_OBJECT

    SPRMainModel *model;
    SPRSeparateModel *separateModel;
public:
    SPRSeparateDetailTableWidget(QWidget *parent = nullptr);

private:
    Ui::SPRSeparateDetailTableWidget ui;

    // ISPRWidget interface
public:
    virtual ISPRModelData *setModelData(ISPRModelData *data);
    virtual ISPRModelData *getModelData();

protected:

    SPRThreadList getVisbleThreads();
public slots:
    virtual void onModelChanget(IModelVariable *variable);
    virtual void widgetsShow();

    void onChangeThreadList(bool value);
    void onClickButtomCommand(bool);
    void onChangeIntValue(int);
    void onChandeServerState(uint32_t);

    // ISPRWidget interface
public:
    virtual void storeWidgetsStates(QString groupId, QSettings *mainSettings);
    virtual void restoreWidgetsStates(QString groupId, QSettings *mainSettings);
protected slots:
    void onSeparateDataReady();
};

#endif // SPRSEPARATEDETAILTABLEWIDGET_H
