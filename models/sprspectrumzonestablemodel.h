#ifndef SPRSPECTRUMZONESTABLEMODEL_H
#define SPRSPECTRUMZONESTABLEMODEL_H

#include <QObject>
#include <QDomDocument>

#include "models/isprmodeldata.h"
#include "models/sprelementsmodel.h"
#include "variables/sprvariable.h"
#include "models/sprspectrumzonesmodel.h"


class SPRSpectrumZonesTableModel : public ISPRModelData
{
    SPRVariable<uint> *threads;

public:
    SPRSpectrumZonesModel *getItem(int _tIndex);

    QVector<SPRSpectrumZonesModel*> items;
    SPRElementsModel *elementsProperty;

    SPRSpectrumZonesTableModel();
    SPRSpectrumZonesTableModel(QDomDocument *_doc, SPRElementsModel *elements = nullptr, ISPRModelData *parent = nullptr);
    ~SPRSpectrumZonesTableModel();

    SPRVariable<uint> *getThreads() const;
    void setThreads(SPRVariable<uint> *value);

    QMapElementsRanges getElementsRanges(uint _tIndex=-1);
    SpectorRange* getElementsRanges(int _tIndex, EnumElements _el);
    SPRElementsModel *getElementsProperty();
    void setElementsProperty(SPRElementsModel *value);

    void setMinMaxValues(EnumElements el, uint _min, uint _max, int tIndex=-1);

    // IModelVariable interface
public slots:
    virtual void onModelChanget(IModelVariable *send);

    // IModelVariable interface
public slots:
    void store();
};

#endif // SPRSPECTRUMZONESTABLEMODEL_H
