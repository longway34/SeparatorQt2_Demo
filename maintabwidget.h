#ifndef MAINTABWIDGET_H
#define MAINTABWIDGET_H

#include "ui_maintabwidget.h"
#include "isprwidget.h"
#include "models/sprseparatemodel.h"
#include "tcp/tcpechologswidget.h"

class SPRSeparatorLogsModel;

class MainTabWidget : public QTabWidget, public ISPRWidget
{
    Q_OBJECT

public:
    explicit MainTabWidget(QWidget *parent = nullptr);
    MainTabWidget(QDomDocument *_doc, SPRSeparatorLogsModel *_logsModel, QWidget *parent = nullptr);
    MainTabWidget(QString _fName, QWidget *parent = nullptr);

    SPRSeparatorLogsModel *getLogsModel();

    SPRMainModel *model;
    SPRSeparatorLogsModel *logModel;
    Ui::MainTabWidget ui;

    SPRSeparateModel *separateModel;
    int errorSeparateState;

    QList<ISPRWidget*> myChildren;

    bool isMasterMode;


signals:
    void doStore();
    void doShow();
    void saveAsSettings(IModelVariable*);

    // ISPRWidget interface
public:
    virtual ISPRModelData *getModelData(){return model;}
    bool isCloseEnabled();
    
public slots:
    virtual ISPRModelData *setModelData(SPRMainModel *_model);
    virtual void viewChange(){}
    virtual void viewChange(int){}
    virtual void viewChange(QTableWidget *, int, int){}
    virtual void widgetsShow();

    void onChangeFileSettings(QString fName);
    void onChangeFileSpectrum(QString fName);
//    void onClickSetSeparateButton(bool value);
    virtual void onModelChanget(IModelVariable *);

    // ISPRWidget interface

    // ISPRWidget interface
protected:

    // ISPRWidget interface
public:
    virtual void setDoc(QString _fName);
    bool getIsMasterMode() const;
    void setIsMasterMode(bool value);
    SPRMainModel *getModel() const;

    // ISPRWidget interface
public:
    void setStateSeparate(bool on_off);

    // ISPRWidget interface
public:
    virtual void storeWidgetsStates(QString groupId, QSettings *mainSettings);
    virtual void restoreWidgetsStates(QString groupId, QSettings *mainSettings);
    SPRPitatelControlWidget *getPitatelControl();
    void setLogModel(SPRSeparatorLogsModel *_logModel);
    SPRSeparateControlWidget *getSeparateControl();
    bool isStartStopProcess();
protected slots:
    void onSaveAsSettings(IModelVariable *var);
    void onServerStateChanget(uint32_t);
};



#endif // MAINTABWIDGET_H
