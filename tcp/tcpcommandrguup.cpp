#include "tcpcommandrguup.h"

TCPCommandRGUUp::TCPCommandRGUUp()
{

}

TCPCommandRGUUp::TCPCommandRGUUp(ServerConnect *_server, TCPTimeOutWidget *_widget):
    TCPCommandSet(_server, _widget, {})
{
}

void TCPCommandRGUUp::go(TCPCommand *_command)
{
    if(!_command){

        clear();

        this->command  = setRGUUp;
        QByteArray rguUp = QByteArray::fromRawData("\x01", 1);
    //    addCommand(new TCPCommand(getrgu2));
        TCPCommand *crgu = new TCPCommand(setrgu2); crgu->addSendData(rguUp);
        addCommand(crgu);
        addCommand(new TCPTimeOutCommand(timeoutcommand, 1000, 5, widget,
                                          tr("Поднятие РГУ."), tr("Поднятие РГУ. \nПодождите...")));
        addCommand(new TCPCommand(getrgu2));
    }
    TCPCommandSet::go(_command);
}

//void TCPCommandRGUUp::go(TCPCommand *_command)
//{
//    if(_command){
//          int num = commandSet.indexOf(_command);
//          if(_command != commandSet.last()){
//            commandSet[num+1]->send(server);
//            return;
//           } else {
//            QByteArray res = commandSet[commandSet.size()-1]->getResult();
//            if(res.size()>0){
//                if(res[0] != 1){
//                    commandSet[1]->send(server);
//                } else {
//                    emit commandComplite(this);
//                }
//            } else {
//                emit commandComplite(this);
//            }
//        }
//    } else {
//        if(commandSet.size() > 0){
//            commandSet[0]->send(server);
//        } else {
//            emit commandComplite(this);
//        }
//    }

//}
