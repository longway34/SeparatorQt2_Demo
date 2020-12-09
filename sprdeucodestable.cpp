#include "sprdeucodestable.h"
#include <QHeaderView>

#include <QSpinBox>

QWidget *SPRDEUCodesTableDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
        QSpinBox * lineEdit = new QSpinBox(parent);
        lineEdit->setRange(0, 255);
        lineEdit->setSingleStep(1);
        return lineEdit;
}

void SPRDEUCodesTableDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
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

void SPRDEUCodesTableDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QSpinBox *sb = qobject_cast<QSpinBox*>(editor);
    if(sb){
        QString txt = sb->text();
        model->setData(index, QVariant(txt));
    }
}


Qt::ItemFlags SPRDEUCodesTableModel::flags(const QModelIndex &index) const
{
    return Qt::ItemIsEditable | QAbstractTableModel::flags(index);
}

bool SPRDEUCodesTableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if(rsModel){
        if(role == Qt::EditRole){
            bool isOk;
            if(index.column() < rsModel->getThreads()->getData()){
                uint val = value.toInt(&isOk);
                if(index.row() == 0){
                    rsModel->deuCodes[index.column()]->setData(val);
                } else if(index.row() == 1){
                    rsModel->cpCodes[index.column()]->setData(val);
                }
            }
            return isOk;
        }
    }
    return QAbstractItemModel::setData(index, value, role);
}

void SPRDEUCodesTableModel::setModelData(SPRSettingsRentgenModel *_rsModel){
    if(rsModel != _rsModel){
        if(rsModel){
            disconnect(rsModel, &SPRSettingsRentgenModel::modelChanget, this, &SPRDEUCodesTableModel::onModelChanget);
        }
    }
    rsModel = _rsModel;
    if(rsModel){
        connect(_rsModel, &SPRSettingsRentgenModel::modelChanget, this, &SPRDEUCodesTableModel::onModelChanget);
    }
}

void SPRDEUCodesTableModel::onModelChanget(IModelVariable *var)
{
    if(var == rsModel){
//        beginResetModel();
//        endResetModel();
        emit dataChanged(this->index(0,0), this->index(rowCount(QModelIndex())-1,columnCount(QModelIndex())-1));
    }
}

int SPRDEUCodesTableModel::rowCount(const QModelIndex &parent) const
{
    return 2;
}

int SPRDEUCodesTableModel::columnCount(const QModelIndex &parent) const
{
    if(rsModel){
        return rsModel->getThreads()->getData();
    }
}

QVariant SPRDEUCodesTableModel::data(const QModelIndex &index, int role) const
{
    QVariant res;
    if(rsModel){
        if(index.column() < rsModel->getThreads()->getData()){
            if(role == Qt::DisplayRole || role == Qt::EditRole){
                if(index.row() == 0){
                    res = QVariant(QString::number(rsModel->deuCodes[index.column()]->getData()));
                } else if(index.row() == 1){
                    res = QVariant(QString::number(rsModel->cpCodes[index.column()]->getData()));
                }
            }
            if(role == Qt::ToolTipRole){
                if(index.row() == 0){
                    res = QVariant(QString(tr("Код ДЭУ для ручья %1 (%2)")).arg(index.column()+1).arg(rsModel->deuCodes[index.column()]->getData()));
                } else if(index.row() == 1){
                    res = QVariant(QString(tr("Код ЦП для ручья %1 (%2)")).arg(index.column()+1).arg(rsModel->cpCodes[index.column()]->getData()));
                }
            }
            if(role == Qt::TextAlignmentRole){
                res = QVariant(Qt::AlignCenter);
            }
        }
    }
    return res;
}

QVariant SPRDEUCodesTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    QVariant res = QAbstractTableModel::headerData(section, orientation, role);
    if(rsModel){
       if(role == Qt::DisplayRole || role == Qt::EditRole){
            if(orientation == Qt::Horizontal){
                if(section < rsModel->getThreads()->getData()){
                    QString tt = QString(tr("Ручей %1").arg(section + 1));
                    res = QVariant(tt);
                }
            } else if(orientation == Qt::Vertical){
                if(section == 0){
                    QString tt = QString(tr("Коды ДЭУ"));
                    res = QVariant(tt);
                } else if(section == 1){
                    QString tt = QString(tr("Коды ЦП"));
                    res = QVariant(tt);
                }
            }
        }
       if(role == Qt::TextAlignmentRole){
           if(orientation == Qt::Horizontal){
               res = QVariant(Qt::AlignCenter);
           } else {
               res = QVariant(Qt::AlignLeft);
           }
       }

//       if(role == Qt::SizeHintRole){
//           QVariant vRes = headerData(section, orientation, Qt::DisplayRole);
//           QString tt = vRes.value<QString>();
//           int size = qobject_cast<QWidget*>(parent())->fontMetrics().width(tt);
//           res = QVariant(size);
//       }
    }
    return  res;
}

SPRDEUCodesTable::SPRDEUCodesTable(QWidget *parent):
    QTableView(parent), rsModel(nullptr), tableModel(nullptr), tableDelegate(nullptr)
{
}

ISPRModelData *SPRDEUCodesTable::setModelData(SPRSettingsRentgenModel *data)
{
    if(data){
        if(rsModel){
            disconnect(rsModel, SIGNAL(modelChanget(IModelVariable*)), this, SLOT(onModelChanget(IModelVariable*)));
        }
        rsModel = data;
        connect(rsModel, SIGNAL(modelChanget(IModelVariable*)), this, SLOT(onModelChanget(IModelVariable*)));

        tableModel = new SPRDEUCodesTableModel(this);
        tableModel->setModelData(rsModel);
        setModel(tableModel);

        tableDelegate = new SPRDEUCodesTableDelegate(this);
        setItemDelegate(tableDelegate);
    }
    widgetsShow();
}

void SPRDEUCodesTable::widgetsShow()
{
    if(rsModel){
        tableModel->onModelChanget(rsModel);
        resizeColumnsToContents();
        resizeRowsToContents();
//        updateGeometry();
    }
}

ISPRModelData *SPRDEUCodesTable::getModelData()
{
    return rsModel;
}


void SPRDEUCodesTable::onModelChanget(IModelVariable *var)
{
    if(sender() == rsModel || var == rsModel){
        widgetsShow();
    }
}

QSize SPRDEUCodesTable::sizeHint() const
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

