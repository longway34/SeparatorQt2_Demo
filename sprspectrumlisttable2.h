#ifndef SPRSPECTRUMLISTTABLE2_H
#define SPRSPECTRUMLISTTABLE2_H

#include <QObject>
#include <QWidget>
#include <QTableWidget>
#include <QFile>
#include <QVector>
#include <QLabel>

#include "models/sprspectrumlistitemsmodel.h"
#include "isprwidget.h"
#include "firstcollumn2.h"
#include <QStyledItemDelegate>

class SPRSpectrumListTable2;

class SPRSpectrumListTable2Delegate: public QStyledItemDelegate{

    Q_OBJECT
    // QAbstractItemDelegate interface
public:
    SPRSpectrumListTable2Delegate(QObject *parent);
    virtual QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    virtual void setEditorData(QWidget *editor, const QModelIndex &index) const;
    virtual void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;
signals:
    void spectrumNameChanget(int, QString);
};

class SPRSpectrumListTable2 : public QTableWidget, public ISPRWidget
{
    Q_OBJECT

    struct sectionStates {
        int size;
        bool hide;
        sectionStates(int s, bool h): size(s), hide(h){}
    };



protected:
    SPRSpectrumListItemsModel* model;

    QList<SPRSpectrumItemModel*> *spectrums;

    QList<SPRSpectrumItemModel*> storeCheckedItems;
    int storeCheckedItemsNum;
    SPRSpectrumItemModel* storeCurrentItem;

    QStringList commonHTitles;

    QAction *actHideColumn, *actShowAllColumns;

    SPRTypeSpectrumSet typeData;
public:
    explicit SPRSpectrumListTable2(QWidget *parent = nullptr);

    ISPRModelData *setModelData(SPRSpectrumListItemsModel *_model, SPRTypeSpectrumSet _type);
    virtual ISPRModelData *setModelData(ISPRModelData *_model){return setModelData((SPRSpectrumListItemsModel*)_model, spectrumsOnly);}
    // ISPRWidget interface
    SPRSpectrumListItemsModel *getModels(){ return model; }
    virtual ISPRModelData *getModelData()
    {
        return model;
    }

//    QList<int> getSelectedItemsNumbers();

    QList<SPRSpectrumItemModel*> getSelectedItems();
    QList<SPRSpectrumItemModel *> getAllItems();

protected:
    void resizeEvent(QResizeEvent *event);

//    int getMaxColumnWidth(int col);

public slots:
    virtual void widgetsShow();
    void myHideColumn(bool);
    void myShowColumns(bool);
    void onCurrentPosChanged(int row, int);
    void onDeleteRow(IModelVariable *var);
    void onRowSelect(IModelVariable *var, bool);
    virtual void onModelChanget(IModelVariable *var);
    void onChangeSpectrumName();
signals:
    void doShow();
//    void rowSelectedChecked(QList<int>, int);
    void rowSelectedChecked3(QList<SPRSpectrumItemModel*>, SPRSpectrumItemModel*);
//    void rowSelectedChecked2(int, SPRSpectrumListTable2*);
//    void itemChangeColor(int num);
//    void itemChangeColor(SPRSpectrumItemModel*, QColor);
//    void rowDeleted(int row);

    // ISPRWidget interface
public:

    // ISPRWidget interface
    SPRTypeSpectrumSet getTypeData() const;

protected:
    SPRSpectrumItemModel *getItemModelByRow(int row);
    void setRowItems(SPRSpectrumItemModel *item);
    void setTableWidgetItemProperty(QTableWidgetItem *wi, int col, QString _text="", QString _tooltype="", bool _editable = false);
protected slots:
    void onSpectrumColorChange(QColor color);

    // ISPRWidget interface
public:
    virtual void storeWidgetsStates(QString groupId, QSettings *mainSettings);
    virtual void restoreWidgetsStates(QString groupId, QSettings *mainSettings);
};



#endif // SPRSPECTRUMLISTTABLE2_H
