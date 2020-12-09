#include "tcpcommandseparatoronfull.h"
#include "tcp/tcpcommandrentgersetup.h"
#include "tcp/tcpcommandseparatoroff.h"
#include "tcp/tcpteststopseparate.h"
#include "tcp/tcpclearstates.h"

void TCPCommandSeparatorOnFull::setModelData(SPRMainModel *value)
{
    if(value){
        model =  value;

    }
}

bool TCPCommandSeparatorOnFull::isRentgenReady(){
    QByteArray res = findCommands(getren).last()->getReplayData().right(4);
    uint mka, mkv;
    memcpy(&mkv, res.constData(), 2);
    memcpy(&mka, res.constData()+2, 2);
    if(mkv >= 0x0600 && mka >= 0x0600){
        return true;
    } else {
        return false;
    }

}

void TCPCommandSeparatorOnFull::setFullMode(bool value)
{
    fullMode = value;
}

bool TCPCommandSeparatorOnFull::getFullMode() const
{
    return fullMode;
}

TCPCommandSeparatorOnFull::TCPCommandSeparatorOnFull(ServerConnect *server, SPRMainModel *_model, TCPTimeOutWidget *_widget, bool _isFullMode):
    TCPCommandSet(server, _widget, {}), model(nullptr)
{
    command = setSeparatorOnFull;
    fullMode = _isFullMode;
    setModelData(_model);
}

void TCPCommandSeparatorOnFull::go(TCPCommand *_command)
{
    if(!_command){
        qDebug() << "TCPCommandSeparatorOnFull starting...";

        clear();

        if(model){
            addCommand(new TCPClearStates());
            addCommand(new TCPCommand(getstate));

        }
    } else {
        if(_command->getCommand() == getstate){
            int err = _command->getErrors();
            int state = _command->getReplayData()[1];
            if(state != REMOTE_WAIT_C){
                addCommand(new TCPTestStopSeparate(getTimeOutWidget(), model));
            }

            addCommand(new TCPCommand(initada));
            addCommand(new TCPCommand(onsep));
            if(fullMode){
//                addCommand(new TCPCommand(onren));

                uint timeHotTube = model->getSettingsRentgenModel()->timeOnRA->getData();
                addCommand(new TCPTimeOutCommand(onren, timeHotTube * 1000 + 1000, 10, widget,
                                MSG_TIME_OUT_REN_ON, MSG_TIME_OUT_REN_ON_MSG(timeHotTube)));
                addCommand(new TCPCommandRentgerSetup(model->getServer(), this->widget, model->getSettingsRentgenModel()));
            }
            addCommand(new TCPCommand(offosw));
            addCommand(new TCPCommand(onosw));
        }
    }

    TCPCommandSet::go(_command);
}
