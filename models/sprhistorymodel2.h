#ifndef SPRHISTORYMODEL2_H
#define SPRHISTORYMODEL2_H

#include <QObject>
#include <QWidget>

#include "models/isprmodeldata.h"
#include "_types.h"
#include "models/sprseparatemodel.h"

#include <QDateTime>
#include <QQueue>

class SPRMainModel;
class SPRSeparateModel;
class SPRSpectrumListItemsModel;
class SPRSpectrumItemModel;

class SPRSeparatorLogsModel;
//class SPRWorkSeparate;
//type SPRWorkGistogrammRow;

#define DEF_SPR_HISTOTY_AVR_TIME_MSEC   (10* 60 * 1000) // 10 minutes

class SPRSettingsControlModel;

class SPRTimeAvrVariable : public QObject{

    Q_OBJECT

    quint64 maxSizeAvrInMsec;

    struct sVars{
        QDateTime dt;
        double value;
        sVars(double _value, QDateTime _dt):value(_value), dt(_dt){}
    };

    QQueue<sVars*> vars;
    double summ;
    quint64 size_in_ms;

public:
    SPRTimeAvrVariable(qint64 _maxTime_in_msec, QObject *parent=nullptr);
    ~SPRTimeAvrVariable();

    void addVar(double var, QDateTime dt);
    double getAvr_in_sec();
    double getAvr_in_msec();
    quint64 getSize_in_ms() const;

};

class SPRKSpectrumSeparateSummary{
public:
    double correl;
    double correlCritical;

    double airLoad;
    uint airLoadCriticalMin;
    uint airLoadCriticalMax;

    double power;
    double diffPower;
    uint powerCritical;

    double speedStone;
    uint speedStoneCritical;

    bool powerError;
    bool correlError;
    bool airError;
    bool speedStoneError;

    bool isErrors;

    SPRSettingsControlModel *model;

    SPRKSpectrumSeparateSummary():
        model(nullptr)
    {
        init();
    }
    SPRKSpectrumSeparateSummary(SPRSpectrumItemModel *kspect, SPRSpectrumItemModel *bspect, SPRSettingsControlModel *_model):
        model(nullptr)
    {
        setSpectrumData(kspect, bspect, _model);
    }
    SPRKSpectrumSeparateSummary(int count, qint64 time_in_msec, SPRSettingsControlModel *_model = nullptr):
        model(nullptr)
    {
        setSeparateData(count, time_in_msec);
    }
    void setSpectrumData(SPRSpectrumItemModel *kspect, SPRSpectrumItemModel *bspect, SPRSettingsControlModel *_model=nullptr);
    void setSeparateData(int count, qint64 time_in_msec, SPRSettingsControlModel *_model=nullptr);
    void init(SPRSettingsControlModel *_model = nullptr);
};

class SPRHistoryModel2 : public ISPRModelData
{
    Q_OBJECT

public:
    class histDataOneThread :public QObject{
    public:
        QDateTime dt; // date time
        bool starting;
        struct _data{
            quint64 getSize_in_ms() const;

            int id;
            qint64 mdt; // millisec
            qint64 size; // milli
            int thread;
            double i_prd[3]; // 0 - concentrate, 1 - tail, 2 - all
            double p_prd[3]; // --
            double p_tk; // 8 = 8
            double p_tkh1; // 8
            double p_tkh2; // 8
            double p_tkh3; // 8
            double wcount; // 8
            double s_rst[5];         // MAX_GR = 5 = 8x5 = 40;
        } data;

    private:
        void set(SPRWorkSeparate *ws, int th);
    public:
        void clear();
        histDataOneThread();
        histDataOneThread(SPRWorkSeparate *ws, int th);
        histDataOneThread(const histDataOneThread &o);
        histDataOneThread &operator=(histDataOneThread &o);
        histDataOneThread &operator+=(const histDataOneThread &o);
        friend const histDataOneThread operator+(histDataOneThread a, const histDataOneThread &o){
            return a += o;
        }

        bool isNotStouns();
        void setDiff(histDataOneThread *last, histDataOneThread *prev);
        void addAvr(SPRHistoryModel2::histDataOneThread *last);
        void toDebug(QString note="");

    };

    class avrDataOneThread : public QObject{
    public:
        QDateTime dt;
        struct _data
        {
            SPRTimeAvrVariable *i_prd[3];  // 0 - concentrate, 1 - tail, 2 - all
        } data;
        avrDataOneThread(quint64 time_avr_in_msec){
            for(int i=0; i < 3; i++){
                data.i_prd[i] = new SPRTimeAvrVariable(time_avr_in_msec);
            }
        }
        ~avrDataOneThread(){
            for(int i=0; i < 3; i++){
                delete data.i_prd[i];
            }
        }

        void addAvr(histDataOneThread *diff, histDataOneThread *avr, QDateTime dt){
            for(int i=0; i<3; i++){
                data.i_prd[i]->addVar(diff->data.i_prd[i], dt);

                avr->data.i_prd[i] = data.i_prd[i]->getAvr_in_msec();
            }
        }
        void toDebug(QString note=""){
            if(!note.isEmpty()){
                qDebug() << "******** " << note << " ***********";
            }
            qDebug() << "i_prd_con:" << data.i_prd[0] << "; i_prd_tail:" << data.i_prd[1] << "; i_prd_all:" << data.i_prd[2];
        }

    };


private:
    SPRMainModel *mainModel;
    SPRSeparateModel *separateModel;
    SPRSpectrumListItemsModel *spectrumsModel;
    SPRSeparatorLogsModel *logModel;

    QVector<SPRKSpectrumSeparateSummary> spectrumSummaryTable;

    SPRWorkSeparate sourseLast;
    QVector<histDataOneThread> last;
    QVector<histDataOneThread> lastSession;
    QVector<histDataOneThread> diff;
    QVector<histDataOneThread> avr10m;

    QVector<avrDataOneThread*> tempAvr;

    bool startingGist;
    bool startingSeparateData;

    QVector<SPRWorkGistogrammRow> gistInt;
    QVector<SPRWorkGistogrammRow> gistInpLast;
    QVector<QVector<QPointF>> gistOut;

    double gStep; double gMin; double gMax;
    int gCol;

    QVector<bool> flagsCorrelCriticalMessageSended;
    QVector<bool> flagsAirCriticalMessageSended;
    QVector<bool> flagsPowerCriticalMessageSended;


public:
    SPRHistoryModel2(IModelVariable *parent, SPRSpectrumListItemsModel *spectrums = nullptr);

    ~SPRHistoryModel2();

    void setModelData(SPRMainModel *_main, SPRSpectrumListItemsModel *_spetrums);

    void setLogModel(SPRSeparatorLogsModel *_logModel);

    // IModelVariable interface
    QVector<histDataOneThread> *getLastRecords();
    QVector<histDataOneThread> *getDiffRecords();
    QVector<histDataOneThread> *getAvr10mRecords();
    QVector<histDataOneThread> *getLastSessionRecords();

    QVector<QVector<QPointF>> getGistOut() const;
    QVector<QPointF> *getGistogrammContentData2(int thread);

    SPRSeparateModel *getSeparateModel() const;

    SPRMainModel *getMainModel() const;

//    QVector<QPointF> *getGistogrammContentData2(int thread);
    QVector<SPRKSpectrumSeparateSummary> *getSpectrumSummaryTable();

    bool isKSpectrumsError();
    void clear();
public slots:
    void onModelChanget(IModelVariable *send);
    void onSeparateDataReady();
    QVector<SPRKSpectrumSeparateSummary> *onKSpectrumsReady();

    SPRThreadList getThreadList();
    void setWorkSeparateData(QByteArray rawData);
    void setWorkGistogrammData(QByteArray rawData, int thread);
signals:
    void separateDataReady();
    void gistogrammDataReady();
    void kspectrumDataReady();

protected:
    void reinitGistMatrixs();
};

#endif // SPRHISTORYMODEL2_H
