#ifndef SPRSEPRATEMAINTABLE_H
#define SPRSEPRATEMAINTABLE_H

#include <QObject>
#include <QTableView>
#include <QAbstractTableModel>
#include <QAbstractItemModel>

#include "models/sprmainmodel.h"
#include "models/sprspectrumlistitemsmodel.h"
#include "models/sprhistorymodel2.h"
class SPRSeparateMainTableModel : public QAbstractTableModel{

    Q_OBJECT
    // QAbstractItemModel interface
//    SPRThreadList threads;
    QVector<QVector<double>> hxTable; QVector<double> all_hx_Table;
    QVector<QVector<double>> i_prd; QVector<double> all_i_prd;
    QVector<double> correlTable;
    QVector<double> powerSpectrums;
    QVector<double> airSpectrum;

    SPRVariable<double> *correlPorogPermiss, *correlPorogCritical;
    SPREnumVariable<EnumElements> *correlControlArea;

    SPRVariable<uint> *spectrumAirLoadingMinPermiss; // загрузка по воздуху
    SPRVariable<uint> *spectrumAirLoadingMaxPermiss;
    SPRVariable<uint> *spectrumAirLoadingMinCritical;
    SPRVariable<uint> *spectrumAirLoadingMaxCritical;

    SPRVariable<uint> *spectrumDiffPowerPermiss;
    SPRVariable<uint> *spectrumDiffPowerCritical;


    SPRHistoryModel2 *historyModel;
    SPRSpectrumListItemsModel *spectrumsModel;

    SPRMainModel *mainModel;

public:
    SPRSeparateMainTableModel(QObject *parent = nullptr);

    void init(SPRMainModel *_mainModel, SPRHistoryModel2 *_historyModel, SPRSpectrumListItemsModel *_spectrumsModel);
    virtual int rowCount(const QModelIndex &) const;
    virtual int columnCount(const QModelIndex &) const;
    virtual QVariant data(const QModelIndex &index, int role) const;
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    void clearTables();

    void setPorogsVariable(){
        if(mainModel){
            correlPorogCritical = mainModel->getSettingsControlModel()->correlSpectrumCritical;
            correlPorogPermiss = mainModel->getSettingsControlModel()->correlSpectrumPermiss;
            correlControlArea = mainModel->getSettingsControlModel()->controlArea;

            spectrumAirLoadingMinPermiss = mainModel->getSettingsControlModel()->airLoadingMinPermiss;
            spectrumAirLoadingMaxPermiss = mainModel->getSettingsControlModel()->airLoadingMaxPermiss;
            spectrumAirLoadingMinCritical = mainModel->getSettingsControlModel()->airLoadingMinCritical;
            spectrumAirLoadingMaxCritical = mainModel->getSettingsControlModel()->airLoadingMaxCritical;

            spectrumDiffPowerPermiss = mainModel->getSettingsControlModel()->diffCenterGravityPermiss;
            spectrumDiffPowerCritical = mainModel->getSettingsControlModel()->diffCenterGravityCritical;
        }
    }

    void setHxTable(const QVector<QVector<double> > &value);
    void setHxTable();
    void setI_prd(const QVector<QVector<double> > &value);
    void setI_prd();

    void setSeparTables(const QVector<QVector<double>> _hxs, const QVector<QVector<double>> _i_prds);

    void setCorrelTable(const QVector<double> &value);
    void setCorrelTable();

    SPRHistoryModel2 *getHistoryModel() const;

    void setMainModel(SPRMainModel *value);
    void setHistoryModel(SPRHistoryModel2 *value);
    void setSpectrumsModel(SPRSpectrumListItemsModel *value);

    void setPowerTable(const QVector<double> &value);
    void setPowerTable();
    void setAirTable(const QVector<double> &value);
    void setAirTable();
    QVector<double> getAirTable();

    void resetData();

public slots:
    void onModelChanget(IModelVariable* send);
    void onSeparateDataReady();
    void onKSpectrumDataReady();
};

class SPRSeprateMainTable : public QTableView
{
    Q_OBJECT

    SPRSeparateMainTableModel *tableModel;
    SPRMainModel *mainModel;
    SPRHistoryModel2 *separateModel;
    SPRSpectrumListItemsModel *spectrumModel;

public:
    SPRSeprateMainTable(QWidget *parent = nullptr);

    virtual ~SPRSeprateMainTable();
    void init(SPRMainModel *_mainModel, SPRHistoryModel2 *_histModel, SPRSpectrumListItemsModel *_kSpectModel);
public slots:
    void onModelChanget(IModelVariable* source);
};

#endif // SPRSEPRATEMAINTABLE_H
