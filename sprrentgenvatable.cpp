#include "sprrentgenvatable.h"
#include <QHeaderView>

#include <QSpinBox>

/**
 * @brief SPRRentgenVATableDelegate::createEditor
 * @param parent
 * @param option
 * @param index
 * @return
 */

QWidget *SPRRentgenVATableDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QSpinBox * lineEdit = new QSpinBox(parent);
    lineEdit->setRange(0, 255);
    lineEdit->setSingleStep(1);
    return lineEdit;
}

void SPRRentgenVATableDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    QSpinBox *sb = qobject_cast<QSpinBox*>(editor);
    if(sb){
        bool isOk;
        int value = index.model()->data(index).toInt(&isOk);
        if(isOk){
            sb->setValue(value);
        }
    }
}

void SPRRentgenVATableDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QSpinBox *sb = qobject_cast<QSpinBox*>(editor);
    if(sb){
//        QString txt =
        int val = sb->value();
        model->setData(index, QVariant(val));
    }
}

/**
 * @brief SPRRentgenVATableModel::columnCount
 * @param parent
 * @return
 */
int SPRRentgenVATableModel::columnCount(const QModelIndex &parent) const
{
    if(rsModel){
        return rsModel->getRentgens()->getData();
    }
}

QVariant SPRRentgenVATableModel::data(const QModelIndex &index, int role) const
{
    QVariant res;
    if(rsModel){
        if(index.column() < rsModel->getRentgens()->getData()){
            if(role == Qt::DisplayRole || role == Qt::EditRole){
                if(index.row() == 0){
                    res = QVariant(QString::number(rsModel->iTubes[index.column()]->getData()));
                } else if(index.row() == 1){
                    res = QVariant(QString::number(rsModel->uTubes[index.column()]->getData()));
                }
            }
            if(role == Qt::ToolTipRole){
                uint val = index.column() == 0 ? rsModel->iTubes[index.column()]->getData() : rsModel->uTubes[index.column()]->getData();
                if(index.row() == 0){
                    QString tt = QString(tr("Величина устанавливаемого тока для трубки №%1 - %2мкА").arg(index.column()+1).arg(val));
                    res = QVariant(tt);
                } else if(index.row() == 1){
                    QString tt = QString(tr("Величина устанавливаемого напряжения для трубки №%1 - %2кВ").arg(index.column()+1).arg(val));
                    res = QVariant(tt);
                }
            }
            if(role == Qt::TextAlignmentRole){
                res = QVariant(Qt::AlignCenter);
            }
        }
    }
    return res;
}

bool SPRRentgenVATableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if(rsModel){
        if(index.column() < rsModel->getRentgens()->getData()){
            if(role == Qt::EditRole){
                bool isOk = false;
                uint val = value.toInt(&isOk);
                if(index.row()== 0){
                    rsModel->iTubes[index.column()]->setData(val);
                } else if(index.row() == 1){
                    rsModel->uTubes[index.column()]->setData(val);
                }
                return isOk;
            }
        }
    }
    return QAbstractItemModel::setData(index, value, role);
}

QVariant SPRRentgenVATableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    QVariant res = QAbstractItemModel::headerData(section, orientation, role);
    if(rsModel){
        if(role == Qt::DisplayRole || role == Qt::EditRole){
            if(orientation == Qt::Horizontal){
                QString tt = QString(tr("Трубка №%1")).arg(section + 1);
                res = QVariant(tt);
            } else if(orientation == Qt::Vertical){
                QString tt = section == 0 ? QString(tr("I (мкА)")) : QString(tr("U (кВ)"));
                res = QVariant(tt);
            }
        }
        if(role == Qt::TextAlignmentRole){
            if(orientation == Qt::Horizontal){
                res = QVariant(Qt::AlignCenter);
            } else {
                res = QVariant(Qt::AlignLeft);
            }
        }
    }
    return res;
}

Qt::ItemFlags SPRRentgenVATableModel::flags(const QModelIndex &index) const
{
    return Qt::ItemIsEditable | QAbstractTableModel::flags(index);
}

void SPRRentgenVATableModel::setModelData(SPRSettingsRentgenModel *_rsModel){
    if(_rsModel && _rsModel != rsModel){
         connect(_rsModel, &IModelVariable::modelChanget, this, &SPRRentgenVATableModel::onModelChanget);
    }
    rsModel = _rsModel;
}

void SPRRentgenVATableModel::onModelChanget(IModelVariable *var){
    if(sender() == rsModel || var == rsModel){
//        beginResetModel();
//        endResetModel();
        emit dataChanged(this->index(0,0), this->index(rowCount(QModelIndex())-1,columnCount(QModelIndex())-1));
    }
}

SPRRentgenVATable::SPRRentgenVATable(QWidget *parent): QTableView(parent), rsModel(nullptr), tableModel(nullptr), tableDelegate(nullptr)
{
}


void SPRRentgenVATable::widgetsShow()
{
    if(rsModel){
       tableModel->onModelChanget(rsModel);
//       resizeColumnsToContents();
//       resizeRowsToContents();
//       updateGeometry();
    }
}


ISPRModelData *SPRRentgenVATable::setModelData(ISPRModelData *data)
{
    if(data){
        if(rsModel){
            delete rsModel;
        }
        rsModel = (SPRSettingsRentgenModel*)data;
        if(rsModel){
            connect(rsModel, SIGNAL(modelChanget(IModelVariable*)), this, SLOT(onModelChanget(IModelVariable*)));
        }

        tableModel = new SPRRentgenVATableModel(this);
        tableModel->setModelData(rsModel);
        this->setModel(tableModel);
        tableDelegate = new SPRRentgenVATableDelegate(this);
        this->setItemDelegate(tableDelegate);
    }
//    onModelChanget(model);
    widgetsShow();
    return rsModel;
}

ISPRModelData *SPRRentgenVATable::getModelData()
{
    return rsModel;
}

void SPRRentgenVATable::onModelChanget(IModelVariable *)
{
    if(sender() == rsModel){
//        widgetsShow();
    }
}

QSize SPRRentgenVATable::sizeHint() const
{


    QHeaderView *hhor = horizontalHeader();
    QHeaderView *hver = verticalHeader();

    QRect rect = geometry();

    QAbstractItemModel *mod = model();

    if(mod){
        int row = mod->rowCount();
        int col = mod->columnCount();

        int w = !hver->isHidden() ? hver->width()+2*lineWidth() : lineWidth();
        int h = !hhor->isHidden() ? hhor->height()+2*lineWidth() : lineWidth();
        for(int c=0; c<col; c++){
            if(!isColumnHidden(c))
                w += hhor->sectionSize(c) + lineWidth();
        }
        for(int r=0; r< row; r++){
            if(!isRowHidden(r))
                h += hver->sectionSize(r) + lineWidth();
        }

        rect.setWidth(w);
        rect.setHeight(h);
    }
//    setGeometry(rect);
//    view->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
//    view->updateGeometry();
    return rect.size();
}


