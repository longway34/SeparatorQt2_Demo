#ifndef SPRPOROGSWIDGET2_H
#define SPRPOROGSWIDGET2_H

#include "ui_sprporogswidget2.h"

#include <QAbstractTableModel>
#include <QItemDelegate>
#include <QStyledItemDelegate>

#include "models/sprsettingsporogsmodel.h"
#include "isprsettingswidget.h"

class SPRPorogsWidget2;

class SPRTablePorogsDelegate :public QStyledItemDelegate{


    // QAbstractItemDelegate interface
public:
    SPRTablePorogsDelegate(SPRPorogsWidget2 *_wParent);

    virtual QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    virtual void setEditorData(QWidget *editor, const QModelIndex &index) const;
    virtual void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;


};

class SPRTablePorogsModel :public QAbstractTableModel{
    Q_OBJECT
    // QAbstractItemModel interface
    SPRPorogsWidget2 *wParent;
    SPRSettingsPorogsModel *wModel;
public:
    SPRTablePorogsModel(SPRPorogsWidget2 *_wParent): wParent(_wParent), wModel(nullptr){}
    void setParentWidget(SPRPorogsWidget2 *_wParent);

    virtual int rowCount(const QModelIndex &parent) const;
    virtual int columnCount(const QModelIndex &parent) const;
    virtual QVariant data(const QModelIndex &index, int role) const;
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    virtual bool setData(const QModelIndex &index, const QVariant &value, int role);
    virtual Qt::ItemFlags flags(const QModelIndex &index) const;

public:

public slots:
    void onModelChanget(IModelVariable *);

    // QAbstractItemModel interface
public:
};

typedef enum{
    forSettings, forSeparate, allWisible
} SPRPorogsFormMode;

class SPRPorogsWidget2 : public QWidget, public ISPRSettingsWidget
{


    Q_OBJECT

    SPRSettingsPorogsModel *porogsModel;
    SPRMainModel *mainModel;

    SPRTablePorogsModel *tableModel;
    SPRTablePorogsDelegate *delegate;

    SPRPorogsFormMode widgetMode;

public:
    Ui::SPRPorogsWidget2 ui;
    explicit SPRPorogsWidget2(QWidget *parent = nullptr);

    ISPRModelData *setModelData(ISPRModelData *_model);
    ISPRModelData *getModelData();

    void widgetsShow();

    SPRPorogsFormMode getWidgetMode() const;
    void setWidgetMode(const SPRPorogsFormMode &value);

public slots:
    void onModelChanget(IModelVariable *var);
    void onClickEquialentButton(bool);
    void onChangeTypeConditions(bool value);
    void onChangeInvertion(bool value);
    void onChangeXRay();
    void onChangeTailOrConcentrate(bool value);
    void onServerStateChange(uint32_t);
    void onClickedUpDownButtons(bool);
    void onChangeAvrType(bool value);
    void onClickedCommandButtons(bool);
    void onClickedCalculateButton(bool);
//    void onChandeActivate(const QModelIndex _index);
private:

    // ISPRWidget interface
public:

    // ISPRSettingsWidget interface
public:
    void setMasterMode(bool value);
};

#endif // SPRPOROGSWIDGET2_H
