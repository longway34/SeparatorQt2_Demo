#ifndef SPRSETTINGSRENTGENCONTROLWIDGET_H
#define SPRSETTINGSRENTGENCONTROLWIDGET_H

#include "ui_sprsettingsrentgencontrolwidget.h"
#include "models/sprsettingsrentgenmodel.h"
#include "isprwidget.h"

#include <QStyledItemDelegate>

#include "isprsettingswidget.h"

class SPRSettingsRentgenControlWidget;

class SPRSettingsRentgenControlWidget : public QWidget, public ISPRWidget, public ISPRSettingsWidget
{
    Q_OBJECT

    SPRSettingsRentgenModel *model;
public:
    explicit SPRSettingsRentgenControlWidget(QWidget *parent = nullptr);

private:
    Ui::SPRSettingsRentgenControlWidget ui;

    // ISPRWidget interface
public:
    virtual ISPRModelData *setModelData(SPRSettingsRentgenModel *data);
    virtual ISPRModelData *getModelData()
    {
        return model;
    }

//    void setThreads(SPRVariable<uint> *threads){
//        model->setThreads(threads);
////        createDEUCodesTable();
//    }

protected:
//    void createDEUCodesTable();

    // ISPRWidget interface
protected slots:
//    virtual void viewChange(QTableWidget *, int, int);
    virtual void viewChange(int value);
    virtual void viewChange(bool value);
    virtual void viewChange();
    virtual void onModelChanget(IModelVariable *send);



    // ISPRWidget interface
public slots:
    virtual void widgetsShow();

    // ISPRWidget interface
protected:

    // ISPRSettingsWidget interface
public:
    virtual void setMasterMode(bool value);
};



#endif // SPRSETTINGSRENTGENCONTROLWIDGET_H
