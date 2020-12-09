#ifndef SPRSEPARATEDETAILSTABLE_H
#define SPRSEPARATEDETAILSTABLE_H

#include <QObject>
#include <QWidget>
#include <QTableView>
#include <QAbstractTableModel>
#include <QAbstractItemModel>

//#include "models/sprseparatemodel.h"
//#include "models/sprseparateoutputmodel.h"

#include "models/sprhistorymodel2.h"

#include "isprwidget.h"

/**
 * @brief The SPRSeparateDetailsTableModel class
 */
class SPRSeparateDetailsTableModel : public QAbstractTableModel
{
    Q_OBJECT

//    SPRWorkSeparate2* prev;
//    SPRWorkSeparate2* last;

//    SPRSeparateOutputModel *model;

    SPRHistoryModel2 *model;

    int minStoneTime;
    bool startStop;
    SPRThreadList threads;

public:
    struct mData {
        QDateTime dt;
        qint64 mdt;
        uint8_t thread;
        double H1;
        double H2;
        double H3;
        double i_prd_All;
        double i_prd_Concentrale;
        double i_prd_Tail;
        double t_scope;
        mData(uint8_t _thread): thread(_thread), H1(0), H2(0), H3(0), i_prd_All(0), i_prd_Concentrale(0), i_prd_Tail(0), t_scope(0){
            dt = QDateTime::currentDateTime();
            mdt = QDateTime::currentMSecsSinceEpoch();
        }
    };

    QVector<struct mData*> mdata;

    SPRSeparateDetailsTableModel(SPRHistoryModel2 *_model, QObject *parent = nullptr):
        QAbstractTableModel(parent), model(nullptr), minStoneTime(16), startStop(false)
    {
        setModelData(_model);
        threads = getAllThreadsListDefault();
    }



    // QAbstractItemModel interface
public:
    virtual int rowCount(const QModelIndex&) const {return mdata.size();}
    virtual int columnCount(const QModelIndex&) const {return 8;}
    virtual QVariant data(const QModelIndex &index, int role) const;
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role) const;

    void setModelData(SPRHistoryModel2 *_model);
    int getMinStoneTime() const;
    void setMinStoneTime(int value);

    void startStopScope(bool _startStop);
    bool isStartingScope();
    void clear();
    // IModelVariable interface
    SPRThreadList getThreads() const;
    void setThreads(const SPRThreadList &value);

private slots:
    virtual void onModelChanget(IModelVariable*);
    virtual void onSeparateDataReady();
protected:
//    QVector<SPRSeparateDetailsTableModel::mData *> diff() const;
    void clearLast();
};

/**
 * @brief The SPRSeparateDetailsTable class
 */
class SPRSeparateDetailsTable : public QTableView, public ISPRWidget
{
    Q_OBJECT
    SPRMainModel *mainModel;
    SPRHistoryModel2 *history;

    SPRSeparateDetailsTableModel *myModel;

public:
    SPRSeparateDetailsTable(QWidget *parent);
    virtual ~SPRSeparateDetailsTable();
    // ISPRWidget interface
public:
    ISPRModelData *setModelData(SPRMainModel *data);
    virtual ISPRModelData *getModelData();
    void clear();

public slots:
    virtual void onModelChanget(IModelVariable *);
    virtual void widgetsShow();
    void startStopScope(bool value);
    bool isStartingScope();
    void setMinStoneTime(int _time);
    int getMinStoneTime();
    void setVisibleThreads(SPRThreadList _threads);
    SPRThreadList getVisibleThreads();

protected:
};

#endif // SPRSEPARATEDETAILSTABLE_H
