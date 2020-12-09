#include "tcpstartsoptestimscommand.h"
#include "tcp/ServerConnect.h"

TCPCommand *TCPStartSopTestIMSCommand::getStopTestCommand() const
{
    return stopTestCommand;
}

TCPStartSopTestIMSCommand::TCPStartSopTestIMSCommand()
{
    setParams({}, 40, 1, 1);
}

TCPStartSopTestIMSCommand::TCPStartSopTestIMSCommand(ServerConnect *_server, TCPTimeOutWidget *_widget)
    :TCPCommandSet(_server, _widget, {}), stopTestCommand(nullptr), getStateCommand(nullptr)
{
    setParams({}, 40, 1, 1);
    addCommand(new TCPCommand(nocommand)); // 0
    addCommand(new TCPCommand(testim)); // 1
    addCommand(new TCPTimeOutCommand(timeoutcommand, timeWork, 10, widget, tr("Тест исполнительных механизмов"))); // 2
    addCommand(new TCPCommand(getsepar)); // 3
    addCommand(new TCPCommand(stoptest)); // 4

//    stopTestCommand = new TCPCommand(stoptest);

}

void TCPStartSopTestIMSCommand::setParams(QVector<uint8_t> _ims, int _delay, int _timeWork, int _freq){
    ims = _ims;
    delay = _delay;
    timeWork = _timeWork;
    freq = _freq;
    //        findCommands(timeoutcommand).first()->setTimeOut(timeWork);
}

void TCPStartSopTestIMSCommand::go(TCPCommand *_command)
{
    if(!_command){
        clear();
        countIms = 0;
        addCommand(getstate);

        //       for(uint8_t th=0; th<ims.size(); th++){
        //            addCommand(testim)->addSendData(&ims[th], 1)->addSendData(&freq, sizeof (freq))->addSendData(&delay, sizeof (delay));
//            addCommand(getstate);
//            addCommand(new TCPTimeOutCommandWC(timeoutcommand, timeWork * 1000, 1, widget,
//                                    tr("Тест исполнительных механизмов"),
//                                    tr("Тест механизма ручья %1 из %2").arg(th+1).arg(ims.size())));
//            addCommand(stoptest);
//            addCommand(getstate);
//       }
//       addCommand(new TCPCommand(getsepar)); // 3
//       addCommand(new TCPCommand(stoptest)); // 4
        running = true;
    } else {
        if(running){
            if(_command == commandSet.last()){
                if(countIms < ims.size()){
                    if(server && server->isState(spr_state_test_ims)){
                        addCommand(stoptest);
                    }
                    addCommand(testim)->addSendData(&ims[countIms], 1)->addSendData(&freq, sizeof(freq))->addSendData(&delay, sizeof(delay));
                    addCommand(getstate);
                    addCommand(new TCPTimeOutCommandWC(timeoutcommand, timeWork * 1000, 1, widget,
                                            tr("Тест исполнительных механизмов"),
                                            tr("Тест механизма ручья %1 из %2").arg(countIms+1).arg(ims.size())));
                    addCommand(getstate);
                    countIms++;
                } else {
                    running = false;
                    if(server && server->isState(spr_state_test_ims)){
                        if(stopTestCommand) delete stopTestCommand; stopTestCommand = nullptr;
                        stopTestCommand = new TCPCommand(stoptest);
                        stopTestCommand->send(server);

                        if(!getStateCommand) getStateCommand = new TCPCommand(getstate);
                        getStateCommand->send(server);
                    }
                    if(countIms == ims.size()){
                        emit commandComplite(this);
                    } else {
                        emit cancelCommand(this);
                    }
                    return;
                }
            }
        } else {
            if(server && server->isState(spr_state_test_ims)){
                if(stopTestCommand) delete stopTestCommand; stopTestCommand = nullptr;
                stopTestCommand = new TCPCommand(stoptest);

                stopTestCommand->send(server);

                if(!getStateCommand) getStateCommand = new TCPCommand(getstate);
                getStateCommand->send(server);
            }
            return;
//            if(countIms < ims.size()){
//                emit cancelCommand(this);
//            } else {
//                emit commandComplite(this);
//            }
        }
    }
    TCPCommandSet::go(_command);
}

QByteArray TCPStartSopTestIMSCommand::getReplayData(){
    return findCommands(getsepar).first()->getReplayData();
}
QByteArray TCPStartSopTestIMSCommand::getResult(){
    return findCommands(getsepar).first()->getResult();
}

void TCPStartSopTestIMSCommand::stopTest(){
    running = false;
    countIms = 10000;

    if(server && server->isState(spr_state_test_ims)){
        if(stopTestCommand) delete stopTestCommand; stopTestCommand = nullptr;
        stopTestCommand = new TCPCommand(stoptest);
        stopTestCommand->send(server);

        if(!getStateCommand) getStateCommand = new TCPCommand(getstate);
        getStateCommand->send(server);
    }
    onCancelCommand(this);

//    if(widget){
//        widget->setWindowModality(Qt::ApplicationModal);
//    }
//    stopTestCommand->send(server);
//    emit commandComplite(stopTestCommand);
    return;
}

void TCPStartSopTestIMSCommand::onCancelCommand(TCPCommand *var)
{
    if(server && server->isState(spr_state_test_ims)){
        if(stopTestCommand) delete stopTestCommand; stopTestCommand = nullptr;
        stopTestCommand = new TCPCommand(stoptest);
        stopTestCommand->send(server);

    }
    QMessageBox::information(nullptr, tr("Тесты ИМС"), tr("Тесты прерваны пользователем..."));
    TCPCommandSet::onCancelCommand(var);
}
