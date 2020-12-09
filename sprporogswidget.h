#ifndef SPRPOROGSWIDGET_H
#define SPRPOROGSWIDGET_H

#include <QLineEdit>
#include <QValidator>
#include <QDomDocument>

#include "ui_sprporogswidget.h"
#include "_types.h"
#include "models/sprporogsmodel.h"
#include "variables/sprvariable.h"
#include "isprwidget.h"

class SPRPorogsWidget : public QWidget, public ISPRWidget
{
    Q_OBJECT
    SPRPorogsModel *model;
    QDoubleValidator *porogValidator;

public:
    explicit SPRPorogsWidget(QWidget *parent = nullptr);
    virtual ~SPRPorogsWidget(){
        if(porogValidator){
            delete porogValidator;
        }
    }
    Ui::SPRPorogsWidget ui;

    void setThreads(SPRVariable<uint>* threads);
    void setConditions(SPREnumVariable<TypeConditions>* conditions) {this->model->setConditions(conditions);}
    ISPRModelData *setModelData(ISPRModelData *model);

    // ISPRWidget interface
signals:
    void tableChange(QTableWidget *table, int row, int col);
protected:


    // ISPRWidget interface
public slots:
    virtual ISPRModelData *getModelData();
    virtual void widgetsShow();
    virtual void onModelChanget(IModelVariable *send);


    // ISPRWidget interface
    void viewChange();


    // ISPRWidget interface
protected:
};

#endif // SPRPOROGSWIDGET_H
