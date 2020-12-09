#ifndef SPRZONESRANGES2_H
#define SPRZONESRANGES2_H

#include <QObject>
#include <QWidget>
#include <QTableWidget>
#include <QMessageBox>
#include <QTableWidgetItem>

#include "models/sprspectrumzonesmodel.h"
#include "isprwidget.h"
#include "firstcollumn2.h"

#include <QStyledItemDelegate>

typedef enum {noName = 0, minField, maxField} SPRTypeMinMaxField;

class SPRZonesRanges2Delegate: public QStyledItemDelegate{

    Q_OBJECT

    // QAbstractItemDelegate interface
public:
    SPRZonesRanges2Delegate(QObject *parent);
    virtual QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    virtual void setEditorData(QWidget *editor, const QModelIndex &index) const;
    virtual void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;
signals:
    void rangeChanget(int, int);
};

//Q_DECLARE_METATYPE(SPRTypeMinMaxField)

class SPRZonesRanges2 : public QTableWidget, ISPRWidget
{
    Q_OBJECT


    SPRSpectrumZonesModel *zoneModel;
    bool firstCollumnVisible;
    bool hHeaderVisible;
    bool selectEnable;

    QObject *lastCurrent;

    SPRZonesRanges2Delegate *tableDelegate;

    QList<SPRElementsProperty*> listElements;
public:
    SPRZonesRanges2(QWidget *parent=nullptr);
    virtual bool eventFilter(QObject *watched, QEvent *event);

//    SPRZonesRanges2(SPRSpectrumZonesModel *_model, QWidget *parent=nullptr);

    void setModelData(SPRSpectrumZonesModel *_model);

//    QSize getMySize(){

//    }
    bool getFirstCollumnVisible() const
    {
        return firstCollumnVisible;
    }
    void setFirstCollumnVisible(bool value)
    {
        firstCollumnVisible = value;
        widgetsShow();
    }

//    bool getHHeaderVisible() const
//    {
//        return hHeaderVisible;
//    }
//    void setHHeaderVisible(bool value){
//        hHeaderVisible = value;
//    }

//    bool getSelectEnable() const
//    {
//        return selectEnable;
//    }
//    void setSelectEnable(bool value)
//    {
//        selectEnable = value;
//    }

//    EnumElements getCurrentElement(bool *result = nullptr);
public slots:
    void widgetsShow();
    void onChangeValue();

    // ISPRWidget interface
//    void onChangeColor(QColor color);
    void onDeleteElements(IModelVariable *element);
    //    void onFocusCurrentCell(QModelIndex index);
//    void onChangeSelectedRow(bool value, int row);
    virtual void onModelChanget(IModelVariable *element);
    void onChangeValueWithDelegate();
public:
    virtual ISPRModelData *getModelData()
    {
        return zoneModel;
    }
    QSize sizeHint() const;


signals:
//    void deleteElement(IModelVariable *element);
//    void gotFocus(SPRZonesRanges2 *, QLineEdit*);
    // QWidget interface
    void gotFocus(SPRZonesRanges2 *, QObject*);
    void updateSize();
    // QObject interface
public:
//    virtual bool eventFilter(QObject *watched, QEvent *event);

    // QWidget interface
//    QObject *getLastCurrent() const;

    uint8_t getThread();
    SPRSpectrumZonesModel *getZoneModel() const;

    virtual ~SPRZonesRanges2();
    EnumElements getCurrentElement(bool *isOk);
    SPRTypeMinMaxField getCurrentFieldType();
    bool getTableData(EnumElements el, SPRTypeMinMaxField type, int *res);
protected:
//    virtual void resizeEvent(QResizeEvent *event);

    // ISPRWidget interface
protected:
    void setRowItems(int row, SPRElementsProperty *element);
    void setRowItemsWithDelegate(int row, SPRElementsProperty *element);
    void setTableWidgetItemProperty(QLineEdit *le, int row, int col, QString _text, QString _tooltype);
    int getRowByElement(EnumElements el);
    QTableWidgetItem *getTableWidgetItem(EnumElements el, SPRTypeMinMaxField type);

    // QAbstractItemView interface
public:
//    virtual int sizeHintForColumn(int column) const;

protected slots:
    void onCurrentItemChanged(QTableWidgetItem *current, QTableWidgetItem *previous);
//    currentItemChanged(QTableWidgetItem *current, QTableWidgetItem *previous)
};











#endif // SPRZONESRANGES2_H
