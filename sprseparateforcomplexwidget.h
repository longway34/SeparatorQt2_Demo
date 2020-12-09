#ifndef SPRSEPARATEFORCOMPLEXWIDGET_H
#define SPRSEPARATEFORCOMPLEXWIDGET_H

#include "ui_sprseparateforcomplexwidget.h"
#include <QMdiSubWindow>
#include "isprwidget.h"

#include "models/sprmainmodel.h"

class SPRSeparatorForComplexSubWindow : public QMdiSubWindow, public ISPRWidget{

    Q_OBJECT


    QDateTime lastUpdate;
    // QObject interface
public:
    explicit SPRSeparatorForComplexSubWindow(QWidget *parent = nullptr): QMdiSubWindow(parent){

    }
    virtual bool eventFilter(QObject *watched, QEvent *event);
signals:
    void closeSeparator(SPRSeparatorForComplexSubWindow *);
    void selectSeparator(SPRSeparatorForComplexSubWindow *);

    // QWidget interface
protected:
    virtual void closeEvent(QCloseEvent *event);
protected:
    void mouseDoubleClickEvent(QMouseEvent *event);

    // ISPRWidget interface
public:
    virtual void widgetsShow();
    virtual ISPRModelData *getModelData();

protected:
    virtual void onModelChanget(IModelVariable *);

    // ISPRWidget interface
public:
    virtual void storeWidgetsStates(QString groupId, QSettings *mainSettings);
    virtual void restoreWidgetsStates(QString groupId, QSettings *);
};

class SPRSeparateForComplexWidget : public QWidget
{

    Q_OBJECT

    SPRMainModel *model;

public:
    explicit SPRSeparateForComplexWidget(QWidget *parent = nullptr);

    ISPRModelData *setModelData(ISPRModelData *_model);

private:
    Ui::SPRSeparateForComplexWidget ui;

    // QWidget interface

};

#endif // SPRSEPARATEFORCOMPLEXWIDGET_H
