#ifndef SPRSETTINGSPOROGSWIDGET_H
#define SPRSETTINGSPOROGSWIDGET_H

#include "ui_sprsettingsporogswidget.h"
#include "models/sprsettingsporogsmodel.h"
#include "models/isprmodeldata.h"
#include "_types.h"
#include "isprwidget.h"
#include <qwt_plot_canvas.h>
#include <qwt_legend.h>
#include <qwt_plot_grid.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_picker.h>
#include <qwt_symbol.h>
#include <QDomDocument>
#include <QButtonGroup>

#include "isprsettingswidget.h"

class SPRSettingsPorogsWidget : public QWidget, public ISPRWidget, public ISPRSettingsWidget
{
    Q_OBJECT

protected:
    SPRSettingsPorogsModel *model;
    QwtPlotPicker *pickter;
public:
    explicit SPRSettingsPorogsWidget(QWidget *parent = 0);
    Ui::SPRSettingsPorogsWidget ui;
private:

public:
    ISPRModelData *setModelData(ISPRModelData *value);

    QButtonGroup *bgTypeSelection;
    QButtonGroup *bgTypeConditions;
    QwtPlotCurve *curve;
    QwtPlotGrid *grid;

    void setConditions(SPREnumVariable<TypeConditions> *var){
        model->setConditions(var);
//        ui.wPorogs->setConditions(var);
        widgetsShow();
    }
    void setThreads(SPRVariable<uint> *var){
        model->setThreads(var);

        widgetsShow();
    }
    void setTypeSelections(SPREnumVariable<TypeSelection> *var){
        model->setSelection(var);
    }

public slots:
    void repaintGraphic(double);

    // ISPRWidget interface
public slots:
    virtual ISPRModelData *getModelData();
    virtual void widgetsShow();
    virtual void viewChange(int data);
    virtual void viewChange(bool data);
    virtual void viewChange(double data);
    virtual void viewChange(QAbstractButton *btn);

signals:
    void doShow();

    // ISPRWidget interface
//protected:
//    virtual void viewChange(QTableWidget*, int, int);

    // ISPRWidget interface
protected slots:
    virtual void onModelChanget(IModelVariable*send);

    // ISPRSettingsWidget interface
public:
    virtual void setMasterMode(bool value);
};



#endif // SPRSETTINGSPOROGSWIDGET_H
