#ifndef SPRSETTINGSIMSMODEL_H
#define SPRSETTINGSIMSMODEL_H

#include <QObject>
#include <QDateTime>

#include "models/isprmodeldata.h"
#include "variables/sprvariable.h"
#include "variables/sprbooleanvariable.h"
#include "variables/sprbooleanvariable.h"
#include "_types.h"


class SPRMainModel;
class SPRHistoryModel2;

class SPRSettingsIMSModel : public ISPRModelData
{
    SPRVariable<uint> *ims;
    struct lineFormula{
        double k;
        double b;
    };

    SPRBooleanVariable *invertSelection;
    QDateTime dtLastUpdateAbrassions;
    SPRHistoryModel2 *history;
//    SPRMainModel
public:
    SPRVariable<uint> *tMeteringMinStone;
    SPRVariable<uint> *tMeteringMaxStone;
    SPRVariable<uint> *tMeteringMaxMaxStone;

    SPRVariable<uint> *tDelayMinStone;
    SPRVariable<uint> *tDelayMaxStone;
    SPRVariable<uint> *tDelayMaxMaxStone;

    SPRVariable<uint> *tDurationMinStone;
    SPRVariable<uint> *tDurationMaxStone;
    SPRVariable<uint> *tDurationMaxMaxStone;

    SPRBooleanVariable *withMaxMaxStone;

    SPRVariable<uint> *blockImsParam;
    SPRVariable<uint> *blockImsParam2;
    SPRVariable<double> *kSpeedOreA;
    SPRVariable<double> *kSpeedOreB;

    QVector<SPRVariable<uint>*> timesMettering;

    QVector<SPRVariable<double>*> kKoeffDelay;
    QVector<SPRVariable<double>*> bKoeffDelay;
    QVector<SPRVariable<double>*> kKoeffDuration;
    QVector<SPRVariable<double>*> bKoeffDuration;

    SPRVariable<qulonglong> *limitAbrasion;
    QVector<SPRVariable<qulonglong>*> abrasions;


    SPRSettingsIMSModel();
    SPRSettingsIMSModel(QDomDocument *_doc, ISPRModelData *parent = nullptr);
    virtual ~SPRSettingsIMSModel();

    SPRVariable<uint> *getIms() const;
    void setIms(SPRVariable<uint> *value);

    struct lineFormula getLineKoeff(double x1, double y1, double x2, double y2);
    void setTimesMeassureDelayDuration(uint tmin = 0xFFFF, uint tmax = 0xFFFF, uint delayMin = 0xFFFF, uint delayMax = 0xFFFF, uint durationMin = 0xFFFF, uint durationMax = 0xFFFF);

    SPRVariable<uint> *getTMeteringMinStone() const;
    SPRVariable<uint> *getTMeteringMaxStone() const;

    void setHistoryModel(SPRHistoryModel2 *_model);

    void clearAbrasions(QList<int> threads = {});
    // IModelVariable interface
    QString getFormulaString(bool delayDuration);
public slots:
    void onModelChanget(IModelVariable *send);
protected slots:
    void onSeparateDataReday();
};

#endif // SPRSETTINGSIMSMODEL_H
