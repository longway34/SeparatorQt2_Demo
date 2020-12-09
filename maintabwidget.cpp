#include "maintabwidget.h"
#include "tcp/TCPCommand.h"
#include "models/sprseparatorlogsmodel.h"
#include <QFileInfo>

MainTabWidget::MainTabWidget(QWidget *parent) :
    QTabWidget(parent), ISPRWidget(), model(nullptr), errorSeparateState(SPR_SEPARATE_STATE_OK)
{
    ui.setupUi(this);

    connect(this, SIGNAL(currentChanged(int)), this, SLOT(widgetsShow()));

    isMasterMode = false;
}

MainTabWidget::MainTabWidget(QDomDocument *_doc, SPRSeparatorLogsModel *_logsModel, QWidget *parent):
    QTabWidget(parent), model(nullptr), logModel(_logsModel), errorSeparateState(SPR_SEPARATE_STATE_OK)
{
    ui.setupUi(this);

    isMasterMode = false;

    ISPRWidget::setDoc(doc);
    setModelData(new SPRMainModel(doc, logModel));

}

void MainTabWidget::setLogModel(SPRSeparatorLogsModel *_logModel){
    logModel = _logModel;

    if(model){
        model->setLogModel(_logModel);
    }

    ui.logWidget->setModelsData(_logModel);
    ui.wSeparateWigget->setLogModel(logModel);
}

SPRSeparatorLogsModel *MainTabWidget::getLogsModel()
{
    return logModel;
}


void MainTabWidget::onSaveAsSettings(IModelVariable *var){
    emit saveAsSettings(var);
}

MainTabWidget::MainTabWidget(QString _fName, QWidget *parent):
    QTabWidget(parent), ISPRWidget(),
    model(nullptr), errorSeparateState(SPR_SEPARATE_STATE_OK),
    logModel(nullptr)
{
    ui.setupUi(this);

    setDoc(_fName);

}

SPRMainModel *MainTabWidget::getModel() const
{
    return model;
}

void MainTabWidget::setStateSeparate(bool on_off)
{
}

bool MainTabWidget::isCloseEnabled()
{
    if(model){
        if(model->getServer()->isState(spr_state_pitatel_on) ||
           model){

        }
    }
}

SPRPitatelControlWidget *MainTabWidget::getPitatelControl()
{
    return ui.wSeparateWigget->getPitagelControl();
}

bool MainTabWidget::isStartStopProcess(){
    return ui.wSeparateWigget->isStartStopProcess();
}

SPRSeparateControlWidget *MainTabWidget::getSeparateControl(){
    return ui.wSeparateWigget->getSeparateControl();
}

bool MainTabWidget::getIsMasterMode() const
{
    return isMasterMode;
}

void MainTabWidget::setIsMasterMode(bool value)
{
    isMasterMode = value;
    ui.tabSettings->setMasterMode(isMasterMode);
    ui.tabSpectrum->setMasterMode(isMasterMode);
    ui.tabTest->setMasterMode(isMasterMode);
}

void MainTabWidget::onServerStateChanget(uint32_t){

}

ISPRModelData *MainTabWidget::setModelData(SPRMainModel *_model)
{
    if(_model){
        if(model){
            disconnect(model, SIGNAL(modelChanget(IModelVariable*)), this, SLOT(onModelChanget(IModelVariable*)));
            disconnect(model->getServer(), &ServerConnect::serverStateChangeOut, this, &MainTabWidget::onServerStateChanget);
        }
        model = _model;
        separateModel = model->getSeparateModel();

        connect(model, SIGNAL(modelChanget(IModelVariable*)), this, SLOT(onModelChanget(IModelVariable*)));
        ui.tabSettings->setModelData(model);
        connect(ui.tabSettings, &SPRSettingsWidget::saveAsSettings, this, &MainTabWidget::onSaveAsSettings);

        ui.tabSpectrum->setModelData(model);

        ui.tabTest->setModelData(model);
        ui.wSeparateWigget->setModelData(model);


        myChildren = QList<ISPRWidget*>({
            ui.tabSettings, ui.tabSpectrum, ui.tabTest, ui.wSeparateWigget, ui.logWidget
        });

        setCurrentWidget(ui.tabSettings);

        model->setServerConnecting(true);

        model->setSettingsChanget(false);
    }

}

void MainTabWidget::widgetsShow(){

    ui.tabSettings->widgetsShow();

    ui.tabSpectrum->widgetsShow();
    ui.tabTest->widgetsShow();
    ui.wSeparateWigget->widgetsShow();

}

void MainTabWidget::onChangeFileSettings(QString fName)
{
    setDoc(fName);
    setModelData(new SPRMainModel(doc, logModel));
    emit doShow();
}

void MainTabWidget::onChangeFileSpectrum(QString fName){
    QFile spc(fName);
    if(spc.open(QIODevice::ReadOnly)){
        if(model){
            blockSignals(true);
            model->getSpectrumListItemsModel()->clearSpectrums(spectrumsAll);
            model->getSpectrumListItemsModel()->addSpectrums(fName);
            blockSignals(false);
        }
        spc.close();
    }
}


void MainTabWidget::onModelChanget(IModelVariable *)
{
}


void MainTabWidget::setDoc(QString _fName)
{
    bool createNewSettingsDoc = false;
    bool createNewLogsDoc = false;

    if(!QFile::exists(_fName)){
        std::cout << "File not fount " << _fName.toStdString().c_str() << " used from recource..." << std::endl;
        _fName = ":/def/defSettings.xml";
        createNewSettingsDoc = true;

    } else {
        std::cout << "File fount " << _fName.toStdString().c_str() << " ..." << std::endl;
    }


    ISPRWidget::setDoc(_fName);

    setModelData(new SPRMainModel(doc, nullptr));


    if(createNewSettingsDoc){
        model->setFName(docFileName);
        QFile out(docFileName);
        if(out.open(QIODevice::WriteOnly)){
        QTextStream stream( &out );
          stream << doc->toString();

          out.close();
        }
    } else {
        if(model)
            model->setFName(docFileName);
    }

    connect(this, SIGNAL(currentChanged(int)), this, SLOT(widgetsShow()));
}


void MainTabWidget::storeWidgetsStates(QString groupId, QSettings *mainSettings)
{
    foreach(ISPRWidget *w, myChildren){
        w->storeWidgetsStates(groupId, mainSettings);
    }
    mainSettings->beginGroup(groupId);
    mainSettings->setValue(objectName(), saveGeometry());
    mainSettings->setValue(objectName()+".cuttentTab", QVariant(currentIndex()));
    mainSettings->endGroup();

}

void MainTabWidget::restoreWidgetsStates(QString groupId, QSettings *mainSettings)
{
    foreach(ISPRWidget *w, myChildren){
        w->restoreWidgetsStates(groupId, mainSettings);
    }
    mainSettings->beginGroup(groupId);
    restoreGeometry(mainSettings->value(objectName()).toByteArray());
    setCurrentIndex(mainSettings->value(objectName()+".cuttentTab").toInt());
    mainSettings->endGroup();
}
