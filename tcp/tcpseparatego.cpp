#include "tcp/tcpseparatego.h"
#include "tcp/tcpgetspectrumsgistogramms.h"


SPRMainModel *TCPSeparateGo::getModel() const
{
    return model;
}

void TCPSeparateGo::setModel(SPRMainModel *value)
{
    model = value;

//    kspectCommand->setModelData(value);
//    histCommand->setModelData(value);
//    QVector<TCPCommand*> vcomm= findCommands(setGetSpectrumsGistorfamms);
//    for(int i=0; i<vcomm.size();i++){
//        ((TCPGetSpectrumsGistogramms*)vcomm[i])->setThreadTimer(model->getSettingsMainModel()->getThreads()->getData());
//    }
}

void TCPSeparateGo::setLogWidget(TCPEchoLogsWidget *value)
{
    logWidget = value;
    if(getseparCommand) getseparCommand->setLogWidget(value);
    if(kspectCommand) kspectCommand->setLogWidget(value);
    if(histCommand) histCommand->setLogWidget(value);
}

TCPGetSpectrumsGistogramms *TCPSeparateGo::getKspectCommand() const
{
    return kspectCommand;
}

TCPGetSpectrumsGistogramms *TCPSeparateGo::getHistCommand() const
{
    return histCommand;
}

TCPCommand *TCPSeparateGo::getGetseparCommand() const
{
    return getseparCommand;
}

TCPSeparateGo::TCPSeparateGo()
{

}

TCPSeparateGo::TCPSeparateGo(SPRMainModel *_model, TCPEchoLogsWidget *log):
    TCPCommandSet(nullptr, nullptr, {}), model(_model), logWidget(log),
    kspectCommand(nullptr),
    histCommand(nullptr),
    getseparCommand(nullptr)
{

    command = setSeparateGo;
    setTimeOut(1000);

}

void TCPSeparateGo::go(TCPCommand *_command)
{
    if(!_command){
        clear();
        if(model){


            addCommand(new TCPTimeOutCommand(getstate, timeout, 1));


            getseparCommand = new TCPCommand(getsepar);
            getseparCommand->setLogWidget(getLogWidget());
            addCommand(getseparCommand);

            QList<uint8_t> lth = model->getThreadsList();

            kspectCommand = new TCPGetSpectrumsGistogramms(nullptr, getkspk, nullptr, 1, lth, nullptr, getLogWidget());
            addCommand(kspectCommand);

            histCommand = new TCPGetSpectrumsGistogramms(nullptr, getgist, nullptr, 1, lth, nullptr, getLogWidget());
            addCommand(histCommand);

            setLogWidget(getLogWidget());

            toCount = 0;
//            return;
        }
    } else {
        if(_command->getCommand() == getstate){
            QByteArray res = _command->getReplayData();
            memcpy(&stateResult, res.constData(), sizeof(stateResult));
            uint tick;
            if(stateResult.error == 0 && stateResult.state == 0x02){
                if(model){
                    tick = model->getSettingsControlModel()->tMeassureForData->getData();
                } else {
                    tick = DEF_SPR_CONTROL_T_MEASSURE_FOR_DATA;
                }
                if(toCount % tick == 0){
                     getseparCommand->send(server);
                }

                if(model){
                    tick = model->getSettingsControlModel()->tMeassureForSpectrum->getData();
                } else {
                    tick = DEF_SPR_CONTROL_T_MEASSURE_FOR_SPECTRUM;
                }
                if(toCount % tick == 0){
                    kspectCommand->send(server);
                }

                if(model){
                    tick = model->getSettingsControlModel()->tMeassureForHistogramm->getData();
                } else {
                    tick = DEF_SPR_CONTROL_T_MEASSURE_FOR_HISTOGRAMM;
                }
                if(toCount % tick == 0){
                    histCommand->send(server);
                }

                toCount++;

                commandSet.first()->send(server);
                return;
            } else {
                if(logWidget){
                    logWidget->addLogsMessage(tr("Сортировка завершена..."));
                }
//                server->timerStart();
                emit commandComplite(this);
            }
        }
        if(_command == getseparCommand){
            if(logWidget){
               logWidget->addLogsMessage(tr("separate data ready..."));
            }
            emit separateDataReady(_command);
            commandSet.first()->send(server);
            return;
        }
        if(_command == kspectCommand){
            if(logWidget){
               logWidget->addLogsMessage(tr("kspectrums data ready..."));
            }
            emit kspectrumsDataReady(_command);
            commandSet.first()->send(server);
            return;
        }
        if(_command == histCommand){
            if(logWidget){
               logWidget->addLogsMessage(tr("historgamms data ready..."));
            }
            emit gistogrammsDataReady(_command);
            commandSet.first()->send(server);
            return;
        }
    }
    TCPCommandSet::go(_command);
}

