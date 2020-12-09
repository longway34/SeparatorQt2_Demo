#ifndef SPRRENTGENCONTROLTIMESTABLE_H
#define SPRRENTGENCONTROLTIMESTABLE_H

#include <QObject>
#include <QWidget>
#include <QStyledItemDelegate>
#include <QTableView>


#include "models/sprsettingsrentgenmodel.h"

class SPRRentgenControlsTimesDelegate: public QStyledItemDelegate{


    // QAbstractItemDelegate interface
public:
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;
};

class SPRRentgenControlsTimesModel: public QAbstractTableModel{

    Q_OBJECT

    SPRSettingsRentgenModel *rsModel;
    // QAbstractItemModel interface
public:
    SPRRentgenControlsTimesModel(QObject *parent = nullptr);
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

class SPRRentgenControlTimesTable : public QTableView
{
    Q_OBJECT
public:
    explicit SPRRentgenControlTimesTable(QWidget *parent = nullptr);

    SPRRentgenControlsTimesModel *tableModel;
    SPRRentgenControlsTimesDelegate *tableDelegate;

    void setModelData(SPRSettingsRentgenModel *_model);
signals:

public slots:

    // QWidget interface
public:
    QSize sizeHint() const;
};

#endif // SPRRENTGENCONTROLTIMESTABLE_H
