#ifndef SPRZONESRANGE5_H
#define SPRZONESRANGE5_H

#include <QObject>
#include <QWidget>

#include <QStyledItemDelegate>
#include <QTableView>

#include "_types.h"

class IModelVariable;
class SPRSpectrumZonesModel;

class SPRZoneRangesTableGelegate5 :public QStyledItemDelegate{


    // QAbstractItemDelegate interface
public:
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;
};

/**
 * @brief The SPRZoneRangesTableModel class
 */
class SPRZoneRangesTableModel5 :public QAbstractTableModel{

    Q_OBJECT

    SPRSpectrumZonesModel *zModel;

public:
    SPRZoneRangesTableModel5(QObject *parent=nullptr): QAbstractTableModel(parent), zModel(nullptr){
    }

    void setModelData(SPRSpectrumZonesModel *_model);

    void resetModel();
    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role);
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
private slots:
    void onModelChanget(IModelVariable *var);
};

class SPRZonesRange5 : public QTableView
{
    Q_OBJECT

    SPRSpectrumZonesModel *zoneModel;
    uint8_t threadNumber;

public:
    explicit SPRZonesRange5(QWidget *parent = nullptr);
    virtual void onModelChanget(IModelVariable *var);

    void setModelData(SPRSpectrumZonesModel *_zone);

    // QAbstractItemView interface
protected slots:
    void currentChanged(const QModelIndex &current, const QModelIndex &previous);
signals:
    void gotFocus(SPRZonesRange5*, QModelIndex);
    void resizeMe(QSize);

    // QAbstractItemView interface
public:
    int sizeHintForColumn(int) const;
    int sizeHintForRow(int) const;

    uint8_t getThreadNumber();
    // QWidget interface
public:
    virtual QSize sizeHint() const;

    // QAbstractItemView interface
protected slots:
    void selectionChanged(const QItemSelection &selected, const QItemSelection &deselected);
};

#endif // SPRZONESRANGE5_H
