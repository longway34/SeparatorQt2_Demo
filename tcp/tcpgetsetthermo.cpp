#include "tcpgetsetthermo.h"



TCPGetSetThermo::sterms TCPGetSetThermo::getTerms() const
{
    return terms;
}

TCPGetSetThermo::TCPGetSetThermo(bool _get_set, uint16_t _min, uint16_t _max):
    TCPCommandSet(nullptr, nullptr, {}),
    getSetCommand(_get_set),
    setTermMax(_max),
    setTermMin(_min)
{
}


void TCPGetSetThermo::go(TCPCommand *_command)
{
    if(!_command){
        if(server){
            if(server->isState(spr_state_therm_set)){
                addCommand(gettemp);
                addCommand(new TCPTimeOutCommand(timeoutcommand, 1000));
            }
        }
        addCommand(settemp)->addSendData(&setTermMin, sizeof (setTermMin))->addSendData(&setTermMax, sizeof (setTermMax));
        addCommand(new TCPTimeOutCommand(timeoutcommand, 1000));
        addCommand(gettemp);
    } else if(_command->getCommand() == gettemp){
        if(_command->noErrors()){
            QByteArray res = _command->getReplayData();
            if(res.size() >= sizeof (sterms)){
                memcpy(&terms.termLo,       (res.constData()+1), 8);
                memcpy(&terms.termHi,       (res.constData()+9), 8);
                memcpy(&terms.termCurrent,  (res.constData()+17), 8);
            }
        }
    }
    TCPCommandSet::go(_command);
}
