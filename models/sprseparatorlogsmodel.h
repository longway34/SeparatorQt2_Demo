#ifndef SPRSEPARATORLOGSMODEL_H
#define SPRSEPARATORLOGSMODEL_H

#include <QObject>
#include <QWidget>

#include <QDomDocument>

#include "models/isprmodeldata.h"
#include "models/imodelvariable.h"

#include "_types.h"
#include "variables/sprqstringvariable.h"
#include "variables/sprdatetimevariable.h"
#include "variables/sprenumvariable.h"
#include "tcp/ITCPCommand.h"
#include "tcp/TCPCommand.h"

//#include "models/sprhistorymodel2.h"

class SPRHistoryModel2;
class SPRMainModel;

class SPRSeparatorLogsModel;

class SPRSeparateMsgModel : public ISPRModelData{
    Q_OBJECT

    int separatorId;
    int smenaId;
    SPRVariable<int> *id;
    SPRDateTimeVariable *dt;
    SPREnumVariable<EnumCommands> *comId;
    SPRQStringVariable *msgText;
public:
    friend SPRSeparatorLogsModel;

    SPREnumVariable<SPRTypeLogMessage> *type;

    SPRSeparateMsgModel(QDomDocument *doc, int _separatorId, int _smenaId, int _id, IModelVariable *parent, SPRTypeLogMessage _type = tMessage, QString _string="", EnumCommands _commands=nocommand);
    ~SPRSeparateMsgModel();

    EnumCommands getCommands();
    QString getString();
    SPRTypeLogMessage getType();
    QDateTime getDateTime();
    void messageRemoveFromXML();

};

class SPRSeparateResumeItemModel : public ISPRModelData{
    Q_OBJECT

    struct thVars {
        int separatorId;
        int smenaId;
        int resumeId;
        SPRVariable<int> *id;
        SPRVariable<long> *i_con;
        SPRVariable<long> *i_tail;
        SPRVariable<long> *i_all;
        thVars(QDomDocument *doc, int _separatorId, int _smenaId, int _resumeId, int _id, IModelVariable *parent);
        ~thVars();
        void thVarsRemoveFromXML();
    };

    int separatorId;
    int smenaId;
    SPRVariable<int> *id;
    SPRDateTimeVariable *begin;
    SPRDateTimeVariable *end;
    SPRVariable<int> *thCount;
    QList<thVars*> threadsResume;

public:
    friend SPRSeparatorLogsModel;

    SPRSeparateResumeItemModel(QDomDocument *doc, int _separatorId, int _smenaId, int _id, IModelVariable *parent, int _thCount = 0);
    ~SPRSeparateResumeItemModel();
    int getId();
    QDateTime getBegin();
    QDateTime getEnd();
    int getThCount();

    void endResume();
    bool isNullResume();
    QVector<QVector<long> > getResumeResults(int _thId = -1); // -1 - summ all channels; > thNum - all channels

    QString toTableViewStringResume();
    double toTableViewPercentResume();
    long toTableViewAllCountResume();
    void *updateResumeValues(QVector<QVector<double> > *vars);

    void resumeRemoveFromXML();
};

class SPRSeparateSmenaModel : public ISPRModelData{
    Q_OBJECT

    int separatorId;
    int thCount;

    SPRVariable<int> *id;
    SPRVariable<int> *uid;
    SPRVariable<int> *sid;

    SPRVariable<int> *mcount;
//    SPRVariable<int> *ecount;
    SPRVariable<int> *rcount;

    SPRDateTimeVariable *begin;
    SPRDateTimeVariable *end;

    QList<SPRSeparateMsgModel*> messages;
//    QList<SPRSeparateMsgModel*> errors;
    QList<SPRSeparateResumeItemModel*> resume;

public:
    friend SPRSeparatorLogsModel;

    SPRSeparateSmenaModel(QDomDocument *doc, int _separatorId, int _id, IModelVariable *parent, int _sid = 0, int _uid = 0);
    ~SPRSeparateSmenaModel();

    // IModelVariable interface
    int getId();
    int getUid();
    int getSid();
    int getMCount();
    int getECount();
    int getRCount();

    void setMCount(int _count);
    void setECount(int _count);
    void setRCount(int _count);
    void endSmena();
    bool isNullSmena();
    QDateTime getSmenaBegin();
    QDateTime getSmenaEnd();

    void endSeparate(int _id = -1);
    //    QVector<QVector<double> > getResults(int _id, int _thId); // _id: -1 - last resume; vary big number - summs all
    SPRSeparateMsgModel * addLogMessage(int _separatorId, SPRTypeLogMessage _type, QString _msg = "", EnumCommands _command = nocommand);
//    QVector<QVector<double> > getResults(int _id);

    void *updateResumeValues(QVector<QVector<double> > *vars, int _id=-1);
    QList<SPRSeparateResumeItemModel *> getAllResumes();
    SPRSeparateResumeItemModel *fingResumeById(int _id = -1);
    void setUid(int id);
    void setSid(int id);
    SPRSeparateResumeItemModel *addNewResume();
    SPRSeparateResumeItemModel *startNewSeparate();
    QList<SPRSeparateMsgModel *> getMessages() const;

    void smenaRemoveFromXML();
};

class SPRUsersSmenaModel;
class SPRUserModel;
class SPRSmenaModel;

class SPRSeparatorLogsModel :public ISPRModelData
{
    Q_OBJECT

    SPRVariable<int> *id;
    SPRVariable<int> *scount;
    QList<SPRSeparateSmenaModel*> smenas;

    SPRMainModel *mainModel;
    SPRHistoryModel2 *histModel;
    SPRUsersSmenaModel *userSmenaModel;

    SPRUserModel *currentUser;
    SPRSmenaModel *currentSmena;

public:

    struct ToTableViewResumeData{
        SPRSeparateResumeItemModel *res;
        QDateTime dtBegin;
        QDateTime dtEnd;
        QString userNicName;
        QString smenaName;
        QString strResume;
        long countAllResume;
        double prAllResume;
    };

    struct ToTableViewMessagesData{
        SPRSeparateMsgModel *msg;
        QDateTime dt;
        QString userNicName;
        QString smenaName;
        SPRTypeLogMessage type;
        EnumCommands _commandId;
        QString messageText;
    };

    QVector<ToTableViewResumeData*> tableViewResumeData;
    QVector<ToTableViewMessagesData*> tableViewMessageData;
//    QVector<ToTableViewMessagesData*> tableViewMessageLastSmenaData;

    QString name;

    SPRSeparatorLogsModel(QDomDocument *doc, int _separatorId, IModelVariable *parent);
    SPRSeparatorLogsModel(QString fName, int _separatorId, IModelVariable *parent);

    SPRSeparateSmenaModel *addNewSmena();

    // IModelVariable interface
    void endSmena(int id=-1);
    SPRSeparateMsgModel *addLogMessage(TCPCommand *_command, SPRTypeLogMessage type, QString msg = "");
    QList<SPRSeparateSmenaModel*> getSmenas();
    void setUserSmenaModel(SPRUsersSmenaModel *_model);
public slots:
    void onSelectUser(SPRUserModel *mod);
    void onSelectSmena(SPRSmenaModel *mod);

    QVector<ToTableViewResumeData*> *getTableViewResumeData();
    QVector<ToTableViewMessagesData*> *getTableViewMessagesData();
protected:
    ToTableViewResumeData *findFromTableByResume(SPRSeparateResumeItemModel *id);

    void init(int _separatorId);
    SPRSeparateResumeItemModel *findResumeById(int id=-1);
    SPRSeparateSmenaModel *findSmenaById(int id=-1);
    QList<SPRSeparateSmenaModel*> findBefore(QDateTime dt);

    // IModelVariable interface
    void fullTableViewResumeData();
    void fullTableViewMessagesData();

    SPRSeparatorLogsModel::ToTableViewMessagesData *findFromTableByMessage(SPRSeparateMsgModel *msg, QVector<SPRSeparatorLogsModel::ToTableViewMessagesData*> *sTable);
public:
    virtual SPRMainModel *getMainModel();
    void deleteSmena(SPRSeparateSmenaModel *sm);
    void onSeparateDataReady(SPRHistoryModel2 *hist);
    void separatorRemoveFromXML();
    QString getName() const;
    void setName(const QString &value);

protected slots:

signals:
    void resumesTableChanget();
    void messagesTableChanget();
    void echoLogMessage(SPRSeparateMsgModel *msg, SPRSeparatorLogsModel *);
};

#endif // SPRSEPARATORLOGSMODEL_H
