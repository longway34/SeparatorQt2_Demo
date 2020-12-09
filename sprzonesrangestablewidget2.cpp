#include "sprzonesrangestablewidget2.h"
#include "sprelementpropertydialog.h"
#include "sprzonesranges.h"
#include "_types.h"

#include <QMessageBox>

ISPRModelData *SPRZonesRangesTableWidget2::setModelData(SPRSpectrumZonesTableModel *value)
{
    if(value){
        if(model){
            disconnect(model, SIGNAL(modelChanget(IModelVariable*)), this, SLOT(onModelChanget(IModelVariable*)));
        }
        model = value;
        connect(model, SIGNAL(modelChanget(IModelVariable*)), this, SLOT(onModelChanget(IModelVariable*)));

        ui.tZonesTable->setModelData(model);
    }
    widgetsShow();
    return model;
}

//void SPRZonesRangesTableWidget2::setSelectBottonRowVisible(bool value)
//{
//    selectBottonRowVisible = value;
//    ui.tZonesTable->setSelectButtonEnable(selectBottonRowVisible);
//    //    widgetsShow();
//}

void SPRZonesRangesTableWidget2::onGotFocus(SPRZonesRanges2 *zone)
{
    lastCurrent.thZone = zone;
    if(zone){
        lastCurrent.element = zone->getCurrentElement(&lastCurrent.ok);
        lastCurrent.type = zone->getCurrentFieldType();
        lastCurrent.thread = zone->getThread();
    } else {
        lastCurrent.ok = false;
    }
    qDebug() << "el:" << lastCurrent.element << " type:" << lastCurrent.type << " th:" << lastCurrent.thread << " ok:" << lastCurrent.isOk() << " zone:" << lastCurrent.thZone;
    showEdabled();
}

SPRZonesRangesTableWidget2::SPRZonesRangesTableWidget2(QWidget *parent) :
    QWidget(parent), model(nullptr)
{
    ui.setupUi(this);

    connect(ui.bAddElement, SIGNAL(clicked(bool)), this, SLOT(onAddClick(bool)));
    connect(ui.bEditElement, SIGNAL(clicked(bool)), this, SLOT(onEditClick(bool)));
    connect(ui.bRoundRanges, SIGNAL(clicked(bool)), this, SLOT(onRoundClick(bool)));

    connect(ui.tZonesTable, &SPRZonesRangesTable2::gotFocus, this, &SPRZonesRangesTableWidget2::onGotFocus);
    connect(ui.tZonesTable, &SPRZonesRangesTable2::updateSize, this, &SPRZonesRangesTableWidget2::onUpdateSize);
    editControlEnable = false;
//    setSelectBottonRowVisible(true);

    ui.tZonesTable->resize(QSize(this->width(), ui.tZonesTable->height()));
    showEdabled();
}

void SPRZonesRangesTableWidget2::onRoundClick(bool){
//    bool noErrors = false;
    if(lastCurrent.isOk()){
        uint min = lastCurrent.thZone->getZoneModel()->getZones()[lastCurrent.element]->min->getData();
        uint max = lastCurrent.thZone->getZoneModel()->getZones()[lastCurrent.element]->max->getData();
        QString txt = tr("Вы действительно мотите сделать %1 значение\nдля области %2 ручья %3 равной %4-му каналу\nвсех остальных ручьев?").
                    arg(lastCurrent.type == minField ? tr("минимальное") : tr("максимальное")).
                    arg(lastCurrent.thZone->getZoneModel()->getElementsProperty()->getFName(lastCurrent.element)).
                    arg(lastCurrent.thread +1).
                    arg(lastCurrent.type == minField ? min : max);
        int res = QMessageBox::question(nullptr, tr("Внимание"), txt);
        if(res == QMessageBox::Yes){
            for(uint8_t th=0; th<MAX_SPR_MAIN_THREADS; th++){
                if(lastCurrent.type == minField){
                    model->items[th]->setMin(lastCurrent.element, min);
                } else if(lastCurrent.type == maxField){
                    model->items[th]->setMax(lastCurrent.element, max);
                }
            }
        }
    } else {
        QMessageBox::warning(nullptr,tr("Ошибка"), tr("Не выбран текущий елемент в таблице..."));
    }
//    EnumElements el = ui.tZonesTable->getCurrentElement(&noErrors);
//    if(noErrors){
//        int tIndex = ui.tZonesTable->getLastCurrentThread();
//        if(tIndex < 0){
//            return;
//        } else {
//            uint min = ui.tZonesTable->getLastCurrentTable()->getZoneModel()->getZones()[el]->min->getData();
//            uint max = ui.tZonesTable->getLastCurrentTable()->getZoneModel()->getZones()[el]->max->getData();
//            for(int th=0; th<model->items.size(); th++){
//                model->items[th]->setMinMax(el, min, max);
//            }
//        }
////        widgetsShow();
//    }
}

void SPRZonesRangesTableWidget2::onEditClick(bool){
    if(lastCurrent.isOk()){
        SPRElementPropertyDialog *dialog = new SPRElementPropertyDialog(lastCurrent.thZone->getZoneModel(), lastCurrent.element, this);
        dialog->setWindowTitle(tr("Редактирование свойств и границ элемента %1").
                arg(dialog->ui.leFName->text()));
        int res = dialog->exec();
        if(res == QDialog::DialogCode::Accepted){
            if(model){
                    EnumElements el = dialog->ui.cbElements->currentData().value<EnumElements>();
                    QColor color = dialog->ui.fc->getColor();
                    QString sName = dialog->ui.leSName->text();
                    QString fName = dialog->ui.leFName->text();

                    int min = dialog->ui.leMin->value();
                    int max = dialog->ui.leMax->value();

                    int thread = dialog->getThread();

                    model->elementsProperty->addElement(el, sName, fName, color);
                    model->setMinMaxValues(el, min, max, thread);
    //            SPRElementsModel *_elModel = model->elementsProperty->addElement();
            }
        }

    } else {
        QMessageBox::warning(nullptr,tr("Ошибка"), tr("Не выбран текущий елемент в таблице..."));
    }


//    bool noErrors = false;
//    EnumElements el = ui.tZonesTable->getCurrentElement(&noErrors);
//    if(noErrors){
//        SPRZonesRanges *zr = ui.tZonesTable->getLastCurrentTable();
//        if(zr){
//            SPRSpectrumZonesModel *mod = zr->getZoneModel();
//            SPRElementPropertyDialog *dialog = new SPRElementPropertyDialog(mod, el, this);
//            int res = dialog->exec();
//            if(res == QDialog::DialogCode::Accepted){
//                if(model){
//                        EnumElements el = dialog->ui.cbElements->currentData().value<EnumElements>();
//                        QColor color = dialog->ui.fc->getColor();
//                        QString sName = dialog->ui.leSName->text();
//                        QString fName = dialog->ui.leFName->text();

//                        int min = dialog->ui.leMin->value();
//                        int max = dialog->ui.leMax->value();

//                        model->elementsProperty->addElement(el, sName, fName, color);
//                        model->setMinMaxValues(el, min, max, mod->getThreadNumber());
//        //            SPRElementsModel *_elModel = model->elementsProperty->addElement();
//                }
//            }
//            delete dialog;
//        }
//    }
}

void SPRZonesRangesTableWidget2::onAddClick(bool){
    SPRElementPropertyDialog *dialog = new SPRElementPropertyDialog(model->elementsProperty, this);
    dialog->setWindowTitle(tr("Добавление нового элемента"));


    int res = dialog->exec();

    if(res == QDialog::DialogCode::Accepted){
        if(model){
                EnumElements el = dialog->ui.cbElements->currentData().value<EnumElements>();
                QColor color = dialog->ui.fc->getColor();
                QString sName = dialog->ui.leSName->text();
                QString fName = dialog->ui.leFName->text();

                int min = dialog->ui.leMin->value();
                int max = dialog->ui.leMax->value();

                int thread = dialog->getThread();

                model->elementsProperty->addElement(el, sName, fName, color);
                model->setMinMaxValues(el, min, max, thread);
//            SPRElementsModel *_elModel = model->elementsProperty->addElement();
        }
    }

    delete dialog;
    widgetsShow();
}

void SPRZonesRangesTableWidget2::widgetsShow()
{
    if(model){
        ui.tZonesTable->widgetsShow();
        showEdabled();
    }
}

void SPRZonesRangesTableWidget2::onUpdateSize()
{
//    ui.tZonesTable->resize(QSize(this->width(), ui.tZonesTable->height()));
    updateGeometry();
}

void SPRZonesRangesTableWidget2::showEdabled(){
    if(lastCurrent.isOk()){
        ui.bEditElement->setEnabled(true);
        ui.bRoundRanges->setEnabled(true);
    } else {
        ui.bEditElement->setEnabled(false);
        ui.bRoundRanges->setEnabled(false);
    }
//    if(editControlEnable){
//        ui.bEditElement->setEnabled(true);
//        ui.bRoundRanges->setEnabled(true);
//    } else {
//        ui.bEditElement->setEnabled(false);
//        ui.bRoundRanges->setEnabled(false);
//    }
//    if(model->getElementsProperty()->getElements().size() < MAX_SPR_SPECTOR_ELEMENTS){
//        ui.bAddElement->setEnabled(true);
//    } else {
//        ui.bAddElement->setEnabled(false);
//    }

}

//void SPRZonesRangesTableWidget2::setControlEnabled(SPRZonesRanges*, QLineEdit *_el){
////    bool res = false;
////    ui.tZonesTable->getCurrentElement(&res);
//    if(_el){
//        editControlEnable = true;
////        ui.bEdidElement->setEnabled(true);
////        ui.bRoundRanges->setEnabled(true);
//    } else {
//        if(ui.tZonesTable->getLastCurrentTable()){
//            bool res = false;
//            ui.tZonesTable->getLastCurrentTable()->getCurrentElement(&res);
//            if(res){
//                editControlEnable = true;
//            }
//        } else {
//            editControlEnable = false;
//        }
////        ui.bEdidElement->setEnabled(false);
////        ui.bRoundRanges->setEnabled(false);
//    }
////    if(model->getElementsProperty()->getElements().size() < MAX_SPR_SPECTOR_ELEMENTS){
////        ui.bAddElement->setEnabled(true);
////    } else {
////        ui.bAddElement->setEnabled(false);
////    }
//    showEdabled();
//}

ISPRModelData *SPRZonesRangesTableWidget2::getModelData()
{
    return model;
}


void SPRZonesRangesTableWidget2::onModelChanget(IModelVariable *)
{
//    widgetsShow();
}

void SPRZonesRangesTableWidget2::setThreadsVisible(QList<uint8_t> visible){
    ui.tZonesTable->setThreadList(visible);
}

void SPRZonesRangesTableWidget2::setThreadsVisible(uint8_t thr){
    ui.tZonesTable->setThreadList(thr);
}


void SPRZonesRangesTableWidget2::setMasterMode(bool value)
{
    this->setEnabled(value);
}
