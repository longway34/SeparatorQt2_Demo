#ifndef SPRDEUCODESTABLE_H
#define SPRDEUCODESTABLE_H

#include <QObject>
#include <QWidget>
#include <QTableWidget>
#include <QAbstractTableModel>
#include <QStyledItemDelegate>

#include "isprwidget.h"
#include "models/sprsettingsrentgenmodel.h"

class SPRDEUCodesTable;

class SPRDEUCodesTableDelegate: public QStyledItemDelegate{


    // QAbstractItemDelegate interface
public:
    SPRDEUCodesTableDelegate(QObject *parent = nullptr): QStyledItemDelegate(parent){}
    virtual QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    virtual void setEditorData(QWidget *editor, const QModelIndex &index) const;
    virtual void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;
};

class SPRDEUCodesTableModel : public QAbstractTableModel{
    Q_OBJECT

    SPRSettingsRentgenModel *rsModel;

    // QAbstractItemModel interface
public:
    SPRDEUCodesTableModel(QWidget *parent = nullptr) :QAbstractTableModel(parent), rsModel(nullptr){
    }
    virtual int rowCount(const QModelIndex &parent) const;
    virtual int columnCount(const QModelIndex &parent) const;
    virtual QVariant data(const QModelIndex &index, int role) const;
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    virtual bool setData(const QModelIndex &index, const QVariant &value, int role);

    void setModelData(SPRSettingsRentgenModel *_rsModel);
public slots:
    void onModelChanget(IModelVariable *var);

    // QAbstractItemModel interface
public:
    virtual Qt::ItemFlags flags(const QModelIndex &index) const;
};

class SPRDEUCodesTable : public QTableView, public ISPRWidget
{

    Q_OBJECT

    SPRSettingsRentgenModel *rsModel;
    SPRDEUCodesTableModel *tableModel;
    SPRDEUCodesTableDelegate *tableDelegate;

public:
    explicit SPRDEUCodesTable(QWidget *parent=nullptr);

    // ISPRWidget interface
    virtual ISPRModelData *getModelData();
    virtual ISPRModelData *setModelData(SPRSettingsRentgenModel *data);
public slots:
    virtual void widgetsShow();
    virtual void onModelChanget(IModelVariable *var);

    // ISPRWidget interface
protected slots:

    // ISPRWidget interface
protected:

    // QWidget interface
public:
    QSize sizeHint() const;
};









#endif // SPRDEUCODESTABLE_H
