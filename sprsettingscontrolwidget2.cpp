#include "sprsettingscontrolwidget2.h"
#include "models/sprmainmodel.h"

#include <QMessageBox>

SPRSettingsControlWidget2::SPRSettingsControlWidget2(QWidget *parent) :
    QWidget(parent)
{
    ui.setupUi(this);
    model = nullptr;

    QVector<QAbstractSpinBox*> les = {
        ui.leWeightAvgConcentrate, ui.leWeightAvgTail,
        ui.leTimeMeassureData, ui.leTimeMeassureHistorramm, ui.leTimeMeassureSpector,
        ui.leVEMSLess, ui.leVEMSMaxCode, ui.leVEMSBeginCode,
        ui.leCorrectStream,
        ui.sbCorrelPermitt, ui.sbCorrelCritical,
        ui.sbSpeedStonePermitt, ui.sbSpeedStoneCrytical,
        ui.sbCenterPowerPermitt, ui.sbCenterPowerCritical,
        ui.sbAirPermittMin, ui.sbAirPermittMax, ui.sbAirCriticalMin, ui.sbAirCriticalMax
    };
    for(int i=0; i<les.size(); i++){
        connect(les[i], &QAbstractSpinBox::editingFinished, this, &SPRSettingsControlWidget2::onChangeSpinBoxFieldValue);
    }


    connect(ui.cbControlArea, SIGNAL(currentIndexChanged(int)), this, SLOT(viewChange(int)));
    connect(ui.cbAutoControlStream, SIGNAL(toggled(bool)), this, SLOT(viewChange(bool)));

    forBlockSignalsWidget = this->findChildren<QWidget*>();
}


void SPRSettingsControlWidget2::widgetsShow()
{
    if(model){
        foreach(QWidget* w, forBlockSignalsWidget){
            w->blockSignals(true);
        }

        QLocale loc(QLocale::C);
        loc.setNumberOptions(QLocale::RejectGroupSeparator);

        ui.sbCorrelCritical->setMinimum(0); ui.sbCorrelCritical->setMaximum(model->correlSpectrumPermiss->getData()-0.01);
        ui.sbCorrelPermitt->setMinimum(model->correlSpectrumCritical->getData()+0.01); ui.sbCorrelPermitt->setMaximum(1);

        ui.sbCorrelPermitt->setValue(model->correlSpectrumPermiss->getData());
        ui.sbCorrelPermitt->setToolTip(tr("Допустимое значение корреляции спектра (%1..%2)").arg(ui.sbCorrelPermitt->minimum()).arg(ui.sbCorrelPermitt->maximum()));
        ui.sbCorrelCritical->setValue(model->correlSpectrumCritical->getData()); //
        ui.sbCorrelCritical->setToolTip(tr("Критичное значение корреляции спектра (%1..%2)").arg(ui.sbCorrelCritical->minimum()).arg(ui.sbCorrelCritical->maximum()));


        ui.sbSpeedStonePermitt->setMinimum(0); ui.sbSpeedStonePermitt->setMaximum(model->speedStreamCritical->getData()-1);
        ui.sbSpeedStoneCrytical->setMinimum(model->speedStreamPermiss->getData()+1); ui.sbSpeedStoneCrytical->setMaximum(100);

        ui.sbSpeedStonePermitt->setValue(model->speedStreamPermiss->getData()); //'', 0, 25, tr("Допустимое значение скорости потока камней"));
        ui.sbSpeedStonePermitt->setToolTip(tr("Допустимое значение скорости потока камней в секунду (%1..%2").arg(ui.sbSpeedStonePermitt->minimum()).arg(ui.sbSpeedStonePermitt->maximum()));
        ui.sbSpeedStoneCrytical->setValue(model->speedStreamCritical->getData());// 0, 25, tr("Критичное значение скорости потока камней"));
        ui.sbSpeedStoneCrytical->setToolTip(tr("Критичное значение скорости потока камней в секунду (%1..%2").arg(ui.sbSpeedStoneCrytical->minimum()).arg(ui.sbSpeedStoneCrytical->maximum()));

        ui.sbCenterPowerPermitt->setMinimum(0); ui.sbCenterPowerPermitt->setMaximum(model->diffCenterGravityCritical->getData()-1);
        ui.sbCenterPowerCritical->setMinimum(model->diffCenterGravityPermiss->getData() + 1); ui.sbCenterPowerCritical->setMaximum(100);

        ui.sbCenterPowerPermitt->setValue(model->diffCenterGravityPermiss->getData()); //, 0, 100, tr("Допустимое значение отклонения центра тяжести"));
        ui.sbCenterPowerPermitt->setToolTip(tr("Допустимое значение отклонения центра тяжести, отн.% (%1..%2").arg(ui.sbCenterPowerPermitt->minimum()).arg(ui.sbCenterPowerPermitt->maximum()));
        ui.sbCenterPowerCritical->setValue(model->diffCenterGravityCritical->getData()); //, 0, 100, tr("Критичное значение отклонения центра тяжести"));
        ui.sbCenterPowerCritical->setToolTip(tr("Критичное значение отклонения центра тяжести, отн.% (%1..%2").arg(ui.sbCenterPowerCritical->minimum()).arg(ui.sbCenterPowerCritical->maximum()));

        ui.sbAirCriticalMin->setMinimum(0); ui.sbAirCriticalMin->setMaximum(model->airLoadingMinPermiss->getData()-1);
        ui.sbAirPermittMin->setMinimum(model->airLoadingMinCritical->getData()+1); ui.sbAirPermittMin->setMaximum(model->airLoadingMaxPermiss->getData()-1);
        ui.sbAirPermittMax->setMinimum(model->airLoadingMinPermiss->getData()+1); ui.sbAirPermittMax->setMaximum(model->airLoadingMaxCritical->getData()-1);
        ui.sbAirCriticalMax->setMinimum(model->airLoadingMaxPermiss->getData()+1); ui.sbAirCriticalMax->setMaximum(100000);

        ui.sbAirPermittMin->setValue(model->airLoadingMinPermiss->getData()); //, 0, 10000, tr("Допустимое минимальное значение загрузки по воздуху"));
        ui.sbAirPermittMin->setToolTip(tr("Допустимое минимальное значение загрузки по воздуху (%1..%2)").arg(ui.sbAirPermittMin->minimum()).arg(ui.sbAirPermittMin->maximum()));
        ui.sbAirPermittMax->setValue(model->airLoadingMaxPermiss->getData()); //, 0, 10000, tr("Допустимое максимальное значение загрузки по воздуху"));
        ui.sbAirPermittMax->setToolTip(tr("Допустимое максимальное значение загрузки по воздуху (%1..%2)").arg(ui.sbAirPermittMax->minimum()).arg(ui.sbAirPermittMax->maximum()));
        ui.sbAirCriticalMin->setValue(model->airLoadingMinCritical->getData()); //, 0, 10000, tr("Критичное минимальное значение загрузки по воздуху"));
        ui.sbAirCriticalMin->setToolTip(tr("Критичное минимальное значение загрузки по воздуху (%1..%2)").arg(ui.sbAirCriticalMin->minimum()).arg(ui.sbAirCriticalMin->maximum()));
        ui.sbAirCriticalMax->setValue(model->airLoadingMaxCritical->getData()); //, 0, 10000, tr("Критичное максимальное значение загрузки по воздуху"));
        ui.sbAirCriticalMin->setToolTip(tr("Критичное максимальное значение загрузки по воздуху (%1..%2)").arg(ui.sbAirCriticalMax->minimum()).arg(ui.sbAirCriticalMax->maximum()));


        ui.leWeightAvgConcentrate->setValue(model->weightAvgStoneConcentrate->getData());
        ui.leWeightAvgTail->setValue(model->weightAvgStoneTail->getData());

        ui.leTimeMeassureData->setValue(model->tMeassureForData->getData());
        ui.leTimeMeassureSpector->setValue(model->tMeassureForSpectrum->getData());
        ui.leTimeMeassureHistorramm->setValue(model->tMeassureForHistogramm->getData());

        ui.leCorrectStream->setValue(model->correctOptimalOreStream->getData());
        ui.cbAutoControlStream->setChecked(model->autoOreStreamControl->getData());

        ui.leVEMSLess->setValue(model->VEMSLevelLess->getData());
        ui.leVEMSMaxCode->setValue(model->VEMSMaxCode->getData());
        ui.leVEMSBeginCode->setValue(model->getVEMSBeginCode());

        ui.cbControlArea->clear();
        QList<EnumElements> permittElements;
        SPRMainModel *mm = model->getMainModel();
        if(mm){
            permittElements = mm->getElementsModel()->getUsedlElementKeys();
            foreach (EnumElements el, permittElements) {
                QVariant value; value.setValue<EnumElements>(el);
                QString sName = mm->getElementsModel()->getSName(el), fName = mm->getElementsModel()->getFName(el);
                ui.cbControlArea->addItem(sName +" ("+fName+")", value);
            }
        } else {
            QVariant value; value.setValue<EnumElements>(Ns);
        }

        QVariant cur; cur.setValue<EnumElements>(model->controlArea->getData());
        int curIndex = ui.cbControlArea->findData(cur);
        ui.cbControlArea->setCurrentIndex(curIndex);

        foreach(QWidget* w, forBlockSignalsWidget){
            w->blockSignals(false);
        }
    }
}

ISPRModelData *SPRSettingsControlWidget2::setModelData(ISPRModelData *data)
{
    if(data){
        if(model){
            disconnect(model, SIGNAL(modelChanget(IModelVariable*)), this, SLOT(onModelChanget(IModelVariable*)));
        }
        SPRMainModel *_mm = data->getMainModel();
        if(_mm){
            model = _mm->getSettingsControlModel();
            connect(model, SIGNAL(modelChanget(IModelVariable*)), this, SLOT(onModelChanget(IModelVariable*)));
            SPRElementsModel *em = _mm->getElementsModel();
            if(em){
                connect(em, &SPRElementsModel::modelChanget, this, &SPRSettingsControlWidget2::onModelChanget);
            }
        }
    }
    return model;
}

ISPRModelData *SPRSettingsControlWidget2::getModelData()
{
    return model;
}

void SPRSettingsControlWidget2::onChangeSpinBoxFieldValue()
{
    if(model){
        if(sender() == ui.leTimeMeassureData){
            model->tMeassureForData->setData(ui.leTimeMeassureData->value());
            return;
        }
        if(sender() == ui.leTimeMeassureHistorramm){
            model->tMeassureForHistogramm->setData(ui.leTimeMeassureHistorramm->value());
            return;
        }
        if(sender() == ui.leTimeMeassureSpector){
            model->tMeassureForSpectrum->setData(ui.leTimeMeassureSpector->value());
            return;
        }
        if(sender() == ui.leWeightAvgConcentrate){
            model->weightAvgStoneConcentrate->setData(ui.leWeightAvgConcentrate->value());
            return;
        }
        if(sender() == ui.leWeightAvgTail){
            model->weightAvgStoneTail->setData(ui.leWeightAvgTail->value());
            return;
        }
        if(sender() == ui.leVEMSLess){
            model->VEMSLevelLess->setData(ui.leVEMSLess->value());
            return;
        }
        if(sender() == ui.leVEMSMaxCode){
            model->VEMSMaxCode->setData(ui.leVEMSMaxCode->value());
            return;
        }
        if(sender() == ui.leVEMSBeginCode){
            model->setVEMSBeginCode(ui.leVEMSBeginCode->value());
            return;
        }
        if(sender() == ui.leCorrectStream){
            model->correctOptimalOreStream->setData(ui.leCorrectStream->value());
            return;
        }
        if(sender() == ui.sbCorrelPermitt){
            onChangeCorrelValues(model->correlSpectrumCritical->getData(), ui.sbCorrelPermitt->value());
//            model->correlSpectrumPermiss->setData(ui.sbCorrelPermitt->value());
            return;
        }
        if(sender() == ui.sbCorrelCritical){
            onChangeCorrelValues(ui.sbCorrelCritical->value(), model->correlSpectrumPermiss->getData());
//            model->correlSpectrumCritical->setData(ui.sbCorrelCritical->value());
            return;
        }
        if(sender() == ui.sbSpeedStonePermitt){
            onChangeSpeedStoneValues(ui.sbSpeedStonePermitt->value(), model->speedStreamCritical->getData());
//            model->speedStreamPermiss->setData(ui.sbSpeedStonePermitt->value());
            return;
        }
        if(sender() == ui.sbSpeedStoneCrytical){
            onChangeSpeedStoneValues(model->speedStreamPermiss->getData(), ui.sbSpeedStoneCrytical->value());
//            model->speedStreamCritical->setData(ui.sbSpeedStoneCrytical->value());
            return;
        }
        if(sender() == ui.sbCenterPowerPermitt){
            onChangePowerValues(ui.sbCenterPowerPermitt->value(), model->diffCenterGravityCritical->getData());
//            model->diffCenterGravityPermiss->setData(ui.sbCenterPowerPermitt->value());
            return;
        }
        if(sender() == ui.sbCenterPowerCritical){
            onChangePowerValues(model->diffCenterGravityPermiss->getData(), ui.sbCenterPowerCritical->value());
//            model->diffCenterGravityCritical->setData(ui.sbCenterPowerCritical->value());
            return;
        }
        if(sender() == ui.sbAirPermittMin){
            onChangeAirControlValues(ui.sbAirPermittMin->value(), model->airLoadingMaxPermiss->getData(), model->airLoadingMinCritical->getData(), model->airLoadingMaxCritical->getData());
//            model->airLoadingMinPermiss->setData(ui.sbAirPermittMin->value());
            return;
        }
        if(sender() == ui.sbAirPermittMax){
            onChangeAirControlValues(model->airLoadingMinPermiss->getData(), ui.sbAirPermittMax->value(), model->airLoadingMinCritical->getData(), model->airLoadingMaxCritical->getData());
//            model->airLoadingMaxPermiss->setData(ui.sbAirPermittMax->value());
            return;
        }
        if(sender() == ui.sbAirCriticalMin){
            onChangeAirControlValues(model->airLoadingMinPermiss->getData(), model->airLoadingMaxPermiss->getData(), ui.sbAirCriticalMin->value(), model->airLoadingMaxCritical->getData());
//            model->airLoadingMinCritical->setData(ui.sbAirCriticalMin->value());
            return;
        }
        if(sender() == ui.sbAirCriticalMax){
            onChangeAirControlValues(model->airLoadingMinPermiss->getData(), model->airLoadingMaxPermiss->getData(),  model->airLoadingMinCritical->getData(), ui.sbAirCriticalMax->value());
//            model->airLoadingMaxCritical->setData(ui.sbAirCriticalMax->value());
            return;
        }

    }
}

void SPRSettingsControlWidget2::viewChange(bool value)
{
    if(model){
        if(sender() == ui.cbAutoControlStream){
            model->autoOreStreamControl->setData(value);
        }
    }

}

void SPRSettingsControlWidget2::viewChange()
{
}


void SPRSettingsControlWidget2::viewChange(int index)
{
    if(model){
        if(sender() == ui.cbControlArea){
            EnumElements el = ui.cbControlArea->itemData(index).value<EnumElements>();
            model->controlArea->setData(el);
        }
    }
}



void SPRSettingsControlWidget2::onModelChanget(IModelVariable *send)
{
    if(model){
        SPRMainModel *mm = model->getMainModel();
        if(mm){
            SPRElementsModel *em = mm->getElementsModel();
            if(em && sender() == em){
                widgetsShow();
            }
        }
    }
    if(sender() == model){
        widgetsShow();
    }
}

bool SPRSettingsControlWidget2::onChangeAirControlValues(uint aPermissMin, uint aPermissMax, uint aCriticMin, uint aCriticMax)
{
    bool isOk = false;
    if(model){
        isOk = true;
        QString patern = tr("%1 больше %2 (%3 > %4)"); QString msg = "";
        QString sCMin = tr("Критичный мининимум"); QString sCMax = tr("критичный максимум");
        QString sCMin_rp = tr("Критичного мининимума"); QString sCMax_rp = tr("критичного максимума");
        QString sPMin = tr("Допустимый мининимум"); QString sPMax = tr("Допустимый максимум");
        QString sPMin_rp = tr("допустимого мининимума"); QString sPMax_rp = tr("допустимого максимума");

        if(aCriticMin >= aPermissMin){ isOk = false; msg = patern.arg(sCMin).arg(sPMin_rp).arg(aCriticMin).arg(aPermissMin);}
        if(aPermissMin > aPermissMax){ isOk = false; msg = patern.arg(sPMin).arg(sPMax_rp).arg(aPermissMin).arg(aPermissMax);}
        if(aPermissMax >= aCriticMax){ isOk = false; msg = patern.arg(sPMax).arg(sCMax_rp).arg(aPermissMax).arg(aCriticMax);}

        if(isOk){
            model->airLoadingMaxCritical->setData(aCriticMax);
            model->airLoadingMaxPermiss->setData(aPermissMax);
            model->airLoadingMinCritical->setData(aCriticMin);
            model->airLoadingMinPermiss->setData(aPermissMin);
        } else {
            QMessageBox::warning(nullptr, tr("Ошибка..."), msg);
            widgetsShow();
        }
    }
    return isOk;
}

bool SPRSettingsControlWidget2::onChangeCorrelValues(double min, double max){
    if(min > max){
        QString patern = tr("Критичное значение корреляции больше допустимого (%1 > %2)").arg(min).arg(max);
        QMessageBox::warning(nullptr, tr("Ошибка..."), patern);
        widgetsShow();
    } else {
        model->correlSpectrumCritical->setData(min);
        model->correlSpectrumPermiss->setData(max);
    }
}

bool SPRSettingsControlWidget2::onChangePowerValues(uint min, uint max){
    if(min >= max){
        QString patern = tr("Критичное значение отклонения центра тяжести меньше допустимого (%1 > %2)").arg(min).arg(max);
        QMessageBox::warning(nullptr, tr("Ошибка..."), patern);
        widgetsShow();
    } else {
        model->diffCenterGravityPermiss->setData(min);
        model->diffCenterGravityCritical->setData(max);
    }
}

bool SPRSettingsControlWidget2::onChangeSpeedStoneValues(double min, double max){
    if(min >= max){
        QString patern = tr("Критичное значение скорости меньше допустимого (%1 < %2)").arg(min).arg(max);
        QMessageBox::warning(nullptr, tr("Ошибка..."), patern);
        widgetsShow();
    } else {
        model->speedStreamPermiss->setData(min);
        model->speedStreamCritical->setData(max);
    }
}


void SPRSettingsControlWidget2::setMasterMode(bool value)
{
    this->setEnabled(value);
}
