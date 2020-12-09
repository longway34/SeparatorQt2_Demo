#ifndef SPRZONESRANGE4_H
#define SPRZONESRANGE4_H

#include <QObject>
#include <QWidget>

#include <QTableWidget>
#include <QTableView>
#include <QStyledItemDelegate>
#include <QLineEdit>
#include <QIntValidator>
#include <QLayout>

#include <QMessageBox>
#include <models/sprmainmodel.h>

#include "models/sprspectrumzonesmodel.h"
#include "firstcollumn2.h"

class SPRZoneRangesTableGelegate :public QStyledItemDelegate{


    // QAbstractItemDelegate interface
public:
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;
};

/**
 * @brief The SPRZoneRangesTableModel class
 */
class SPRZoneRangesTableModel :public QAbstractTableModel{

    Q_OBJECT

    SPRSpectrumZonesModel *zModel;

public:
    SPRZoneRangesTableModel(QObject *parent=nullptr): QAbstractTableModel(parent), zModel(nullptr){
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

/**
 * @brief The SPRZoneRangeTable class
 */
class SPRZoneRangeTable :public QTableView
{
    Q_OBJECT

public:
    SPRZoneRangeTable(QWidget *parent=nullptr);
//    void clearFocus();
    // QAbstractItemView interface
protected slots:
    void currentChanged(const QModelIndex &current, const QModelIndex &previous);
    int sizeHintForColumn(int) const;
signals:
    void gotFocus(QModelIndex);

};

/**
 * @brief The SPRZonesRange4 class
 */
class SPRZonesRange4 : public QWidget
{
    Q_OBJECT

    SPRSpectrumZonesModel *zoneModel;
    uint8_t threadNumber;

    QTableWidget *fcWidget;
    SPRZoneRangeTable *tRanges;
    SPRZoneRangesTableModel *tableModel;
    SPRZoneRangesTableGelegate *delegate;

    bool firstColumnVisible;

public:
    explicit SPRZonesRange4(QWidget *parent = nullptr);

    virtual ~SPRZonesRange4();

    virtual void setModelData(SPRSpectrumZonesModel *_zModel);
    void clearCurrent();
signals:

    void gotFocus(SPRZonesRange4*, QModelIndex, EnumElements);
    void rowCountChanget();
protected slots:
    virtual void onCurrentItemChanged(QModelIndex current);
    virtual void onColorChanget(IModelVariable *var, QColor color);
    virtual void onDeleteElement(IModelVariable *var);
    virtual void onModelChanget(IModelVariable *);
    // QWidget interface
public:
    QSize sizeHint() const;
    uint8_t getThreadNumber() const;
    bool getFirstColumnVisible() const;
    void setFirstColumnVisible(bool value);
};

#endif // SPRZONESRANGE4_H
