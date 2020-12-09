#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ui_mainwindow.h"
#include "aboutdialog.h"

#include "QDomDocument"
#include "models/sprmainmodel.h"
#include "maintabwidget.h"
#include "sprseparateforcomplexwidget.h"
#include "models/sprseparatorlogsmodel.h"

#include <QMdiSubWindow>

#include "sprsmenasuserspropertywidget.h"

class SPRSeparatorWidgetData{
public:
    SPRMainModel *model;
    SPRSeparatorLogsModel *logModel;
    MainTabWidget *widget;
    SPRSeparateForComplexWidget *complexWidget;
    SPRSeparatorForComplexSubWindow *sw;
//        SPRSeparateForComplexWidget *complexWidget;
    QString fName;
    QString logFName;
    QString title;
    QColor color;
    QDomElement domElement;
    int tabIndex;
    int id;
    QIcon icon;
    SPRSeparatorWidgetData(): model(nullptr), logModel(nullptr), widget(nullptr), complexWidget(nullptr), sw(nullptr), fName(""), logFName(""), title(""), id(0){}
    SPRSeparatorWidgetData(QString _fName, QString _logFName, QString _title, QColor _color, int _id):
        model(nullptr), logModel(nullptr), widget(nullptr), complexWidget(nullptr), sw(nullptr), fName(_fName), logFName(_logFName), title(_title), color(_color), id(_id){}
    ~SPRSeparatorWidgetData(){
        if(model) delete model;
        if(logModel) delete logModel;
        if(widget) delete widget;
        if(complexWidget) delete complexWidget;
        if(sw) delete sw;
    }
};


class MainWindow : public QMainWindow
{
    Q_OBJECT

    AboutDialog *about;

    QTextCodec *iniCodec;
    QSettings *mainSettings;

    SPRUsersSmenaModel *usModel;

    SPRSmenasUsersPropertyWidget *smenaUsersChoiceDialog;
    
    SPRUserModel *currentUser;
    SPRSmenaModel *currentSmena;

    QList<SPRMainModel*> listMainModel;

public:
    explicit MainWindow(QWidget *parent = nullptr);

    QList<SPRSeparatorWidgetData*> separators;

    SPRMainModel *defMainModel;

    SPRSeparatorWidgetData *findByIndex(int index);
    
    QString windowTitlePrefix = tr("(c) ООО \"ТЕХНОРОС\" 2018 Программа работы сепаратора ");
    
    void setMainWindowTitle();
    QList<SPRMainModel *> *getSeparatorsModels();

    QList<SPRMainModel *> getListMainModel() const;

public slots:
    void onAbout(bool);
    void onChangeSeparatorState(MainTabWidget *widget);
    void onChangeSeparatorColor(MainTabWidget *widget, QColor color);
    void onChangeSeparatorTitle(MainTabWidget *widget, QString title);
    void onRefreshAction(bool);
    void onAddSeparator(bool);
    void onCloseSeparator(int index);
    void onCloseSeparator(SPRSeparatorForComplexSubWindow* sw);
    void widgetsShow();
    void onSelectSeparator(SPRSeparatorForComplexSubWindow* sw);
protected:
    SPRSeparatorWidgetData *findByWidget(QWidget *widget);
    SPRSeparatorWidgetData *findByModel(SPRMainModel *model);
    void setMasterMode(bool mode);
    bool isUnicalSettings(SPRMainModel *model, int *res);
    QString findUnicalFName();
    SPRSeparatorWidgetData *findByServer(ServerConnect *server);
    SPRSeparatorWidgetData *findBySubWindow(SPRSeparatorForComplexSubWindow *sw);
    void storeConfig();
    void restoreWidgetsStates();
    void storeWidgetsStates();
protected slots:
    void onChangeMasterMode();
    void onCloseCommand(bool);
    void onModelChange(IModelVariable *variable);
    void onServerChangeState(uint32_t);
    void onSelectSeparatorChanget(int index);
    void onBeginSelectFileName();
    void onSaveSettingsCommand(IModelVariable *var);
    void onChangeCurrentTab(int index);
    void onSaveSettingsfromMenu(bool);
    void onUsersTableOpen(bool);
    void onUsersSmenaModelChaget(IModelVariable *var);
    void onChangeCurrentUser(SPRUserModel *current);
    void onChangeCurrentUserByMenu(bool val);
    void onSelectSmena(SPRSmenaModel *mod);
    void onSelectUser(SPRUserModel *mod);
    void onSmenasTableOpen(bool);
    void onChangeCurrentSmenaByMenu(bool val);
private:
    Ui::MainWindow ui;
    QDomDocument docSettings;
    QDomElement docSeparators;

    bool isMasterMode;

    QList<SPRSeparatorWidgetData*> fullConfigsFromFile(QString _fName);

    SPRSeparatorWidgetData *findByFName(QString fName);

    QString readXMLContents(QString _fName);
};

#endif // MAINWINDOW_H
