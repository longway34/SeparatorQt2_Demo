#include "tcpseparatego2.h"

TCPSeparateGo2::TCPSeparateGo2(SPRMainModel *_model):
    TCPCommandSet(nullptr, nullptr, {}),
    kspectCommand(nullptr),
    histCommand(nullptr),
    getseparCommand(nullptr),
    getStateCommand(nullptr),
    stopSeparator(nullptr)

{
    model = _model;
    connect(&selfTimer, SIGNAL(timeout()), this, SLOT(onTimeOut()));
    selfTimer.setInterval(1000);

}



void TCPSeparateGo2::go(TCPCommand *_command)
{
    if(model){
        if(!_command){

            selfTimer.stop();

            if(getStateCommand) delete getStateCommand;
            getStateCommand = new TCPCommand(getstate);
            connect(getStateCommand, &TCPCommand::commandComplite, this, &TCPSeparateGo2::onCommandComplite);

            if(getseparCommand) delete getseparCommand;
            getseparCommand = new TCPCommand(getsepar);
            connect(getseparCommand, &TCPCommand::commandComplite, this, &TCPSeparateGo2::onCommandComplite);
            connect(getseparCommand, &TCPCommand::commandNotComplite, this, &TCPSeparateGo2::commandNotComplite);

            if(kspectCommand) delete kspectCommand;
            kspectCommand = new TCPGetSpectrumsGistogramms(model->getServer(), getkspk, model);
            connect(kspectCommand, &TCPGetSpectrumsGistogramms::commandComplite, this, &TCPSeparateGo2::onCommandComplite);
            connect(kspectCommand, &TCPGetSpectrumsGistogramms::commandNotComplite, this, &TCPSeparateGo2::onCommandNotComplite);

            if(histCommand) delete histCommand;
            histCommand = new TCPGetSpectrumsGistogramms(model->getServer(), getgist, model);
            connect(histCommand, &TCPGetSpectrumsGistogramms::commandComplite, this, &TCPSeparateGo2::onCommandComplite);
            connect(histCommand, &TCPGetSpectrumsGistogramms::commandNotComplite, this, &TCPSeparateGo2::onCommandNotComplite);

//            model->getServer()->timerStop();
            selfTimer.start(1000);
            countTimer = 1;
            getStateCommand->send(model->getServer());
        }
    }
}

void TCPSeparateGo2::onTimeOut()
{
    bool removeSeparate = model->getServer()->isState(spr_state_separated);
    if(!getStateCommand->noErrors()){
        selfTimer.stop();
//        model->getServer()->timerStart();
        emit commandNotComplite(getStateCommand);
        return;
    }
    if(!removeSeparate){
        selfTimer.stop();
        emit commandComplite(this);
//        model->getServer()->timerStart();
        return;
    }

    getStateCommand->send(model->getServer());

    int triks = countTimer % model->getSettingsControlModel()->tMeassureForData->getData();
    if(triks == 0){
        getseparCommand->send(model->getServer());
    }

    triks = countTimer % model->getSettingsControlModel()->tMeassureForSpectrum->getData();
    if(triks == 0){
        kspectCommand->send(model->getServer());
    }

    triks = countTimer % model->getSettingsControlModel()->tMeassureForHistogramm->getData();
    if(triks == 0){
        histCommand->send(model->getServer());
    }

    countTimer++;
}

void TCPSeparateGo2::onCommandComplite(TCPCommand *_command)
{
    if(_command == getStateCommand){
        return;
    }
    if(_command == getseparCommand){
        emit separateDataReady(getseparCommand);
        return;
    }
    if(_command == kspectCommand){
        emit kspectrumsDataReady(kspectCommand);
        return;
    }
    if(_command == histCommand){
        emit gistogrammsDataReady(histCommand);
        return;
    }
    if(_command == stopSeparator){
        emit commandComplite(this);
        return;
    }
}



TCPGetSpectrumsGistogramms *TCPSeparateGo2::getKspectCommand() const
{
    return kspectCommand;
}

TCPGetSpectrumsGistogramms *TCPSeparateGo2::getHistCommand() const
{
    return histCommand;
}

TCPCommand *TCPSeparateGo2::getGetseparCommand() const
{
    return getseparCommand;
}

void TCPSeparateGo2::stopSeparate()
{
    if(model){
        selfTimer.stop();
        if(stopSeparator) delete stopSeparator;

        stopSeparator = new TCPCommandSeparatorOff(nullptr, nullptr);

        connect(stopSeparator, &TCPCommandSeparatorOff::commandComplite, this, &TCPSeparateGo2::onCommandComplite);
        stopSeparator->setVisibleTOWidgets(this->visible);

        stopSeparator->send(model->getServer());
    }
}




bool TCPSeparateGo2::isProcessRun() const
{
    if(stopSeparator){
        return stopSeparator->isProcessRun();
    }
    return false;
}
