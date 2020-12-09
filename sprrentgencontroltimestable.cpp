#include "sprrentgencontroltimestable.h"

#include <QSpinBox>
#include <QHeaderView>

QWidget *SPRRentgenControlsTimesDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QSpinBox * lineEdit = new QSpinBox(parent);
    lineEdit->setRange(0, 60);
    lineEdit->setSingleStep(1);
    return lineEdit;
}

void SPRRentgenControlsTimesDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
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

void SPRRentgenControlsTimesDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QSpinBox *sb = qobject_cast<QSpinBox*>(editor);
    if(sb){
        QString txt = sb->text();
        model->setData(index, QVariant(txt));
    }
}


SPRRentgenControlsTimesModel::SPRRentgenControlsTimesModel(QObject *parent): QAbstractTableModel(parent), rsModel(nullptr)
{
}

int SPRRentgenControlsTimesModel::rowCount(const QModelIndex &parent) const
{
    return 1;
}

int SPRRentgenControlsTimesModel::columnCount(const QModelIndex &parent) const
{
    return 3;
}

Qt::ItemFlags SPRRentgenControlsTimesModel::flags(const QModelIndex &index) const
{
    return Qt::ItemIsEditable | QAbstractTableModel::flags(index);
}

QVariant SPRRentgenControlsTimesModel::data(const QModelIndex &index, int role) const
{
    QVariant res;
    if(role == Qt::DisplayRole || role == Qt::EditRole){
        if(rsModel && index.row() == 0){
            if(index.column() == 0){
                res = QVariant(rsModel->timeOnRA->getData());
            } else if(index.column() == 1){
                res = QVariant(rsModel->timeOffRA->getData());
            } else if(index.column() == 2){
                res = QVariant(rsModel->timeHotRA->getData());
            }
        }
        if(role == Qt::ToolTipRole){
            if(index.column() == 0){
                res = QVariant(tr("Время включения рентгеновского аппарата (0..60) сек."));
            } else if(index.column() == 1){
                res = QVariant(tr("Время выключения рентгеновского аппарата (0..60) сек."));
            } else if(index.column() == 2){
                res = QVariant(tr("Время прогрева рентгеновского аппарата (0..60) сек."));
            }
        }
        if(role == Qt::TextAlignmentRole){
            res = QVariant(Qt::AlignCenter);
        }
    }
    return res;
}

bool SPRRentgenControlsTimesModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if(rsModel && index.row() == 0){
        if(role == Qt::DisplayRole || role == Qt::EditRole){
            if(index.column() == 0){
                rsModel->timeOnRA->setData(value.toInt());
            } else if(index.column() == 1){
                rsModel->timeOffRA->setData(value.toInt());
            } else if(index.column() == 2){
                rsModel->timeHotRA->setData(value.toInt());
            }
            return true;
        }
    }
    return false;
}

QVariant SPRRentgenControlsTimesModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    QVariant res = QAbstractTableModel::headerData(section, orientation, role);
    if(role == Qt::DisplayRole || role == Qt::EditRole){
        if(orientation == Qt::Horizontal){
            if(section == 0){ res = QVariant(tr("Включение"));}
            else if(section == 1){ res = QVariant(tr("Выключение"));}
            else if(section == 2){ res = QVariant(tr("Прогрев"));}
        } else if(orientation == Qt::Vertical){
            if(section == 0){
                res = QVariant(tr("Время, (сек.)"));
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
    return res;
}

void SPRRentgenControlsTimesModel::setModelData(SPRSettingsRentgenModel *_rsModel)
{
    rsModel = _rsModel;
    connect(rsModel, &SPRSettingsRentgenModel::modelChanget, this, &SPRRentgenControlsTimesModel::onModelChanget);
    emit dataChanged(index(0,0), index(0,2));
}

void SPRRentgenControlsTimesModel::onModelChanget(IModelVariable *var)
{
    if(sender() == rsModel){
        emit dataChanged(index(0,0), index(0,2));
    }
}

SPRRentgenControlTimesTable::SPRRentgenControlTimesTable(QWidget *parent) : QTableView(parent), tableModel(nullptr), tableDelegate(nullptr)
{
}


extern QRect resizeTableViewToMinimum(QTableView *view);

void SPRRentgenControlTimesTable::setModelData(SPRSettingsRentgenModel *_model)
{
    tableModel = new SPRRentgenControlsTimesModel(this);
    tableModel->setModelData(_model);
    setModel(tableModel);

    tableDelegate = new SPRRentgenControlsTimesDelegate();
    setItemDelegate(tableDelegate);

//    resizeColumnsToContents(); resizeRowsToContents();
//    setMinimumSize(sizeHint());
//    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
//    updateGeometry();
//    resizeTableViewToMinimum(this);
    resizeRowsToContents();
    resizeColumnsToContents();
}

QSize SPRRentgenControlTimesTable::sizeHint() const
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
