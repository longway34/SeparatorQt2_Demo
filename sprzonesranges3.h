#ifndef SPRZONESRANGES3_H
#define SPRZONESRANGES3_H

#include <QObject>
#include <QWidget>
#include <QTableWidget>
#include <QMessageBox>
#include <QTableWidgetItem>

#include "models/sprspectrumzonesmodel.h"
#include "isprwidget.h"
#include "firstcollumn2.h"

#include <QStyledItemDelegate>

typedef enum {noName3 = 0, minField3, maxField3} SPRTypeMinMaxField3;

class SPRZonesRanges3Delegate: public QStyledItemDelegate{

    Q_OBJECT

    // QAbstractItemDelegate interface
public:
    SPRZonesRanges3Delegate(QObject *parent);
    virtual QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    virtual void setEditorData(QWidget *editor, const QModelIndex &index) const;
    virtual void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;
signals:
    void rangeChanget(int, int);
};

class SPRZonesRanges3 : public QTableWidget, ISPRWidget
{
    Q_OBJECT


    SPRSpectrumZonesModel *zoneModel;
    bool firstCollumnVisible;
    bool hHeaderVisible;
    bool selectEnable;

    QObject *lastCurrent;

    SPRZonesRanges3Delegate *tableDelegate;

    QList<SPRElementsProperty*> listElements;
public:
    SPRZonesRanges3(QWidget *parent=nullptr);
    virtual bool eventFilter(QObject *watched, QEvent *event);
    void setModelData(SPRSpectrumZonesModel *_model);

    bool getFirstCollumnVisible() const
    {
        return firstCollumnVisible;
    }
    void setFirstCollumnVisible(bool value)
    {
        firstCollumnVisible = value;
        widgetsShow();
    }

public slots:
    void widgetsShow();
    void onChangeValue();

    void onDeleteElements(IModelVariable *element);
    virtual void onModelChanget(IModelVariable *element);
    void onChangeValueWithDelegate();
public:
    virtual ISPRModelData *getModelData()
    {
        return zoneModel;
    }
    QSize sizeHint() const;


signals:
    void gotFocus(SPRZonesRanges3 *, QObject*);
    void updateSize();
    // QObject interface
public:
    uint8_t getThread();
    SPRSpectrumZonesModel *getZoneModel() const;

    virtual ~SPRZonesRanges3();
    EnumElements getCurrentElement(bool *isOk);
    SPRTypeMinMaxField3 getCurrentFieldType();
    bool getTableData(EnumElements el, SPRTypeMinMaxField3 type, int *res);
protected:
    void setRowItemsWithDelegate(int row, SPRElementsProperty *element);
    int getRowByElement(EnumElements el);
    QTableWidgetItem *getTableWidgetItem(EnumElements el, SPRTypeMinMaxField3 type);

    // QAbstractItemView interface
protected slots:
    void onCurrentItemChanged(QTableWidgetItem *current, QTableWidgetItem *previous);

    // QWidget interface
protected:
    virtual void resizeEvent(QResizeEvent *event);
};











#endif // SPRZONESRANGES2_H
