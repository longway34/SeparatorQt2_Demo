#ifndef SPRZONESTHREADRANGETABLE4_H
#define SPRZONESTHREADRANGETABLE4_H

#include <QObject>
#include <QWidget>
#include <QTableWidget>

#include "sprzonesrange4.h"
#include "models/sprspectrumzonestablemodel.h"

class SPRZonesThreadRangeTable4 : public QTableWidget
{
    Q_OBJECT

    SPRSpectrumZonesTableModel *zonesTableModel;
    QList<uint8_t> threadsVisible;
public:
    explicit SPRZonesThreadRangeTable4(QWidget *parent = nullptr);

    void setThreadsVisible(QList<uint8_t> _threadsVisible);
    void setThreadsVisible(uint8_t th);

    void setModelData(SPRSpectrumZonesTableModel *_zoneTableModel);

signals:
    void gotFocus(SPRZonesRange4*, QModelIndex, EnumElements);
public slots:
    void onGotFocus(SPRZonesRange4 *zr, QModelIndex index, EnumElements el);
    void onModelChanget(IModelVariable *var);
    void resizeMe();

    // QWidget interface
protected:
//    virtual void resizeEvent(QResizeEvent *event);
};

#endif // SPRZONESTHREADRANGETABLE4_H
