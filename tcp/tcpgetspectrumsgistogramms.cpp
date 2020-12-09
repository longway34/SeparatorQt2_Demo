#include "tcp/tcpgetspectrumsgistogramms.h"
#include "tcp/tcpexpositiononoff.h"
#include "tcp/tcpcommandrguupdown.h"

void TCPGetSpectrumsGistogramms::setThreadTimer(double _time_spk_in_sec, SPRThreadList _wtList)
{
    if(model){
        timeOfSpectorScope_in_sec = _time_spk_in_sec;
        if(workingThreads.size() > 0){
            workingThreads.clear();
        }
        workingThreads.append(SPRThreadList(_wtList));
    }
}

EnumCommands TCPGetSpectrumsGistogramms::getDataType() const
{
    return dataType;
}

//TCPLogsWigtets *TCPGetSpectrumsGistogramms::getLogWidget() const
//{
//    return logWidget;
//}

//void TCPGetSpectrumsGistogramms::setLogWidget(TCPLogsWigtets *value)
//{
//    logWidget = value;
//}

int TCPGetSpectrumsGistogramms::getWithRGU()
{
    return withRGU;
}

void TCPGetSpectrumsGistogramms::setWithRGU(int value, bool _toUpDown)
{
    withRGU = value;
    toUpDown = _toUpDown;
}

bool TCPGetSpectrumsGistogramms::getWithOffExp() const
{
    return withOffExp;
}

void TCPGetSpectrumsGistogramms::setWithOffExp(bool value)
{
    withOffExp = value;
}

TCPGetSpectrumsGistogramms::TCPGetSpectrumsGistogramms()
{
}

TCPGetSpectrumsGistogramms::TCPGetSpectrumsGistogramms(ServerConnect *_server, EnumCommands _dataType, SPRMainModel *_model, double _timeSpk_in_sec, SPRThreadList _threads, TCPTimeOutWidget *_toWidget, TCPEchoLogsWidget *_logWidgets):
    TCPCommandSet(_server, _toWidget, {}), expositionOn(nullptr),
    model(_model), dataType(_dataType),
    timeOfSpectorScope_in_sec(_timeSpk_in_sec)

{
    if(!getTimeOutWidget()){
        setTimeOutWidget(nullptr, selfTOWidget);
    }
    workingThreads = _threads;
    withOffExp = false;
    withRGU = false;
}

QByteArray TCPGetSpectrumsGistogramms::getKSpectrumData(int thread){
    QList<TCPCommand*>vspect = findCommands(getkspk);
    QByteArray res0, res;
    if(vspect.size() > 0){
        if(thread < 0 || thread >= vspect.size())
            thread = vspect.size()-1;

        res0 = vspect[thread]->getReplayData();
        //            res = res0.right(res0.size() - (5));
        res = res0.right(DEF_SPECTRUM_DATA_LENGTH_BYTE);
    }
    return res;
}

uint32_t TCPGetSpectrumsGistogramms::getKSpectrumTime(int thread){
    QList<TCPCommand*>vspect = findCommands(getkspk);
    QByteArray res0; uint32_t res = 0;
    if(vspect.size() > 0){
        if(thread < 0 || thread >= vspect.size())
            thread = vspect.size()-1;

        res0 = vspect[thread]->getReplayData();
//        res0 = res0.left(5);
//        res0 = res0.right(4);
        memcpy(&res, res0.constData()+1, sizeof(res));
        //            res = res0.toInt();
    }
    return res;
}

QByteArray TCPGetSpectrumsGistogramms::getSpectrumData(int thread) {
    QList<TCPCommand* >vspect = findCommands(getspk);
    QByteArray res;
    if(vspect.size() > 0){
        if(thread < 0 || thread >= vspect.size())
            thread = vspect.size()-1;

        res = vspect[thread]->getReplayData().right(DEF_SPECTRUM_DATA_LENGTH_BYTE);
    }
    return res;
}


void TCPGetSpectrumsGistogramms::go(TCPCommand *_command)
{
    if(!_command){
        clear();

        if(dataType == getspk){
            char ren = '\0';
            if((!server->isState(spr_state_rentgen_on)) || server->isState(spr_state_rentgen_not_regime) || (!server->isState(spr_state_separator_on))){
                addCommand(new TCPCommandSeparatorOnFull(server, model, getTimeOutWidget()));
//                findCommands().last()->setObjectName("TCPCommandSeparatorOnFull");
            }
            if(withRGU){
                if(toUpDown){
                    addCommand(new TCPCommandRGUUpDown(server, getTimeOutWidget(), true));
//                    findCommands().last()->setObjectName("TCPCommandRGUUpDown");
                } else {
                    addCommand(new TCPCommandRGUUpDown(server, getTimeOutWidget(), false));
//                    findCommands().last()->setObjectName("TCPCommandRGUUpDown");
                }
            }
            if(!server->isState(spr_state_exposition_on)){
//                if(server->isState(spr_state_rentgen_not_regime)){
//                    addCommand(new TCPExpositionOnOff(server, false, true, model, getTimeOutWidget(), getLogWidget()));
//                }
                expositionOn = new TCPExpositionOnOff(server, true, true, model, getTimeOutWidget(), getLogWidget());
                addCommand(expositionOn);
//                findCommands().last()->setObjectName("TCPExpositionOnOff");

            }
            int32_t spkT = timeOfSpectorScope_in_sec * 10;
            addCommand(new TCPCommand(setspk))->addSendData(&spkT, sizeof(spkT), setspk);
//            findCommands().last()->setObjectName("setspk");
            addCommand(new TCPTimeOutCommand(timeoutcommand, (int)(timeOfSpectorScope_in_sec * 1000 + 2000), 5, getTimeOutWidget(),
                                MSG_TIME_OUT_SET_SPK, MSG_TIME_OUT_SET_SPK_MSG(timeOfSpectorScope_in_sec)));
//                                QString(tr("Накопление спектра...")),
//                                QString(tr("Накопление спектра за %1 сек...")).arg(timeOfSpectorScope_in_msec)));
//            findCommands().last()->setObjectName("TCPTimeOutCommand");
        }

        uint8_t threads = MAX_SPR_MAIN_THREADS;
        if(model){
            threads = model->getSettingsMainModel()->getThreadsVariable()->getData();
        }
        if(workingThreads.size() > 0){
            threads = workingThreads.size();
        }

        if(dataType == getspk || dataType == getkspk || dataType == getgist){
            for(uint8_t i=0; i<threads; i++){
               addCommand(dataType)->addSendData(&i, sizeof(i), dataType);
//               findCommands(dataType).last()->setSendData(&i, sizeof(i));
            }
        } else {
            addCommand(dataType);
        }
        if(dataType == getspk){
            if(withOffExp){
                TCPExpositionOnOff *exponOff = new TCPExpositionOnOff(server, false, true, model, getTimeOutWidget(), getLogWidget());
                addCommand(exponOff);
//                connect(exponOff, SIGNAL(commandNotComplite(TCPCommand*)), this, SLOT(onCommandNotComplite(TCPCommand*)));
            }
        }

//        addCommand(new TCPCommand(nocommand));
    } else {
        if(_command == expositionOn){
            uint16_t mkv, mka; uint8_t err;
            QList<TCPCommand*> lgetren = _command->findCommands(getren);
            if(lgetren.size() > 0){
                bool res = lgetren.last()->isRentgenReady(lgetren.last()->getReplayData(), &mkv, &mka, &err);
                if(!res) {
                    TCPExpositionOnOff *off = new TCPExpositionOnOff(server, false, true, model, getTimeOutWidget(), getLogWidget());
                    addCommand(off);
                    off->send(server);

                    emit rentgenNotReady(lgetren.last());
                    emit commandNotComplite(lgetren.last());
                    return;
                }
            }
        }
    }
    TCPCommandSet::go(_command);
}

void TCPGetSpectrumsGistogramms::onCommandNotComplite(TCPCommand *_command)
{
    if(_command->getCommand() == getren){
        uint16_t mka, mkv; uint8_t err;
        isRentgenReady(_command->getReplayData(), &mkv, &mka, &err);
        qDebug() << "************************************";
        qDebug() << "command getRen not complite..." << QString::number(mkv, 16) << " " << QString::number(mka, 16);
        setProcessRun(false);
        emit rentgenNotReady(_command);
        emit commandNotComplite(_command);
        return;
    }
    emit commandNotComplite(_command);
    qDebug() << "************************************";
    qDebug() << "ather command not complite..." << _command->toDebug();
    return;
}


bool TCPGetSpectrumsGistogramms::isCommamdCompare(TCPCommand *_command)
{
    if(_command == expositionOn){
        if(!_command->noErrors()){
//            setProcessRun(false);
            return false;
        }
    }
    return TCPCommandSet::isCommamdCompare(_command);
}
