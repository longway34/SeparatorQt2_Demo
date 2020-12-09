#ifndef SPRSEPARATESUMMARYGISTOGRAMMWIDGET_H
#define SPRSEPARATESUMMARYGISTOGRAMMWIDGET_H

#include <QObject>
#include <QWidget>

//#include "models/sprmainmodel.h"

#include "models/sprhistorymodel.h"
#include "sprseparatesummarygistogramm.h"
#include "models/sprseparatemodel.h"

#include <QTableWidget>
#include <QBoxLayout>
#include <QAbstractTableModel>

class SPRMainModel;
class SPRHistoryModel2;

class SPR_SSGW_TableViewModel : public QAbstractTableModel{

    Q_OBJECT

    // QAbstractItemModel interface
    SPRMainModel *mainModel;
//    ISPRModelData *extModel;
    SPRHistoryModel2 *extModel;
    SPRSeparateModel *separateModel;

    SPRWorkSeparateTypeData typeModelData;

public:
    SPR_SSGW_TableViewModel(QWidget *parent = nullptr);
    virtual int rowCount(const QModelIndex &parent) const;
    virtual int columnCount(const QModelIndex &parent) const;
    virtual QVariant data(const QModelIndex &index, int role) const;

    void setModelData(SPRMainModel *_mainModel, SPRWorkSeparateTypeData _type);
protected slots:
    void onModelChanget(IModelVariable *);
    void onSeparateDataReady();

    // QAbstractItemModel interface
public:
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role) const;
};

class SPR_SSGW_TableView : public QTableView{
    Q_OBJECT

public:
    SPR_SSGW_TableView(QWidget *parent);


    // QWidget interface
public:
    virtual QSize sizeHint() const;

    // QWidget interface
public:
    virtual QSize minimumSizeHint() const;
};

class SPRSeparateSummaryGistogrammWidget : public QFrame
{
    Q_OBJECT

private:
    SPRMainModel *mainModel;
//    SPRHistoryModel *historyModel;
    SPRHistoryModel2 *historyModel2;

    SPRSeparateSummaryGistogramm *gistogramm;

    SPRWorkSeparateTypeData typeWidget;
    SPR_SSGW_TableView *oneRowTable;

    QWidget *forVGroup;
    bool woDownTable;

    QList<uint8_t> threadsList;

public:
    explicit SPRSeparateSummaryGistogrammWidget(QWidget *parent = nullptr);

    void setModelData(SPRMainModel* _mainModel, SPRWorkSeparateTypeData _tydeWidget, bool _woDownTable);
    void widgetsShow();

public slots:
    void onModelChanget(IModelVariable *send);
protected:
    void adjustTableToSize(QTableView *tv);
signals:

    // QWidget interface
protected:
    virtual void resizeEvent(QResizeEvent *event);
};

#endif // SPRSEPARATESUMMARYGISTOGRAMMWIDGET_H
