#include "tcpteststartseparate.h"
#include "tcp/tcpcommandseparatoronfull.h"
#include "tcp/tcpcommandseparatoroff.h"
#include "tcp/tcpcommandrentgersetup.h"
#include "tcp/tcpgetspectrumsgistogramms.h"
#include "tcp/tcpseparatego.h"
#include <QMessageBox>

#include "models/sprseparatorlogsmodel.h"

TCPSeparateGo *TCPTestStartSeparate::getSeparateGoCommand() const
{
    return separateGoCommand;
}

TCPGetSpectrumsGistogramms *TCPTestStartSeparate::getGetBaseSpectrumCommand() const
{
    return getBaseSpectrumCommand;
}

TCPTestStartSeparate::TCPTestStartSeparate(ServerConnect *_server, SPRMainModel *_model, TCPTimeOutWidget *_widget, TCPEchoLogsWidget *log):
    TCPCommandSet(server, _widget, {}), separateModel(nullptr), mainModel(_model), separateGoCommand(nullptr), separateStop(nullptr), separatorStop(nullptr), getBaseSpectrumCommand(nullptr)
{
    command = setTestStartSeparate;
    separateModel = mainModel->getSeparateModel();
}

void TCPTestStartSeparate::go(TCPCommand *_command)
{
    if(!_command){
        if(separateModel && mainModel){
            clear();

            addCommand(getstate);

            getBaseSpectrumCommand = new TCPGetSpectrumsGistogramms(mainModel->getServer(), getspk, mainModel, 1, {}, getTimeOutWidget(), getLogWidget());
            QList<uint8_t> lth;
            for(uint8_t th; th<mainModel->getThreads()->getData(); th++) lth << th;
            getBaseSpectrumCommand->setThreadTimer(1, lth);
            getBaseSpectrumCommand->setWithOffExp(false);
            getBaseSpectrumCommand->setWithRGU(false, true);
            addCommand(getBaseSpectrumCommand);

            addCommand(new TCPCommand(setsepar))->addSendData(separateModel->toByteArray(&errorSeparateState));
            addCommand(new TCPCommand(startsep));

            //          addCommand(new TCPCommand(oniw));
            addCommand(new TCPTimeOutCommand(timeoutcommand, 2000, 100, getTimeOutWidget(),
                            MSG_TIME_OUT_ON_SEPAR, MSG_TIME_OUT_ON_SEPAR_MSG( 2 )));

            countTry = 0; numTry = 5;

            separateGoCommand = new TCPSeparateGo(mainModel, getLogWidget());
            addCommand(separateGoCommand);

            if(separateStop) delete separateStop;
            separateStop = new TCPTestStopSeparate(getTimeOutWidget(), mainModel);

            addCommand(separateStop);
//************* past 17.12.2018 **********************
//            separatorStop = new TCPCommandSeparatorOff(mainModel->getServer(), getTimeOutWidget());
//            addCommand(separatorStop);
//************* past 17.12.2018 **********************
        }
    } else {
        if(_command->getCommand() == getren){
            uint16_t kV, mka; uint8_t err;
            if(!isRentgenReady(_command->getReplayData(), &kV, &mka, &err)){
                if(!mainModel->getSettingsMainModel()->ignoreRentgenErrors->getData()){
                    if(separatorStop) delete separatorStop;

                    separatorStop = new TCPCommandSeparatorOff(server, getTimeOutWidget());
                    separatorStop->setVisibleTOWidgets(visible);
                    separatorStop->send(server);
                    if(mainModel->getLogModel()){
                        QString msg = MSG_TCPSET_RENTGEN_NOT_READY_MSG( kV, mka) + tr(" Сортировка будет остановлена...");
                        mainModel->getLogModel()->addLogMessage(nullptr, tError, msg);
                    }
                    emit commandNotComplite(this);
                    return;
                } else {
                    if(mainModel->getLogModel()){
                        QString msg = MSG_TCPSET_RENTGEN_NOT_READY_MSG( kV, mka) + tr(" Игнорировано...");
                        mainModel->getLogModel()->addLogMessage(nullptr, tError, msg);
                    }
                }
            }
        }
//        if(_command == separatorStop){
//            if(countTry < numTry){
//                int res = QMessageBox::question(nullptr, tr("Неудачная попытка..."),
//                            tr("Попытаться еще разок (%1/%2)?").arg(QString::number(countTry)).arg(QString::number(numTry)));
//                if(res == QMessageBox::StandardButton::Yes){
//                    TCPCommand *_next = findCommands(getstate).first();
//                    if(_next){
//                        _next->send(server);
//                    }
//                    return;

//                } else {
//                    emit commandNotComplite(this); // output with error
//                    return;
//                }
//            }
//        }
        if(_command == separateGoCommand){
            emit commandComplite(this);
        }
    }
    TCPCommandSet::go(_command);
}

SPRMainModel *TCPTestStartSeparate::getModelData() const
{
    return mainModel;
}

void TCPTestStartSeparate::setModeDatal(SPRMainModel *value)
{
    if(value){
        mainModel = value;
        separateModel = mainModel->getSeparateModel();
        ((TCPSeparateGo*)findCommands(setSeparateGo).first())->setModel(mainModel);
    }
}

//SPRMainModel *TCPTestStartSeparate::getMainModel() const
//{
//    if(separateModel){
//        return separateModel->getModelData();
//    }
//    return nullptr;
//}


