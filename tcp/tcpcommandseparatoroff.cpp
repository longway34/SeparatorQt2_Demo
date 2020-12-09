#include "tcpcommandseparatoroff.h"
#include "tcp/ServerConnect.h"
#include "tcp/tcpclearstates.h"

TCPCommandSeparatorOff::TCPCommandSeparatorOff()
{

}

TCPCommandSeparatorOff::TCPCommandSeparatorOff(ServerConnect *_server, TCPTimeOutWidget *_widget)
    :TCPCommandSet(_server, _widget, {}), stopRudosrusk(nullptr)
{
    command = setSeparatorOn;
}

void TCPCommandSeparatorOff::go(TCPCommand *_command)
{
    if(!_command){
        clear();

        char ren = 0;

        if(this->server && server->isState(spr_state_separated)){
            addCommand(stopsep);
        }

        addCommand(new TCPClearStates());

//        char ch = 0;
//        if(this->server && server->isState(spr_state_spector_scope)){
//            addCommand(getspk)->setSendData(&ch, 1, lastcommand);
//        }
//        if(this->server && server->isState(spr_state_gru_moved)){
//            addCommand(setrgu2)->addSendData(&ch, 1, lastcommand);
//        }
        addCommand(expoff)->addSendData(&ren, sizeof(ren), expoff);
        addCommand(offosw)->addCommand(onosw)->addCommand(offosw)->addCommand(onosw)->
        addCommand(offren);

        if(stopRudosrusk) delete stopRudosrusk;
        stopRudosrusk = new TCPStopRudostuskCommand(server, getTimeOutWidget());
        stopRudosrusk->setVisibleTOWidgets(visible);

        addCommand(stopRudosrusk);
        addCommand(offosw)->
        addCommand(offsep);
   }

   TCPCommandSet::go(_command);
}
