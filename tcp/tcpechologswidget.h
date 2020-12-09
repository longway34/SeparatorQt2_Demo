#ifndef TCPECHOLOGSWIDGET_H
#define TCPECHOLOGSWIDGET_H

#include "tcp/TCPCommand.h"
#include "isprwidget.h"
#include "ui_tcpechologswidget.h"

#include <QWidget>

class ServerConnect;

class MainWindow;
class SPRSeparatorLogsModel;
class SPRSeparateMsgModel;
class SPRMainModel;

class IModelVariable;

class TCPEchoLogsWidget : public QWidget, public ISPRWidget
{

    Q_OBJECT

protected:
    QString myName;
    QString addLogsCommand(TCPCommand *command);


    friend class ServerConnect;
    friend class MainWindow;

    QTextCharFormat textCharFormatDefault;
    QTextCharFormat textCharFormatAther;

    SPRMainModel *mainModel;
//    SPRSeparatorLogsModel *logModel;
public:
    explicit TCPEchoLogsWidget(QWidget *parent = nullptr);
    Ui::TCPEchoLogsWidget ui;

    QString getMyName() const;
    void setMyName(const QString &value);

    void setModels(SPRMainModel *_mainModel, SPRSeparatorLogsModel *_logModel);
public slots:
//    void onErrorLogsCommand(TCPCommand *command, QString _prefix);
    void addLogsMessage(QString msg, QColor _color = QColor());
//    void onLogsCommand(TCPCommand *command, QString _prefix);

    void onEchoLogMessages(SPRSeparateMsgModel *msg, SPRSeparatorLogsModel *send);
protected slots:
    void onClear(bool);

signals:
//    void logWidgetEvent(QString, QColor);

private:

    // ISPRWidget interface
public:
    virtual void widgetsShow();
    virtual ISPRModelData *getModelData();

protected:
    virtual void onModelChanget(IModelVariable *);

    // ISPRWidget interface
public:
    virtual void storeWidgetsStates(QString groupId, QSettings *mainSettings);
    virtual void restoreWidgetsStates(QString groupId, QSettings *mainSettings);
};

#endif // TCPECHOLOGSWIDGET_H
