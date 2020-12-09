#ifndef SPRRENTGENVATABLE_H
#define SPRRENTGENVATABLE_H

#include <QObject>
#include <QWidget>
#include <QTableWidget>

#include "isprwidget.h"
#include "models/sprsettingsrentgenmodel.h"

#include <QStyledItemDelegate>

class SPRRentgenVATable;

class SPRRentgenVATableDelegate : public QStyledItemDelegate{


    // QAbstractItemDelegate interface
public:
    SPRRentgenVATableDelegate(QObject *parent=nullptr): QStyledItemDelegate(parent){}
    virtual QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    virtual void setEditorData(QWidget *editor, const QModelIndex &index) const;
    virtual void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;
};

class SPRRentgenVATableModel : public QAbstractTableModel
{
    Q_OBJECT

    SPRSettingsRentgenModel *rsModel;
    // QAbstractItemModel interface
public:
    SPRRentgenVATableModel(QWidget *parent=nullptr): QAbstractTableModel(parent), rsModel(nullptr){}
    virtual int rowCount(const QModelIndex &parent) const {return 2;}
    virtual int columnCount(const QModelIndex &parent) const;
    virtual QVariant data(const QModelIndex &index, int role) const;
    virtual bool setData(const QModelIndex &index, const QVariant &value, int role);
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    virtual Qt::ItemFlags flags(const QModelIndex &index) const;

    void setModelData(SPRSettingsRentgenModel *_rsModel);
public slots:
    void onModelChanget(IModelVariable*var);
};

class SPRRentgenVATable : public QTableView, public ISPRWidget
{
    Q_OBJECT

    SPRSettingsRentgenModel *rsModel;
    SPRRentgenVATableModel *tableModel;
    SPRRentgenVATableDelegate *tableDelegate;
public:
    SPRRentgenVATable(QWidget *parent=nullptr);

    // ISPRWidget interface
public:
    virtual ISPRModelData *setModelData(ISPRModelData *data);
    virtual ISPRModelData *getModelData();

protected slots:
    virtual void onModelChanget(IModelVariable *);

public slots:
    virtual void widgetsShow();

    // QWidget interface
public:
    QSize sizeHint() const;
};

#endif // SPRRENTGENVATABLE_H
