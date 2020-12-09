#ifndef SPRZONESRANGESTABLEWIDGET2_H
#define SPRZONESRANGESTABLEWIDGET2_H

#include "ui_sprzonesrangestablewidget2.h"
#include "models/sprspectrumzonestablemodel.h"
#include "isprwidget.h"
#include "sprzonesrangestable2.h"
#include "isprsettingswidget.h"

class SPRZonesRangesTableWidget2 : public QWidget, public ISPRWidget, public ISPRSettingsWidget
{
    Q_OBJECT


    SPRSpectrumZonesTableModel *model;
    bool editControlEnable;
    bool selectBottonRowVisible;
    struct lz {
        uint8_t thread;
        bool ok;
        SPRZonesRanges2 *thZone;
        SPRTypeMinMaxField type;
        EnumElements element;
        lz(): ok(false), thZone(nullptr){}
        bool isOk(){
            return (thZone != nullptr && ok);
        }
    } lastCurrent;
public:
    explicit SPRZonesRangesTableWidget2(QWidget *parent = nullptr);

    ISPRModelData *setModelData(SPRSpectrumZonesTableModel *value);

    void setThreadsVisible(QList<uint8_t> visible);

    void setThreadsVisible(uint8_t thr);
public slots:
    void onAddClick(bool);
    void onEditClick(bool);
    void onGotFocus(SPRZonesRanges2 *zone);
//    void setControlEnabled(SPRZonesRanges *_thread, QLineEdit *_el);
    void onRoundClick(bool);
    virtual void onModelChanget(IModelVariable *);
    virtual void widgetsShow();
    void onUpdateSize();
private:
    Ui::SPRZonesRangesTableWidget2 ui;

    // ISPRWidget interface
public:
    virtual ISPRModelData *getModelData();
    void setSelectBottonRowVisible(bool value);

protected:
    void showEdabled();

    // ISPRWidget interface
protected:

    // ISPRSettingsWidget interface
public:
    virtual void setMasterMode(bool value);
};









#endif // SPRZONESRANGESTABLEWIDGET2_H
