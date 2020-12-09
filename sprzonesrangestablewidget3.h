#ifndef SPRZONESRANGESTABLEWIDGET3_H
#define SPRZONESRANGESTABLEWIDGET3_H

#include "ui_sprzonesrangestablewidget3.h"
#include "models/sprspectrumzonestablemodel.h"
#include "isprwidget.h"
//#include "sprzonesrangestable3.h"
#include "isprsettingswidget.h"

class SPRZonesRangesTableWidget3 : public QWidget, public ISPRWidget, public ISPRSettingsWidget
{
    Q_OBJECT


    SPRSpectrumZonesTableModel *model;
    bool editControlEnable;
    bool selectBottonRowVisible;
    struct lz {
        uint8_t thread;
        SPRTypeMinMaxField5 type;
        EnumElements element;
        bool ok;
        lz(): ok(false){}
        bool isOk(){
            return (ok);
        }
    } lastCurrent;

    bool recompliteBaseSpectrumsVisible;
public:
    explicit SPRZonesRangesTableWidget3(QWidget *parent = nullptr);

    ISPRModelData *setModelData(SPRSpectrumZonesTableModel *value);

    void setThreadsVisible(QList<uint8_t> visible);

    void setThreadsVisible(uint8_t thr);

    void setRecompliteButtonVisible(bool value);
public slots:
    void onAddClick(bool);
    void onEditClick(bool);
    void onGotFocus(EnumElements el, uint8_t th, SPRTypeMinMaxField5 type);
//    void setControlEnabled(SPRZonesRanges *_thread, QLineEdit *_el);
    void onRoundClick(bool);
    virtual void onModelChanget(IModelVariable *);
    virtual void widgetsShow();
    void onUpdateSize();
    void onRecompliteBaseSpectrums(bool);
private:
    Ui::SPRZonesRangesTableWidget3 ui;

    // ISPRWidget interface
public:
    virtual ISPRModelData *getModelData();
    void setSelectBottonRowVisible(bool value);

protected:
    void showEnabled();

    // ISPRWidget interface
protected:

    // ISPRSettingsWidget interface
public:
    virtual void setMasterMode(bool value);

    // QWidget interface
protected:
    virtual void resizeEvent(QResizeEvent *event);
};









#endif // SPRZONESRANGESTABLEWIDGET3_H
