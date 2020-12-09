#include "tcp/tcpautosetrentgen.h"
#include "tcp/tcpgetspectrumsgistogramms.h"
#include "tcp/tcpgetspectrums2.h"

#include <QDebug>

bool TCPAutoSetRentgen::getUseRGU() const
{
    return useRGU;
}

void TCPAutoSetRentgen::setUseRGU(bool value)
{
    useRGU = value;
}

SPRThreadList TCPAutoSetRentgen::getThreads()
{
    return threads;
}

void TCPAutoSetRentgen::setThreads(SPRThreadList &value)
{
    threads = value;
}


void TCPAutoSetRentgen::setCodesDEU(QByteArray &value)
{
    codesDEU = value;
}

void TCPAutoSetRentgen::setCodesCP(QByteArray &value)
{
    codesCP = value;
}

void TCPAutoSetRentgen::settingCodes()
{
    findCommands(setudeu).last()->addSendData(codesDEU);
    findCommands(setptdeu).last()->addSendData(codesCP);
}

void TCPAutoSetRentgen::settingRentgenVA()
{
//    uint8_t rentgenNum = 1; /* DEF_SPR_MAIN_RENTGENS;*/
    QByteArray va;
    va.append("\0", 1);
    double dmkv = mainModel->getSettingsRentgenModel()->uTubes[0]->getData() * 21.8 - 23;
    double dmka = mainModel->getSettingsRentgenModel()->uTubes[0]->getData() * 23.4 - 101;
    uint16_t mkv = round(dmkv);
    uint16_t mka = round(dmka);
    va.append((char*)&mkv, sizeof(mkv));
    va.append((char*)&mka, sizeof(mka));

    findCommands(setren).last()->addSendData(va);
}

TCPAutoSetRentgen::TCPAutoSetRentgen(SPRMainModel *_model, TCPTimeOutWidget *_toWidget)
    :TCPCommandSet(nullptr, _toWidget, {}), mainModel(nullptr)
{
    mainModel = _model;

//    separatorOn = new TCPCommandSeparatorOnFull(mainModel->getServer(), mainModel, _toWidget);
//    commandRGU = new TCPCommandRGUUpDown2(mainModel->getServer(), _toWidget, DownRGU);
}

//#define _MULAGE_DEBUG

void TCPAutoSetRentgen::go(TCPCommand *_command)
{
//            uint time_out_hot_tube = mainModel->getSettingsRentgenModel()->timeOnRA->getData();
//            uint _time_out_expon = mainModel->getSettingsRentgenModel()->timeHotRA->getData() * 1000 + 1000;
//            uint _time_out_on_deu_cp = 2000;
//            uint32_t spkTime = 0x64;
//            uint _time_out_spk = 11000;
    if(mainModel){

        if(!_command){
            clear();
qDebug() << "TCPAutoSetRentgen starting...";

//            TCPCommandSeparatorOnFull *onCommand = new TCPCommandSeparatorOnFull(mainModel->getServer(), mainModel, getTimeOutWidget(), true);
            TCPGetSpectrums2 *getSpectrum = new TCPGetSpectrums2(mainModel, 5, DownRGU, threads, false, getTimeOutWidget());
            addCommand(getSpectrum);
        }
    }
    TCPCommandSet::go(_command);
}
