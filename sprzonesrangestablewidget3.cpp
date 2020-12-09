#include "sprzonesrangestablewidget3.h"
#include "sprelementpropertydialog.h"
#include "sprzonesranges.h"
#include "_types.h"
#include "models/sprmainmodel.h"
#include "models/sprspectrumlistitemsmodel.h"
#include "models/sprseparatorlogsmodel.h"

#include <QMessageBox>

ISPRModelData *SPRZonesRangesTableWidget3::setModelData(SPRSpectrumZonesTableModel *value)
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

void SPRZonesRangesTableWidget3::onGotFocus(EnumElements el, uint8_t th, SPRTypeMinMaxField5 type)
{
    if(model){
        lastCurrent.element = el;
        lastCurrent.type = type;
        lastCurrent.thread = th;
        if(th < model->getThreads()->getData() && (type == minField5 || type == maxField5)){
            lastCurrent.ok = true;
        } else {
            lastCurrent.ok = false;
        }
    } else {
        lastCurrent.ok = false;
    }
//    qDebug() << "el:" << lastCurrent.element << " type:" << lastCurrent.type << " th:" << lastCurrent.thread << " ok:" << lastCurrent.isOk() << " zone:" << lastCurrent.thZone;
    showEnabled();
}

SPRZonesRangesTableWidget3::SPRZonesRangesTableWidget3(QWidget *parent) :
    QWidget(parent), model(nullptr), recompliteBaseSpectrumsVisible(true)
{
    ui.setupUi(this);

    connect(ui.bAddElement, SIGNAL(clicked(bool)), this, SLOT(onAddClick(bool)));
    connect(ui.bEditElement, SIGNAL(clicked(bool)), this, SLOT(onEditClick(bool)));
    connect(ui.bRoundRanges, SIGNAL(clicked(bool)), this, SLOT(onRoundClick(bool)));

    ui.bRecompliteBaseSpectrums->setEnabled(false);
    connect(ui.bRecompliteBaseSpectrums, &QPushButton::clicked, this, &SPRZonesRangesTableWidget3::onRecompliteBaseSpectrums);

    connect(ui.tZonesTable, &SPRZonesTable5::gotFocus, this, &SPRZonesRangesTableWidget3::onGotFocus);
//    connect(ui.tZonesTable, &SPRZonesRangesTable3::updateSize, this, &SPRZonesRangesTableWidget3::onUpdateSize);
    editControlEnable = false;

    ui.tZonesTable->resize(QSize(this->width(), ui.tZonesTable->height()));
    showEnabled();
}

void SPRZonesRangesTableWidget3::onRoundClick(bool){
//    bool noErrors = false;
    if(lastCurrent.isOk()){
        SPRSpectrumZonesModel *zone = model->getItem(lastCurrent.thread);
        QMapElementsRanges zr = zone->getZones();
        if(zr.contains(lastCurrent.element)){
            SpectorRange *sr = zr[lastCurrent.element];
            uint min = sr->min->getData();
            uint max = sr->max->getData();
            QString sName = zone->getElementsProperty()->getSName(lastCurrent.element);
            QString fName = zone->getElementsProperty()->getFName(lastCurrent.element);
            QString txt = tr("Вы действительно хотите применить %1 значение\nдля области %2 ручья %3 равной %4-му каналу\nвсех остальных ручьев?").
                        arg(lastCurrent.type == minField5 ? tr("минимальное") : tr("максимальное")).
                        arg(fName).
                        arg(lastCurrent.thread +1).
                        arg(lastCurrent.type == minField5 ? min : max);
            int res = QMessageBox::question(nullptr, tr("Внимание"), txt);
            if(res == QMessageBox::Yes){
                for(uint8_t th=0; th<MAX_SPR_MAIN_THREADS; th++){
                    if(lastCurrent.type == minField5){
                        model->items[th]->setMin(lastCurrent.element, min);
                    } else if(lastCurrent.type == maxField5){
                        model->items[th]->setMax(lastCurrent.element, max);
                    }
                }
            }
        }
    } else {
        QMessageBox::warning(nullptr,tr("Ошибка"), tr("Не выбран текущий елемент в таблице..."));
    }
}

void SPRZonesRangesTableWidget3::onEditClick(bool){
    if(lastCurrent.isOk()){
        SPRSpectrumZonesModel *zone = model->getItem(lastCurrent.thread);
        SPRElementPropertyDialog *dialog = new SPRElementPropertyDialog(zone, lastCurrent.element, this);
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


}

void SPRZonesRangesTableWidget3::onAddClick(bool){
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
        }
    }

    delete dialog;
//    widgetsShow();
}

void SPRZonesRangesTableWidget3::widgetsShow()
{
    if(model){
//        ui.tZonesTable->widgetsShow();
        showEnabled();
    }
}

void SPRZonesRangesTableWidget3::onUpdateSize()
{
    updateGeometry();
    QSize s1 = ui.tZonesTable->sizeHint();
//    QSize s2 = ui.wZones->sizeHint();
    QSize s3 = ui.wVButtons->sizeHint();
    QSize s4 = ui.wHBottons->sizeHint();
    QSize s5 = ui.wSelf->sizeHint();

    int w = qMax(s1.width(), s3.width());
    int h = s3.height() + s1.height();
    QSize res = QSize(w+2, h+5);
    setMinimumSize(res);


    resize(res);
    setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
}

void SPRZonesRangesTableWidget3::onRecompliteBaseSpectrums(bool)
{
    if(model){
        SPRMainModel *mainModel = model->getMainModel();
        if(mainModel){
            SPRSpectrumListItemsModel *spModel = mainModel->getSpectrumListItemsModel();
            if(spModel){
                QList<SPRSpectrumItemModel*> *bLst = spModel->getSpectrumsModelBase();
                if(bLst && bLst->size() > 0){
                    uint32_t res = QMessageBox::No;
                    res = QMessageBox::warning(nullptr, tr("Необходимо подтверждение"),
                                         tr("Вы действительно хотите пересчитать БАЗОВЫЕ спектры?"),
                                         QMessageBox::Yes, QMessageBox::No);
                    if(res == QMessageBox::Yes){

                        model->doStore();

                        foreach(SPRSpectrumItemModel *item, (*bLst)){
                            if(item){
                                item->recomplite();
                            }
                        }
                        bool res = mainModel->getSpectrumListItemsModel()->saveAsAllData();
                        if(mainModel->getLogModel()){
                            mainModel->getLogModel()->addLogMessage(nullptr, tWarning, tr("Базовые спектры пересчитаны..."));
                            if(res){
                                mainModel->getLogModel()->addLogMessage(nullptr, tWarning, tr("Спектры сохранены в файле %1").arg(mainModel->getSpectrumFName()->getData()));
                            } else {
                                mainModel->getLogModel()->addLogMessage(nullptr, tError, tr("Ошибка сохранения спектров в файле %1").arg(mainModel->getSpectrumFName()->getData()));
                            }
                        }
                        ui.bRecompliteBaseSpectrums->setEnabled(false);
                    }
                }

            }
        }
    }
}

void SPRZonesRangesTableWidget3::showEnabled(){
    if(lastCurrent.isOk()){
        ui.bEditElement->setEnabled(true);
        ui.bRoundRanges->setEnabled(true);
    } else {
        ui.bEditElement->setEnabled(false);
        ui.bRoundRanges->setEnabled(false);
    }
    ui.bRecompliteBaseSpectrums->setVisible(recompliteBaseSpectrumsVisible);
}

ISPRModelData *SPRZonesRangesTableWidget3::getModelData()
{
    return model;
}


void SPRZonesRangesTableWidget3::onModelChanget(IModelVariable *)
{
    if(sender() == model){
        ui.bRecompliteBaseSpectrums->setEnabled(true);
    }
}

void SPRZonesRangesTableWidget3::setThreadsVisible(QList<uint8_t> visible){
    ui.tZonesTable->setThreadList(visible);
}

void SPRZonesRangesTableWidget3::setThreadsVisible(uint8_t thr){
    ui.tZonesTable->setThread(thr);
}

void SPRZonesRangesTableWidget3::setRecompliteButtonVisible(bool value)
{
    recompliteBaseSpectrumsVisible = value;
    ui.bRecompliteBaseSpectrums->setVisible(recompliteBaseSpectrumsVisible);
}


void SPRZonesRangesTableWidget3::setMasterMode(bool value)
{
    this->setEnabled(value);
}


void SPRZonesRangesTableWidget3::resizeEvent(QResizeEvent *event)
{
    onUpdateSize();
}
