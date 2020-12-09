#include "tcpteststopseparate.h"
#include "tcp/tcpclearstates.h"
#include "tcp/tcpexpositiononoff.h"


TCPTestStopSeparate::TCPTestStopSeparate():
    TCPCommandSet(nullptr, nullptr, {})
{
}

TCPTestStopSeparate::TCPTestStopSeparate(TCPTimeOutWidget *_widget, SPRMainModel *_model):
    TCPCommandSet(nullptr, _widget, {}), model(nullptr)
{
    setModelData(_model);
}

void TCPTestStopSeparate::setModelData(SPRMainModel *_model)
{
    model = _model;
}


void TCPTestStopSeparate::go(TCPCommand *_command)
{
    if(!_command){
        clear();

        command = setTestStopSeparate;
//        addCommand(new TCPClearStates());
        addCommand(getstate);

//    } else {
//        if(_command->getCommand() == getstate){
//            if(_command->getState() != REMOTE_WAIT_C && _command->getState() != REMOTE_SEP){
//                getLogWidget()->onErrorLogsCommand(_command, MSG_NO_OFF_SEPARATE_MSG);
//qDebug() << "spector scope on. Testins getspectrum for clear err not complite!!!!!!!....";
//                emit commandNotComplite(_command);
//                return;
//            } else {
                addCommand(new TCPCommand(stoppuw));

                if(server->isState(spr_state_pitatel_on)){
                    addCommand(new TCPTimeOutCommand(timeoutcommand, 10*1000, 10, widget,
                                                     MSG_TIME_OUT_OFF_PITATEL,
                                                     MSG_TIME_OUT_OFF_PITAPEL_MSG ( 10 )));
                }
                char ch0 = '\0';
                addCommand(new TCPCommand(offiw));

                if(server->isState(spr_state_rasklad_on)){
                    addCommand(new TCPTimeOutCommand(timeoutcommand, 10*1000, 10, widget,
                                                     MSG_TIME_OUT_OFF_RASKLAD,
                                                     MSG_TIME_OUT_OFF_RASKLAD_MSG( 10 )));
                }
                addCommand(new TCPCommand(stopsep));

                addCommand(new TCPExpositionOnOff(server, false, true, model, getTimeOutWidget(), getLogWidget()));

//                TCPCommand *exp = new TCPCommand(expoff); exp->addSendData(&ch0, sizeof(ch0));
//                addCommand(exp);


//************** past 17.12.2018 ***************************************
//                addCommand(new TCPCommand(offren));

//                int tOffRentgen = DEF_SPR_RENTGEN_TIME_OFF_RA;
//                if(model){
//                    tOffRentgen = model->getSettingsRentgenModel()->timeOffRA->getData();
//                }
//                if(server->isState(spr_state_rentgen_on)){
//                    addCommand(new TCPTimeOutCommand(timeoutcommand, tOffRentgen * 1000, 10, widget,
//                                                    MSG_TIME_OUT_REN_OFF,
//                                                    MSG_TIME_OUT_REN_OFF_MSG( tOffRentgen )));
//                }
//                addCommand(new TCPCommand(offosw));
//                addCommand(new TCPCommand(onosw));
//                addCommand(new TCPCommand(offosw));
//                addCommand(new TCPCommand(onosw));
//                addCommand(new TCPCommand(offosw));
//                addCommand(new TCPCommand(offsep));
//************** past 17.12.2018 ***************************************




                //            }

//        }


    }
    TCPCommandSet::go(_command);
}
