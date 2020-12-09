#include "sprspectrumlisttablewidget.h"
#include <QHeaderView>
#include <QMessageBox>
#include <QFileDialog>
#include <QDir>
#include "tcp/tcpexpositiononoff.h"

#include "models/sprseparatorlogsmodel.h"

#include <qwt_plot_grid.h>

SPRViewGraphicsMode SPRSpectrumListTableWidget::getGraphicsMode() const
{
    SPRViewGraphicsMode val = viewModeAsIs;
    if(ui.gbViewAsIs->isChecked()){
        if(ui.rbAsOneSecond){
            val = viewModeOneSecond;
        }
        if(ui.rbScales){
            val = viewModeScales;
        }
    }
//    if(val != graphicsMode){
//        graphicsMode = val;
//    }
    return val;
}

//void SPRSpectrumListTableWidget::setGraphicsMode(const SPRViewGraphicsMode &value)
//{
//    graphicsMode = value;
//    ui.graphic->setGraphicsMode(value);
//}

SPRSpectrumListTableWidget::SPRSpectrumListTableWidget(QWidget *parent) :
    QWidget(parent), ISPRWidget(),
    model(nullptr),
    spectrums(nullptr),
    choiseTimeDialog(nullptr),
    gettingSpectrumsCommand(nullptr),
    gettingBaseSpectrumsCommand(nullptr),
    separatorOffCommand(nullptr),
//    rguUpDownCommand(nullptr),
    rentgenOffCommand(nullptr),
    commands(nullptr),
    toWidget(nullptr),
    masterMode(false)/*,
    graphicsMode(viewModeAsIs)*/
{
    ui.setupUi(this);
//    ui.graphic->setCanvasBackground(QBrush(Qt::black));

    serverConnectNeededWidget = QList<QWidget*>({
        ui.bGetBaseSpectrum, /*ui.bGetSpectrums, */ui.bSeparatorOff
    });

    connect(ui.tListSpectrumItem, &SPRSpectrumListTable2::rowSelectedChecked3, this, &SPRSpectrumListTableWidget::onSpectSpectrumTableClick);
//    connect(ui.tListSpectrumItem, SIGNAL(rowSelectedChecked3(QList<SPRSpectrumItemModel*>,SPRSpectrumItemModel*)), this, SLOT(onSpectSpectrumTableClick(QList<SPRSpectrumItemModel*>,SPRSpectrumItemModel*)));
//    connect(ui.tListSpectrumItem, SIGNAL(itemChangeColor(SPRSpectrumItemModel*,QColor)), ui.graphic, SLOT(onChangeGraphicItemsColor(SPRSpectrumItemModel*,QColor)));

//    connect(ui.tListBasedSpectrumItem, SIGNAL(cellClicked(int,int)), this, SLOT(onSpectSpectrumTableClick(int,int)));
    connect(ui.tListBasedSpectrumItem, SIGNAL(rowSelectedChecked3(QList<SPRSpectrumItemModel*>,SPRSpectrumItemModel*)), this, SLOT(onSpectSpectrumTableClick(QList<SPRSpectrumItemModel*>,SPRSpectrumItemModel*)));

//    connect(ui.tRangesChannel, SIGNAL(changeColor(EnumElements,QColor)), this, SLOT(onChangeZoneColor(EnumElements,QColor)));
//    connect(ui.tRangesChannel, SIGNAL(tableChange(EnumElements,int,int)), this, SLOT(onChangeZoneRange(EnumElements,int,int)));

    connect(ui.bGetSpectrums, SIGNAL(clicked(bool)), this, SLOT(onGetSpectrums(bool)));
    connect(ui.bGetBaseSpectrum, SIGNAL(clicked(bool)), this, SLOT(onGetSpectrums(bool)));

    connect(ui.bAverage, &QPushButton::clicked, this, &SPRSpectrumListTableWidget::onClickedBooton);
    connect(ui.bSeparatorOff, SIGNAL(clicked(bool)), this, SLOT(onClickedBooton(bool)));
    connect(ui.bRecompliteSpectrums, SIGNAL(clicked(bool)), this, SLOT(onClickedBooton(bool)));
    connect(ui.bOpenSpectrums, SIGNAL(clicked(bool)), this, SLOT(onClickedBooton(bool)));
    connect(ui.bSaveSpectrums, SIGNAL(clicked(bool)), this, SLOT(onClickedBooton(bool)));
    connect(ui.bSpectrumFNameSelect, SIGNAL(clicked(bool)), this, SLOT(onClickedBooton(bool)));
    connect(ui.bDeleteSpectors, &QPushButton::clicked, this, &SPRSpectrumListTableWidget::onClickedBooton);

    connect(ui.gbViewAsIs, SIGNAL(toggled(bool)), this, SLOT(onClickedBooton(bool)));
    connect(ui.rbAsOneSecond, SIGNAL(toggled(bool)), this, SLOT(onClickedBooton(bool)));
    connect(ui.rbScales, SIGNAL(toggled(bool)), this, SLOT(onClickedBooton(bool)));

    connect(ui.leSpectrumsFName, SIGNAL(editingFinished()), this, SLOT(onChangeSpectrumsFileName()));

    connect(ui.cbBasetSpectrumVisible, &QCheckBox::toggled, this, &SPRSpectrumListTableWidget::onClickedBooton);

    ui.gbBasetSpectrums->setVisible(ui.cbBasetSpectrumVisible->isChecked());

//    ui.tRangesChannel->setSelectBottonRowVisible(false);
    ui.graphic->setWithLegend(true);

    QwtPlotGrid *grid = new QwtPlotGrid();
    grid->setMajorPen(QColor(Qt::white), 1);
    grid->attach(ui.graphic->getCanvas());

    ui.tRangesChannel->setRecompliteButtonVisible(false);
}

void SPRSpectrumListTableWidget::onChangeSpectrumsFileName(){
    if(model){
        blockSignals(true);
        if(sender() == ui.leSpectrumsFName){
            QString fName = ui.leSpectrumsFName->text();
            if(!fName.isEmpty()){
                if(!fName.toUpper().endsWith(".SPC")){
                    fName = fName + ".spc";
                    ui.leSpectrumsFName->blockSignals(true);
                    ui.leSpectrumsFName->setText(fName);
                    ui.leSpectrumsFName->blockSignals(false);
                }
//                model->getSettingsMainModel()->getSpectrumFileNameVariable()->setData(fName);
                ui.bSaveSpectrums->setEnabled(true);
            } else {
                ui.bSaveSpectrums->setEnabled(false);
            }
        }
        blockSignals(false);
    }
}

void SPRSpectrumListTableWidget::setMasterMode(bool value)
{
//    masterMode = value;
//    ui.tRangesChannel->setEnabled(masterMode);
}

bool SPRSpectrumListTableWidget::isMasterMode()
{
    return masterMode;
}

void SPRSpectrumListTableWidget::changeGraphicViewMode(){
    SPRViewGraphicsMode mode = viewModeAsIs;
    if(ui.gbViewAsIs->isChecked()){
        if(ui.rbAsOneSecond->isChecked()){
            mode = viewModeOneSecond;
        }
        if(ui.rbScales->isChecked()){
            mode = viewModeScales;
        }
    }

//    if(model){
//        QList<SPRSpectrumItemModel*> *spects = spectrums->getSpectrumsModelAll();
//        spectrums->setV
//    }
    spectrums->setGraphicMode(mode);

    switch(mode){
    case viewModeAsIs:
        ui.gbViewAsIs->setTitle(tr("Отображение (как есть)"));
        break;
    case viewModeOneSecond:
        ui.gbViewAsIs->setTitle(tr("Отображение (1 секунда)"));
        break;
    case viewModeScales:
        ui.gbViewAsIs->setTitle(tr("Отображение (масштаб)"));
        break;
    }
}

void SPRSpectrumListTableWidget::onClickedBooton(bool value){
    if(model){
        if(sender() == ui.bAverage){
            QList<SPRSpectrumItemModel*> sels = ui.tListSpectrumItem->getSelectedItems();
            if(sels.size() > 1){
                this->model->getSpectrumListItemsModel()->averageSpectrums(sels);
            }
        }
        if(sender() == ui.cbBasetSpectrumVisible){
            ui.gbBasetSpectrums->setVisible(ui.cbBasetSpectrumVisible->isChecked());
        }
        if(sender() == ui.gbViewAsIs){
//            if(!value)
            changeGraphicViewMode();
        }
        if(sender() == ui.rbAsOneSecond){
            if(value)
                changeGraphicViewMode();
        }
        if(sender() == ui.rbScales){
            if(value)
                changeGraphicViewMode();
        }
        if(sender() == ui.bSeparatorOff){
            if(separatorOffCommand){
                delete separatorOffCommand; separatorOffCommand = nullptr;
                separatorOffCommand = new TCPCommandSeparatorOff(nullptr, nullptr);
            }
            separatorOffCommand->send(model->getServer());
        }
        if(sender() == ui.bOpenSpectrums){
            QString fname = QFileDialog::getOpenFileName(this, tr("Выберите файл с готовыми спекрами..."), QDir::currentPath(), tr("файлы спектров (*.spc)"));

            this->blockSignals(true);
            model->getSpectrumListItemsModel()->addSpectrums(fname);
            this->blockSignals(false);
        }
        if(sender() == ui.bRecompliteSpectrums){

            QList<SPRSpectrumItemModel*> lst = ui.tListSpectrumItem->getSelectedItems();
            QList<SPRSpectrumItemModel*> bLst = ui.tListBasedSpectrumItem->getSelectedItems();

            uint32_t res = QMessageBox::No;

            int selectedSize = lst.size() + bLst.size();
            int s = selectedSize % 10;


            if(lst.size() == 0){
                res = QMessageBox::warning(nullptr, tr("Необходимо подтверждение"),
                                     tr("Вы действительно хотите пересчитать ВСЕ спектры включая базовые?"),
                                     QMessageBox::Yes, QMessageBox::No);

                lst = ui.tListSpectrumItem->getAllItems();
                bLst = ui.tListBasedSpectrumItem->getAllItems();

            } else {
                QString str = (s == 1) ?
                            tr("выделенный спектр") : tr("%1 выделенных спектра").arg(selectedSize);
                res = QMessageBox::warning(nullptr, tr("Необходимо подтверждение"),
                                     tr("Вы действительно хотите пересчитать %1?").arg(str),
                                     QMessageBox::Yes, QMessageBox::No);
            }

            if(res == QMessageBox::Yes){
                this->blockSignals(true);

                foreach(SPRSpectrumItemModel *it, lst){
                    it->recomplite();
                }
                foreach(SPRSpectrumItemModel *it, bLst){
                    it->recomplite();
                }
//                model->getSpectrumListItemsModel()->recomplite();

                blockSignals(false);
                ui.tListSpectrumItem->widgetsShow();
                ui.tListBasedSpectrumItem->widgetsShow();
            }
        }
        if(sender() == ui.bSpectrumFNameSelect){
            QString fName = model->getSettingsMainModel()->getSpectrumFileNameVariable()->getData();
            QString nfName = QFileDialog::getSaveFileName(nullptr,
                                QString(tr("Выберите файл для записи спектров")),
                                QDir::currentPath(),
                                QString(tr("файлы спектров (*.spc)")));

            if(fName != nfName){
                if(!nfName.toUpper().endsWith(".SPC")){
                    nfName = nfName + ".spc";
                }
//                model->getSettingsMainModel()->getSpectrumFileNameVariable()->setData(nfName);
                ui.leSpectrumsFName->blockSignals(true);
                ui.leSpectrumsFName->setText(nfName);
                ui.leSpectrumsFName->blockSignals(false);
           }
        }
        if(sender() == ui.bSaveSpectrums){
            QString oldFName = model->getSettingsMainModel()->getSpectrumFileNameVariable()->getData();
            QString newFName = ui.leSpectrumsFName->text();
            bool res = model->getSpectrumListItemsModel()->saveAsAllData(newFName);
            if(res){
                if(model->getLogModel()){
                    model->getLogModel()->addLogMessage(nullptr, tWarning, tr("Спектры сохранены в файле %1").arg(newFName));
                }
                ui.leSpectrumsFName->blockSignals(true);
                ui.leSpectrumsFName->setText(oldFName);
                ui.leSpectrumsFName->blockSignals(false);
            } else {
                if(model->getLogModel()){
                    model->getLogModel()->addLogMessage(nullptr, tError, tr("Ошибка сохранения спектров в файле %1").arg(newFName));
                }
            }
        }
        if(sender() == ui.bDeleteSpectors){
            ui.tListSpectrumItem->onDeleteRow(nullptr);
        }
    }
}

void SPRSpectrumListTableWidget::onGetSpectrums(bool){
    if(sender() == ui.bGetSpectrums){
        if(choiseTimeDialog){
            choiseTimeDialog->setModelData(model);

            choiseTimeDialog->setMasterMode(isMasterMode());

            int res = choiseTimeDialog->exec();

            if(res == QDialog::Accepted){
                double time = choiseTimeDialog->getTime();
                QList<uint8_t> lth = choiseTimeDialog->getThreads();
                SPRTypeRGUMoved rgu = choiseTimeDialog->isRGUDown() ? DownRGU : noRGU;

                if(gettingSpectrumsCommand) delete gettingSpectrumsCommand;
                gettingSpectrumsCommand = new TCPGetSpectrums2(model, time, rgu, lth, choiseTimeDialog->isRentgenOff());
                mainConnect(gettingSpectrumsCommand);

                gettingSpectrumsCommand->setOnExposition(choiseTimeDialog->isRentgenOn());

                gettingSpectrumsCommand->send(model->getServer());
            }
        }
    }
    if(sender() == ui.bGetBaseSpectrum){

        QList<uint8_t> lthreads = model->getThreadsList();

        if(gettingBaseSpectrumsCommand) delete gettingBaseSpectrumsCommand;
        gettingBaseSpectrumsCommand = new TCPGetSpectrums2(model, 30, UpRGU, lthreads, true);

        mainConnect(gettingBaseSpectrumsCommand);
        gettingBaseSpectrumsCommand->send(model->getServer());
    }
}

void SPRSpectrumListTableWidget::onCommandComplite(TCPCommand *command)
{

    if(command == gettingSpectrumsCommand){

        QList<uint8_t> lth = choiseTimeDialog->getThreads();

        QList<IModelVariable*> receiveSpect;
        for(uint8_t th=0; th<lth.size(); th++) {
            SPRSpectrumItemModel *item =
            model->getSpectrumListItemsModel()->addSpectrum(gettingSpectrumsCommand->getSpectrumData(th), choiseTimeDialog->getTime() * 1000, lth[th], choiseTimeDialog->getPrefix());
            item->setTimeScope(choiseTimeDialog->getTime() * 1000);
            receiveSpect.append(item);
        }
        for(int r=0; r<ui.tListSpectrumItem->rowCount();r++){
            FirstCollumn2 *fc = qobject_cast<FirstCollumn2 *>(ui.tListSpectrumItem->cellWidget(r, 0));
            if(fc){
                IModelVariable *item = fc->getModelPrt();
                if(receiveSpect.contains(item)){
                    fc->setSelect(true);
                }
            }
        }
        widgetsShow();
    }
    if(command == gettingBaseSpectrumsCommand){
        QList<TCPCommand*> vec = gettingBaseSpectrumsCommand->findCommands(getspk);

        for(uint8_t th=0; th<vec.size(); th++){
            QByteArray buf = gettingBaseSpectrumsCommand->getSpectrumData(th);
            model->getSpectrumListItemsModel()->setSpectrumData(th, buf, spectrumBase);
        }
        widgetsShow();
    }
    ui.graphic->setGraphicsMode(getGraphicsMode());
}

void SPRSpectrumListTableWidget::onCommandNotComplite(TCPCommand *command)
{
//    if(command->getCommand() == getren){
////        QMessageBox::information(this, QString(tr("Рентген не готов")), QString(tr("Рентген не готов")), QMessageBox::Ok);
//    }
//    if(command->getCommand() == getstate){
////        QMessageBox::information(this, QString(tr("Сортировочн не готов")), QString(tr("Сепаратор не готов")), QMessageBox::Ok);
//    }
}

void SPRSpectrumListTableWidget::mainConnect(TCPCommand *command){
    if(command){
        connect(command, &TCPCommand::commandComplite, this, &SPRSpectrumListTableWidget::onCommandComplite);
        connect(command, &TCPCommand::commandNotComplite, this, &SPRSpectrumListTableWidget::onCommandNotComplite);
    }
}

ISPRModelData *SPRSpectrumListTableWidget::setModelData(SPRMainModel *mainModel){
    if(mainModel){
        if(model){
            disconnect(model, SIGNAL(modelChanget(IModelVariable*)), this, SLOT(onModelChanget(IModelVariable*)));
        }
        model = mainModel;
        connect(model, SIGNAL(modelChanget(IModelVariable*)), this, SLOT(onModelChanget(IModelVariable*)));
        connect(model->getServer(), &ServerConnect::serverStateChangeOut, this, &SPRSpectrumListTableWidget::onServerStateChange);

        connect(model->getSpectrumZonesTableModel(), &SPRSpectrumZonesTableModel::modelChanget, this, &SPRSpectrumListTableWidget::onModelChanget);


        spectrums = mainModel->getSpectrumListItemsModel();

        ui.graphic->setModelData(spectrums, spectrumsAll, true);

        ui.tListBasedSpectrumItem->setModelData(spectrums, spectrumBase);
        ui.tListSpectrumItem->setModelData(spectrums, spectrumsOnly);

        ui.tRangesChannel->setModelData(mainModel->getSpectrumZonesTableModel());
        ui.tRangesChannel->setThreadsVisible(0);

        QBrush back = QBrush(QColor(Qt::lightGray));
        ui.graphic->ui.canvas->setCanvasBackground(back);

        if(choiseTimeDialog){
            delete choiseTimeDialog;
        }

        choiseTimeDialog = new SPRSpectrumChoiseTimeDialog(model, this);

        if(!toWidget){
            toWidget = new TCPTimeOutWidget();
        }

        if(!separatorOffCommand){
            separatorOffCommand = new TCPCommandSeparatorOff(nullptr, toWidget);
            connect(separatorOffCommand, SIGNAL(commandComplite(TCPCommand*)), this, SLOT(onCommandComplite(TCPCommand*)));
            connect(separatorOffCommand, SIGNAL(commandNotComplite(TCPCommand*)), this, SLOT(onCommandNotComplite(TCPCommand*)));
        }
    }

    widgetsShow();
    return model;
}

void SPRSpectrumListTableWidget::widgetsShow()
{

    if(model){
        blockSignals(true);
        ui.leSpectrumsFName->setText(model->getSettingsMainModel()->getSpectrumFileNameVariable()->getData());
        blockSignals(false);
    }
    onServerStateChange(0);
}

void SPRSpectrumListTableWidget::onServerStateChange(uint32_t)
{
    bool enabled = false;
    bool enabledGetSpectrumWithRentren = true;
    if(model){
        ServerConnect *server = model->getServer();
        if(server){
            if(!server->isState(spr_state_server_connect) || server->isState(spr_state_separated)){
                enabled = false;
            } else {
                enabled = true;
            }

            enabledGetSpectrumWithRentren = !server->isState(spr_state_separator_wo_rentgen);
        }
    }

    if(choiseTimeDialog){
        choiseTimeDialog->setWithRentgenMode(enabledGetSpectrumWithRentren);
    }

    foreach(QWidget *w, serverConnectNeededWidget){
        w->setEnabled(enabled);
    }

//    ui.bGetSpectrums->setEnabled(enabledGetSpectrumWithRentren);
    ui.bGetBaseSpectrum->setEnabled(enabledGetSpectrumWithRentren && enabled);
    ui.bGetSpectrums->setEnabled(enabled);
}

ISPRModelData *SPRSpectrumListTableWidget::getModelData()
{
    return model;
}


void SPRSpectrumListTableWidget::onSpectSpectrumTableClick(QList<SPRSpectrumItemModel*>, SPRSpectrumItemModel *_current){
    blockSignals(true);


    if(_current){
        uint8_t thr = _current->getThread();
        ui.tRangesChannel->setThreadsVisible(thr);
    }

    QList<SPRSpectrumItemModel*> toGraphSelected;
    foreach(SPRSpectrumItemModel* model, ui.tListSpectrumItem->getSelectedItems()){
        if(model){
            toGraphSelected.push_back(model);
        }
    }

    if(toGraphSelected.size() > 1){
        ui.bAverage->setEnabled(true);
    } else {
        ui.bAverage->setEnabled(false);
    }

    if(sender() == ui.tListSpectrumItem && _current){
        if(toGraphSelected.size() > 0){
            ui.bDeleteSpectors->setEnabled(true);
        } else {
            ui.bDeleteSpectors->setEnabled(false);
        }
    } else {
        ui.bDeleteSpectors->setEnabled(false);
    }

    foreach(SPRSpectrumItemModel* model, ui.tListBasedSpectrumItem->getSelectedItems()){
        if(model){
            toGraphSelected.push_back(model);
        }
    }

    ui.graphic->onChangeSelectedCheckedItems(toGraphSelected, _current);
    ui.graphic->widgetsShow();
    blockSignals(false);

}

void SPRSpectrumListTableWidget::viewChange(int row)
{
    widgetsShow();
}

void SPRSpectrumListTableWidget::viewChange(bool value)
{
}

void SPRSpectrumListTableWidget::onChangeZoneRange(EnumElements el, int thread, int col)
{
    widgetsShow();
}


void SPRSpectrumListTableWidget::onModelChanget(IModelVariable *variable)
{
    if((sender() == model && variable != spectrums) || (!sender())){
        QObject *send=sender();
        widgetsShow();
    }
}


void SPRSpectrumListTableWidget::setLogModel(TCPEchoLogsWidget *value)
{
//    ISPRWidget::setLogModel(value);
//    gettingSpectrumsCommand->setLogWidget(value);
//    gettingBaseSpectrumsCommand->setLogWidget(value);
//    rguUpDownCommand->setLogWidget(value);
    separatorOffCommand->setLogWidget(value);
}


void SPRSpectrumListTableWidget::storeWidgetsStates(QString groupId, QSettings *mainSettings)
{
    mainSettings->beginGroup(groupId);
    mainSettings->setValue(objectName()+"."+ui.spBaseSpectrums->objectName(), ui.spBaseSpectrums->saveState());
    mainSettings->setValue(objectName()+"."+ui.spTables->objectName(), ui.spTables->saveState());
    mainSettings->setValue(objectName()+"."+ui.spElements->objectName(), ui.spElements->saveState());
    mainSettings->setValue(objectName()+"."+ui.cbBasetSpectrumVisible->objectName(), QVariant(ui.cbBasetSpectrumVisible->isChecked()));
    mainSettings->endGroup();

    ui.tListSpectrumItem->storeWidgetsStates(groupId, mainSettings);
    ui.tListBasedSpectrumItem->storeWidgetsStates(groupId, mainSettings);
}

void SPRSpectrumListTableWidget::restoreWidgetsStates(QString groupId, QSettings *mainSettings)
{
    mainSettings->beginGroup(groupId);
    ui.spBaseSpectrums->restoreState(mainSettings->value(objectName()+"."+ui.spBaseSpectrums->objectName()).toByteArray());
    ui.spTables->restoreState(mainSettings->value(objectName()+"."+ui.spTables->objectName()).toByteArray());
    ui.spElements->restoreState(mainSettings->value(objectName()+"."+ui.spElements->objectName()).toByteArray());
    ui.cbBasetSpectrumVisible->setChecked(mainSettings->value(objectName()+"."+ui.cbBasetSpectrumVisible->objectName()).toBool());
    mainSettings->endGroup();

    ui.tListSpectrumItem->restoreWidgetsStates(groupId, mainSettings);
    ui.tListBasedSpectrumItem->restoreWidgetsStates(groupId, mainSettings);
}
