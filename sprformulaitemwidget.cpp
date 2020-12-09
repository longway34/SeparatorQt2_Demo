#include "sprformulaitemwidget.h"

ISPRModelData *SPRFormulaItemWidget::setModelData(ISPRModelData *value)
{
    if(value){
        if(model){
            disconnect(model, SIGNAL(modelChanget(IModelVariable*)), this, SLOT(onModelChanget(IModelVariable*)));
        }
        model = (SPRFormulaItemModel*)value;
        connect(model, SIGNAL(modelChanget(IModelVariable*)), this, SLOT(onModelChanget(IModelVariable*)));

        QVector<SPRFormulaElement*> elems =
                {
                                                ui.lElementUp1,
                                                ui.lElementUp2,
                                                ui.lElementDown1,
                                                ui.lElementDown2,
                                                ui.lElementDown3,
                                                ui.lElementDown4};
        QVector<SPREnumVariable<EnumElements>*> vars = {
            model->ElementUp1, model->ElementUp2,
            model->ElementDown1, model->ElementDown2,
            model->ElementDown3, model->ElementDown4
        };
        for(int i=0; i<elems.size(); i++){
            elems[i]->setModelData(model->getElements());
            elems[i]->setElement(vars[i]);
        }


        QDoubleValidator *valid = new QDoubleValidator(this);
        valid->setLocale(QLocale::C);

        ui.leMulUp->setValidator(valid); ui.leMulUp->setLocale(QLocale::C);
        ui.leMulDown->setValidator(valid); ui.leMulDown->setLocale(QLocale::C);
        ui.leMin->setValidator(valid); ui.leMin->setLocale(QLocale::C);
        ui.leMax->setValidator(valid); ui.leMax->setLocale(QLocale::C);
//        ui.leMulUp->setValidator(new QDoubleValidator()); ui.leMulUp->setLocale(QLocale::English);
//        ui.leMulDown->setValidator(new QDoubleValidator()); ui.leMulDown->setLocale(QLocale::English);
//        ui.leMin->setValidator(new QDoubleValidator()); ui.leMin->setLocale(QLocale::English);
//        ui.leMax->setValidator(new QDoubleValidator()); ui.leMax->setLocale(QLocale::English);

//        ui.leMulUp->setValidator(new SPRDoubleValidator()); ui.leMulUp->setLocale(QLocale::English);
//        ui.leMulDown->setValidator(new SPRDoubleValidator()); ui.leMulDown->setLocale(QLocale::English);
//        ui.leMin->setValidator(new SPRDoubleValidator()); ui.leMin->setLocale(QLocale::English);
//        ui.leMax->setValidator(new SPRDoubleValidator()); ui.leMax->setLocale(QLocale::English);

        ui.lResult->setText(QString("H")+QString::number(model->index+1));
        widgetsShow();
    }
    return model;
}

void SPRFormulaItemWidget::setVisibleMinMax(bool visible)
{
    visibleMinMax = visible;
    ui.wDown->setVisible(visible);
}

bool SPRFormulaItemWidget::isVisibleMinMax()
{
    return visibleMinMax;
}

void SPRFormulaItemWidget::viewChange()
{
    QLineEdit *send = qobject_cast<QLineEdit*>(sender());
    if(send){
        QString sVal = send->text().replace(',', '.');
        bool ok;
        double val = sVal.toDouble(&ok);
        if(ok){
//            double val = ((QLineEdit*)sender())->text().toDouble();
            if(sender() == ui.leMulUp){
                model->MulUp->setData(val);
                return;
            }
            if(sender() == ui.leMulDown){
                model->MulDown->setData(val);
                return;
            }
            if(sender() == ui.leMin){
                model->min->setData(val);
                if(model->min->getData() > model->max->getData()){
                    model->max->setData(model->min->getData());
        //            widgetsShow();
                }
                return;
            }
            if(sender() == ui.leMax){
                model->max->setData(val);
                if(model->max->getData()<model->min->getData()){
                    model->min->setData(model->max->getData());
        //            widgetsShow();
                }
                return;
            }
        }
    }
}

SPRFormulaItemWidget::SPRFormulaItemWidget(QWidget *parent) :
    QWidget(parent), model(nullptr)
{
    ui.setupUi(this);

    connect(ui.leMulUp, SIGNAL(editingFinished()),SLOT(viewChange()));
    connect(ui.leMulDown, SIGNAL(editingFinished()),SLOT(viewChange()));
    connect(ui.leMin, SIGNAL(editingFinished()),SLOT(viewChange()));
    connect(ui.leMax, SIGNAL(editingFinished()),SLOT(viewChange()));

    forBlockSignalsWidget = findChildren<QWidget*>();
}


void SPRFormulaItemWidget::widgetsShow()
{
    if(model){

        foreach(SPRFormulaElement* el, QList<SPRFormulaElement*>({
                    ui.lElementUp1, ui.lElementUp2, ui.lElementDown1,
                    ui.lElementDown2, ui.lElementDown3, ui.lElementDown4
                    })){
            el->widgetsShow();
        }

        ui.leMulUp->blockSignals(true); ui.leMulUp->setText(model->MulUp->toString()); ui.leMulUp->blockSignals(false);
        ui.leMulDown->blockSignals(true); ui.leMulDown->setText(model->MulDown->toString()); ui.leMulDown->blockSignals(false);
        ui.leMin->blockSignals(true); ui.leMin->setText(model->min->toString()); ui.leMin->blockSignals(false);
        ui.leMax->blockSignals(true); ui.leMax->setText(model->max->toString()); ui.leMax->blockSignals(false);

        setVisibleMinMax(visibleMinMax);
    }
}


ISPRModelData *SPRFormulaItemWidget::getModelData()
{
    return model;
}


void SPRFormulaItemWidget::onModelChanget(IModelVariable *send)
{

    if(send == model->MulDown){
        ui.leMulDown->blockSignals(true); ui.leMulDown->setText(model->MulDown->toString()); ui.leMulDown->blockSignals(false);
    } else if(send == model->MulUp){
        ui.leMulUp->blockSignals(true); ui.leMulUp->setText(model->MulUp->toString()); ui.leMulUp->blockSignals(false);
    } else if(send == model->min){
        ui.leMin->blockSignals(true); ui.leMin->setText(model->min->toString()); ui.leMin->blockSignals(false);
    } else if(send == model->max){
        ui.leMax->blockSignals(true); ui.leMax->setText(model->max->toString()); ui.leMax->blockSignals(false);
    }

}
