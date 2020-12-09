#include "sprzonesrange4.h"

#include <QHeaderView>

/**
 * @brief SPRZoneRangesTableGelegate::createEditor
 * @param parent
 * @param option
 * @param index
 * @return
 */
QWidget *SPRZoneRangesTableGelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QLineEdit *editor = new QLineEdit(parent);
    editor->setValidator(new QIntValidator(0, 255, editor));
    return editor;

}

void SPRZoneRangesTableGelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    QLineEdit *ed = qobject_cast<QLineEdit*>(editor);
    if(ed){
        ed->setText(index.model()->data(index).value<QString>());
    }

}

void SPRZoneRangesTableGelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QLineEdit *ed = qobject_cast<QLineEdit*>(editor);
    if(ed){
        model->setData(index, QVariant(ed->text()));
    }
}

/**
 * @brief SPRZoneRangesTableModel::setModelData
 * @param _model
 */
void SPRZoneRangesTableModel::setModelData(SPRSpectrumZonesModel *_model){
    if(zModel) delete zModel;
    zModel = _model;
    if(zModel){
        connect(zModel, &SPRSpectrumZonesModel::modelChanget, this, &SPRZoneRangesTableModel::onModelChanget);
    }
}

void SPRZoneRangesTableModel::onModelChanget(IModelVariable *var){
    if(sender() == zModel){
        resetModel();
    }
}

void SPRZoneRangesTableModel::resetModel(){
    beginResetModel();
    endResetModel();
//    emit dataChanged(index(0,0), index(rowCount(QModelIndex())-1, columnCount(QModelIndex())-1));
}

int SPRZoneRangesTableModel::rowCount(const QModelIndex &parent) const
{
    if(zModel){
        return zModel->getElementsProperty()->getElements().size();
    }
    return 0;
}

int SPRZoneRangesTableModel::columnCount(const QModelIndex &parent) const
{
    return 2;
}

QVariant SPRZoneRangesTableModel::data(const QModelIndex &index, int role) const
{
    QVariant res;
    if(zModel){
        if(index.row() < rowCount(QModelIndex()) && index.column() < 2){
            EnumElements el = zModel->getElements().keys()[index.row()];
            if(role == Qt::DisplayRole || role == Qt::EditRole){
                if(index.column() == 0){
                    res = QVariant(QString::number(zModel->getElements()[el]->min->getData()));
                } else if(index.column() == 1){
                    res = QVariant(QString::number(zModel->getElements()[el]->max->getData()));
                }
            }
            if(role == Qt::ToolTipRole){
                if(index.column() == 0){
                    res = QVariant(tr("Минимальное значение для %1 - %2").arg(zModel->getElementsProperty()->getFName(el)).arg(zModel->getElements()[el]->min->getData()));
                } else if(index.column() == 1){
                    res = QVariant(tr("Максимальное значение для %1 - %2").arg(zModel->getElementsProperty()->getFName(el)).arg(zModel->getElements()[el]->max->getData()));
                }
            }
            if(role == Qt::TextAlignmentRole){
                res = QVariant(Qt::AlignCenter);
            }

        }
    }
    return res;
}

bool SPRZoneRangesTableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    bool res = false;
    if(zModel){
        if(index.row() < rowCount(QModelIndex()) && index.column() < 2){
            if(role == Qt::EditRole){
                bool isOk;
                int iVal= value.toInt(&isOk);
                if(isOk){
                    EnumElements el = zModel->getElements().keys()[index.row()];
                    SpectorRange *ranges = zModel->getElements()[el];
                    uint min = ranges->min->getData(); uint max = ranges->max->getData();
                    if(index.column() == 0){
                        min = iVal;
                    } else if(index.column() == 1){
                        max = iVal;
                    }
                    isOk = zModel->setMinMax(el, min, max);
                }
                return isOk;
            }
        }
    }
    return res;
}

QVariant SPRZoneRangesTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    QVariant res = QAbstractTableModel::headerData(section, orientation, role);
    if(role == Qt::DisplayRole && orientation == Qt::Horizontal){
        res = section == 0 ? QVariant(QString("<<")) : QVariant(QString(">>"));
    }
    return res;
}

Qt::ItemFlags SPRZoneRangesTableModel::flags(const QModelIndex &index) const
{
    return Qt::ItemIsEditable | QAbstractTableModel::flags(index);
}

/**
 * @brief SPRZoneRangeTable::SPRZoneRangeTable
 * @param parent
 */
SPRZoneRangeTable::SPRZoneRangeTable(QWidget *parent): QTableView(parent){
    verticalHeader()->setVisible(false);
}

//void SPRZoneRangeTable::clearFocus()
//{
//    setCurrentIndex(QModelIndex());
//}

void SPRZoneRangeTable::currentChanged(const QModelIndex &current, const QModelIndex &previous)
{
    emit gotFocus(current);
}

int SPRZoneRangeTable::sizeHintForColumn(int ) const
{
    int w = fontMetrics().width("999999");
    return w;
}


/**
 * @brief SPRZonesRange4::SPRZonesRange4
 * @param parent
 */

bool SPRZonesRange4::getFirstColumnVisible() const
{
    return firstColumnVisible;
}

void SPRZonesRange4::setFirstColumnVisible(bool value)
{
    firstColumnVisible = value;
    fcWidget->setVisible(value);
}

SPRZonesRange4::SPRZonesRange4(QWidget *parent) : QWidget(parent)
{
    zoneModel = nullptr;

    fcWidget = new QTableWidget(0, 1, nullptr);
    fcWidget->setColumnCount(1);
    fcWidget->setRowCount(0);
    fcWidget->horizontalHeader()->setVisible(true);
    fcWidget->setHorizontalHeaderLabels(QStringList({"*"}));
    fcWidget->verticalHeader()->setVisible(false);
    fcWidget->setGridStyle(Qt::SolidLine);

    tableModel = new SPRZoneRangesTableModel();
    tRanges = new SPRZoneRangeTable();
    delegate = new SPRZoneRangesTableGelegate();


}

SPRZonesRange4::~SPRZonesRange4()
{
//    if(zoneModel) delete zoneModel;
    if(fcWidget) delete fcWidget;
    if(tableModel) delete tableModel;
    if(tRanges) delete tRanges;
    if(delegate) delete delegate;
}

void SPRZonesRange4::setModelData(SPRSpectrumZonesModel *_zModel){
    zoneModel = _zModel;
    if(zoneModel){

        //            if(tableModel) delete tableModel;
        //            tableModel = new SPRZoneRangesTableModel();
        tableModel->setModelData(zoneModel);

        threadNumber = zoneModel->getThreadNumber();

        //            tRanges - new SPRZoneRangeTable(this);
        tRanges->setModel(tableModel);

        //            delegate = new SPRZoneRangesTableGelegate();
        tRanges->setItemDelegate(delegate);

        firstColumnVisible = true;
        threadNumber = 0;


        tRanges->setItemDelegate(new SPRZoneRangesTableGelegate());

        tRanges->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
//        tRanges->setAlternatingRowColors(true);
        tRanges->setSelectionMode(QAbstractItemView::SingleSelection);
        tRanges->setSelectionBehavior(QAbstractItemView::SelectItems);
        tRanges->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        tRanges->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        tRanges->setSizeAdjustPolicy(QAbstractItemView::AdjustToContents);
        fcWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        fcWidget->setSelectionMode(QAbstractItemView::NoSelection);
        fcWidget->setSelectionBehavior(QAbstractItemView::SelectItems);

//        fcWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        fcWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        fcWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        fcWidget->setSizeAdjustPolicy(QAbstractItemView::AdjustToContents);

        QGridLayout *gLayout = new QGridLayout();
        gLayout->addWidget(fcWidget,0,0,1,1,Qt::AlignCenter);
        gLayout->addWidget(tRanges,0,1,1,1,Qt::AlignCenter);
        gLayout->setMargin(0);
        gLayout->setSpacing(0);

        setLayout(gLayout);

        connect(zoneModel, &SPRSpectrumZonesModel::modelChanget, this, &SPRZonesRange4::onModelChanget);
        connect(tRanges, &SPRZoneRangeTable::gotFocus, this, &SPRZonesRange4::onCurrentItemChanged);


        onModelChanget(nullptr);
    }
}

void SPRZonesRange4::clearCurrent(){
    tRanges->setCurrentIndex(QModelIndex(tRanges->model()->index(-1,-1)));
//    tRanges->lost
//    tRanges->clearFocus();
//    tRanges->clearSelection();
    fcWidget->setCurrentItem(nullptr);
}

void SPRZonesRange4::onCurrentItemChanged(QModelIndex current){
    FirstCollumn2 *fc = qobject_cast<FirstCollumn2*>(fcWidget->cellWidget(current.row(), 0));
    if(fc){
        IModelVariable *mv = fc->getModelPrt();
        SPRElementsProperty *var = qobject_cast<SPRElementsProperty*>(mv);
        if(var){
            emit gotFocus(this, current, var->key->getData());
        }
    }
}

void SPRZonesRange4::onColorChanget(IModelVariable *var, QColor color){
    if(zoneModel){
        SPRElementsProperty *v = qobject_cast<SPRElementsProperty*>(var);
        if(v){
            v->color->setData(color);
        }
    }
}

void SPRZonesRange4::onDeleteElement(IModelVariable *var){
    SPRElementsProperty *em = dynamic_cast<SPRElementsProperty*>(var);
    if(em){
        if(em->key->getData() == Ns){
            QMessageBox::warning(nullptr, tr("Ошибка"), tr("Элемент %1 удалить нельзя...\n"
                                                           "Это важный параметр в системе...").arg(em->fName->getData()));
            return;

        }
        QString txt = tr("Вы действительно хотите удалить елемент %1 из списка?").
                arg(em->fName->getData());
        int res = QMessageBox::question(nullptr, tr("Внимание"), txt);
        if(res == QMessageBox::Yes){
            SPRMainModel *mm = zoneModel->getMainModel();
            if(mm){
                SPRSettingsFormulaModel *fm = mm->getSettingsFormulaModel();
                int numFormula = -1;
                if(fm->isUsed(em->key->getData(), &numFormula)){
                    QMessageBox::warning(nullptr, tr("Ошибка"), tr("Элемент %1 удалить нельзя...\n"
                                                                   "Он используется в расчете параметра H%2").arg(em->fName->getData()).arg(numFormula+1));
                    return;
                }
                if(mm->getSettingsControlModel()->controlArea->getData() == em->key->getData()){
                    QMessageBox::warning(nullptr, tr("Ошибка"), tr("Элемент %1 удалить нельзя...\n"
                                                                   "Он используется для контроля корреляции...").arg(em->fName->getData()));
                    return;

                }
                EnumElements el = em->key->getData();
                zoneModel->deleteElement(el);
            }
        }
    }

}

void SPRZonesRange4::onModelChanget(IModelVariable *){
    if(!sender() || sender() == zoneModel){
        bool sizeChanget = false;
        if(firstColumnVisible){
            int rowsCount = zoneModel->getElementsProperty()->getElements().size();
            int r = 0;
            for(; r<rowsCount; r++){
                EnumElements el = zoneModel->getElements().keys()[r];
                SPRElementsProperty *elProp = zoneModel->getElementsProperty()->getElements()[el];
                if(fcWidget->rowCount() <= r){
                    FirstCollumn2 *f2 = new FirstCollumn2();
                    connect(f2, &FirstCollumn2::colorChanged, this, &SPRZonesRange4::onColorChanget);
                    connect(f2, &FirstCollumn2::deleteRow, this, &SPRZonesRange4::onDeleteElement);

                    fcWidget->insertRow(fcWidget->rowCount());
                    sizeChanget = true;
                    fcWidget->setCellWidget(r, 0, f2);
                }
                QWidget *w = fcWidget->cellWidget(r,0);
                FirstCollumn2 *fc = qobject_cast<FirstCollumn2*>(w);
                if(fc){
                    fc->setParams(r, elProp->sName->getData(),
                                  elProp->color->getData(),
                                  false, true, true, false);
                    fc->setModelPrt(zoneModel->getElementsProperty()->getElementProperty(el));
                    fc->widgetsShow();
                }
            }
            rowsCount = fcWidget->rowCount();
//            fcWidget->blockSignals(true);
            while(fcWidget->rowCount() > r){
                fcWidget->removeRow(fcWidget->rowCount()-1);
                sizeChanget = true;
            }
            fcWidget->resizeColumnsToContents();
            fcWidget->setMinimumSize(fcWidget->sizeHint());
            //                fcWidget->resizeRowsToContents();
//            fcWidget->blockSignals(false);

        }
        tRanges->resizeColumnsToContents();
        QSize s = sizeHint();
        setMinimumSize(sizeHint());
        resize(s.width(), s.height());
//        setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

        updateGeometry();
        if(sender() && sizeChanget){
            emit rowCountChanget();
        }
        //            tableModel->resetModel();
    }
}

QSize SPRZonesRange4::sizeHint() const
{
    QSize s = QWidget::sizeHint();

    int w = firstColumnVisible ? fcWidget->sizeHint().width() + fcWidget->lineWidth()*2 + tRanges->lineWidth() : tRanges->lineWidth();
//    w += tRanges->horizontalHeader()->sectionSize(0) + tRanges->horizontalHeader()->sectionSize(1) + tRanges->lineWidth() * 2;

    w += tRanges->minimumSize().width() + tRanges->lineWidth() * 2;

    int h = tRanges->horizontalHeader()->height()+tRanges->lineWidth();
        if(tRanges->model()){
            for(int r = 0; r < tRanges->model()->rowCount(); r++){
                h += tRanges->verticalHeader()->sectionSize(r) + tRanges->lineWidth();
            }
    }

    s.setWidth(w);
    s.setHeight(h);

    return s;
}

uint8_t SPRZonesRange4::getThreadNumber() const
{
    if(zoneModel){
        return zoneModel->getThreadNumber();
    }
    return threadNumber;
}
