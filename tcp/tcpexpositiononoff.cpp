#include "tcpexpositiononoff.h"
#include "models/sprseparatorlogsmodel.h"
#include <QMessageBox>

SPRMainModel *TCPExpositionOnOff::getModelData() const
{
    return model;
}

void TCPExpositionOnOff::setModelData(SPRMainModel *value)
{
    model = value;
    if(model){
        setLogModel(model->getLogModel());
    }
}

TCPExpositionOnOff::TCPExpositionOnOff(ServerConnect *_server, bool _onOff, bool _offIfError, SPRMainModel *_model, TCPTimeOutWidget *_toWidget, TCPEchoLogsWidget *_logWidget):
    TCPCommandSet(_server, _toWidget, {}), model(_model), onOff(_onOff), offIfError(_offIfError)
{
    setLogWidget(_logWidget);
}

const QByteArray TCPExpositionOnOff::getDeuCodes(bool deu_cp){
    QByteArray res;
    if(model){
        SPRSettingsRentgenModel *rentgen = model->getSettingsRentgenModel();
        if(rentgen){
            for(int th=0;th<MAX_SPR_MAIN_THREADS;th++){
                if(deu_cp){
                    uint16_t c = rentgen->deuCodes[th]->getData();
                    res.append((char*)&c, sizeof(c));
                } else {
                    uint16_t c = rentgen->cpCodes[th]->getData();
                    res.append((char*)&c, sizeof(c));
                }
            }
        }
    }
    return res;
}



void TCPExpositionOnOff::go(TCPCommand *_command)
{
    if(!_command){
        qDebug() << "TCPExpositionOnOff starting...";
        clear();
            addCommand(getstate);
            char ren = 0;
            if(onOff){
                addCommand(getblk);
                if(!server->isState(spr_state_codes_is_stated)){
                    QByteArray codes = getDeuCodes(true);
                    addCommand(setudeu)->setSendData(codes);
                    addCommand(new TCPTimeOutCommand(timeoutcommand, 1000, 5, getTimeOutWidget(),
                                                     MSG_TIME_OUT_SET_DEU, MSG_TIME_OUT_SET_DEU_MSG( 1 )));
                    codes = getDeuCodes(false);
                    addCommand(setptdeu)->setSendData(codes);
                    addCommand(new TCPTimeOutCommand(timeoutcommand, 1000, 5, getTimeOutWidget(),
                                                     MSG_TIME_OUT_SET_CP, MSG_TIME_OUT_SET_CP_MSG( 1 )));
                }
                if(!server->isState(spr_state_exposition_on) || !server->isState(spr_state_rentgen_on_correct)){
                    addCommand(expon)->addSendData(&ren, sizeof(ren), expon);
//                    findCommands().last()->setObjectName("expon");
                    addCommand(onosw)->addCommand(offosw);

                    uint32_t toExp = DEF_SPR_RENTGEN_TIME_HOT_RA;
                    if(model){
                        toExp = model->getSettingsRentgenModel()->timeHotRA->getData();
                    }
                    addCommand(new TCPTimeOutCommand(timeoutcommand, toExp * 1000 + 1000, 5, getTimeOutWidget(),
                                                     QString(tr("Включение рентгена")), QString(tr("Включение экспозиции (%1 сек.)...")).arg(toExp)));

                    addCommand(getren)->addSendData(&ren, sizeof(ren), getren);

                }
            } else {
                addCommand(expoff)->addSendData(&ren, sizeof(ren), expoff);
//                addCommand(offren)->addSendData(&ren, sizeof(ren), offren);
                addCommand(onosw)->addCommand(offosw)->addCommand(onosw)->addCommand(offosw);
            }

    } else {
        if(_command->getCommand() == getblk){
            bool ok = true;
            int res = _command->getResult().left(1).toInt(&ok);
            if(!ok || res != 0xb1){
                bool ignoreDoors = model->getSettingsMainModel()->ignoreDoors->getData();
                if(model->getLogModel()){
                    QString msg = MSG_BLK_NOT_CLOSED;
                    SPRTypeLogMessage type = ignoreDoors ? tWarning : tError;
                    if(ignoreDoors){
                        msg += tr(" Игнориуем...");
                    } else {
                        msg += MSG_BLK_NOT_CLOSED_MSG;
                    }
                    model->getLogModel()->addLogMessage(_command, type, msg);
                }
                if(!ignoreDoors){
                    QMessageBox::critical(nullptr, MSG_BLK_NOT_CLOSED, MSG_BLK_NOT_CLOSED_MSG);
                    emit commandNotComplite(_command);
                    return;
                }
            }
        }
        if(_command->getCommand() == getren){
            uint16_t mka, mkv; uint8_t err;
            char ren = 0;
            bool ignoreRAErrors = model->getSettingsMainModel()->ignoreRentgenErrors->getData();
            if(!isRentgenReady(_command->getReplayData(), &mka, &mkv, &err)){
                QString errMsg = MSG_TCPSET_RENTGEN_NOT_READY_MSG( mka, mkv );
                if(ignoreRAErrors){
                    errMsg += tr(" Игнорируем...");
                } else {
                    errMsg += tr(" Операция будет прервана...");
                }
                if(model->getLogModel()){
                    model->getLogModel()->addLogMessage(nullptr, tError, errMsg);
                }
                if(!ignoreRAErrors){
                    if(visible){
                        errMsg += tr("\nРекомендуем выключить и снова включить сортировочную машину.");
                        QMessageBox::critical(nullptr, MSG_TCPSET_RENTGEN_NOT_READY, errMsg);
                    }
                    addCommand(expoff)->addSendData(&ren, sizeof(ren), expoff);
                    addCommand(offosw)->addCommand(onosw)->addCommand(offosw);
                    emit commandNotComplite(_command);
                    TCPCommand *off = findCommands(expoff).last();
                    off->send(server);
                    return;
                }
            }
        }
    }
    EnumCommands _name = nocommand;
    if(_command)
        _name = _command->getCommand();
    TCPCommandSet::go(_command);
}
