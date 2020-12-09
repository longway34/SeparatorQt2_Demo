#include "sprrentgencontroldifftable.h"

#include <QSpinBox>
#include <QHeaderView>

QWidget *SPRRentgenControlsDiffDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QSpinBox * lineEdit = new QSpinBox(parent);
    lineEdit->setRange(0, 60);
    lineEdit->setSingleStep(1);
    return lineEdit;
}

void SPRRentgenControlsDiffDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
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

void SPRRentgenControlsDiffDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QSpinBox *sb = qobject_cast<QSpinBox*>(editor);
    if(sb){
        QString txt = sb->text();
        model->setData(index, QVariant(txt));
    }
}


SPRRentgenControlsDiffModel::SPRRentgenControlsDiffModel(QObject *parent): QAbstractTableModel(parent), rsModel(nullptr)
{
}

int SPRRentgenControlsDiffModel::rowCount(const QModelIndex &parent) const
{
    return 1;
}

int SPRRentgenControlsDiffModel::columnCount(const QModelIndex &parent) const
{
    return 2;
}
Qt::ItemFlags SPRRentgenControlsDiffModel::flags(const QModelIndex &index) const
{
    return Qt::ItemIsEditable | QAbstractTableModel::flags(index);
}


QVariant SPRRentgenControlsDiffModel::data(const QModelIndex &index, int role) const
{
    QVariant res;
    if(role == Qt::DisplayRole || role == Qt::EditRole){
        if(rsModel && index.row() == 0){
            if(index.column() == 0){
                res = QVariant(rsModel->diffParamAllow->getData());
            } else if(index.column() == 1){
                res = QVariant(rsModel->diffParamDisallow->getData());
            }
        }
        if(role == Qt::ToolTipRole){
               if(index.column() == 0){
                   res = QVariant(tr("Допустимое значение отклонения параметров (0-100), %"));
               } else if(index.column() == 1){
                   res = QVariant(tr("Критичное значение отклонения параметров (0-100), %"));
               }
        }
//        if(role == Qt::TextAlignmentRole){
//            res = QVariant(Qt::AlignCenter);
//        }
    }
    return res;
}

bool SPRRentgenControlsDiffModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if(rsModel && index.row() == 0){
        if(role == Qt::DisplayRole || role == Qt::EditRole){
            if(index.column() == 0){
                rsModel->diffParamAllow->setData(value.toInt());
            } else if(index.column() == 1){
                rsModel->diffParamDisallow->setData(value.toInt());
            }
            return true;
        }
    }
    return false;
}

QVariant SPRRentgenControlsDiffModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    QVariant res = QAbstractTableModel::headerData(section, orientation, role);
    if(role == Qt::DisplayRole || role == Qt::EditRole){
        if(orientation == Qt::Horizontal){
            if(section == 0){ res = QVariant(tr("Допустимо"));}
            else if(section == 1){ res = QVariant(tr("Критично"));}
        } else if(orientation == Qt::Vertical){
            if(section == 0){
                res = QVariant(tr("%"));
            }
        }
    }
    if(role == Qt::TextAlignmentRole){
        if(orientation == Qt::Horizontal){
            res = QVariant(Qt::AlignCenter);
        } else {
            res = QVariant(Qt::AlignHCenter);
        }
    }
    return res;
}

void SPRRentgenControlsDiffModel::setModelData(SPRSettingsRentgenModel *_rsModel)
{
    rsModel = _rsModel;
    connect(rsModel, &SPRSettingsRentgenModel::modelChanget, this, &SPRRentgenControlsDiffModel::onModelChanget);

//    emit dataChanged(index(0,0), index(0,2));
}

void SPRRentgenControlsDiffModel::onModelChanget(IModelVariable *var)
{
    if(sender() == rsModel){
        emit dataChanged(index(0,0), index(0,1));
    }
}

SPRRentgenControlDiffTable::SPRRentgenControlDiffTable(QWidget *parent) : QTableView(parent), tableModel(nullptr), tableDelegate(nullptr)
{
}

QRect resizeTableViewToMinimum(QTableView *view){
    view->resizeRowsToContents();
    view->resizeColumnsToContents();

    QAbstractItemModel *model = view->model();
    QHeaderView *hhor = view->horizontalHeader();
    QHeaderView *hver = view->verticalHeader();

    QRect rect = view->geometry();

    int row = model->rowCount();
    int col = model->columnCount();

    int w = !hver->isHidden() ? hver->width()+2*view->lineWidth() : view->lineWidth();
    int h = !hhor->isHidden() ? hhor->height()+2*view->lineWidth() : view->lineWidth();
    for(int c=0; c<col; c++){
        if(!view->isColumnHidden(c))
            w += hhor->sectionSize(c) + view->lineWidth();
    }
    for(int r=0; r< row; r++){
        if(!view->isRowHidden(r))
            h += hver->sectionSize(r) + view->lineWidth();
    }

    rect.setWidth(w);
    rect.setHeight(h);

    view->setGeometry(rect);
    view->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    view->updateGeometry();
    return rect;
}

void SPRRentgenControlDiffTable::setModelData(SPRSettingsRentgenModel *_model)
{
    if(_model){
        tableModel = new SPRRentgenControlsDiffModel(this);
        tableModel->setModelData(_model);
        setModel(tableModel);

        tableDelegate = new SPRRentgenControlsDiffDelegate();
        setItemDelegate(tableDelegate);

        resizeColumnsToContents(); resizeRowsToContents();
//        setMinimumSize(size());
        updateGeometry();
//        resizeTableViewToMinimum(this);
    }
}

QSize SPRRentgenControlDiffTable::sizeHint() const
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
