#include "tcp/tcpechologswidget.h"
#include <QDateTime>
#include <QTextCharFormat>
#include <QBrush>
#include "models/sprseparatorlogsmodel.h"
#include "models/sprmainmodel.h"
#include "models/sprhistorymodel2.h"

#include <QAbstractTableModel>

#include "tcp/TCPCommand.h"
/**
 * @brief TCPEchoLogsWidget::TCPEchoLogsWidget
 * @param parent
 */

TCPEchoLogsWidget::TCPEchoLogsWidget(QWidget *parent) :
    QWidget(parent), mainModel(nullptr)
{
    ui.setupUi(this);

    logModel = nullptr;

    connect(ui.bClearLogs, SIGNAL(clicked(bool)), this, SLOT(onClear(bool)));
    textCharFormatDefault = ui.ptLogs->currentCharFormat();
    textCharFormatAther = textCharFormatDefault;
}

void TCPEchoLogsWidget::setModels(SPRMainModel *_mainModel, SPRSeparatorLogsModel *_logModel){
    if(_logModel){
        logModel = _logModel;
    }
    if(_mainModel){
        mainModel = _mainModel;
    }
}

void TCPEchoLogsWidget::onEchoLogMessages(SPRSeparateMsgModel *msg, SPRSeparatorLogsModel *send){
    QColor color = QColor();
    if(msg->getType() == tError){
        color = QColor(Qt::red);
    } else if(msg->getType() == tWarning){
        color = QColor(Qt::blue);
    }

    QString txtDt;
    if(!msg->getDateTime().isNull() && msg->getDateTime().isValid()){
        txtDt = msg->getDateTime().toString("HH:mm:ss");
    } else {
        txtDt = QDateTime::currentDateTime().toString("HH:mm:ss");
    }

    QString txtName = "";
    if(sender() && sender() != logModel){
        SPRSeparatorLogsModel *lm = dynamic_cast<SPRSeparatorLogsModel*>(sender());
        if(lm){
            txtName = "<b>" + lm->getName() + ":</b>";
        }
    }
    QString txtMsg = msg->getString();

    if(msg->getCommands() != nocommand){
        if(mapTCPCommandMessages.contains(msg->getCommands())){
            txtMsg = mapTCPCommandMessages[msg->getCommands()] + txtMsg;
        }
    }
    QString text = QString("%1: %2 %3").arg(txtDt).arg(txtName).arg(txtMsg);

    addLogsMessage(text, color);
}

void TCPEchoLogsWidget::onClear(bool){
    ui.ptLogs->clear();
}

QString TCPEchoLogsWidget::getMyName() const
{
    return myName;
}

void TCPEchoLogsWidget::setMyName(const QString &value)
{
    myName = value;
}

void TCPEchoLogsWidget::addLogsMessage(QString msg, QColor _color){
    if(_color.isValid()){
        textCharFormatAther.setForeground(QBrush(_color));
        msg = tr("<font color=\"%1\">%2</font>").arg(_color.name()).arg(msg);
        ui.ptLogs->setCurrentCharFormat(textCharFormatAther);
    } else {
        ui.ptLogs->setCurrentCharFormat(textCharFormatDefault);
    }
//    QString text = ui.ptLogs->to;
//    text = msg + "<br>" + text;
    ui.ptLogs->appendHtml(msg);
}

void TCPEchoLogsWidget::widgetsShow()
{
}

ISPRModelData *TCPEchoLogsWidget::getModelData()
{
    return logModel;
}

void TCPEchoLogsWidget::onModelChanget(IModelVariable *)
{
}


void TCPEchoLogsWidget::storeWidgetsStates(QString groupId, QSettings *mainSettings)
{
    mainSettings->beginGroup(groupId);
    mainSettings->setValue(objectName()+"."+ui.splitter->objectName(), ui.splitter->saveState());
    mainSettings->endGroup();
}

void TCPEchoLogsWidget::restoreWidgetsStates(QString groupId, QSettings *mainSettings)
{
    mainSettings->beginGroup(groupId);
    ui.splitter->restoreState(mainSettings->value(objectName()+"."+ui.splitter->objectName()).toByteArray());
    mainSettings->endGroup();
}
