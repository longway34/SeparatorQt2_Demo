#include "tcpclearstates.h"

#include "tcp/ServerConnect.h"

TCPClearStates::TCPClearStates():TCPCommand()
{
}

void TCPClearStates::send(ServerConnect *server)
{
    if(server){
        int state = server->getLastStateCommand();
        if(state == REMOTE_RGU){
            char stop = 0;
            command = setrgu2;
            setSendData(&stop, 1);
        } else if(state == REMOTE_SPK){
            char th = 0;
            command = getspk;
            setSendData(&th, 1);
        } else if(state == REMOTE_TEST){
            command = stoptest;
        } else if(state == REMOTE_TERM){
            command = gettemp;
        } else {
            command = nocommand;
        }
        TCPCommand::send(server);
    }
}

