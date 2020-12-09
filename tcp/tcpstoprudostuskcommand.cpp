#include "tcpstoprudostuskcommand.h"

TCPStopRudostuskCommand::TCPStopRudostuskCommand()
{

}

TCPStopRudostuskCommand::TCPStopRudostuskCommand(ServerConnect *_server, TCPTimeOutWidget *_widget)
    :TCPCommandSet(_server, _widget, {})
{
    command = setStopRudospuk;
    addCommand(new TCPCommandStopPitatel(_server, _widget));
    addCommand(new TCPCommand(offiw));
}

void TCPStopRudostuskCommand::go(TCPCommand *_command)
{
    if(!_command){
        clear();
        addCommand(new TCPCommandStopPitatel(server, getTimeOutWidget()));
        addCommand(new TCPCommand(offiw));
    }
    TCPCommandSet::go(_command);
}
