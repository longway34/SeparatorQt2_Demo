#ifndef SPRZONESTABLE5_H
#define SPRZONESTABLE5_H

#include <QObject>
#include <QWidget>
#include <QTableWidget>

#include "models/sprspectrumzonestablemodel.h"

class SPRZonesRange5;

typedef enum {noName5 = 0, minField5, maxField5} SPRTypeMinMaxField5;

class SPRZonesPropertyTable5 : public QTableWidget
{
    Q_OBJECT

    SPRMainModel *mainModel;
    int rowHeight;
    QSet<IModelVariable*> mySenders;

public:
    SPRZonesPropertyTable5(QWidget *parent = nullptr);

    void setModelData(SPRMainModel *_MainModel);
    void setRowHeight(int value);

protected slots:
    void onModelChanget(IModelVariable *var);
    virtual void onColorChanget(IModelVariable *var, QColor color);
    virtual void onDeleteElement(IModelVariable *var);
};

class SPRZonesTable5 : public QTableWidget
{
    Q_OBJECT

    SPRMainModel *mainModel;
    SPRSpectrumZonesTableModel *ztModel;
    SPRZonesPropertyTable5 *fcWidget;
    SPRThreadList threadList;

public:
    SPRZonesTable5(QWidget *parent = nullptr);

    void setModelData(SPRSpectrumZonesTableModel *_zModel);

    void setThreadList(const SPRThreadList &value);
    void setThread(uint8_t th);

private slots:
    virtual void onModelChanget(IModelVariable *var);
    void onGotFocus(SPRZonesRange5 *zr, QModelIndex index);
signals:
    void gotFocus(EnumElements, uint8_t, SPRTypeMinMaxField5);
};

#endif // SPRZONESTABLE5_H
