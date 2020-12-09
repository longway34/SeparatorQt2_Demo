#ifndef ISPRWIDGET_H
#define ISPRWIDGET_H

#include <QTableWidget>
#include <QString>
#include <QLineEdit>
#include <QValidator>
#include "models/isprmodeldata.h"
#include "_types.h"
#include <QSettings>


class SPRSeparatorLogsModel;

class ISPRWidget
{
protected:
    QString docFileName;
    QString docFilePath;

    QDomDocument document;
    QDomDocument *doc;
    SPRSeparatorLogsModel *logModel;

    QList<QWidget*> forBlockSignalsWidget;

public:
    ISPRWidget();
    virtual ~ISPRWidget();
    virtual void setDoc(QString _fName);
    virtual void setDoc(QDomDocument *_doc);

    virtual void widgetsShow()=0;

    virtual ISPRModelData *setModelData(ISPRModelData* data=nullptr);
    virtual ISPRModelData *getModelData()=0;
//    virtual void cellLeChanget(QTableWidgt *table, QLineEdit *le, int x, int y)=0;
    virtual SPRSeparatorLogsModel *getLogModel() const;
    virtual void setLogModel(SPRSeparatorLogsModel *value);
    virtual void setStateSeparate(bool);

    virtual void storeWidgetsStates(QString groupId, QSettings *);
    virtual void restoreWidgetsStates(QString groupId, QSettings *);
protected:
    QLineEdit* setNumberCell(QTableWidget *table, int row, int col, int value, int min, int max, const QString toolType);
    virtual void viewChange(){}
    virtual void viewChange(QColor){}
    virtual void viewChange(int){}
    virtual void viewChange(double){}
    virtual void viewChange(bool){}
    virtual void viewChange(int, int){}
    virtual void viewChange(QTableWidget*, int, int){}
    virtual void onModelChanget(IModelVariable*)=0;
};

#endif // ISPRWIDGET_H
