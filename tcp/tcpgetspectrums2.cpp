#include "tcpgetspectrums2.h"

QByteArray TCPGetSpectrums2::getCodesDEU() const
{
    return codesDEU;
}

void TCPGetSpectrums2::setCodesDEU(const QByteArray &value)
{
    codesDEU = value;
}

QByteArray TCPGetSpectrums2::getCodesCP() const
{
    return codesCP;
}

void TCPGetSpectrums2::setCodesCP(const QByteArray &value)
{
    codesCP = value;
}

void TCPGetSpectrums2::setOnExposition(bool value)
{
    onExposition = value;
}

TCPGetSpectrums2::TCPGetSpectrums2(SPRMainModel *_model, double _timeScope_in_sec, SPRTypeRGUMoved _moved, SPRThreadList ths, bool _offExposition, TCPTimeOutWidget *_toWidget):
    TCPCommandSet(nullptr, _toWidget, {})
{
    model = _model;
    rguMoved = _moved;
    threads = ths;
    offExposition = _offExposition;
    timeScope_in_sec = _timeScope_in_sec;
    onExposition = true;

    codesDEU = QByteArray(); codesCP = QByteArray();

    if(!getTimeOutWidget()){
        setTimeOutWidget(nullptr, selfTOWidget);
    }

}

QByteArray TCPGetSpectrums2::getSpectrumData(int thread) {
    QList<TCPCommand* >vspect = findCommands(getspk);
    QByteArray res;
    if(vspect.size() > 0){
        if(thread < 0 || thread >= vspect.size())
            thread = vspect.size()-1;

        res = vspect[thread]->getReplayData().right(DEF_SPECTRUM_DATA_LENGTH_BYTE);
    }
    return res;
}


void TCPGetSpectrums2::go(TCPCommand *_command)
{
    if(model){
        if(!_command){
        clear();
qDebug() << "TCPGetSpectrums2 starting";
            addCommand(getstate);
            uint8_t st = server->getLastStateCommand();

            if(!server->isState(spr_state_rentgen_on) || !server->isState(spr_state_separator_on) || st != REMOTE_WAIT_C){
                if(onExposition){
                    separatorOn = new TCPCommandSeparatorOnFull(server, model, getTimeOutWidget(), true);
                } else {
                    separatorOn = new TCPCommandSeparatorOnFull(server, model, getTimeOutWidget(), false);
                }
                addCommand(separatorOn);
            }
            if(rguMoved != noRGU){
                rguMovedCommand = new TCPCommandRGUUpDown2(model, getTimeOutWidget(), rguMoved);
                rguMovedCommand->setLogModel(getLogModel());
                addCommand(rguMovedCommand);
            }
            if(!server->isState(spr_state_rentgen_on_correct) ||
                    !server->isState(spr_state_exposition_on) ||
                    !server->isState(spr_state_codes_is_stated)){

                if(onExposition){
                    expositionOn = new TCPExpositionOnOff(server, true, true, model, getTimeOutWidget(), getLogWidget());
                    addCommand(expositionOn);
                }
            }
            if(codesDEU.size() == MAX_SPR_MAIN_THREADS){
                addCommand(new TCPTimeOutCommand(setudeu, 2000, 10, getTimeOutWidget(),
                           MSG_TIME_OUT_SET_DEU, MSG_TIME_OUT_SET_DEU_MSG( 2 ), selfTOWidget))
                        ->addSendData(codesDEU);



//                addCommand(setudeu)->addSendData(codesDEU);
            }
            if(codesCP.size() == MAX_SPR_MAIN_THREADS){
                addCommand(new TCPTimeOutCommand(setptdeu, 2000, 10, getTimeOutWidget(),
                           MSG_TIME_OUT_SET_CP, MSG_TIME_OUT_SET_CP_MSG( 2 ), selfTOWidget))
                        ->setSendData(codesCP);
//                addCommand(setptdeu)->addSendData(codesCP);
            }
            uint32_t tSpk = timeScope_in_sec * 10;
            addCommand(new TCPTimeOutCommand(setspk, timeScope_in_sec * 1000 + 1000, 100, getTimeOutWidget(),
                                             MSG_TIME_OUT_SET_SPK , MSG_TIME_OUT_SET_SPK_MSG( timeScope_in_sec )))
                    ->addSendData(&tSpk, sizeof(tSpk));

            foreach(uint8_t th, threads){
                addCommand(getspk)->addSendData(&th, 1);
            }

            if(offExposition){
                TCPExpositionOnOff *expOffCommand = new TCPExpositionOnOff(server, false, true, model, getTimeOutWidget(), getLogWidget());
                addCommand(expOffCommand);
            }
        } else if(_command->getCommand() == getstate) {
        }
        TCPCommandSet::go(_command);
    }
}


bool TCPGetSpectrums2::getOffExposition() const
{
    return offExposition;
}

void TCPGetSpectrums2::setOffExposition(bool value)
{
    offExposition = value;
}

SPRTypeRGUMoved TCPGetSpectrums2::getRguMoved() const
{
    return rguMoved;
}

void TCPGetSpectrums2::setRguMoved(const SPRTypeRGUMoved &value)
{
    rguMoved = value;
}

SPRMainModel *TCPGetSpectrums2::getModel() const
{
    return model;
}

void TCPGetSpectrums2::setModel(SPRMainModel *value)
{
    model = value;
}


SPRThreadList TCPGetSpectrums2::getThreads() const
{
    return threads;
}

void TCPGetSpectrums2::setThreads(const SPRThreadList &value)
{
    threads = value;
}
