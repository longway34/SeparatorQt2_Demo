#include "tcpstartrudospuskcommand.h"
#include "tcp/tcpcommandrguupdown2.h"

TCPStartRudospuskCommand::TCPStartRudospuskCommand()
{
}

TCPStartRudospuskCommand::TCPStartRudospuskCommand(ServerConnect *_server, SPRMainModel *_model, TCPTimeOutWidget *_widget)
    :TCPCommandSet(_server, _widget, {}), model(_model)
{
}

void TCPStartRudospuskCommand::go(TCPCommand *_command)
{
    if(!_command){
        clear();

        addCommand(new TCPCommandRGUUpDown2(model, nullptr, UpRGU));

        addCommand(new TCPTimeOutCommandWC(oniw, 10000, 100, getTimeOutWidget(), MSG_TIME_OUT_ON_RASKLAD, MSG_TIME_OUT_ON_RASKLAD_MSG( 10 ), selfTOWidget));
        addCommand(new TCPCommandStartPitatel(server, model, getTimeOutWidget()));

    }
    TCPCommandSet::go(_command);
}
