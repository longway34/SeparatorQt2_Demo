#include "sprzonesrange5.h"
#include "models/sprspectrumzonesmodel.h"

#include <QLineEdit>
#include <QHeaderView>

/**
 * @brief SPRZoneRangesTableGelegate5::createEditor
 * @param parent
 * @param option
 * @param index
 * @return
 */
QWidget *SPRZoneRangesTableGelegate5::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QLineEdit *editor = new QLineEdit(parent);
    editor->setValidator(new QIntValidator(0, 255, editor));
    return editor;

}

void SPRZoneRangesTableGelegate5::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    QLineEdit *ed = qobject_cast<QLineEdit*>(editor);
    if(ed){
        ed->setText(index.model()->data(index).value<QString>());
    }

}

void SPRZoneRangesTableGelegate5::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QLineEdit *ed = qobject_cast<QLineEdit*>(editor);
    if(ed){
        model->setData(index, QVariant(ed->text()));
    }
}

/**
 * @brief SPRZoneRangesTableModel5::setModelData
 * @param _model
 */
void SPRZoneRangesTableModel5::setModelData(SPRSpectrumZonesModel *_model){
    if(zModel) delete zModel;
    zModel = _model;
    if(zModel){
        connect(zModel, &SPRSpectrumZonesModel::modelChanget, this, &SPRZoneRangesTableModel5::onModelChanget);
    }
}

void SPRZoneRangesTableModel5::onModelChanget(IModelVariable *var){
    if(sender() == zModel){
        resetModel();
    }
}

void SPRZoneRangesTableModel5::resetModel(){
    beginResetModel();
    endResetModel();
//    emit dataChanged(index(0,0), index(rowCount(QModelIndex())-1, columnCount(QModelIndex())-1));
}

int SPRZoneRangesTableModel5::rowCount(const QModelIndex &parent) const
{
    if(zModel){
        return zModel->getElementsProperty()->getElements().size();
    }
    return 0;
}

int SPRZoneRangesTableModel5::columnCount(const QModelIndex &parent) const
{
    return 2;
}

QVariant SPRZoneRangesTableModel5::data(const QModelIndex &index, int role) const
{
    QVariant res;
    if(zModel){
        if(index.row() < rowCount(QModelIndex()) && index.column() < 2){
            SpectorRange *range = zModel->getElementRange(index.row());
            if(range){
                EnumElements el = zModel->getElementKey(index.row());
                if(role == Qt::DisplayRole || role == Qt::EditRole){
                    if(index.column() == 0){
                        res = QVariant(QString::number(range->min->getData()));
                    } else if(index.column() == 1){
                        res = QVariant(QString::number(range->max->getData()));
                    }
                }
                if(role == Qt::ToolTipRole){
                    if(index.column() == 0){
                        res = QVariant(tr("Минимальное значение для %1 - %2").arg(zModel->getElementsProperty()->getFName(el)).arg(zModel->getElements()[el]->min->getData()));
                    } else if(index.column() == 1){
                        res = QVariant(tr("Максимальное значение для %1 - %2").arg(zModel->getElementsProperty()->getFName(el)).arg(zModel->getElements()[el]->max->getData()));
                    }
                }
            }
            if(role == Qt::TextAlignmentRole){
                res = QVariant(Qt::AlignCenter);
            }

        }
    }
    return res;
}

bool SPRZoneRangesTableModel5::setData(const QModelIndex &index, const QVariant &value, int role)
{
    bool res = false;
    if(zModel){
        if(index.row() < rowCount(QModelIndex()) && index.column() < 2){
            if(role == Qt::EditRole){
                bool isOk;
                uint iVal= value.toUInt(&isOk);
                if(isOk){
                    SpectorRange *ranges = zModel->getElementRange(index.row());
                    if(ranges){
                        EnumElements el = zModel->getElementKey(index.row());
                        uint min = ranges->min->getData();
                        uint max = ranges->max->getData();
                        if(index.column() == 0){
                            min = iVal;
                        } else if(index.column() == 1){
                            max = iVal;
                        }
                        isOk = zModel->setMinMax(el, min, max);
                    }
                }
                return isOk;
            }
        }
    }
    return res;
}

QVariant SPRZoneRangesTableModel5::headerData(int section, Qt::Orientation orientation, int role) const
{
    QVariant res = QAbstractTableModel::headerData(section, orientation, role);
    if(role == Qt::DisplayRole && orientation == Qt::Horizontal){
        res = section == 0 ? QVariant(QString("<<")) : QVariant(QString(">>"));
    }
    return res;
}

Qt::ItemFlags SPRZoneRangesTableModel5::flags(const QModelIndex &index) const
{
    return Qt::ItemIsEditable | QAbstractTableModel::flags(index);
}


/**
 * @brief SPRZonesRange5::SPRZonesRange5
 * @param parent
 */
SPRZonesRange5::SPRZonesRange5(QWidget *parent) :QTableView(parent)
{

}

void SPRZonesRange5::onModelChanget(IModelVariable *var)
{
    if(zoneModel){
        if(sender() == zoneModel){
            SPRElementsModel *em = zoneModel->getElementsProperty();
            if(var == em){
                resize(sizeHint());
                updateGeometry();
//                emit resizeMe(sizeHint());
            }
        }
    }
}

void SPRZonesRange5::setModelData(SPRSpectrumZonesModel *_zone)
{
    zoneModel = _zone;
    if(zoneModel){
        threadNumber = zoneModel->getThreadNumber();
        SPRZoneRangesTableModel5 *tm = new SPRZoneRangesTableModel5(this);
        tm->setModelData(zoneModel);
        setModel(tm);

        connect(zoneModel, &SPRSpectrumZonesModel::modelChanget, this, &SPRZonesRange5::onModelChanget);
        setItemDelegate(new SPRZoneRangesTableGelegate5());
        resizeColumnsToContents();
        verticalHeader()->setVisible(false);
//        resizeRowsToContents();

//        resize(sizeHint());

//        horizontalHeader()->setSizeAdjustPolicy(QHeaderView::AdjustToContents);

        setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

//        QSize s = size();
//        QSize sh = sizeHint();
//        QSize sm = minimumSize();
//        QSize smh = minimumSizeHint();

//        int rh = verticalHeader()->sectionSize(0);
//        int rhh = verticalHeader()->sectionSizeHint(0);
//        int headerh = horizontalHeader()->height();

//        int cw = horizontalHeader()->sectionSize(0);


//        qDebug() << "size: "<< s << " sizeHint: " << sh << " minSize: " << sm << " minSizeHint: "<< smh;
//        qDebug() << "row heigth: "<< rh << " row sizeHint: " << rhh << " header Size: " << headerh << " col width: "<< cw;

//        int W0 = model()->columnCount() * cw + (3 * lineWidth());
//        int H0 = lineWidth() + headerh + (model()->rowCount() * rh + lineWidth());

        resize(sizeHint());
        setSizeAdjustPolicy(QHeaderView::AdjustIgnored);
        setMinimumSize(sizeHint());
        setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

        updateGeometry();

//        emit resizeMe(sizeHint());
    }
}

void SPRZonesRange5::currentChanged(const QModelIndex &current, const QModelIndex &previous)
{
    if(current.isValid()){

        emit gotFocus(this, current);
    }
//    qDebug() << tr("LastCurrnt: I is th: %1, cur: (%2,%3), prev: (%4,%5)")
//                .arg(this->getThreadNumber())
//                .arg(QString::number(current.row()))
//                .arg(QString::number(current.column()))
//                .arg(QString::number(previous.row()))
//                .arg(QString::number(previous.column()));
    QTableView::currentChanged(current, previous);
}

int SPRZonesRange5::sizeHintForColumn(int) const
{
    int w = fontMetrics().width("999999999");
    return w;
}

int SPRZonesRange5::sizeHintForRow(int) const
{
    int rh = QTableView::verticalHeader()->defaultSectionSize();
    if(model()){
        if(model()->rowCount() > 0){
            rh = verticalHeader()->sectionSize(0);
        }
    }
    return rh;
}

uint8_t SPRZonesRange5::getThreadNumber()
{
    if(zoneModel){
        return zoneModel->getThreadNumber();
    }
    return 255;
}

QSize SPRZonesRange5::sizeHint() const
{
    if(model() && model()->columnCount() > 0 && model()->rowCount() > 0){
        int cw = horizontalHeader()->sectionSize(0);
        int headerh = horizontalHeader()->height();
        int rh = verticalHeader()->sectionSize(0);

        int W0 = (model()->columnCount() * cw) + (model()->columnCount() * lineWidth());
        int H0 = lineWidth() + headerh + (model()->rowCount() * rh + lineWidth());
        QSize res(W0, H0);
        return res;
    } else {
        return QTableView::sizeHint();
    }
}

void SPRZonesRange5::selectionChanged(const QItemSelection &selected, const QItemSelection &deselected)
{
//    qDebug() << "selected: " << selected << " deselected:" << deselected;
    if(deselected.indexes().size() > 0){
        if(deselected.indexes().at(0).column() == 0 && deselected.indexes().at(0).row() == 0){
            return;
        }
    }
    QAbstractItemView::selectionChanged(selected, deselected);
}
