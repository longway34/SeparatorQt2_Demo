#include "tcp/tcptimeoutwigget.h"
#include <QDateTime>

TCPTimeOutWidget::TCPTimeOutWidget(QWidget *parent) :
    QWidget(parent)
{
    ui.setupUi(this);

//    connect(ui.bCancel, &QAb)
    ui.wCancelButtons->setVisible(false);
}

void TCPTimeOutWidget::onRangeChange(uint step, uint all)
{
    ui.thRange->setScale(0, double(all));
    ui.thRange->setValue(double(step));

//    ui.wCancelButtons->setVisible(false);
//    QDateTime dt = QDateTime::currentDateTime();
//    qDebug() << dt.toString("hh:mm:ss") << "to widget step: " <<step<<" all: "<< all << " i am:"<< QString::number(ulong(this), 16) << " msg:" << ui.lMessage->text() << " parent:" << QString::number((ulong)this->parentWidget(), 16);
    if(step == all){
//        qDebug() << dt.toString("hh:mm:ss") << "to widget close: " << QString::number(ulong(this), 16) << " msg:" << ui.lMessage->text() ;
//        this->close();
    }
}

void TCPTimeOutWidget::onCancelButton(bool)
{
    emit cancelCommand();
}
