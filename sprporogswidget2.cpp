#include "sprporogswidget2.h"

#include "models/sprmainmodel.h"
#include "models/sprhistorymodel2.h"
#include <QDoubleSpinBox>
#include <QCheckBox>
#include <QAbstractTableModel>

#include <QSpinBox>

SPRPorogsFormMode SPRPorogsWidget2::getWidgetMode() const
{
    return widgetMode;
}

void SPRPorogsWidget2::setWidgetMode(const SPRPorogsFormMode &value)
{
    widgetMode = value;
    widgetsShow();
}

SPRPorogsWidget2::SPRPorogsWidget2(QWidget *parent) :
    QWidget(parent), porogsModel(nullptr), mainModel(nullptr), tableModel(nullptr), delegate(nullptr)
{
    ui.setupUi(this);

    tableModel = new SPRTablePorogsModel(this);
    delegate = new SPRTablePorogsDelegate(this);


    ui.tPorogs->setModel(tableModel);
    ui.tPorogs->setSelectionMode(QAbstractItemView::ExtendedSelection);
    ui.tPorogs->setDragEnabled(true);
    ui.tPorogs->setAcceptDrops(true);
    ui.tPorogs->setDropIndicatorShown(true);
    ui.tPorogs->setItemDelegate(delegate);
    ui.tPorogs->setEditTriggers(QTableView::AllEditTriggers);

    widgetMode = forSettings;
//    Qt::ItemFlags MyModel::flags(const QModelIndex &index) const
//    {
//        return Qt::ItemIsEditable | QAbstractTableModel::flags(index);
//    }
}

ISPRModelData *SPRPorogsWidget2::setModelData(ISPRModelData *_model)
{
    mainModel = _model->getMainModel();
    if(mainModel){
        porogsModel = mainModel->getSettingsPorogsModel();
        tableModel->setParentWidget(this);
        if(porogsModel){

            connect(porogsModel, &SPRSettingsPorogsModel::modelChanget, this, &SPRPorogsWidget2::onModelChanget);

            QObject::connect(ui.bEquial, &QAbstractButton::clicked, this, &SPRPorogsWidget2::onClickEquialentButton);

            QObject::connect(ui.rbH1, &QAbstractButton::toggled, this, &SPRPorogsWidget2::onChangeTypeConditions);
            QObject::connect(ui.rbH2, &QAbstractButton::toggled, this, &SPRPorogsWidget2::onChangeTypeConditions);
            QObject::connect(ui.rbH3, &QAbstractButton::toggled, this, &SPRPorogsWidget2::onChangeTypeConditions);

            QObject::connect(ui.cbInvert, &QAbstractButton::toggled, this, &SPRPorogsWidget2::onChangeInvertion);
            QObject::connect(ui.rbConcentrat, &QAbstractButton::toggled, this, &SPRPorogsWidget2::onChangeTailOrConcentrate);
            QObject::connect(ui.rbTail, &QAbstractButton::toggled, this, &SPRPorogsWidget2::onChangeTailOrConcentrate);

            QObject::connect(mainModel->getServer(), &ServerConnect::serverStateChangeOut, this, &SPRPorogsWidget2::onServerStateChange);
            QObject::connect(ui.pbDown, &QPushButton::clicked, this, &SPRPorogsWidget2::onClickedUpDownButtons);
            QObject::connect(ui.pbUp, &QPushButton::clicked, this, &SPRPorogsWidget2::onClickedUpDownButtons);

            QObject::connect(ui.rbAvrMaterial, &QRadioButton::toggled, this, &SPRPorogsWidget2::onChangeAvrType);
            QObject::connect(ui.rbAvrProduct, &QRadioButton::toggled, this, &SPRPorogsWidget2::onChangeAvrType);
            QObject::connect(ui.bCalculate, &QPushButton::clicked, this, &SPRPorogsWidget2::onClickedCalculateButton);

            QObject::connect(ui.bCancel, &QPushButton::clicked, this, &SPRPorogsWidget2::onClickedCommandButtons);
            QObject::connect(ui.bComplite, &QPushButton::clicked, this, &SPRPorogsWidget2::onClickedCommandButtons);
            QObject::connect(ui.bExit, &QPushButton::clicked, this, &SPRPorogsWidget2::onClickedCommandButtons);

            connect(ui.leXRayCorrection, &QSpinBox::editingFinished, this, &SPRPorogsWidget2::onChangeXRay);

            QObject::connect(porogsModel, &SPRSettingsPorogsModel::modelChanget, tableModel, &SPRTablePorogsModel::onModelChanget);

    //        QObject::connect(ui.tPorogs, &QTableView::activated, this, &SPRPorogsWidget2::onChandeActivate);
        }
    }
    return _model;
}

//void SPRPorogsWidget2::onChandeActivate(const QModelIndex _index){
//    if(porogsModel){
//        if(_index.row() < mapConditions[porogsModel->getConditions()->getData()]
//                && _index.column() < porogsModel->getThreads()->getData()){
//            ui.bEquial->setEnabled(true);
//        } else {
//            ui.bEquial->setEnabled(false);
//        }
//    }
//}


void SPRPorogsWidget2::onClickedCommandButtons(bool){
    if(sender() == ui.bComplite){
        if(porogsModel){
            porogsModel->doStore();
            return;
        }
    }
    if(sender() == ui.bCancel){
        if(porogsModel){
            porogsModel->doRestore();
            return;
        }
    }
    if(sender() == ui.bExit){
        if(!parent()){
            close();
        }
    }
}

void SPRPorogsWidget2::onClickedCalculateButton(bool){
    if(mainModel){
        SPRThreadList thl = mainModel->getThreadsList();
//        SPRSeparateModel *smodel = mainModel->getSeparateModel();
        SPRHistoryModel2 *smodel = mainModel->getHistoryModel2();
        if(smodel){
            foreach(uint8_t th, thl){
                QVector<QPointF> *data = smodel->getGistogrammContentData2(th);
                double summAll = 0;
                double summConsentrate = 0;
                double porogNow = porogsModel->getPorog(th, 0)->getData();

                for(int i=0; i< (*data).size(); i++){
                    summAll += (*data)[i].ry();
                    double x = (*data)[i].rx();
                    if(x > porogNow){
                        summConsentrate += (*data)[i].ry();
                    }
                }
                double summNeed = 0;
                if(ui.rbAvrProduct->isChecked()){
                    double pKoeff = static_cast<double>(ui.lePercent->value()) / 100.;
                    summNeed = summAll * pKoeff;
                } else {
                    double scNew = summConsentrate + (summConsentrate * static_cast<double>(ui.lePercent->value()) / 100);
                    summNeed = summAll - scNew;
                }
                double summ2 = 0; int ind = 0;
                while(ind < (*data).size()){
                    summ2 += (*data)[ind].ry();
                    if(summ2 >= summNeed){
                        break;
                    }
                    ind++;
                }
                double porog = ind < (*data).size() ? (*data)[ind].rx() : (*data).last().rx();
                porogsModel->setPorog(th, 0, porog);
            }

        }
    }
}

void SPRPorogsWidget2::setMasterMode(bool value)
{
    if(widgetMode == forSettings){
        this->setEnabled(value);
    }
}

void SPRPorogsWidget2::onChangeAvrType(bool value){
    if(value){
        if(sender() == ui.rbAvrMaterial){
            ui.gbPercents->setTitle(tr("Граница (+/-%)"));
            ui.lePercent->setMinimum(-100);
        } else if(sender() == ui.rbAvrProduct){
            ui.gbPercents->setTitle(tr("Желаемый выход концентрата (%)"));
            ui.lePercent->setMinimum(0);
        }
    }
}

void SPRPorogsWidget2::onClickedUpDownButtons(bool){
    if(sender() == ui.pbUp){
        if(ui.lePercent->value() < ui.lePercent->maximum()){
            ui.lePercent->setValue(ui.lePercent->value() + 1);
        }
        return;
    }
    if(sender() == ui.pbDown){
        if(ui.lePercent->value() > ui.lePercent->minimum()){
            ui.lePercent->setValue(ui.lePercent->value() - 1);
        }
        return;
    }
}

void SPRPorogsWidget2::onServerStateChange(uint32_t){
    if(mainModel){
        if(mainModel->getServer()->isState(spr_state_separated)){
            ui.bCalculate->setEnabled(true);
        } else {
            ui.bCalculate->setEnabled(false);
        }
    }
}

void SPRPorogsWidget2::onClickEquialentButton(bool){
    if(porogsModel){
        QModelIndex index = ui.tPorogs->currentIndex();
        TypeConditions cond = porogsModel->getConditions()->getData();
        int rows = mapConditions[cond];

        if(index.row() < rows &&
                index.column() < porogsModel->getThreads()->getData()){
            double val = ui.tPorogs->model()->data(index).toDouble();
            for(uint8_t th=0; th<porogsModel->getThreads()->getData(); th++){
                porogsModel->setPorog(th, index.row(), val);
            }
            ui.tPorogs->show();
        }
    }
}

void SPRPorogsWidget2::onChangeTypeConditions(bool value)
{
    if(porogsModel){
        if(value){
            if(sender() == ui.rbH1){
                porogsModel->getConditions()->setData(TypeConditions::H1);
            } else if(sender() == ui.rbH2){
                porogsModel->getConditions()->setData(TypeConditions::H2);
            } else if(sender() == ui.rbH3){
                porogsModel->getConditions()->setData(TypeConditions::H3);
            }
        }
    }
}

void SPRPorogsWidget2::onChangeInvertion(bool value)
{
    if(porogsModel){
        if(sender() == ui.cbInvert){
            porogsModel->getInvertSelection()->setData(value);
        }
    }
}

void SPRPorogsWidget2::onChangeXRay()
{
    if(porogsModel){
        if(sender() == ui.leXRayCorrection){
            porogsModel->xRayCorrection->setData(ui.leXRayCorrection->value());
        }
    }
}

void SPRPorogsWidget2::onChangeTailOrConcentrate(bool value){
    if(porogsModel){
        if(value){
            if(sender() == ui.rbConcentrat){
                porogsModel->getTypeSelection()->setData(TypeSelection::OnConsentrate);
            } else if(sender() == ui.rbTail){
                porogsModel->getTypeSelection()->setData(TypeSelection::OnTail);
            }
        }
    }
}

ISPRModelData *SPRPorogsWidget2::getModelData()
{
    return porogsModel;
}

void SPRPorogsWidget2::widgetsShow()
{
    if(porogsModel){
        TypeConditions ts = porogsModel->getConditions()->getData();

        ui.rbH1->blockSignals(true); ui.rbH2->blockSignals(true); ui.rbH3->blockSignals(true);
        switch(ts){

        case H1: {
            ui.rbH1->setChecked(true);
            }
            break;
        case H2: {
            ui.rbH2->setChecked(true);

            }
            break;
        case H3:{
            ui.rbH3->setChecked(true);
            }
            break;
        }
        ui.rbH1->blockSignals(false); ui.rbH2->blockSignals(false); ui.rbH3->blockSignals(false);

        TypeSelection tsel = porogsModel->getTypeSelection()->getData();
        ui.rbTail->blockSignals(true); ui.rbConcentrat->blockSignals(true);
        if(tsel == OnTail){
            ui.rbTail->setChecked(true);
        } else {
            ui.rbConcentrat->setChecked(true);
        }
        ui.rbTail->blockSignals(false); ui.rbConcentrat->blockSignals(false);

        ui.cbInvert->blockSignals(true);
        ui.cbInvert->setChecked(porogsModel->getInvertSelection()->getData());
        ui.cbInvert->blockSignals(false);


        ui.leXRayCorrection->blockSignals(true);
        ui.leXRayCorrection->setValue(porogsModel->xRayCorrection->getData());
        ui.leXRayCorrection->blockSignals(false);

        tableModel->onModelChanget(nullptr);

        switch(widgetMode){
        case forSeparate:{
                ui.wDownSettings->setVisible(false);
                ui.gbDownCorrects->setVisible(true);
                ui.bCalculate->setVisible(true);
            }
            break;
        case forSettings:{
                ui.wDownSettings->setVisible(true);
                ui.gbDownCorrects->setVisible(false);
                ui.bCalculate->setVisible(false);
            }
            break;
        case allWisible:{
                ui.wDownSettings->setVisible(true);
                ui.gbDownCorrects->setVisible(true);
                ui.bCalculate->setVisible(true);
            }
            break;
        }

    }
}

void SPRPorogsWidget2::onModelChanget(IModelVariable *var)
{
    QList<IModelVariable*> myVariables = {
        porogsModel->getConditions(), porogsModel->getTypeSelection(), porogsModel->xRayCorrection, porogsModel->invertSelection
    };

    QList<IModelVariable*> ttt = porogsModel->getChildren();

    myVariables.append(porogsModel->getThreads());
    SPRMainModel *mod = porogsModel->getMainModel();
//    if(mod){
//        SPRThreadList thl = mod->getThreadsList();
//        SPRPorogsModel *porogs = porogsModel->getPorogs();
//        if(porogs){
//            myVariables.append(porogs->getAllPorogs());
//        }
//    }
    IModelVariable *send = dynamic_cast<IModelVariable*>(sender());
    if((send && myVariables.contains(send)) || myVariables.contains(var)){
        widgetsShow();
    }
}

/**
 * @brief SPRTablePorogsModel::setParentWidget
 * @param _wParent
 */
Qt::ItemFlags SPRTablePorogsModel::flags(const QModelIndex &index) const
{
    return Qt::ItemIsEditable | QAbstractTableModel::flags(index);
}

void SPRTablePorogsModel::onModelChanget(IModelVariable *)
{
    beginResetModel();
    endResetModel();
}

void SPRTablePorogsModel::setParentWidget(SPRPorogsWidget2 *_wParent)
{
    wParent = _wParent;
    if(wParent){
        wModel = dynamic_cast<SPRSettingsPorogsModel*>(wParent->getModelData());
    }
}

int SPRTablePorogsModel::rowCount(const QModelIndex &parent) const
{
    if(wModel){
        return mapConditions[wModel->getConditions()->getData()];
    } else {
        return MAX_SPR_FORMULA_CONDITION;
    }
}

int SPRTablePorogsModel::columnCount(const QModelIndex &parent) const
{
    if(wModel){
        return wModel->getThreads()->getData();
    } else {
        return MAX_SPR_MAIN_THREADS;
    }
}

QVariant SPRTablePorogsModel::data(const QModelIndex &index, int role) const
{
    QVariant res;
    if(role == Qt::DisplayRole || role == Qt::EditRole){
        if(wModel){
            if(index.column() < wModel->getThreads()->getData()
                    && index.row() < mapConditions[wModel->getConditions()->getData()]){
               res = wModel->getPorog(index.column(), index.row())->getData();
            }
        }
    }
    if(role == Qt::ToolTipRole){
        if(wModel){
            if(index.column() < wModel->getThreads()->getData()
                && index.row() < mapConditions[wModel->getConditions()->getData()]){
                QString HX, value = QString::number(wModel->getPorog(index.column(), index.row())->getData()+1, 'f', 2),
                        rUrovn = headerData(index.row(), Qt::Vertical,Qt::DisplayRole).toString(),
                        th = QString::number(index.column()+1);
                switch (index.row()) {
                case 0: HX = "H1"; break;
                case 1: HX = "H2"; break;
                case 2: HX = "H2"; break;
                case 3: HX = "H2"; break;
                case 4: HX = "H3"; break;
                case 5: HX = "H3"; break;
                }
                
                
                QString tt = tr("Значение порога сортировки %1 ручья %2 (%3)").arg(HX).arg(th).arg(rUrovn);
                res = QVariant(tt);
            }
        }
    }
    if(role == Qt::TextAlignmentRole){
        res = QVariant(Qt::AlignCenter);
    }
    return  res;
}

bool SPRTablePorogsModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if(role == Qt::EditRole){
        bool isOk = false;
        if(wModel){
            if(index.column() < wModel->getThreads()->getData() &&
                    index.row() < mapConditions[wModel->getConditions()->getData()]){
                double val = value.toDouble(&isOk);
                wModel->blockSignals(true);
                wModel->setPorog(index.column(), index.row(), val);
                wModel->blockSignals(false);
            }
        }
        return isOk;
    }
    return QAbstractItemModel::setData(index, value, role);
}

QVariant SPRTablePorogsModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    QVariant res = QAbstractTableModel::headerData(section, orientation, role);
    if(role == Qt::DisplayRole){
        if(orientation == Qt::Horizontal){
            QString txt = tr("Ручей %1").arg(section + 1);
            res = QVariant(txt);
        } else if(orientation == Qt::Vertical){
            switch (section) {
            case 0:
                res = QVariant(tr("H1>"));
                break;
            case 1:
                res = QVariant(tr("и H2<"));
                break;
            case 2:
                res = QVariant(tr("или H2<"));
                break;
            case 3:
                res = QVariant(tr("или H2>"));
                break;
            case 4:
                res = QVariant(tr("иил H3<"));
                break;
            case 5:
                res = QVariant(tr("или H3>"));
                break;
            default:
                break;
            }
        }
    }
    return res;
}

/**
 * @brief SPRTablePorogsDelegate::createEditor
 * @param parent
 * @param option
 * @param index
 * @return
 */

SPRTablePorogsDelegate::SPRTablePorogsDelegate(SPRPorogsWidget2 *_wParent): QStyledItemDelegate (_wParent)
{
}

QWidget *SPRTablePorogsDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QDoubleSpinBox * lineEdit = new QDoubleSpinBox(parent);
    lineEdit->setDecimals(3);
    lineEdit->setRange(-99, 99);
    lineEdit->setSingleStep(0.01);
//    QDoubleValidator *validator = new QDoubleValidator(-99, 99, 2);
//    lineEdit->v;
    return lineEdit;
}

void SPRTablePorogsDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    //в этой функции на входе данные из модели и указатель на виджет редактора
    QString value = index.model()->data(index).toString(); //получаем что уже есть в модели
    QDoubleSpinBox *e = static_cast<QDoubleSpinBox*> (editor); //преобразуем указатель
    e->setValue(value.toDouble()); //устанавливаем текст
}

void SPRTablePorogsDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    //сюда попадаем когда редактор делегата теряем фокус/закрывается
    QDoubleSpinBox *e = static_cast<QDoubleSpinBox*> (editor);
    model->setData(index, QString::number(e->value(), 'f', 3)); //устанавливаем новое значение в модель
}


