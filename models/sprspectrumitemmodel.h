#ifndef SPRSPECTRUMITEMMODEL_H
#define SPRSPECTRUMITEMMODEL_H

#include <QObject>
#include <QPolygonF>
#include <QByteArray>
#include <QDateTime>

#include "models/isprmodeldata.h"
#include "models/sprspectrumzonestablemodel.h"
#include "models/sprsettingsformulamodel.h"

#include "qwt_interval.h"
#include "qwt_plot_histogram.h"

class SPRGraphItem;

typedef class spectumItemData{
    uint16_t bufLength;
    uint8_t  *buf;
public:
    char *name;
    uint32_t *thread;
    uint8_t *red, *green, *blue;
    uint16_t *spect;
    uint32_t *summ;
    uint32_t *peak_value;
    uint32_t *peak;
    double *Rs;
    QMap<EnumElements, uint32_t*> elementsSumm;
    uint32_t  *Ns, *Fe, *Cu, *Mo, *Zn, *Mg;
    double *H1, *H2, *H3;
    double *time_in_sec, *center, *x_ray;
    uint32_t *year, *month, *day, *hours, *min, *sec;

    QMap<EnumElements, QVector<double>> elementsDiff;
    QMap<EnumElements, double> elementsDispersion;
    QMap<EnumElements, double> elementsSigma;
    QMap<EnumElements, double> elementsAverage;
    QMap<EnumElements, double> elementsCount;
    QMap<EnumElements, double> elementsCorrel;
    QMap<EnumElements, double> elementsMunsForCenter;
    double munsForCenter;

    double avr, dispersion, sigma, correl;
    QVector<double> diff;

    spectumItemData(): buf(nullptr){
        setData();
    }
    ~spectumItemData(){
//        if(buf) free(buf);
    }
    void setData(uint8_t* inp = nullptr, uint16_t inplength = DEF_SPECTRUM_DATA_BUF_LENGTH);
    void setData(QByteArray inp);
    void setThread(int _thread){*thread = _thread;}
    uint8_t *getBuf(){ return buf;}
    void setbuf(uint8_t* _buf){ buf = _buf;}
    void setColor(QColor color){
        *red = color.red();
        *green = color.green();
        *blue = color.blue();
    }
    QColor getColor(){
        return QColor(*red, *green, *blue);
    }
    bool isValidData(){
        if(*thread >= MAX_SPR_MAIN_THREADS || *thread < 0){
            return false;
        }
    }

} SpectrumItemData;

Q_DECLARE_METATYPE(SpectrumItemData)

typedef enum _viewAs{
    viewModeAsIs, viewModeOneSecond, viewModeScales
} SPRViewGraphicsMode;

class SPRSpectrumItemModel : public ISPRModelData
{
    SPRSettingsFormulaModel *formulas;
    SPRSpectrumZonesTableModel *zones;
    SPRElementsModel *elementsModel;
    uint8_t thread;
    SpectrumItemData spectrumData;
    QPolygonF spectGraphData;
    QMap<EnumElements, QVector<QwtIntervalSample>> zonesGraphData;

    QString hashKeyDebug;

    SPRGraphItem *graphic;

    SPRViewGraphicsMode graphicMode;
//    double scaleMul;

public:
    SPRSpectrumItemModel();
    SPRSpectrumItemModel(QDomDocument *_doc, uint8_t _index, ISPRModelData *parent = nullptr);
    SPRSpectrumItemModel(SPRSpectrumZonesTableModel *_ranges, SPRSettingsFormulaModel *_formulas, uint8_t _thread, ISPRModelData *parent = nullptr);

    virtual ~SPRSpectrumItemModel();

    double getCorrel(SPRSpectrumItemModel *ather, bool elementsOnly = false, EnumElements _elements = Ns);
    double getCorrel();

    double getXRay();

    ISPRModelData *setModel(SPRSpectrumZonesTableModel *_ranges, SPRSettingsFormulaModel *_formulas);

    QColor getSpectrumColor();


    QString getSpectrumName();


    void setSpectrumThread(uint8_t th);

    uint getSpectrumThread();

    SPRSpectrumZonesModel *getZones();
    void recomplite(SPRSpectrumItemModel* baseItem=nullptr);
    void setZonesTable(SPRSpectrumZonesTableModel *value);
    void setFormulas(SPRSettingsFormulaModel *value);
    SpectrumItemData *getSpectrumData();
    void setSpectrumData(uint8_t *buf, uint16_t len = DEF_SPECTRUM_DATA_BUF_LENGTH, SPRSpectrumItemModel *baseItem = nullptr);

    QMap<EnumElements, QVector<QwtIntervalSample>> getZonesGaphics();
    QPolygonF getSpectrumGraphicsSamples();

    void setSpectrumDateTime(QDateTime _dateTime);
    void setTimeScope(uint32_t _time_in_ms);
    double getTimeScope();
    uint8_t getThread() const;
    void setThread(const uint8_t &value);

    SPRViewGraphicsMode getGraphicMode() const;
    void setGraphicMode(const SPRViewGraphicsMode &value, double _scaleMul = 1);

    SPRGraphItem *getGraphic();
    SPRElementsModel *getElementsModel();

    double getSpectrumCenter(bool forBaseSpectrum, EnumElements el=Ns);
public slots:
    void setSpectrumColor(IModelVariable *var, QColor color);
    void setSpectrumName(QString name);


protected:
    double setCorrelData(SPRSpectrumItemModel *ather);
};

#endif // SPRSPECTRUMITEMMODEL_H
