#include "sprseparatecontrolwidget.h"
#include <QMessageBox>
#include "models/sprhistorymodel2.h"

#include "models/sprseparatorlogsmodel.h"

//#include <windows.h>
//#include <stdio.h>
//#include <psapi.h>

SPRSeparateControlWidget::SPRSeparateControlWidget(QWidget *parent) :
    QWidget(parent),
    model(nullptr),
    histModel(nullptr),
    kSpectrumModel(nullptr),
    startSeparate(nullptr),
    stopSeparator(nullptr),
    stopSeparate(nullptr),
    setSeparate(nullptr),
    getBaseSpectrumsCommand(nullptr),
    toWidget(nullptr)
{
    ui.setupUi(this);


    clearErrorsCounts();

//    spectWarningLoadAirSend = 0;
//    spectWarningCorrelSend = 0;
//    spectWarningDiffSend = 0;
//    speedStoneErrSend = 0;

//    transportWarningSend = false;

    connect(ui.bSeparateStart, SIGNAL(clicked(bool)), this, SLOT(onCommandButtomClick(bool)));
    connect(ui.bSeparateStop, SIGNAL(clicked(bool)), this, SLOT(onCommandButtomClick(bool)));
    connect(ui.bSeparateComplite, SIGNAL(clicked(bool)), this, SLOT(onCommandButtomClick(bool)));
    connect(ui.bClearSeparateStatistic, SIGNAL(clicked(bool)), this, SLOT(onCommandButtomClick(bool)));

    separateTimer.setInterval(1000);
    lastTimerUpdate = QDateTime::currentDateTime();
    connect(&separateTimer, SIGNAL(timeout()), this, SLOT(onTimeOut()));
}

void SPRSeparateControlWidget::setModelData(SPRMainModel *_model, TCPTimeOutWidget *_toWidget, SPRSeparatorLogsModel *_logModel)
{
    model = _model;
    toWidget = _toWidget;
    logModel = _logModel;

    if(model){

        connect(model->getServer(), SIGNAL(serverStateChangeOut(uint32_t)), this, SLOT(onServerStateChange(uint32_t)));

        kSpectrumModel = model->getKSpectrums();

        histModel = model->getHistoryModel2();

//        if(startSeparate) delete startSeparate;
//        startSeparate = new TCPStartSeparate2(model, getTimeOutWidget(), nullptr);
//        startSeparate->setLogModel(logModel);
//        connect(startSeparate, SIGNAL(commandComplite(TCPCommand*)), this, SLOT(onTCPCommandComplite(TCPCommand*)));
//        connect(startSeparate, SIGNAL(commandNotComplite(TCPCommand*)), this, SLOT(onTCPCommandNotComplite(TCPCommand*)));

//        stopSeparate = new TCPTestStopSeparate(toWidget);
//        connect(stopSeparate, SIGNAL(commandComplite(TCPCommand*)), this, SLOT(onTCPCommandComplite(TCPCommand*)));

        if(setSeparate) delete setSeparate;
        setSeparate = new TCPCommand(setsepar);
        connect(setSeparate, SIGNAL(commandComplite(TCPCommand*)), this, SLOT(onTCPCommandComplite(TCPCommand*)));
        int err;
        QByteArray settings = model->getSeparateModel()->toByteArray(&err);
        setSeparate->setSendData(settings);

        separateTimer.start(1000);

        clearErrorsCounts();

//        spectWarningLoadAirSend = 0;
//        spectWarningCorrelSend = 0;
//        spectWarningDiffSend = 0;
//        speedStoneErrSend = 0;
    }

}

bool SPRSeparateControlWidget::isStartStopProcess()
{
    if(startSeparate){
        if(startSeparate->isProcessRun()){
            if(startSeparate->isStartStopProcess()){
                return true;
            } else {
                return false;
            }
        } else {
            return false;
        }
    } else {
        return false;
    }
}

void SPRSeparateControlWidget::separateGo(){
    if(model){
        if(startSeparate) delete startSeparate; startSeparate = nullptr;

        startSeparate = new TCPStartSeparate2(model, getTimeOutWidget(), getLogModel());
        connect(startSeparate, &TCPStartSeparate2::kSpactrumReady, this, &SPRSeparateControlWidget::onKSpectrumReady);
        connect(startSeparate, &TCPStartSeparate2::gistogrammReady, this, &SPRSeparateControlWidget::onGistogrammReady);
        connect(startSeparate, &TCPStartSeparate2::separateDataReady, this, &SPRSeparateControlWidget::onSeparateDataReady);


//        startSeparate->setVisibleTOWidgets(visible);

        startSeparate->send(model->getServer());

        spectWarningLoadAirSend = 0;
        spectWarningCorrelSend = 0;
        spectWarningDiffSend = 0;
        speedStoneErrSend = 0;

        clearErrorsCounts();

        flagSeparateGo = true;
        lastTimerUpdate = QDateTime::currentDateTime();

    }
}


void SPRSeparateControlWidget::separateStartStop(bool onOff, bool toVisible){
    if(model){
        if(onOff){
            if(!flagSeparateGo){
                if(!model->getServer()->isState(spr_state_separated)){
                    if(getBaseSpectrumsCommand) delete getBaseSpectrumsCommand; getBaseSpectrumsCommand = nullptr;

                    getBaseSpectrumsCommand = new TCPGetSpectrums2(model, 5, UpRGU, model->getThreadsList(), false, getTimeOutWidget());
                    connect(getBaseSpectrumsCommand, &TCPGetSpectrums2::commandComplite, this, &SPRSeparateControlWidget::onTCPCommandComplite);
                    connect(getBaseSpectrumsCommand, &TCPGetSpectrums2::commandNotComplite, this, &SPRSeparateControlWidget::onTCPCommandNotComplite);

                    getBaseSpectrumsCommand->setVisibleTOWidgets(toVisible);
                    getBaseSpectrumsCommand->send(model->getServer());
                } else {
                    separateGo();
                }
            }
        } else {
            if(startSeparate){
                TCPSeparateGo2 *goCommand = startSeparate->getSeparateGo();
                if(goCommand){
                    goCommand->stopSeparate();
                }
            } else {
                if(stopSeparator) delete stopSeparator; stopSeparator = nullptr;
                stopSeparator = new TCPCommandSeparatorOff(nullptr, nullptr);

                stopSeparator->send(model->getServer());
            }
        }

    }
}

void SPRSeparateControlWidget::onCommandButtomClick(bool)
{
    if(model){
        if(sender() == ui.bSeparateStart){
//            startSeparate->send(model->getServer());
            separateStartStop(true, true);
            return;
        }
        if(sender() == ui.bSeparateStop){

            separateStartStop(false, true);
            return;
        }
        if(sender() == ui.bClearSeparateStatistic){

//            SPRHistoryModel2 *hist = model->getHistoryModel2();
            if(histModel){
                histModel->clear();
            }
            if(getLogModel()){
                getLogModel()->addLogMessage(nullptr, tWarning, tr("Статистика работы сортировки очищена..."));
            }
            return;
        }
        if(sender() == ui.bSeparateComplite){
            if(setSeparate){
                int err;
                QByteArray settings = model->getSeparateModel()->toByteArray(&err);
                setSeparate->setSendData(settings);
                setSeparate->send(model->getServer());
            }
        }
    }

}

void SPRSeparateControlWidget::onTimeOut()
{
    if(flagSeparateGo){
        qint64 ddiff = llabs(lastTimerUpdate.secsTo(QDateTime::currentDateTime()));

        int MIN_SEC = 60;
        int HOUR_SEC = (MIN_SEC * 60);

        int h = ddiff / (HOUR_SEC);
        int m = (ddiff % HOUR_SEC) / MIN_SEC;
        int sec = (ddiff & MIN_SEC);
        QTime td = QTime(h,  m,  sec);
        if(ddiff  > 0){
            ui.leRudospustTime->setText(td.toString("hh:mm:ss"));
        }
    }
    onServerStateChange(0);
}

void SPRSeparateControlWidget::clearErrorsCounts(){
    spectWarningCorrelSend = 0;
    spectWarningLoadAirSend = 0;
    spectWarningDiffSend = 0;
}


bool SPRSeparateControlWidget::isErrorsOnSpectrumReady(QVector<SPRKSpectrumSeparateSummary> *vSpectSummary){
    QList<int> errCorrel;
    QString strChErrCorrl;
    QString strValCorrel;

    QList<int> errPower;
    QString strChErrPower;
    QString strValPower;

    QList<int> errAir;
    QString strChErrAir;
    QString strValAir;

    QList<QString> vStrErrors;

    SPRTypeLogMessage type = model->getSettingsMainModel()->igroreCriticalErrors->getData() ? tWarning : tError;

    bool isErrors = false;

    for(int th=0; th<vSpectSummary->size(); th++){
        if((*vSpectSummary)[th].isErrors){
            if((*vSpectSummary)[th].correlError){
                isErrors = true;
                if(spectWarningCorrelSend == 0){
                    strChErrCorrl += errCorrel.size() == 0 ? tr("%1").arg(th+1) : tr(", %1").arg(th+1);
                    strValCorrel += errCorrel.size() == 0 ? tr("%1").arg((*vSpectSummary)[th].correl, 2, 'f'):
                                                            tr(", %1").arg((*vSpectSummary)[th].correl, 2, 'f');
                    errCorrel.append(th);
                }
            }
            if((*vSpectSummary)[th].airError){
                isErrors = true;
                if(spectWarningLoadAirSend == 0){
                    strChErrAir += errAir.size() == 0 ? tr("%1").arg(th+1) : tr(", %1").arg(th+1);
                    strValAir += errAir.size() == 0 ? tr("%1").arg((*vSpectSummary)[th].airLoad, 0, 'f') :
                                                      tr(", %1").arg((*vSpectSummary)[th].airLoad, 0, 'f');
                    errAir.append(th);
                }
            }
            if((*vSpectSummary)[th].powerError){
                isErrors = true;
                if(spectWarningDiffSend == 0){
                    strChErrPower += errPower.size() == 0 ? tr("%1").arg(th+1) : tr(", %1").arg(th+1);
                    strValPower += errPower.size() == 0 ? tr("%1").arg((*vSpectSummary)[th].diffPower, 0, 'f'):
                                                          tr(", %1").arg((*vSpectSummary)[th].diffPower, 0, 'f');
                    errPower.append(th);
                }
            }
        }
    }

    if(isErrors){
        if(errCorrel.size() > 0){
            QString ch;
            if(errCorrel.size() > 1){
                ch = tr("в ручье %1").arg(strChErrCorrl);
            } else {
                ch = tr("в ручьях %1").arg(strChErrCorrl);
            }

            QString strErr = tr("Корреляция %1 (%2) ниже критического уровня %3...").
                    arg(ch).
                    arg(strValCorrel).
                    arg(model->getSettingsControlModel()->correlSpectrumCritical->getData());
            vStrErrors.append(strErr);
            spectWarningCorrelSend = 10;
        }

        if(errAir.size() > 0){
            QString ch;
            if(errAir.size() > 1){
                ch = tr("в ручье %1").arg(strChErrAir);
            } else {
                ch = tr("в ручьях %1").arg(strChErrAir);
            }
            QString strErr = tr("Загрузка по воздуху %1 (%2) за пределами критического диапазона %3-%4...").
                    arg(ch).
                    arg(strValAir).
                    arg(model->getSettingsControlModel()->airLoadingMinCritical->getData()).
                    arg(model->getSettingsControlModel()->airLoadingMaxCritical->getData());
            vStrErrors.append(strErr);
            spectWarningLoadAirSend = 10;
        }

        if(errPower.size() > 0){
            QString ch;
            if(errPower.size() > 1){
                ch = tr("в ручье %1").arg(strChErrPower);
            } else {
                ch = tr("в ручьях %1").arg(strChErrPower);
            }
            QString strErr = tr("Отклонение центра тяжести %1 (%2) выше критического %3...").
                    arg(ch).
                    arg(strValPower).
                    arg(model->getSettingsControlModel()->diffCenterGravityCritical->getData());
            vStrErrors.append(strErr);
            spectWarningDiffSend = 10;
        }

        if(model->getLogModel()){
            foreach(QString str, vStrErrors){
                model->getLogModel()->addLogMessage(nullptr, type, str);

                if(model->getSettingsMainModel()->igroreCriticalErrors->getData()){
                    model->getLogModel()->addLogMessage(nullptr, type, tr(" Игнорируем..."));
                } else {
                    model->getLogModel()->addLogMessage(nullptr, type, tr(" Сортировка будет остановлена..."));
                }
            }
        }
    } else {
        if(spectWarningDiffSend > 0) spectWarningDiffSend--;
        if(spectWarningCorrelSend > 0) spectWarningCorrelSend--;
        if(spectWarningLoadAirSend > 0) spectWarningLoadAirSend--;
    }

    return isErrors;
}

void SPRSeparateControlWidget::onKSpectrumReady(TCPCommand *_command){
    if(model && kSpectrumModel){
        TCPGetSpectrumsGistogramms *getCommand = qobject_cast<TCPGetSpectrumsGistogramms*>(_command);
        if(getCommand){

            QList<TCPCommand*> vkspk = getCommand->findCommands(getkspk);
            for(int th=0; th<vkspk.size(); th++){
                QByteArray res = getCommand->getKSpectrumData(th);
                uint32_t t32 = getCommand->getKSpectrumTime(th);

                SPRSpectrumItemModel *spect = kSpectrumModel->setSpectrumData(th, res, spectrumsOnly, t32);
                spect->setTimeScope(t32);
            }
            QVector<SPRKSpectrumSeparateSummary> *vSpectSummary = model->getHistoryModel2()->onKSpectrumsReady();

            bool stopNeed = !model->getSettingsMainModel()->igroreCriticalErrors->getData();
            bool isErrors = isErrorsOnSpectrumReady(vSpectSummary);

            if(stopNeed && isErrors){
                separateStartStop(false, false);
            }
        }
    }
}

void SPRSeparateControlWidget::onGistogrammReady(TCPCommand *command){
    QList<TCPCommand*> vgist = command->findCommands(getgist);
    for(int i=0; i<vgist.size();i++){
        QByteArray gist = vgist[i]->getReplayData();
        int s = gist.size();
        if(s > 1){
            histModel->setWorkGistogrammData(gist.right(s-1), i);
        }
    }
    return;
}

void SPRSeparateControlWidget::onSeparateDataReady(TCPCommand *command){

    QByteArray res = command->getReplayData();
    int s = res.size();
    if(s > 1){
        histModel->setWorkSeparateData(res.right(s-1));

        uint critSpeed = model->getSettingsControlModel()->speedStreamCritical->getData();

        QVector<SPRHistoryModel2::histDataOneThread> *diff = histModel->getDiffRecords();
        QString errChannelStr = "";
        QString errValueStr;
        QList<int> errChannelNum;

        for(int th=0; th < (*diff).size(); th++){
            double t = diff->at(th).data.mdt / 1000;
            double ss = 0;

            if(t > 1e-6){
                ss = diff->at(th).data.i_prd[2] / t;
            }
            if(ss > critSpeed){
                errChannelNum.append(th);
                if(errChannelNum.size() > 1) {
                    errChannelStr += ", ";
                    errValueStr += ", ";
                }
                errChannelStr += tr("%1").arg(th+1);
                errValueStr += tr("%1").arg(ss, 0, 'f');
            }
        }

        if(errChannelNum.size() > 0){
            bool ignoreErrors = model->getSettingsMainModel()->igroreCriticalErrors->getData();
            SPRTypeLogMessage type = ignoreErrors ? tWarning : tError;
            if(speedStoneErrSend == 0 || !ignoreErrors){
                speedStoneErrSend = 10;
                QString errMsg = tr("Скорость потока камней по ");
                if(errChannelNum.size() > 1){
                    errMsg += tr("ручьям %1 (%2)").arg(errChannelStr).arg(errValueStr);
                } else {
                    errMsg += tr("ручью %1 (%2)").arg(errChannelStr).arg(errValueStr);
                }
                errMsg += tr(" выше критической (%1 к./сек.)").arg(critSpeed);
                if(model->getLogModel()){
                    model->getLogModel()->addLogMessage(nullptr, type, errMsg);
                }
//                model->getLogModel()->addLogMessage(nullptr, type, errMsg);
                if(!ignoreErrors){
                    if(model->getLogModel()){
                        model->getLogModel()->addLogMessage(nullptr, type, tr("Сортировка будет остановлена..."));
                    }
                    separateStartStop(false, false);
                } else {
                    if(model->getLogModel()){
                        model->getLogModel()->addLogMessage(nullptr, type, tr("Игнорируем..."));
                    }
                }
            }
        } else {
            if(speedStoneErrSend > 0) speedStoneErrSend--;
        }
    }
    return;
}

void SPRSeparateControlWidget::onTCPCommandComplite(TCPCommand *command)
{
    if(model && kSpectrumModel){

        if(sender() == getBaseSpectrumsCommand){
            QList<TCPCommand*> vspect = command->findCommands(getspk);

//            QList<int> corrErr;
//            QString errChannels = "";

            if(vspect.size() == model->getThreads()->getData()){

                for(int i=0; i<vspect.size(); i++){
                    QByteArray ba = getBaseSpectrumsCommand->getSpectrumData(i);

                    SPRSpectrumItemModel *item = kSpectrumModel->setSpectrumData(i, ba, spectrumsOnly, 1000);
                    item->setTimeScope(5000);

                }

                clearErrorsCounts();
                QVector<SPRKSpectrumSeparateSummary> *vSpectSummary = model->getHistoryModel2()->onKSpectrumsReady();

                bool isErrors = isErrorsOnSpectrumReady(vSpectSummary);
                bool stopNeed = !model->getSettingsMainModel()->igroreCriticalErrors->getData();

                if(isErrors){
                    SPRTypeLogMessage type = stopNeed ? tError : tWarning;
                    QString errMsg = tr("Обнаружены критичные значения контрольного спектра.\nВозможно неправильно сняты базовые спектры...");

                    if(stopNeed){
                        errMsg += tr("\nСтарт сортировки не возможен...");
                    } else {
                        errMsg += tr("\nИгнорируем...");
                    }
                    if(model->getLogModel()){
                        model->getLogModel()->addLogMessage(nullptr, type, errMsg);
                    }
                    if(getBaseSpectrumsCommand->getVisibleTOWidgets() && stopNeed){
                        QMessageBox::critical(nullptr, tr("Ошибка старта сортировки..."), errMsg);
                    }
                    if(stopNeed){
                        separateStartStop(false, getBaseSpectrumsCommand->getVisibleTOWidgets());
                        return;
                    }
                }

                separateGo();
            } else {
                qDebug() << "SPRSeparateControl : error getSpectrumCommand";
            }
        }

        if(sender() == setSeparate){
            if(logModel){
                logModel->addLogMessage(nullptr, tWarning, tr("Применены новые настройки сортировки..."));
            }
            QMessageBox::information(nullptr, tr("Настройки сортировочной машины"), tr("Применены новые настройки сортировки..."));
        }

    }

}

void SPRSeparateControlWidget::onTCPCommandNotComplite(TCPCommand *command)
{
    if(sender() == startSeparate){
        if(command->getCommand() == getren){
            uint16_t mka, mkv; uint8_t err;
            startSeparate->isRentgenReady(command->getReplayData(), &mkv, &mka, &err);
            QMessageBox::warning(nullptr, tr("Ошибка включения рентгена"),
                                 tr("Рентген не вышел на рабочий режим (%1, %2 < 0x600)\n"
                                    "Попробуйте выключить рентген\n"
                                    "И запустить сортировку еще раз...").
                                 arg(mkv, 0, 16).arg(mka, 0, 16));

        }
    }
}


void SPRSeparateControlWidget::onServerStateChange(uint32_t)
{
    if(model){
        if(!model->getServer()->isState(spr_state_server_connect)){
            this->setEnabled(false);
            return;
        } else {
            this->setEnabled(true);
        }

        if(model->getServer()->isState(spr_state_separator_on) && !model->getServer()->isState(spr_state_separator_wo_rentgen)){
            ui.bSeparateStart->setEnabled(true);
        } else {
            ui.bSeparateStart->setEnabled(false);
        }
        if(model->getServer()->isState(spr_state_separated)){
            if(!flagSeparateGo){


                separateGo();

//                if(startSeparate) delete startSeparate; startSeparate = nullptr;
//                startSeparate = new TCPStartSeparate2(model, getTimeOutWidget(), getLogModel());



//                startSeparate->send(model->getServer());
//                flagSeparateGo = true;
            }
//            separateTimer.start();
        } else {
            flagSeparateGo = false;
        }
    }
}

void SPRSeparateControlWidget::onModelChanget(IModelVariable *)
{

}

SPRSpectrumListItemsModel *SPRSeparateControlWidget::getKSpectrumModel() const
{
    return kSpectrumModel;
}

void SPRSeparateControlWidget::setKSpectrumModel(SPRSpectrumListItemsModel *value)
{
    kSpectrumModel = value;
}

TCPTimeOutWidget *SPRSeparateControlWidget::getTimeOutWidget() const
{
    return toWidget;
}

void SPRSeparateControlWidget::widgetsShow()
{
}

ISPRModelData *SPRSeparateControlWidget::getModelData()
{
    return model;
}
