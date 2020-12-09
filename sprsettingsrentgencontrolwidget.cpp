#include "sprsettingsrentgencontrolwidget.h"
/**
 * @brief SPRSettingsRentgenControlWidget::SPRSettingsRentgenControlWidget
 * @param parent
 */
SPRSettingsRentgenControlWidget::SPRSettingsRentgenControlWidget(QWidget *parent) :
    QWidget(parent), model(nullptr)
{
    ui.setupUi(this);

    forBlockSignalsWidget = findChildren<QWidget*>();
}


void SPRSettingsRentgenControlWidget::widgetsShow()
{
//    createDEUCodesTable();
    foreach(QWidget *w, forBlockSignalsWidget){
        w->blockSignals(true);
    }

//    ui.tDEUCode->widgetsShow();
    foreach(QWidget *w, forBlockSignalsWidget){
        w->blockSignals(false);
    }
    ui.slTimeMoveRGU->blockSignals(true); ui.lTimeMoveRGU->blockSignals(true);
    ui.slTimeMoveRGU->setValue(model->timeMoveRGU->getData());
    ui.lTimeMoveRGU->setValue(model->timeMoveRGU->getData());
    ui.slTimeMoveRGU->blockSignals(false); ui.lTimeMoveRGU->blockSignals(false);
}

ISPRModelData *SPRSettingsRentgenControlWidget::setModelData(SPRSettingsRentgenModel *data)
{
    if(data){
        if(model){
            disconnect(model, SIGNAL(modelChanget(IModelVariable*)), this, SLOT(onModelChanget(IModelVariable*)));
        }
        model = data;
        connect(model, SIGNAL(modelChanget(IModelVariable*)), this, SLOT(onModelChanget(IModelVariable*)));

        ui.tDiffRA->setModelData(model);
        ui.tTimesRA->setModelData(model);

        ui.lTimeMoveRGU->setValue(model->timeMoveRGU->getData());
        ui.slTimeMoveRGU->setValue(model->timeMoveRGU->getData());

        connect(ui.cbWOControlPlace, SIGNAL(toggled(bool)), SLOT(viewChange(bool)));
        connect(ui.slTimeMoveRGU, SIGNAL(valueChanged(int)), this, SLOT(viewChange(int)));
        connect(ui.lTimeMoveRGU, SIGNAL(valueChanged(int)), this, SLOT(viewChange(int)));
    }
}

void SPRSettingsRentgenControlWidget::viewChange(int value)
{
    if(sender() == ui.slTimeMoveRGU){
        model->timeMoveRGU->setData(value);
        ui.lTimeMoveRGU->blockSignals(true); ui.lTimeMoveRGU->setValue(value); ui.lTimeMoveRGU->blockSignals(false);
    }
    if(sender() == ui.lTimeMoveRGU){
        model->timeMoveRGU->setData(value);
        ui.slTimeMoveRGU->blockSignals(true); ui.slTimeMoveRGU->setValue(value); ui.slTimeMoveRGU->blockSignals(false);
    }
}

void SPRSettingsRentgenControlWidget::viewChange(bool value)
{
    if(sender() == ui.cbWOControlPlace){
        model->woControlPlace->setData(value);
        ui.slTimeMoveRGU->setEnabled(value);
    }
}

void SPRSettingsRentgenControlWidget::viewChange()
{
}


void SPRSettingsRentgenControlWidget::onModelChanget(IModelVariable *send)
{
    IModelVariable *var = dynamic_cast<IModelVariable*>(sender());
    if(sender() == model){
        widgetsShow();
    }
}


void SPRSettingsRentgenControlWidget::setMasterMode(bool value)
{
    this->setEnabled(value);
}

