#include "tcpstartseparate2.h"
#include "tcp/tcpexpositiononoff.h"
#include "tcp/tcpcommandstartpitatel.h"
#include <QMessageBox>
#include <QAbstractButton>

#include "models/sprseparatorlogsmodel.h"

TCPGetSpectrums2 *TCPStartSeparate2::getGetBaseSpectrum() const
{
    return getBaseSpectrum;
}

TCPSeparateGo2 *TCPStartSeparate2::getSeparateGo() const
{
    return separateGo;
}

bool TCPStartSeparate2::isStartStopProcess()
{
    bool res = false;
    if(getBaseSpectrum){
       res = getBaseSpectrum->isProcessRun();
    }
    if(separateGo){
        res = res || separateGo->isProcessRun();
    }
    return res;
}

TCPStartSeparate2::TCPStartSeparate2(SPRMainModel *_model, TCPTimeOutWidget *_toWidget, SPRSeparatorLogsModel *_logModel):
    TCPCommandSet(nullptr, _toWidget, {}), model(_model),
     getBaseSpectrum(nullptr), separateGo(nullptr), separatorStop(nullptr), errorSeparateState(SPR_SEPARATE_STATE_OK)
{
    setLogModel(_logModel);
}

TCPGetSpectrumsGistogramms *TCPStartSeparate2::getKSpectrunsCommand(){
    if(separateGo){
        return separateGo->getKspectCommand();
    }
    return nullptr;
}

TCPGetSpectrumsGistogramms *TCPStartSeparate2::getHistogrammCommand(){
    if(separateGo){
        return separateGo->getHistCommand();
    }
    return nullptr;
}

void TCPStartSeparate2::go(TCPCommand *_command){
    if(model){
        if(!_command){
            clear();
            addCommand(getstate);
            if(getBaseSpectrum) delete getBaseSpectrum;
            getBaseSpectrum = nullptr;



            if(!server->isState(spr_state_rasklad_on) || !server->isState(spr_state_rasklad_on)){

                addCommand(oniw);
                addCommand(new TCPTimeOutCommand(timeoutcommand, 2000, 100, getTimeOutWidget(),
                                MSG_TIME_OUT_ON_RASKLAD, MSG_TIME_OUT_ON_RASKLAD_MSG( 2 )));

                TCPCommandStartPitatel *startPitatel = new TCPCommandStartPitatel(model->getServer(), model, getTimeOutWidget());
                addCommand(startPitatel);

            }
            if(!server->isState(spr_state_separated)){

                QByteArray separData = model->getSeparateModel()->toByteArray(&errorSeparateState);
                addCommand(new TCPCommand(setsepar))->addSendData(separData, setsepar);

                addCommand(new TCPCommand(startsep));
            }


            if(separateGo) delete separateGo;
            separateGo = nullptr;

            separateGo = new TCPSeparateGo2(model);
            connect(separateGo, &TCPSeparateGo2::gistogrammsDataReady, this, &TCPStartSeparate2::onGistorgammsDataReady);
            connect(separateGo, &TCPSeparateGo2::kspectrumsDataReady, this, &TCPStartSeparate2::onKSpectrumsDataReady);
            connect(separateGo, &TCPSeparateGo2::separateDataReady, this, &TCPStartSeparate2::onSeparateDataReady);
            addCommand(separateGo);

            if(separatorStop) delete separatorStop;
            separatorStop = nullptr;

        /*} else {
            if(_command->getCommand() == getstate){
                if(!server->isState(spr_state_separated)){
                    getBaseSpectrum = new TCPGetSpectrums2(model, 5, UpRGU, model->getThreadsList(), false, getTimeOutWidget());

                    getBaseSpectrum->setLogModel(getLogModel());
                    getBaseSpectrum->setVisibleTOWidgets(visible);
//                    getBaseSpectrum->setThreadTimer(1, model->getThreadsList());
//                    getBaseSpectrum->setWithOffExp(false);
//                    getBaseSpectrum->setWithRGU(false, true);

                    connect(getBaseSpectrum, &TCPStartSeparate2::commandComplite, this, &TCPStartSeparate2::onGetRenComplite);
                    connect(getBaseSpectrum, &TCPStartSeparate2::commandNotComplite, this, &TCPStartSeparate2::onGetRenNotComplite);
                    addCommand(getBaseSpectrum);
                } else {
                    getBaseSpectrum = nullptr;
                    separateGo = new TCPSeparateGo2(model, getLogWidget());
//                    separateGo->setVisibleTOWidgets(visible);

                    addCommand(separateGo);

                }
            } else if(sender() == getBaseSpectrum){
                QByteArray separData = model->getSeparateModel()->toByteArray(&errorSeparateState);
                if(!errorSeparateState){
                    QList<uint8_t> errCorrelThs;
                    foreach(uint8_t th, model->getThreadsList()){
                        QByteArray res = getBaseSpectrum->getSpectrumData(th);
                        SPRSpectrumItemModel *item = new SPRSpectrumItemModel(model->getSpectrumZonesTableModel(), model->getSettingsFormulaModel(), th);
                        if(item){
                            item->setSpectrumData((uint8_t*)res.constData(), res.size());
                            item->setTimeScope(1000);
                            QList<SPRSpectrumItemModel*> lbases = model->getSpectrumListItemsModel()->getSpectrumsItemByThread(th, spectrumBase);
                            if(lbases.size() > 0){
                                double corr = item->getCorrel(lbases.first());
                                if(corr < model->getSettingsControlModel()->correlSpectrumCritical->getData()){
                                    errCorrelThs.append(th);
                                }
                            }
                        }
                    }
                    if(errCorrelThs.size() > 0){
                        QString sth;
                        if(errCorrelThs.size() == 1){
                            sth = QString(tr("для ручья %1")).arg(errCorrelThs.first());
                        } else {
                            sth = QString(tr("для ручьев "));
                            foreach(uint8_t eth, errCorrelThs){
                                sth += eth == errCorrelThs.last() ? QString("%1").arg(eth) : QString("%1, ").arg(eth);
                            }
                        }
                        QString msg = QString(tr("Обнаружено критикеское отклонение корреляции спектров %1\n"
                                                 "Рекомендуем переснять базовые спектры\n"
                                                 "Продолжить работу?")).arg(sth);
                        QMessageBox::StandardButton res = QMessageBox::question(nullptr, tr("Ошибка корреляции спектров"), msg);
                        if(res == QMessageBox::No){
                            return;
                        }
                        if(res == QMessageBox::Yes){
                            if(getLogModel()){
                                getLogModel()->addLogMessage(nullptr, tMessage, tr("Внимание! Продолжение работы с кртическим отклонением параметров спекртов..."));
                            }
                        }
                    }

                    addCommand(new TCPCommand(setsepar))->addSendData(separData, setsepar);

                    addCommand(oniw);
                    addCommand(new TCPTimeOutCommand(timeoutcommand, 2000, 100, getTimeOutWidget(),
                                    MSG_TIME_OUT_ON_RASKLAD, MSG_TIME_OUT_ON_RASKLAD_MSG( 2 )));

                    TCPCommandStartPitatel *startPitatel = new TCPCommandStartPitatel(model->getServer(), model, getTimeOutWidget());
//                    startPitatel->setModelVariable(model->getSettingsControlModel()->getVEMSBeginCodeVariable());
                    addCommand(startPitatel);
                    addCommand(new TCPCommand(startsep));

                    separateGo = new TCPSeparateGo2(model, getLogWidget());

                    connect(separateGo, &TCPSeparateGo2::gistogrammsDataReady, this, &TCPStartSeparate2::onGistorgammsDataReady);
                    connect(separateGo, &TCPSeparateGo2::kspectrumsDataReady, this, &TCPStartSeparate2::onKSpectrumsDataReady);
                    connect(separateGo, &TCPSeparateGo2::separateDataReady, this, &TCPStartSeparate2::onSeparateDataReady);
                    addCommand(separateGo);
//                    separateGo->setVisibleTOWidgets(visible);
                }

            } else if(sender() == separateGo){
//                separatorStop = new TCPTestStopSeparate(getTimeOutWidget(), model);
//                addCommand(separatorStop);
            }*/
        }
        TCPCommandSet::go(_command);
    }
}


void TCPStartSeparate2::onSeparateDataReady(TCPCommand *_command)
{
    emit separateDataReady(_command);
}
void TCPStartSeparate2::onBaseSpectrumsReady(TCPCommand *_command)
{
    emit commandComplite(_command);
}

void TCPStartSeparate2::onGistorgammsDataReady(TCPCommand *_command)
{
    emit gistogrammReady(_command);
}
void TCPStartSeparate2::onKSpectrumsDataReady(TCPCommand *_command)
{
    emit kSpactrumReady(_command);
}
void TCPStartSeparate2::onGetRenComplite(TCPCommand *_command)
{
    emit commandComplite(_command);
}
void TCPStartSeparate2::onGetRenNotComplite(TCPCommand *_command)
{
    emit commandNotComplite(_command);
}

//void TCPStartSeparate2::onGetRenNotComplite(TCPCommand *_command)
//{
//    emit rentgenNotReady(_command);
//}
