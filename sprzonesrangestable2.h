#ifndef SPRZONESRANGESTABLE2_H
#define SPRZONESRANGESTABLE2_H

#include <QObject>
#include <QWidget>
#include <QTableWidget>
#include "models/sprspectrumzonestablemodel.h"

#include "models/sprmainmodel.h"
#include "isprwidget.h"
#include "sprzonesranges2.h"

class SPRZonesRangesTable2 : public QTableWidget, public ISPRWidget
{
    Q_OBJECT


    SPRSpectrumZonesTableModel *zonesModel;
    SPRMainModel *mainModel;

    QList<uint8_t> threadList;

public:
    SPRZonesRangesTable2(QWidget *parent=nullptr);

    // ISPRWidget interface
public:
    virtual void widgetsShow();
    virtual ISPRModelData *setModelData(ISPRModelData *data);
    virtual ISPRModelData *getModelData();
    QSize sizeHint() const;

    void setThreadList(const QList<uint8_t> &value);
    void setThreadList(uint8_t th);
    uint8_t getCurrentThread();

protected slots:
    virtual void onModelChanget(IModelVariable *var);
    void onGotFocus(SPRZonesRanges2 *zone, QObject*);
    void onUpdateItemSize();

signals:
    void gotFocus(SPRZonesRanges2 *zone);
    void updateSize();
};

#endif // SPRZONESRANGESTABLE2_H
