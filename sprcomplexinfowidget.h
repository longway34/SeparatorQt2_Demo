#ifndef SPRCOMPLEXINFOWIDGET_H
#define SPRCOMPLEXINFOWIDGET_H

#include "ui_sprcomplexinfowidget.h"


#include <QDateTime>
#include <QTimer>

#include <qwt_plot_multi_barchart.h>
#include <qwt_scale_draw.h>

#include "isprwidget.h"

class MainWindow;
class SPRMainModel;
class SPRSeparatorWidgetData;

class SPRComplexInfoWidget : public QWidget, public ISPRWidget
{
    Q_OBJECT

//    class DrawTitle : public QwtScaleDraw{

//        bool withThreads;
//        // QwtAbstractScaleDraw interface
//    public:
//        DrawTitle();
//        virtual QwtText label(double v) const;
//    };

    QTimer myTimer;
    QDateTime beginSeparatesProcess;
    bool separateIsRunning;

    QList<SPRSeparatorWidgetData*> *listWidgetsData;

    QwtPlotMultiBarChart *chart;

public:
    explicit SPRComplexInfoWidget(QWidget *parent = nullptr);

    void upDateData(QList<SPRSeparatorWidgetData *> *_listModels = nullptr);
private:
    Ui::SPRComplexInfoWidget ui;

private slots:
    void onTimeOut();

    void onClickCommands(bool);
    // ISPRWidget interface
public:
    virtual void widgetsShow();
    virtual ISPRModelData *getModelData();
    virtual void storeWidgetsStates(QString groupId, QSettings *mainSettings);
    virtual void restoreWidgetsStates(QString groupId, QSettings *mainSettings);

    
protected:
    virtual void onModelChanget(IModelVariable *);
};

#endif // SPRCOMPLEXINFOWIDGET_H
