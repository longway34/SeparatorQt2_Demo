#ifndef SPRSETTINGSCONTROLWIDGET2_H
#define SPRSETTINGSCONTROLWIDGET2_H

#include "ui_sprsettingscontrolwidget2.h"
#include "isprwidget.h"
#include "models/sprsettingscontrolmodel.h"
#include "isprsettingswidget.h"

class SPRSettingsControlWidget2 : public QWidget, public ISPRWidget, public ISPRSettingsWidget
{
    Q_OBJECT

    SPRSettingsControlModel *model;
public:
    explicit SPRSettingsControlWidget2(QWidget *parent = nullptr);

private:
    Ui::SPRSettingsControlWidget2 ui;

    // ISPRWidget interface
public:
//    virtual ISPRModelData *setModelData(SPRSettingsControlModel *data);
    virtual ISPRModelData *setModelData(ISPRModelData *data);
    virtual ISPRModelData *getModelData();

    // ISPRWidget interface
public slots:
    void onChangeSpinBoxFieldValue();
    virtual void viewChange();
    virtual void viewChange(bool value);
    virtual void widgetsShow();
    virtual void viewChange(int index);
    virtual void onModelChanget(IModelVariable *send);



    // ISPRWidget interface
protected:

    bool onChangeAirControlValues(uint aPermissMin, uint aPermissMax, uint aCriticMin, uint aCriticMax);
    bool onChangeCorrelValues(double min, double max);
    bool onChangeSpeedStoneValues(double min, double max);
    bool onChangePowerValues(uint min, uint max);
public:
    virtual void setMasterMode(bool value);
};

#endif // SPRSETTINGSCONTROLWIDGET2_H
