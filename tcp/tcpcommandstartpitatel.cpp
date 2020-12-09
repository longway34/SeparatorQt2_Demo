#include "tcpcommandstartpitatel.h"
#include "_types.h"

#include <QMessageBox>
#include "tcp/ServerConnect.h"
#include "tcp/tcpechologswidget.h"
#include "tcp/ServerConnect.h"
#include "tcp/tcpcommandrguupdown2.h"

#include "models/sprmainmodel.h"

TCPCommandStartPitatel::TCPCommandStartPitatel()
{
}

TCPCommandStartPitatel::TCPCommandStartPitatel(ServerConnect *_server, SPRMainModel *_model, TCPTimeOutWidget *_widget):
    TCPCommandSet(_server, _widget, {}), model(_model), VEMSBeginCode(nullptr)
{
    command = setStartPitatel;

    if(model){
        VEMSBeginCode = model->getSettingsControlModel()->getVEMSBeginCodeVariable();
    }
//    addCommand(new TCPCommand(getrgu2))->addCommand(new TCPCommand(setpuw))->addCommand(new TCPCommand(startpuw));
//    percents = 44;
}

//void TCPCommandStartPitatel::setPercents(uint16_t _persents){
//    if(VEMSBeginCode){
//        VEMSBeginCode->setData(_persents * 20);
//    }
////    percents = _persents;
//}

void TCPCommandStartPitatel::go(TCPCommand *_command)
{
    if(!_command){
       clear();

       addCommand(new TCPCommandRGUUpDown2(model, getTimeOutWidget(), UpRGU));
//       uint16_t temt = VEMSBeginCode->getData();

       uint16_t fq = VEMSBeginCode != nullptr ? VEMSBeginCode->getData() : DEF_SPR_CONTROL_VEMS_BEGIN_CODE;
       addCommand(setpuw); setSendData(&fq, sizeof (fq), setpuw);

       if(!server->isState(spr_state_pitatel_on)){
           addCommand(new TCPTimeOutCommand(startpuw, 4000, 100, getTimeOutWidget(),
                           MSG_TIME_OUT_ON_PITATEL, MSG_TIME_OUT_ON_PITAPEL_MSG( 4 )));

       }

    } else {
//        if(_command->getCommand() == getrgu2){
//            QByteArray res = _command->getResult();
//            if(res.at(0) != 1){
//                if(getLogWidget()){
//                    getLogWidget()->onErrorLogsCommand(_command, tr("РГУ не вверхнем положении, команда не выполнена..."));
//                }
//                QMessageBox::information(nullptr, tr("Ошибка"), tr("РГУ не вверхнем положении, команда не выполнена..."));
//                return;
//            }

//        }
    }
    TCPCommandSet::go(_command);
}
