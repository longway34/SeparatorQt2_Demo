#include "mainwindow.h"
#include <QDialog>
#include <QInputDialog>
#include <QApplication>
#include <QDesktopWidget>
#include <QMdiSubWindow>
#include <QAction>

#include <QMessageBox>
#include <QDir>
#include <QStandardItemModel>

#include <QFileDialog>

QList<SPRMainModel *> MainWindow::getListMainModel() const
{
    return listMainModel;
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent), usModel(nullptr), currentUser(nullptr), currentSmena(nullptr)
{
    ui.setupUi(this);

    isMasterMode = false;

//    QLocale loc = QLocale::system();
//    QLocale c_loc = QLocale("C");
//    loc.setNumberOptions(c_loc.numberOptions());

    setLocale(QLocale::English);

    ui.tabsAllWidgets->setIconSize(QSize(32, 32));

    connect(ui.tabsAllWidgets->tabBar(), SIGNAL(tabCloseRequested(int)), this, SLOT(onCloseSeparator(int)));

    defMainModel = new SPRMainModel(QString());
    defMainModel->setServerConnecting(false);

    ui.wAddSettings->setModelData(defMainModel->getSettingsMainModel());
    ui.wAddSettings->setLogModel(nullptr);

    smenaUsersChoiceDialog = new SPRSmenasUsersPropertyWidget(nullptr);

    usModel = qobject_cast<SPRUsersSmenaModel*>(smenaUsersChoiceDialog->getModelData());

    if(usModel){
        connect(usModel, &IModelVariable::modelChanget, this, &MainWindow::onUsersSmenaModelChaget);

        connect(usModel, &SPRUsersSmenaModel::currentSmenaChanget, this, &MainWindow::onSelectSmena);
        connect(usModel, &SPRUsersSmenaModel::currentUserChanget, this, &MainWindow::onSelectUser);
    }

    connect(ui.bAdd, &QPushButton::clicked, this, &MainWindow::onAddSeparator);
    connect(ui.cbSelectSeparator, SIGNAL(currentIndexChanged(int)), this, SLOT(onSelectSeparatorChanget(int)));
    connect(ui.bFileName, &QPushButton::clicked, this, &MainWindow::onBeginSelectFileName);

    ui.leFileName->setModified(false);

    connect(ui.actMasterMode, SIGNAL(changed()), this, SLOT(onChangeMasterMode()));

    connect(ui.tabsAllWidgets, &QTabWidget::currentChanged, this, &MainWindow::onChangeCurrentTab);

    QRect r = QApplication::desktop()->screenGeometry();
    this->resize(r.width() -10, r.height() -20);

    this->setWindowFlags(Qt::Window
                         | Qt::WindowMinimizeButtonHint
                         | Qt::WindowMaximizeButtonHint
                         );

    connect(ui.actExit, SIGNAL(triggered(bool)), this, SLOT(onCloseCommand(bool)));
    connect(ui.actAbout, SIGNAL(triggered(bool)), this, SLOT(onAbout(bool)));
    connect(ui.actSave, &QAction::triggered, this, &MainWindow::onSaveSettingsfromMenu);
    connect(ui.actSaveAs, &QAction::triggered, this, &MainWindow::onSaveSettingsfromMenu);
    connect(ui.actUsersTable, &QAction::triggered, this, &MainWindow::onUsersTableOpen);
    connect(ui.actSmenasTable, &QAction::triggered, this, &MainWindow::onSmenasTableOpen);

    connect(ui.actRefresh_2, &QAction::triggered, this, &MainWindow::onRefreshAction);

    about = new AboutDialog(nullptr);

    ui.tabsAllWidgets->setCurrentIndex(0);

    restoreWidgetsStates();

    onUsersSmenaModelChaget(nullptr);

    widgetsShow();
}

void MainWindow::restoreWidgetsStates(){
    QString sFileName = QApplication::applicationDirPath()+QDir::separator()+QApplication::applicationName()+".ini";

    iniCodec = QTextCodec::codecForName("UTF-8");
    mainSettings = new QSettings(sFileName, QSettings::IniFormat);
    mainSettings->setIniCodec(iniCodec);

    QString fn = mainSettings->fileName();

    mainSettings->beginGroup("mainWindow");
    restoreGeometry(mainSettings->value(objectName()+".geometry").toByteArray());
    ui.mainSplitter->restoreState(mainSettings->value(ui.mainSplitter->objectName()+".state").toByteArray());
    ui.wLog->restoreWidgetsStates("mainWindow", mainSettings);
    ui.wComplexSummaryGistogramm->restoreWidgetsStates("mainWindow", mainSettings);
    ui.downSplitter->restoreState(mainSettings->value(ui.downSplitter->objectName()+".state").toByteArray());

    mainSettings->endGroup();
}

void MainWindow::storeWidgetsStates(){

    mainSettings->beginGroup("mainWindow");

    QString fn = mainSettings->fileName();
    mainSettings->setValue(objectName()+".geometry", saveGeometry());
    mainSettings->setValue(ui.mainSplitter->objectName()+".state", ui.mainSplitter->saveState());
    mainSettings->setValue(ui.downSplitter->objectName()+".state", ui.downSplitter->saveState());

//    ui.wLog->storeWidgetsStates("mainWindow", mainSettings);
    mainSettings->endGroup();

        ui.wLog->storeWidgetsStates("mainWindow", mainSettings);
        ui.wComplexSummaryGistogramm->storeWidgetsStates("mainWindow", mainSettings);

    foreach(SPRSeparatorWidgetData* sd, separators){
        sd->widget->storeWidgetsStates(sd->title, mainSettings);
        sd->sw->storeWidgetsStates(sd->title, mainSettings);
    }
}

void MainWindow::onSelectSmena(SPRSmenaModel *mod){
    currentSmena = mod;
    QMenu *ms = ui.menuSmenas;
    QList<QAction*> la = ms->actions();
    foreach (QAction *act, la) {
        SPRSmenaModel *m = act->data().value<SPRSmenaModel*>();
        if(m && m == mod){
            act->blockSignals(true); act->setChecked(true); blockSignals(false);
        } else {
            act->blockSignals(true); act->setChecked(false); blockSignals(false);
        }
    }
    setMainWindowTitle();
}

void MainWindow::onSelectUser(SPRUserModel *mod)
{
    currentUser = mod;
    QMenu *mu = ui.menuUsers;
    QList<QAction*> la = mu->actions();
    foreach (QAction *act, la) {
        SPRUserModel *m = act->data().value<SPRUserModel*>();
        if(m && m == mod){
            act->blockSignals(true); act->setChecked(true); blockSignals(false);
        } else {
            act->blockSignals(true); act->setChecked(false); blockSignals(false);
        }
    }
    setMainWindowTitle();
}

void MainWindow::onUsersSmenaModelChaget(IModelVariable *var){
    if(sender() == smenaUsersChoiceDialog->getModelData() || sender() == ui.actMasterMode || !sender()){
//        SPRUsersSmenaModel *usModel = qobject_cast<SPRUsersSmenaModel *>(smenaUsersChoiceDialog->getModelData());
        QMenu *us = ui.menuUsers;
//        QAction *userTable = ui.actUsersTable;
//        foreach (QAction *act, us->actions()) {
//            if(/*act != userTable && */!act->isSeparator()){
//                us->removeAction(act);
//                if(act) delete act;
//            }
//        }
        us->clear();

        QAction *userTable = us->addAction(tr("Все операторы"));
        userTable->setIcon(QIcon(":/icons/icons/icons8-team-80.png"));
        connect(userTable, &QAction::triggered, this, &MainWindow::onUsersTableOpen);

        userTable->setEnabled(isMasterMode);

        us->addSeparator();
        if(usModel){
            foreach(SPRUserModel *mod, usModel->users){
                QString ifn = mod->avatar->getData();
                QIcon ic;
                ic.addFile(ifn);
                QAction *usAction = new QAction(mod->nik->getData(),this);
                QString tt = mod->family->getData()+" "+mod->name->getData()+" "+mod->patronymic->getData();
                usAction->setToolTip(tt);
                usAction->setIcon(ic);
                usAction->setCheckable(true);
                QVariant actData; actData.setValue<SPRUserModel*>(mod);
                usAction->setData(actData);
                if(mod == usModel->currentUser){
                    usAction->setChecked(true);
                } else {
                    usAction->setChecked(false);
                }
                connect(usAction, &QAction::triggered, this, &MainWindow::onChangeCurrentUserByMenu);
                us->addAction(usAction);
            }
        }

        QMenu *ss = ui.menuSmenas;
//        QAction *smenasTable = ui.actSmenasTable;
//        foreach(QAction *act, ss->actions()){
//            if(act != smenasTable && !act->isSeparator()){
//                ss->removeAction(act);
//                if(act) delete act;
//            }
//        }
        ss->clear();
        QIcon st(tr(":/icons/icons/rezhim-raboty-ikonka.png"));
        QIcon sm1(tr(":/icons/icons/b_Smena.png"));
        QAction *smenasTable = new QAction(st, tr("Все смены"));
        connect(smenasTable, &QAction::triggered, this, &MainWindow::onSmenasTableOpen);

        smenasTable->setEnabled(isMasterMode);

        ss->addAction(smenasTable);
        ss->addSeparator();

        if(usModel){
            foreach(SPRSmenaModel *mod, usModel->smenas){
                QString title = mod->name->getData() +" (" +mod->begin->toString("HH:mm") + " - " + mod->end->toString("HH:mm") +")";
                QAction *act = new QAction(sm1, title, this);
                act->setCheckable(true);
                QVariant actData; actData.setValue<SPRSmenaModel*>(mod);
                act->setData(actData);
                if(mod == usModel->currentSmena){
                    act->setChecked(true);
                } else {
                    act->setChecked(false);
                }
                connect(act, &QAction::triggered, this, &MainWindow::onChangeCurrentSmenaByMenu);
                ss->addAction(act);
            }
        }
    }
}

void MainWindow::onChangeCurrentSmenaByMenu(bool val){
    if(val){
        QAction *act = qobject_cast<QAction*>(sender());
        if(act){
            QMenu *menu = ui.menuSmenas;
            foreach(QAction *a, menu->actions()){
                if(a != act){
                    a->blockSignals(true); a->setChecked(false); a->blockSignals(false);
                }
            }
            SPRSmenaModel *mod = act->data().value<SPRSmenaModel*>();
            if(mod){
                currentSmena = mod;
                usModel->setCurrentSmena(mod);
            }
        }
        setMainWindowTitle();
    }
}

void MainWindow::onChangeCurrentUserByMenu(bool val){
    if(val){
        QAction *act = qobject_cast<QAction*>(sender());
        if(act){
            QMenu *menu = ui.menuUsers;
            foreach(QAction *a, menu->actions()){
                if(a != act && a->isCheckable()){

                    a->blockSignals(true); a->setChecked(false); a->blockSignals(false);
                }
            }
            SPRUserModel *mod = act->data().value<SPRUserModel*>();
            if(mod){
                currentUser = mod;
                usModel->setCurrentUser(mod);
            }
        }
        setMainWindowTitle();
    }
}

void MainWindow::onSmenasTableOpen(bool){
    smenaUsersChoiceDialog->ui.gbUsers->setVisible(false);
    smenaUsersChoiceDialog->ui.gbSmena->setVisible(true);
    smenaUsersChoiceDialog->setSelectedUser(currentUser);
    smenaUsersChoiceDialog->setSelectedSmena(currentSmena);
    smenaUsersChoiceDialog->show();
}

void MainWindow::onUsersTableOpen(bool){
    smenaUsersChoiceDialog->ui.gbUsers->setVisible(true);
    smenaUsersChoiceDialog->ui.gbSmena->setVisible(false);
    smenaUsersChoiceDialog->setSelectedUser(currentUser);
    smenaUsersChoiceDialog->setSelectedSmena(currentSmena);
    smenaUsersChoiceDialog->show();
}

void MainWindow::onChangeCurrentTab(int index){
    if(index > 0 && index < ui.tabsAllWidgets->count()-1){
//        if(isMasterMode){
            ui.actSave->setEnabled(true);
            ui.actSaveAs->setEnabled(true);
            setMainWindowTitle();
            return;
//        }
    }
    ui.actSave->setEnabled(false);
    ui.actSaveAs->setEnabled(false);
    
    setMainWindowTitle();
}

void MainWindow::onSaveSettingsfromMenu(bool)
{
    int curr = ui.tabsAllWidgets->currentIndex();
    SPRSeparatorWidgetData *d = findByIndex(curr);
    if(d){
        if(sender() == ui.actSave){
            d->model->setFName(d->fName);
            d->model->saveAsAllData();
            return;
        }
        if(sender() == ui.actSaveAs){
            onSaveSettingsCommand(d->model);
        }
    }
}

void MainWindow::onBeginSelectFileName(){
    QString fName;
    QString dir = QDir::homePath();
    QString fileName = QFileDialog::getOpenFileName(nullptr, tr("Открыть файл..."), dir,
        tr("Файлы настроек (*.xml)"));
    if(!fileName.isEmpty()){
        QFile f(fileName);
        if(f.open(QIODevice::ReadWrite)){
            if(fName != fileName){
                SPRMainModel *mod = new SPRMainModel(fileName);
                if(mod->errorsParsingXml){
                    QMessageBox::warning(nullptr, tr("Ошибка..."), tr("Выбран неверный файл конфигурации..."));
                } else {
                    if(defMainModel) delete defMainModel;
                    defMainModel = mod;

                    ui.leFileName->setText(fileName);

                    ui.wAddSettings->setModelData(defMainModel->getSettingsMainModel());
                    ui.wAddSettings->setLogModel(nullptr);
                    ui.wAddSettings->widgetsShow();
                }
            }
        }
    }
    return;
}

void MainWindow::onSaveSettingsCommand(IModelVariable *var)
{
//    QString fileName = "";

    MainTabWidget *mtw = qobject_cast<MainTabWidget*>(ui.tabsAllWidgets->currentWidget());
    if(mtw){
        mtw->setCurrentIndex(4);
        SPRSettingsWidget *sw = qobject_cast<SPRSettingsWidget*>(mtw->currentWidget());
        if(sw){
            sw->ui.tabSettingsWidget->setCurrentIndex(0);
        }
    }

    QString dir = QDir::homePath();
    QString selectedFilter;
    QString fileName = QFileDialog::getSaveFileName(ui.tabsAllWidgets->currentWidget(),
                                tr("Выберите имя файла name..."),
                                dir,
                                tr("Xml settings Files (*.xml)"),
                                &selectedFilter, QFileDialog::Options());
    if(!fileName.isEmpty()){
        if(!fileName.toLower().endsWith(".xml")){
            fileName = fileName+".xml";
        }
        QStringList path = fileName.split(QDir::separator());
        QString fn = path.last();
        foreach(SPRSeparatorWidgetData *d, separators){
            if(d->fName == fn){
                QMessageBox::critical(nullptr, tr("Файл не записан..."), tr("Файл с именем %1 используется в системе\nЕго перезапись, в данный момент, невозможна ...").arg(fileName));
                return;
            }
        }
        SPRMainModel *storeModel = dynamic_cast<SPRMainModel*>(var);
        if(storeModel){
            storeModel->saveAsAllData(fileName);
            return;
        }
    }
}

void MainWindow::onSelectSeparatorChanget(int index){
    QComboBox *cb = qobject_cast<QComboBox*>(sender());
    if(cb && cb == ui.cbSelectSeparator && index > 0){
        QString fName = cb->itemData(index).value<QString>();
        if(!fName.isEmpty()){
            if(defMainModel) delete defMainModel;

            defMainModel = new SPRMainModel(fName);
            defMainModel->setServerConnecting(false);

            ui.wAddSettings->setModelData(defMainModel->getSettingsMainModel());
            ui.wAddSettings->setLogModel(nullptr);
            ui.wAddSettings->widgetsShow();
        }
    }
}

void MainWindow::onCloseSeparator(SPRSeparatorForComplexSubWindow* sw){
    SPRSeparatorWidgetData *d = findBySubWindow(sw);
    if(d){
        onCloseSeparator(d->tabIndex);
    }
}

//+7-983-201-56-93
//
//Орлова ольга николаевна
//
//ориант персонал

void MainWindow::onCloseSeparator(int index){
    if(index > 0 && index < ui.tabsAllWidgets->count()-1){
        if(isMasterMode){
            SPRSeparatorWidgetData *d = findByIndex(index);
            if(d){
                QString title = d->title;
                QString addr = d->model->getSettingsMainModel()->getIpAddressVariable()->getData();
                QString port = QString::number(d->model->getSettingsMainModel()->getIpPortVariable()->getData());
                QMessageBox::StandardButton res = QMessageBox::question(nullptr, tr("Удаление сепаратора..."),
                            tr("Вы действительно хотите удалить сепаратор %1(%2:%3) из комплекса?").arg(d->title).arg(addr).arg(port));
                if(res == QMessageBox::Yes){
                    docSeparators.removeChild(d->domElement);
                    QDir dir;
                    dir.remove(d->fName);
                    separators.removeAll(d);
                    delete d;
                    storeConfig();
                    widgetsShow();
                }
            }
        }
    }
}

void MainWindow::storeConfig(){
    QFile out("config.xml");
    if(out.open(QIODevice::WriteOnly)){
    QTextStream stream( &out );
      stream << docSettings.toString();

      out.close();
    }

}

SPRSeparatorWidgetData* MainWindow::findByIndex(int index){
    SPRSeparatorWidgetData *res = nullptr;
    foreach(SPRSeparatorWidgetData *d, separators){
        if(d && d->tabIndex == index){
            res = d;
            break;
        }
    }
    return res;
}

void MainWindow::setMainWindowTitle()
{
    QString title = windowTitlePrefix;
    if(ui.tabsAllWidgets->currentIndex() == 0){
        title = title + tr("(Работа комплекса");
    } else if (ui.tabsAllWidgets->currentIndex() == ui.tabsAllWidgets->count() - 1){
        title = title + tr("(Добавление сепаратора");
    } else {
        QString sep = ui.tabsAllWidgets->tabText(ui.tabsAllWidgets->currentIndex());
        title = title + tr("(") + sep;
    }
    if(currentUser){
        title = title + " Оператор: " + currentUser->nik->getData();
    }
    if(currentSmena){
        QString smTitle =
        title = title + " Смена: " +  currentSmena->name->getData() +" (" +currentSmena->begin->toString("HH:mm") + " - " + currentSmena->end->toString("HH:mm") +")";
    }
    if(isMasterMode){
        title = title + tr(" РЕЖИМ МАСТЕРА");
    }
    title = title + ")";
    
    this->setWindowTitle(title);
}

QList<SPRMainModel *> *MainWindow::getSeparatorsModels()
{
//    QList<
}

bool MainWindow::isUnicalSettings(SPRMainModel *model, int *res){
    if(res) *res = 0;
    bool find = false;
    foreach(SPRSeparatorWidgetData *d, separators){
        SPRSettingsMainModel *src = model ? model->getSettingsMainModel() : nullptr;
        SPRSettingsMainModel *dst = d->model ? d->model->getSettingsMainModel() : nullptr;
        if(src && dst){
            if((src->getIpAddressVariable()->getData() == dst->getIpAddressVariable()->getData()) &&
                    src->getIpPortVariable()->getData() == dst->getIpPortVariable()->getData()){
                if(res) *res = 1;
                find = true;
                break;
            }
            if(src->getNameVariable()->getData() == dst->getNameVariable()->getData()){
                if(res) *res = 2;
                find = true;
                break;
            }
        }
    }
    return !find;
}

QString MainWindow::findUnicalFName(){
    bool find = false;
    QString res;
    int index = 0;
    while(!find){
        res = tr("Separator%1.xml").arg(index);
        if(!QFile::exists(res)){
            find = true;
            break;
        }
        index++;
    }
    return res;
}

void MainWindow::onAddSeparator(bool){
    int res;
    defMainModel->store();
    if(isUnicalSettings(defMainModel, &res)){
        QString fName = findUnicalFName();
        defMainModel->setFName(fName);
        defMainModel->setSpectrumFName(fName+".spc");
        defMainModel->saveAsAllData(fName);
        QDomElement sep = docSettings.createElement("INCLUDE");
        sep.setAttribute("INDEX", QString::number(separators.count()));
        sep.setAttribute("FILE", fName);
        sep.setAttribute("TITLE", defMainModel->getSettingsMainModel()->getNameVariable()->getData());
        sep.setAttribute("COLOR", defMainModel->getSettingsMainModel()->getColorVariable()->getData().name());
        docSeparators.appendChild(sep);

        delete defMainModel; defMainModel = nullptr;
        defMainModel = new SPRMainModel(QString(""));

        defMainModel->getSettingsMainModel()->getNameVariable()->setData(tr("СФР")+QString::number(separators.count()+1));

        ui.wAddSettings->setModelData(defMainModel->getSettingsMainModel());

        storeConfig();
        widgetsShow();
    } else {
        if(res == 1){
            QString ip = defMainModel->getSettingsMainModel()->getIpAddressVariable()->getData();
            QString port = QString::number(defMainModel->getSettingsMainModel()->getIpPortVariable()->getData());
            QMessageBox::warning(nullptr, tr("Добавление сепаратора"),
                                 tr("Сепаратор с адресом %1:%2 уже есть в списке...").arg(ip).arg(port));
            return;
        }
        if(res == 2){
            QString name = defMainModel->getSettingsMainModel()->getNameVariable()->getData();
            QMessageBox::warning(nullptr, "Добавление сепаратора",
                                 tr("Сепаратор с именем %1 уже есть в списке...").arg(name));
            return;
        }
    }
}

SPRSeparatorWidgetData *MainWindow::findByWidget(QWidget *widget){
    SPRSeparatorWidgetData *res = nullptr;
    foreach(SPRSeparatorWidgetData *d, separators){
        if(d->widget == widget){
            res = d;
            break;
        }
    }
    return  res;
}



void MainWindow::onChangeSeparatorState(MainTabWidget* widget){
    SPRSeparatorWidgetData *d = findByWidget(widget);
    if(d){
        ui.tabsAllWidgets->tabBar()->setTabText(d->tabIndex, d->title);
        ui.tabsAllWidgets->tabBar()->setTabTextColor(d->tabIndex, d->color);
        QString tt = d->title;
        QString tt_small = d->title;
        d->icon = QIcon(":/icons/icons/Separate_tab_no_colors_icon.png");
        QIcon icon(d->icon.pixmap(QSize(ui.tabsAllWidgets->iconSize()),QIcon::Disabled, QIcon::Off));
        if(d->model){
            QString state = tr(" (нет соединения c %1:%2)").arg(d->model->getSettingsMainModel()->getIpAddressVariable()->getData()).arg(d->model->getSettingsMainModel()->getIpPortVariable()->getData());
            QString smallState = tr(" (нет соединения)");

            d->widget->setStateSeparate(d->model->getServer()->isState(spr_state_separated));

            if(d->model->getServer()->isState(spr_state_server_connect)){
                state = tr(" (соединение установлено)");
                smallState = tr(" (соединено)");

                if(d->model->getServer()->isState(spr_state_separator_on) ||
                        d->model->getServer()->isState(spr_state_rentgen_on)){
                    d->icon = QIcon(":/icons/icons/Separate_tab_no_colors_icon_ws.png");
                    state = tr(" (соединение установлено, сорт. машина включена)");
                }

                if(d->model->getServer()->isState(spr_state_exposition_on)){
                    d->icon = QIcon(":/icons/icons/Separate_tab_no_colors_icon_wr.png");
                    state = tr(" (соединение установлено, включена экспозиция)");
                }
                icon = QIcon(d->icon.pixmap(QSize(ui.tabsAllWidgets->iconSize()), QIcon::Normal, QIcon::On));
            }
            if(d->model->getServer()->isState(spr_state_separated)){
                d->icon = QIcon(":/icons/icons/Separate_tab_icon.png");
                icon = d->icon;
                state = tr(" (идет сортировка)");
                smallState = tr(" (сортировка)");
            }
            tt += state;

            tt_small += smallState;
        }

        ui.tabsAllWidgets->setTabToolTip(d->tabIndex, tt);
        ui.tabsAllWidgets->setTabIcon(d->tabIndex, icon);

        d->sw->setWindowTitle(tt_small);
        d->sw->setWindowIcon(icon);
        d->sw->setToolTip(tt);
        d->sw->setStatusTip(tt);

    }
}

void MainWindow::onChangeSeparatorColor(MainTabWidget *widget, QColor color){
    SPRSeparatorWidgetData *d = findByWidget(widget);
    if(d){
        d->color = color;
        d->domElement.setAttribute("COLOR", color.name());
        storeConfig();

        onChangeSeparatorState(widget);
    }
}

void MainWindow::onChangeSeparatorTitle(MainTabWidget *widget, QString title){
    SPRSeparatorWidgetData *d = findByWidget(widget);
    if(d){
        d->title = title;
        d->domElement.setAttribute("TITLE", title);
        storeConfig();

        onChangeSeparatorState(widget);
        setMainWindowTitle();

    }
}

void MainWindow::setMasterMode(bool mode){
    isMasterMode = mode;
    foreach(SPRSeparatorWidgetData *d, separators){
        d->widget->setIsMasterMode(isMasterMode);
        Qt::WindowFlags flags = d->sw->windowFlags();
        flags = Qt::Window
                | Qt::WindowMinimizeButtonHint
                | Qt::CustomizeWindowHint
                | Qt::WindowTitleHint
                | Qt::WindowSystemMenuHint;

        flags &= ~Qt::WindowCloseButtonHint;

        d->sw->setWindowFlags(flags);
        d->sw->show();
    }
    ui.tabsAllWidgets->tabBar()->setTabEnabled(ui.tabsAllWidgets->count()-1, isMasterMode);
    ui.tabsAllWidgets->setTabsClosable(isMasterMode);

//    ui.actUsersTable->setEnabled(mode);
//    ui.actSmenasTable->setEnabled(mode);

    onUsersSmenaModelChaget(nullptr);
}

SPRSeparatorWidgetData *MainWindow::findBySubWindow(SPRSeparatorForComplexSubWindow* sw){
    SPRSeparatorWidgetData *res = nullptr;
    foreach(SPRSeparatorWidgetData *d, separators){
        if(d->sw == sw){
            res = d;
            break;
        }
    }
    return res;
}


void MainWindow::onRefreshAction(bool)
{
    foreach(SPRSeparatorWidgetData* sep, separators){
        if(sep){
            if(sep->sw){
                sep->sw->setHidden(false);
                sep->sw->show();
            }
        }
    }
}

void MainWindow::widgetsShow(){
    int lastIndex = ui.tabsAllWidgets->currentIndex();
    separators = fullConfigsFromFile("config.xml");

    listMainModel.clear();

    for(int i=ui.tabsAllWidgets->count()-1; i>0; i--){
        ui.tabsAllWidgets->removeTab(i);
    }
    QList<SPRSeparatorForComplexSubWindow*> lw = ui.mdiArea->findChildren<SPRSeparatorForComplexSubWindow*>();
    foreach(SPRSeparatorForComplexSubWindow *sw, lw){
        if(!findBySubWindow(sw)){
            ui.mdiArea->removeSubWindow(sw);
        }
    }

    ui.cbSelectSeparator->clear();
    ui.cbSelectSeparator->addItem(QIcon(":/icons/icons/b_Plus.png"), tr("Выберите сепаратор..."), QVariant(QString("")));
    const QStandardItemModel *m = qobject_cast<QStandardItemModel*>(ui.cbSelectSeparator->model());
    if(m){
        QStandardItem *item = m->item(0);
        if(item){
            item->setCheckable(false);
            item->setEnabled(false);
        }
    }


    int index = 1;
    foreach(SPRSeparatorWidgetData* sep, separators){

        if(!sep->widget){
//            SPRUsersSmenaModel *users = qobject_cast<SPRUsersSmenaModel*>(smenaUsersChoiceDialog->getModelData());
            sep->logModel = new SPRSeparatorLogsModel(sep->logFName, sep->id, nullptr);
            sep->logModel->setUserSmenaModel(usModel);


            sep->widget = new MainTabWidget(sep->fName);
            sep->widget->setLogModel(sep->logModel);
            sep->widget->restoreWidgetsStates(sep->title, mainSettings);

            connect(sep->widget, &MainTabWidget::saveAsSettings, this, &MainWindow::onSaveSettingsCommand);

            sep->model = sep->widget->getModel();
            sep->model->setLogModel(sep->logModel);
            sep->title = sep->model->getSettingsMainModel()->getNameVariable()->getData();
            if(sep->logModel){
                sep->logModel->setName(sep->title);
                connect(sep->logModel, &SPRSeparatorLogsModel::echoLogMessage, ui.wLog, &TCPEchoLogsWidget::onEchoLogMessages);
            }
            if(!listMainModel.contains(sep->model)){
                listMainModel.append(sep->model);
            }

            connect(smenaUsersChoiceDialog, &SPRSmenasUsersPropertyWidget::selectSmena, sep->logModel, &SPRSeparatorLogsModel::onSelectSmena);
            connect(smenaUsersChoiceDialog, &SPRSmenasUsersPropertyWidget::selectUser, sep->logModel, &SPRSeparatorLogsModel::onSelectUser);


            sep->complexWidget = new SPRSeparateForComplexWidget();
            sep->complexWidget->setModelData(sep->model);
            sep->sw = new SPRSeparatorForComplexSubWindow();
            sep->sw->setWidget(sep->complexWidget);
            connect(sep->sw, &SPRSeparatorForComplexSubWindow::selectSeparator, this, &MainWindow::onSelectSeparator);

            connect(sep->sw, SIGNAL(closeSeparator(SPRSeparatorForComplexSubWindow*)), this, SLOT(onCloseSeparator(SPRSeparatorForComplexSubWindow*)));

            connect(sep->model->getSettingsMainModel()->getColorVariable(), SIGNAL(modelChanget(IModelVariable*)), this, SLOT(onModelChange(IModelVariable*)));
            connect(sep->model->getSettingsMainModel()->getNameVariable(), SIGNAL(modelChanget(IModelVariable*)), this, SLOT(onModelChange(IModelVariable*)));
            connect(sep->model->getServer(), &ServerConnect::serverStateChangeOut, this, &MainWindow::onServerChangeState);
        }

        ui.cbSelectSeparator->addItem(QIcon(":/icons/icons/Separate_tab_no_colors_icon.png"), sep->title, QVariant(sep->fName));

        sep->color = sep->model->getSettingsMainModel()->getColorVariable()->getData();

        sep->tabIndex = ui.tabsAllWidgets->addTab(sep->widget, sep->title);

        onChangeSeparatorState(sep->widget);

        if(!lw.contains(sep->sw)){
            ui.mdiArea->addSubWindow(sep->sw);
            sep->sw->show();
            sep->sw->restoreWidgetsStates(sep->title, mainSettings);
        }

        onChangeSeparatorState(sep->widget);
        index++;
    }

    index = ui.tabsAllWidgets->addTab(ui.tabsAddSeparator, QString());
    QIcon icon(":icons/icons/b_Plus.png");
    ui.tabsAllWidgets->setTabIcon(index, icon);

    setMasterMode(isMasterMode);
    ui.tabsAllWidgets->repaint();

    if(lastIndex >= ui.tabsAllWidgets->count()) lastIndex = 0;

    ui.tabsAllWidgets->setCurrentIndex(lastIndex);
    
    setMainWindowTitle();
}

void MainWindow::onSelectSeparator(SPRSeparatorForComplexSubWindow *sw)
{
    SPRSeparatorWidgetData *d = findBySubWindow(sw);
    if(d){
        this->ui.tabsAllWidgets->setCurrentIndex(d->tabIndex);
    }
}

void MainWindow::onChangeCurrentUser(SPRUserModel *current)
{
    currentUser = current;
    
    setMainWindowTitle();
}

SPRSeparatorWidgetData *MainWindow::findByModel(SPRMainModel *model){
    SPRSeparatorWidgetData *res = nullptr;
    foreach(SPRSeparatorWidgetData *d, separators){
        if(d && d->model == model){
            res = d;
            break;
        }
    }
    return res;
}

SPRSeparatorWidgetData *MainWindow::findByServer(ServerConnect *server){
    SPRSeparatorWidgetData *res = nullptr;
    foreach(SPRSeparatorWidgetData *d, separators){
        if(d->model->getServer() == server){
            res = d;
            break;
        }
    }
    return  res;
}
void MainWindow::onServerChangeState(uint32_t){
    ServerConnect *server = dynamic_cast<ServerConnect*>(sender());
    if(server){
        SPRSeparatorWidgetData *d = findByServer(server);
        if(d){
            onChangeSeparatorState(d->widget);
        }
    }

    ui.wComplexSummaryGistogramm->upDateData(&separators);
}

QList<SPRSeparatorWidgetData *> MainWindow::fullConfigsFromFile(QString _fName){
    bool createNewDoc = false;
    QList<SPRSeparatorWidgetData*> res;
    if(!QFile::exists(_fName)){
        std::cout << "File not fount " << _fName.toStdString().c_str() << " used from recource..." << std::endl;
        _fName = ":/def/defConfig.xml";
        createNewDoc = true;

    } else {
        std::cout << "File fount " << _fName.toStdString().c_str() << " ..." << std::endl;
    }

    bool cont = docSettings.setContent(readXMLContents(_fName));
    if(createNewDoc){
        storeConfig();
    }

    QDomElement root = docSettings.documentElement();
    docSeparators = root.firstChildElement("SEPARATORS");
    if(docSeparators.isNull()){
        QDomElement part = docSettings.createElement("SEPARATORS");
        root.appendChild(docSeparators);
    }
    QDomNodeList separs = docSeparators.elementsByTagName("INCLUDE");
    for(int i=0; i< separs.count(); i++){
        QDomNode sep = separs.at(i);
        if(sep.isElement()){
            QString fname = sep.toElement().attribute("FILE", "Separator"+QString::number(i)+".xml");
            int id = sep.toElement().attribute("ID", QString::number(i)).toInt();
            QString sColor = sep.toElement().attribute("COLOR", defThreadsColor[i % 6].name());
            QString sTitle = sep.toElement().attribute("TITLE", tr("Separator_%1").arg(i % 6));
            QColor color(sColor); if(!color.isValid()) color = defThreadsColor[i % 6];
            QString lfName = sep.toElement().attribute("LOGFILE","Separator"+QString::number(i)+".xml.log.xml");
            if(QFile::exists(fname)){
                //                    separatorData *data = new separatorData();
                SPRSeparatorWidgetData *data = findByFName(fname);
                if(!data){
                    data = new SPRSeparatorWidgetData(fname, lfName, sTitle, color, id);
                    separators.append(data);
                } else {
                    data->title = QString(sTitle);
                    data->logFName = QString(lfName);
                    data->fName = QString(fname);
                    data->color = color;
                }
                data->domElement = sep.toElement();
            } else {
                docSeparators.removeChild(sep);
                SPRSeparatorWidgetData *data = findByFName(fname);
                if(data){
                    separators.removeAll(data);
                }
            }
        }
    }
    QFile out(_fName);
    if(out.open(QIODevice::WriteOnly)){
        QTextStream stream( &out );
        stream << docSettings.toString().toUtf8();

        out.close();
    }
    return separators;
}

SPRSeparatorWidgetData *MainWindow::findByFName(QString fName){
    SPRSeparatorWidgetData *res = nullptr;
    foreach(SPRSeparatorWidgetData *d, separators){
        if(d && d->fName == fName){
            res = d;
        }
    }
    return res;
}

QString MainWindow::readXMLContents(QString _fName){
    QString mtUtf8;
    QFile in(_fName);
    if(in.open(QIODevice::ReadOnly | QIODevice::Text)){

        QTextStream ins(&in);
        QByteArray ba = in.readAll();
        in.seek(0);
        QList<QByteArray> listCodecs = {QByteArray("UTF-8"), QByteArray("Windows-1251"), QByteArray("KOI8-R"), QByteArray("UTF-8")};
        int index = 0; bool find = false;
        while(index < listCodecs.size() && !find){
            QTextCodec *codec = QTextCodec::codecForName(listCodecs[index].constData());
            struct QTextCodec::ConverterState state;
            mtUtf8 = codec->toUnicode(ba, ba.length(), &state);
            if(state.invalidChars == 0){
                find = true;
                break;
            }
            index++;
        }

        if(!find){
            qDebug() << "Warnind! Invalid converted file to unucode: " << _fName;
        }
        in.close();
    }
    return mtUtf8;
}

void MainWindow::onModelChange(IModelVariable *variable){
    SPRMainModel *model = variable->getMainModel();
    if(model){
        SPRSeparatorWidgetData *d = findByModel(model);
        if(d){
            if(sender() == d->model->getSettingsMainModel()->getColorVariable() || variable == d->model->getSettingsMainModel()->getColorVariable()){
                SPRQColorVariable *colVariable = dynamic_cast<SPRQColorVariable*>(variable);
                if(colVariable){
                    onChangeSeparatorColor(d->widget, QColor(colVariable->getData()));
                }
            }
            if(sender() == d->model->getSettingsMainModel()->getNameVariable() || variable == d->model->getSettingsMainModel()->getNameVariable()){
                onChangeSeparatorTitle(d->widget, d->model->getSettingsMainModel()->getNameVariable()->getData());
            }
        }
    }
}

void MainWindow::onChangeMasterMode(){
    if(sender() == ui.actMasterMode){
        QAction *act = (QAction*)sender();
        if(act->isChecked()){
            bool isOk = false;
            QString pass = QInputDialog::getText(nullptr, tr("Ввод пароля"), tr("Введите пароль мастера..."), QLineEdit::Password, QString(), &isOk);

            if(isOk && pass == "3912"){
                ui.actExit->setEnabled(true);
                ui.actMasterMode->setChecked(true);
                setMasterMode(true);
                //                ui.wMainTabWidget->setIsMasterMode(true);
                ui.wLog->addLogsMessage(tr("Переход в режим мастера..."));
            } else {
                ui.actMasterMode->setChecked(false);
                ui.actExit->setEnabled(true);
                setMasterMode(false);
                //                ui.wMainTabWidget->setIsMasterMode(false);
                if(isOk){
                    QMessageBox::warning(nullptr, tr("Ошибка ввода..."), tr("Пароль мастера введен неправильно...\nПопробуйте еще раз..."));
                    ui.wLog->addLogsMessage(tr("Пароль мастера не верен..."), QColor(Qt::red));
                }
            }
        } else {
            ui.actMasterMode->setChecked(false);
            ui.actExit->setEnabled(true);
            setMasterMode(false);
            //            ui.wMainTabWidget->setIsMasterMode(false);
            ui.wLog->addLogsMessage(tr("Переход в режим с ограниченными возможностями..."));
        }

        ui.actUsersTable->setEnabled(isMasterMode);
        ui.actSmenasTable->setEnabled(isMasterMode);

        onChangeCurrentTab(ui.tabsAllWidgets->currentIndex());

        setMainWindowTitle();
    }
}

void MainWindow::onAbout(bool){
    about->show();
}

void MainWindow::onCloseCommand(bool)
{
    QList<ServerConnectState> disableStates = {
        spr_state_separated, spr_state_pitatel_on, spr_state_rasklad_on, spr_state_rentgen_on, spr_state_separator_on
    };
    bool noExit = false;


    storeWidgetsStates();

    int res = 1;

    foreach(SPRSeparatorWidgetData* sd, separators){
        SPRMainModel *model = sd->model;
        if(model){
            foreach(ServerConnectState state, disableStates){
                noExit |= model->getServer()->isState(state);
                if(noExit){
                    break;
                }
            }
            if(noExit){
                QMessageBox::warning(nullptr, tr("Ошибка завершения программы"),tr("Завершить программу, в данный момент, невозможно"
                                                    "\nИдет сортировка или включен рентген на устройстве %1"
                                                    "\nПеред завершением программы надо выключить сортировку и оборудование...").
                                     arg(sd->title));
                break;
            } else {
                if(sd->model->isSettingsChanged()){
//                    if(isMasterMode){
                        res = QMessageBox::question(nullptr, tr("Запрос на сохранение данных..."),
                                                        tr("Сохранить измененные настройки для машины %1?").arg(sd->title),
                                                        tr("Да"), tr("Не сохранять"), tr("Прервать выход из программы"), 0, 2);
//                    }
                }
                if(res == 0){
                    sd->model->saveAsAllData();
                    sd->model->getSpectrumListItemsModel()->saveAsAllData();
                }
                if(res == 2){
                    noExit = true;
                    break;
                }
            }
        }
    }
    if(!noExit){
        if(mainSettings) delete mainSettings;
        exit(0);
    }
}
