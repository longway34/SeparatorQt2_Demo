#ifndef SPRRENTGENCONTROLDIFFTABLE_H
#define SPRRENTGENCONTROLDIFFTABLE_H

#include <QObject>
#include <QWidget>

#include <QStyledItemDelegate>
#include <QTableView>

#include "models/sprsettingsrentgenmodel.h"

class SPRRentgenControlsDiffDelegate: public QStyledItemDelegate{


    // QAbstractItemDelegate interface
public:
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;
};

class SPRRentgenControlsDiffModel: public QAbstractTableModel{

    Q_OBJECT

    SPRSettingsRentgenModel *rsModel;
    // QAbstractItemModel interface
public:
    SPRRentgenControlsDiffModel(QObject *parent = nullptr);
    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role);
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    virtual Qt::ItemFlags flags(const QModelIndex &index) const;

    void setModelData(SPRSettingsRentgenModel *_rsModel);
protected slots:
    void onModelChanget(IModelVariable*var);
};

class SPRRentgenControlDiffTable : public QTableView
{
    Q_OBJECT
    SPRRentgenControlsDiffModel *tableModel;
    SPRRentgenControlsDiffDelegate *tableDelegate;
public:
    explicit SPRRentgenControlDiffTable(QWidget *parent = nullptr);


    void setModelData(SPRSettingsRentgenModel *_model);
    QSize sizeHint() const;
signals:


signals:

public slots:
};

#endif // SPRRENTGENCONTROLDIFFTABLE_H
