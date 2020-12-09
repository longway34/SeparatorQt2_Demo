#ifndef SPRHISTORYLOGSWIDGET_H
#define SPRHISTORYLOGSWIDGET_H

#include <QWidget>
#include <QAbstractTableModel>
#include <QStyledItemDelegate>

#include "isprwidget.h"

class SPRSeparatorLogsModel;
class SPRSeparateMsgModel;
class SPRMainModel;
class SPRUsersSmenaModel;
class SPRUserModel;
class SPRSmenaModel;

namespace Ui {
class SPRHistoryLogsWidget;
}

class SPRHtmlViewTableDeledate : public QStyledItemDelegate{


    // QAbstractItemDelegate interface
public:
    virtual QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    virtual void setEditorData(QWidget *editor, const QModelIndex &index) const;

    // QAbstractItemDelegate interface
public:
    virtual void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;

    // QAbstractItemDelegate interface
public:
    virtual void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const;

    // QAbstractItemDelegate interface
public:
    virtual void destroyEditor(QWidget *editor, const QModelIndex &index) const;
};

class SPRHistoryLogsSeparateTableModel : public QAbstractTableModel{

    Q_OBJECT

    SPRSeparatorLogsModel *logModel;

public:
    SPRHistoryLogsSeparateTableModel(SPRSeparatorLogsModel *_logModel, QWidget *parent = nullptr);


    // QAbstractItemModel interface
public:
    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;

public slots:
    void onResumeTableChanget();
};


class SPRHistroryLogsMessagesTableModel : public QAbstractTableModel{

    Q_OBJECT

    SPRSeparatorLogsModel *logModel;
public:
    SPRHistroryLogsMessagesTableModel(SPRSeparatorLogsModel *_logModel, QWidget *parent = nullptr);


    // QAbstractItemModel interface
public:
    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
public slots:
    void onMessageTableChanget();
};

class SPRHistoryLogsWidget : public QWidget, public ISPRWidget
{
    Q_OBJECT

    SPRSeparatorLogsModel *logModel;
//    bool wResumeTable;
//    bool lastSmenaOnly;

//    SPRMainModel *mainModel;
public:
    explicit SPRHistoryLogsWidget(QWidget *parent = nullptr);

    void setModelsData(SPRSeparatorLogsModel *_logModel);

    ~SPRHistoryLogsWidget();

private:
    Ui::SPRHistoryLogsWidget *ui;

    // ISPRWidget interface
public:
    SPRHtmlViewTableDeledate *delegate;
    void widgetsShow();
    ISPRModelData *getModelData();
    void storeWidgetsStates(QString groupId, QSettings *mainSettings);
    void restoreWidgetsStates(QString groupId, QSettings *mainSettings);

//    void setResumeTableVisible(bool visible);
//    void selLastSmenaOnly(bool only);

protected:
    void onModelChanget(IModelVariable *)
    {
    }
protected slots:
    void onResumeTableChanget();
    void onEchoLogMessage(SPRSeparateMsgModel*, SPRSeparatorLogsModel*);

    // QObject interface
public:
    bool eventFilter(QObject *watched, QEvent *event)
    {
    }
};

#endif // SPRHISTORYLOGSWIDGET_H
