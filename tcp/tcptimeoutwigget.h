#ifndef TCPTIMEOUTWIGGET_H
#define TCPTIMEOUTWIGGET_H

#include "ui_tcptimeoutwigget.h"

class TCPTimeOutWidget : public QWidget
{
    Q_OBJECT

public:
    Ui::TCPTimeOutWigget ui;
    explicit TCPTimeOutWidget(QWidget *parent = nullptr);


public slots:
    void onRangeChange(uint step, uint all);
    void onCommandComplite(int){
        this->close();
    }
    void setTitle(QString _title){
        this->setWindowTitle(_title);
    }
    void setMessage(QString _message){
        ui.lMessage->setText(_message);
    }
    void onCancelButton(bool);
signals:
    void cancelCommand();
};

#endif // TCPTIMEOUTWIGGET_H
