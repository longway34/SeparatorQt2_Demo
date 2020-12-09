#include "sprseparatorlogsmodel.h"
#include "models/sprhistorymodel2.h"
#include "models/spruserssmenamodel.h"

#include "models/sprmainmodel.h"
/**
 * @brief SPRSeparateResumeItemModel::thVars::thVars
 * @param doc
 * @param pref
 * @param _id
 * @param parent
 */

SPRSeparateResumeItemModel::thVars::thVars(QDomDocument *doc, int _separatorId, int _smenaId, int _resumeId, int _id, IModelVariable *parent):
    id(nullptr), i_con(nullptr), i_tail(nullptr), i_all(nullptr)
{
    QString pref = tr("SEPARATOR[@ID=%1]/SMENA[@ID=%2]/SEPARATE[@ID=%3]/TH[#%4]").arg(_separatorId).arg(_smenaId).arg(_resumeId).arg(_id);
    id = new SPRVariable<int>(doc, pref+"[ID]", _id, parent);
    QString pref2 = pref + tr("[@ID=%1]").arg(id->getData());

    i_con = new SPRVariable<long>(doc, pref2 + "[I_CON]", 0, parent);
    i_tail = new SPRVariable<long>(doc, pref2 + "[I_TAIL]", 0, parent);
    i_all = new SPRVariable<long>(doc, pref2 + "[I_ALL]", i_con->getData() + i_tail->getData(), parent);
}

SPRSeparateResumeItemModel::thVars::~thVars()
{
    if(id) delete id;
    if(i_con) delete i_con;
    if(i_tail) delete i_tail;
    if(i_all) delete i_all;
}

void SPRSeparateResumeItemModel::thVars::thVarsRemoveFromXML(){
    if(id) {id->removeFromXML(); delete id; id = nullptr;}
    if(i_con) {i_con->removeFromXML(); delete i_con; i_con = nullptr;}
    if(i_tail) {i_tail->removeFromXML(); delete i_tail; i_tail = nullptr;}
    if(i_all) {i_all->removeFromXML(); delete i_all; i_all = nullptr;}
}

/**
 * @brief SPRSeparateMsgModel::SPRSeparateMsgModel
 * @param doc
 * @param _separatorId
 * @param _smenaId
 * @param _id
 * @param parent
 * @param _type
 * @param _string
 * @param _command
 */

SPRSeparateMsgModel::SPRSeparateMsgModel(QDomDocument *doc, int _separatorId, int _smenaId, int _id, IModelVariable *parent, SPRTypeLogMessage _type, QString _string, EnumCommands _command):
    ISPRModelData (doc, parent), id(nullptr), dt(nullptr),  comId(nullptr), msgText(nullptr), type(nullptr), separatorId(_separatorId), smenaId(_smenaId)
{
    if(doc){
        QString pref = QString("SEPARATOR[@ID=%1]/SMENA[@ID=%2]/MESSAGES/M[#%3]").arg(_separatorId).arg(_smenaId).arg(_id);

        id = new SPRVariable<int>(doc, pref + QString("[ID]"), _id, parent);

        QString pref2 = pref + QString("[@ID=%1]").arg(id->getData());

        dt = new SPRDateTimeVariable(doc, pref2+"[DT]", QDateTime::currentDateTime(), this);
        type = new SPREnumVariable<SPRTypeLogMessage>(doc, pref2 + "[TYPE]", _type, this);
        comId = new SPREnumVariable<EnumCommands>(doc, pref2 + "[CID]", _command, this);
        msgText = new SPRQStringVariable(doc, pref2, _string, this);
    }
}

SPRSeparateMsgModel::~SPRSeparateMsgModel(){
    if(id) delete id;
    if(dt) delete dt;
    if(type) delete type;
    if(comId) delete comId;
    if(msgText) delete msgText;
}

EnumCommands SPRSeparateMsgModel::getCommands()
{
    return comId->getData();
}

QString SPRSeparateMsgModel::getString()
{
    QString msg = msgText->getData();
    if(comId->getData() == nocommand){
        if(type->getData() == tError){
            msg = tr(" Ошибка!!! ")+msg;
        }
        return msg;
    } else {
        if(mapTCPCommandMessages.contains(comId->getData())){
            QString msg = mapTCPCommandMessages[comId->getData()];
            if(!msgText->getData().isEmpty() && !msgText->getData().isNull()){
                msg = msg + tr(" (%1)").arg(msgText->getData());
            }
        }
        if(type->getData() == tError){
            msg = tr(" Ошибка!!! ") + msg;
        }
        return msg;
    }
}

SPRTypeLogMessage SPRSeparateMsgModel::getType()
{
    return type->getData();
}

QDateTime SPRSeparateMsgModel::getDateTime()
{
    return dt->getData();
}

void SPRSeparateMsgModel::messageRemoveFromXML()
{
    if(msgText) {msgText->removeFromXML(); delete msgText; msgText = nullptr; }
    if(type) {type->removeFromXML(); delete type; type = nullptr;}
    if(dt) {dt->removeFromXML(); delete dt; dt = nullptr;}
    if(id) {id->removeFromXML(); delete id; id = nullptr; }
    if(comId) {comId->removeFromXML(); comId->removeFromXML(); comId = nullptr;}
}

/**
 * @brief SPRSeparateResumeItemModel::SPRSeparateResumeItemModel
 * @param doc
 * @param _separatorId
 * @param _smenaId
 * @param _id
 * @param parent
 * @param _thCount
 */

SPRSeparateResumeItemModel::SPRSeparateResumeItemModel(QDomDocument *doc, int _separatorId, int _smenaId, int _id, IModelVariable *parent, int _thCount):
    ISPRModelData(doc, parent), id(nullptr), begin(nullptr), end(nullptr), thCount(nullptr), separatorId(_separatorId), smenaId(_smenaId)
{
    QString pref = QString("SEPARATOR[@ID=%1]/SMENA[@ID=%2]/SEPARATE").arg(_separatorId).arg(_smenaId);

    id = new SPRVariable<int>(doc, pref + tr("[ID]"), _id, this);
    QString pref2 = pref + tr("[@ID=%1]").arg(id->getData());

    begin = new SPRDateTimeVariable(doc, pref2 + "[DT]", QDateTime::currentDateTime(), this);
    end = new SPRDateTimeVariable(doc, pref2 + "[END]", QDateTime::currentDateTime(), this);
    thCount = new SPRVariable<int>(doc, pref2 + "THCOUNT", _thCount, this);

    for(int th=0; th<thCount->getData(); th++){

        QString pref3 = pref2 + "/TH";
        thVars *st = new struct thVars(doc, _separatorId, _smenaId, id->getData(), th, this);

        threadsResume.append(st);
    }
}

SPRSeparateResumeItemModel::~SPRSeparateResumeItemModel()
{
    if(id) delete id;
    if(begin) delete begin;
    if(end) delete end;
    if(thCount) delete thCount;
    foreach(thVars *thv, threadsResume){
       if(thv) delete thv;
    }
    threadsResume.clear();
}

void *SPRSeparateResumeItemModel::updateResumeValues(QVector<QVector<double>> *vars){
    void *res = nullptr;
    if(vars->size() == 0){
        return nullptr;
    }
    if((*vars)[0].size() < 3){
        return nullptr;
    }
    for(int th=0; th<(*vars).size(); th++){
        thVars *st = nullptr;
        if(th >= threadsResume.size()){
            st = new thVars(doc, separatorId, smenaId, id->getData(), th, this);
            threadsResume.append(st);
        } else {
            st = threadsResume[th];
        }
        double con = st->i_con->getData() + (*vars)[th][0];
        double tail = st->i_tail->getData() + (*vars)[th][1];
        double all = st->i_all->getData() + (*vars)[th][2];

        st->i_con->setData(static_cast<long>(con));
        st->i_tail->setData(static_cast<long>(tail));
        st->i_all->setData(static_cast<long>(all));

//        st->i_con->setData(static_cast<long>((*vars)[th][0]));
//        st->i_tail->setData(static_cast<long>((*vars)[th][1]));
//        st->i_all->setData(static_cast<long>((*vars)[th][2]));

        if(vars->size() > thCount->getData()) thCount->setData(vars->size());
        res = st;

    }
    return res;
}

void SPRSeparateResumeItemModel::resumeRemoveFromXML()
{
    foreach(thVars *thv, threadsResume){
        thv->thVarsRemoveFromXML();
        delete thv;
        thv = nullptr;
    }
    threadsResume.clear();

    if(begin) {begin->removeFromXML(); delete begin; begin = nullptr;}
    if(end) {end->removeFromXML(); delete end; end = nullptr;}
    if(thCount) {thCount->removeFromXML(); delete thCount; thCount = nullptr;}
    if(id) {id->removeFromXML(); delete id; id = nullptr;}
}

void SPRSeparateResumeItemModel::endResume(){
//    QVector<SPRHistoryModel2::histDataOneThread> *last = model->getLastRecords();
//    QVector<QVector<double>> lastValues;
//    for(int th=0; th<last->size(); th++){
//        lastValues.append(QVector<double>({
//            (*last)[th].data.i_prd[0],
//            (*last)[th].data.i_prd[1],
//            (*last)[th].data.i_prd[2]  }));
//    }

//    updateResumeValues(&lastValues);
    end->setData(QDateTime::currentDateTime());
}

bool SPRSeparateResumeItemModel::isNullResume(){
    foreach(thVars *st, threadsResume){
        if(st->i_all->getData() > 0){
            return false;
        }
    }
    return false;
}

QString SPRSeparateResumeItemModel::toTableViewStringResume(){
    QString res("");
    foreach(thVars *st, threadsResume){
        if(!res.isEmpty()) res = res + "; ";
        double pr = 0;
        if(st->i_all->getData() > 0) pr = (static_cast<double>(st->i_con->getData()) / static_cast<double>(st->i_all->getData())) * 100;
        res = res + tr("Ручей %1: <b>%2%</b>").arg(st->id->getData()+1).arg(QString::number(pr, 'f', 2));
    }
    return res;
}

double SPRSeparateResumeItemModel::toTableViewPercentResume()
{
    double res = 0;
    long s_all = 0, s_con = 0;
    foreach(thVars *st, threadsResume){
        s_all += st->i_all->getData();
        s_con += st->i_con->getData();
    }
    if(s_all > 0){
        res = (static_cast<double>(s_con) / static_cast<double>(s_all)) * 100;
    }
    return res;
}

long SPRSeparateResumeItemModel::toTableViewAllCountResume()
{
    long res = 0;
    foreach(thVars *st, threadsResume){
        res += st->i_all->getData();
    }
    return res;
}

QVector<QVector<long> > SPRSeparateResumeItemModel::getResumeResults(int _thId)
{
    QVector<QVector<long>> res;
    QVector<long> oneChannel(3, 0);
    if(_thId < 0 || _thId >= threadsResume.size()){
        foreach(thVars *thv, threadsResume){
            if(_thId < 0){
                oneChannel[0] += thv->i_con->getData();
                oneChannel[1] += thv->i_tail->getData();
                oneChannel[2] += thv->i_all->getData();
            } else {
                oneChannel[0] = thv->i_con->getData();
                oneChannel[1] = thv->i_tail->getData();
                oneChannel[2] = thv->i_all->getData();
                res.append(oneChannel);
            }
            if(_thId < 0){
                res.append(oneChannel);
            }
        }
    } else {
        oneChannel[0] = threadsResume[_thId]->i_con->getData();
        oneChannel[1] = threadsResume[_thId]->i_tail->getData();
        oneChannel[2] = threadsResume[_thId]->i_all->getData();
        res.append(oneChannel);
    }
    return res;
}

int SPRSeparateResumeItemModel::getId(){
    return id->getData();
}
QDateTime SPRSeparateResumeItemModel::getBegin(){
    return begin->getData();
}
QDateTime SPRSeparateResumeItemModel::getEnd(){
    return end->getData();
}
int SPRSeparateResumeItemModel::getThCount(){
    return thCount->getData();
}

/**
 * @brief SPRSeparateSmenaModel::SPRSeparateSmenaModel
 * @param doc
 * @param _separatorId
 */

QList<SPRSeparateMsgModel *> SPRSeparateSmenaModel::getMessages() const
{
    return messages;
}

void SPRSeparateSmenaModel::smenaRemoveFromXML(){
    foreach(SPRSeparateResumeItemModel *res, resume){
        if(res){
            res->removeFromXML();
            delete res;
            res = nullptr;
        }
    }
    resume.clear();

    foreach(SPRSeparateMsgModel *msg, messages){
        if(msg){
            msg->messageRemoveFromXML();
            delete msg;
            msg = nullptr;
        }
    }
    messages.clear();

    if(id) {id->removeFromXML(); delete id; id = nullptr;}
    if(uid) {uid->removeFromXML(); delete uid; uid = nullptr;}
    if(sid) {sid->removeFromXML(); delete sid; sid = nullptr;}
    if(mcount) {mcount->removeFromXML(); delete mcount; mcount = nullptr;}
    if(rcount) {rcount->removeFromXML(); delete rcount; rcount = nullptr;}
    if(begin) {begin->removeFromXML(); delete begin; begin = nullptr;}
    if(end) {end->removeFromXML(); delete end; end = nullptr;}

}

SPRSeparateSmenaModel::SPRSeparateSmenaModel(QDomDocument *doc, int _separatorId, int _id, IModelVariable *parent, int _sid, int _uid):
    ISPRModelData(doc, parent),
    id(nullptr), uid(nullptr), sid(nullptr),
    mcount(nullptr), /*ecount(nullptr), */rcount(nullptr),
    begin(nullptr), end(nullptr), separatorId(_separatorId)
{
    if(doc){
        QString pref = tr("SEPARATOR[@ID=%1]/SMENA[#%2]").arg(_separatorId).arg(_id);
        id = new SPRVariable<int>(doc, pref + "[ID]", _id, this);

        QString pref2 = pref + tr("[@ID=%1]").arg(id->getData());
        uid = new SPRVariable<int>(doc, pref2 + "[UID]", _uid, this);
        sid = new SPRVariable<int>(doc, pref2 + "[SID]", _sid, this);

        mcount = new SPRVariable<int>(doc, pref2 + "[MCOUNT]", 0, this);
//        ecount = new SPRVariable<int>(doc, pref2 + "[ECOUNT]", 0, this);
        rcount = new SPRVariable<int>(doc, pref2 + "[RCOUNT]", 0, this);

        begin = new SPRDateTimeVariable(doc, pref2 + "[DT]", QDateTime::currentDateTime(), this);
        end = new SPRDateTimeVariable(doc, pref2 + "[END]", QDateTime::currentDateTime(), this);

        for(int sm=0; sm<rcount->getData(); sm++){
            SPRSeparateResumeItemModel *res = new SPRSeparateResumeItemModel(doc, _separatorId, id->getData(), sm, this);
            resume.append(res);
        }
        for(int m=0; m<mcount->getData(); m++){
            SPRSeparateMsgModel *mes = new SPRSeparateMsgModel(doc, _separatorId, id->getData(), m, this);
            messages.append(mes);
        }
//        for(int e=0; e<ecount->getData(); e++){
//            SPRSeparateMsgModel *err = new SPRSeparateMsgModel(doc, SPRSeparateMsgModel::tError, _separatorId, id->getData(), e, this);
//            errors.append(err);
//        }
    }
}

void SPRSeparateSmenaModel::setUid(int id){
    uid->setData(id);
}

void SPRSeparateSmenaModel::setSid(int id){
    sid->setData(id);
}

SPRSeparateSmenaModel::~SPRSeparateSmenaModel(){
    if(id) delete id;
    if(uid) delete uid;
    if(sid) delete sid;

    if(begin) delete begin;
    if(end) delete end;

    if(mcount) delete mcount;
//    if(ecount) delete ecount;
    if(rcount) delete rcount;
    while(messages.size() > 0){
      SPRSeparateMsgModel *mes = messages.takeLast();
      if(mes) delete mes;
    }
//    while(errors.size() > 0){
//      SPRSeparateMsgModel *err = messages.takeLast();
//      if(err) delete err;
//    }
    while (resume.size() >0) {
        SPRSeparateResumeItemModel *res = resume.takeLast();
        if(res) delete res;
    }
}

SPRSeparateResumeItemModel *SPRSeparateSmenaModel::addNewResume(){
    SPRSeparateResumeItemModel *res = new SPRSeparateResumeItemModel(doc, separatorId, id->getData(), resume.size(), this);
    resume.append(res);
    rcount->setData(resume.size());
    return res;
}

void *SPRSeparateSmenaModel::updateResumeValues(QVector<QVector<double>> *vars, int _id){
    SPRSeparateResumeItemModel *res = fingResumeById(_id);
    if(!res){
        res = addNewResume();
    }
    if(res){
        return res->updateResumeValues(vars);
    }
    return nullptr;
}

QDateTime SPRSeparateSmenaModel::getSmenaBegin(){
    return begin->getData();
}

QDateTime SPRSeparateSmenaModel::getSmenaEnd(){
    return end->getData();
}

SPRSeparateResumeItemModel *SPRSeparateSmenaModel::startNewSeparate()
{
    return addNewResume();
}

SPRSeparateResumeItemModel *SPRSeparateSmenaModel::fingResumeById(int _id){
    if(_id < 0){
        if(resume.size() == 0){
            return nullptr;
        } else {
            return resume.last();
        }
    } else {
        foreach(SPRSeparateResumeItemModel* res, resume){
            if(res->getId() == _id){
                return res;
            }
        }
        return  nullptr;
    }
}

void SPRSeparateSmenaModel::endSeparate(int _id)
{
    SPRSeparateResumeItemModel *res = fingResumeById(_id);
    if(res){
        res->endResume();
    }
}

void SPRSeparateSmenaModel::endSmena(){
//    if(model){
//        endSeparate(model);
        end->setData(QDateTime::currentDateTime());
        SPRSeparateResumeItemModel *res = fingResumeById(-1);
        if(res){
            res->endResume();
        }
//    }
}

bool SPRSeparateSmenaModel::isNullSmena(){
    if(messages.size() > 0){
        return false;
    }
    if(resume.size() == 0){
        return true;
    }
    return true;
}

QList<SPRSeparateResumeItemModel*> SPRSeparateSmenaModel::getAllResumes(){
    return resume;
}

int SPRSeparateSmenaModel::getId(){
    return id->getData();
}
int SPRSeparateSmenaModel::getUid(){
    return uid->getData();
}
int SPRSeparateSmenaModel::getSid(){
    return sid->getData();
}
int SPRSeparateSmenaModel::getMCount(){
    mcount->getData();
}
//int SPRSeparateSmenaModel::getECount(){
//    ecount->getData();
//}
int SPRSeparateSmenaModel::getRCount(){
    rcount->getData();
}

void SPRSeparateSmenaModel::setMCount(int _count)
{
    mcount->setData(_count);
}

//void SPRSeparateSmenaModel::setECount(int _count)
//{
//    ecount->setData(_count);
//}

void SPRSeparateSmenaModel::setRCount(int _count)
{
    rcount->setData(_count);
}
/**
 * @brief SPRSeparateLogsModel::SPRSeparateLogsModel
 */

SPRSeparateMsgModel *SPRSeparateSmenaModel::addLogMessage(int _separatorId, SPRTypeLogMessage _type, QString _msg, EnumCommands _command){
    SPRSeparateMsgModel *mod = new SPRSeparateMsgModel(doc, _separatorId, id->getData(), messages.size(), this, _type, _msg, _command);
    if(mod){
        messages.append(mod);
        mcount->setData(messages.size());
        return mod;
    }
    return nullptr;
}

QString SPRSeparatorLogsModel::getName() const
{
    return name;
}

void SPRSeparatorLogsModel::setName(const QString &value)
{
    name = value;
}

SPRSeparatorLogsModel::SPRSeparatorLogsModel(QDomDocument *_doc, int _separatorId, IModelVariable *parent):
    ISPRModelData(_doc, parent), id(nullptr), scount(nullptr), userSmenaModel(nullptr),
    currentUser(nullptr), currentSmena(nullptr), name("")
{
    init(_separatorId);
}

SPRSeparatorLogsModel::SPRSeparatorLogsModel(QString fName, int _separatorId, IModelVariable *parent):
    ISPRModelData (fName, parent), id(nullptr), scount(nullptr), userSmenaModel(nullptr),
    currentUser(nullptr), currentSmena(nullptr)
{
    init(_separatorId);
}

void SPRSeparatorLogsModel::setUserSmenaModel(SPRUsersSmenaModel *_model){
    userSmenaModel = _model;

    if(userSmenaModel){
//        connect(userSmenaModel, &SPRUsersSmenaModel::modelChanget, this, &SPRSeparatorLogsModel::onModelChanget);
        connect(userSmenaModel, &SPRUsersSmenaModel::currentSmenaChanget, this, &SPRSeparatorLogsModel::onSelectSmena);
        connect(userSmenaModel, &SPRUsersSmenaModel::currentUserChanget, this, &SPRSeparatorLogsModel::onSelectUser);
        currentSmena = userSmenaModel->currentSmena;
        currentUser = userSmenaModel->currentUser;

    } else {
        currentSmena = userSmenaModel->findSmenaById(0);
        currentUser = userSmenaModel->findUserById(0);
    }
}

SPRSeparateSmenaModel *SPRSeparatorLogsModel::addNewSmena()
{
    uint uid = currentUser ? currentUser->id->getData() : 0;
    uint sid = currentSmena ? currentSmena->id->getData() : 0;

    SPRSeparateSmenaModel *curr = findSmenaById(-1);
    if(curr){
        if(!curr->isNullSmena()){
            if(curr->getSmenaBegin() == curr->getSmenaEnd()){
                curr->endSmena();
            }
        } else {
            deleteSmena(curr);
        }
    }

    SPRSeparateSmenaModel *sm = new SPRSeparateSmenaModel(doc, id->getData(), smenas.size(), this, sid, uid);
    if(sm){
        smenas.append(sm);
        scount->setData(smenas.size());
    }
    saveAsAllData(fileName);
    return sm;
}

SPRSeparateSmenaModel *SPRSeparatorLogsModel::findSmenaById(int id){
    if(id < 0){
        if(smenas.size() > 0){
            return smenas.last();
        }
    } else {
        foreach(SPRSeparateSmenaModel *s, smenas){
            if(s->getId() == id){
                return  s;
            }
        }
    }
    return nullptr;
}

QList<SPRSeparateSmenaModel*> SPRSeparatorLogsModel::findBefore(QDateTime dt){
    QList<SPRSeparateSmenaModel*> res;
    if(doc){
        foreach(SPRSeparateSmenaModel *sm, smenas){
//            dt.msecsTo(sm->g)
        }
    }
    return res;
}

void SPRSeparatorLogsModel::endSmena(int id){
    SPRSeparateSmenaModel *sm = findSmenaById(id);
    if(sm){
        sm->endSmena();
    }

}

SPRSeparateMsgModel *SPRSeparatorLogsModel::addLogMessage(TCPCommand *_command, SPRTypeLogMessage type, QString msg)
{
    SPRSeparateSmenaModel *sm = findSmenaById();
    if(!sm){
        sm = addNewSmena();
    }
    if(sm){
        EnumCommands ecom = (_command) ? _command->getCommand() : nocommand;
        SPRSeparateMsgModel *mod = sm->addLogMessage(id->getData(), type, msg, ecom);
        if(mod){
            saveAsAllData(fileName);
            fullTableViewMessagesData();
            emit echoLogMessage(mod, this);
        }
    }
}



QList<SPRSeparateSmenaModel *> SPRSeparatorLogsModel::getSmenas()
{
    return smenas;
}

void SPRSeparatorLogsModel::separatorRemoveFromXML(){
    foreach(SPRSeparateSmenaModel *sm, smenas){
        if(sm){
            sm->smenaRemoveFromXML();
            delete sm;
            sm = nullptr;
        }
    }
}

void SPRSeparatorLogsModel::init(int _separatorId)
{
    if(doc){
        QString pref = "SEPARATOR";
        id = new SPRVariable<int>(doc, pref+"[ID]", _separatorId, this);

        pref = pref + tr("[@ID=%1]").arg(id->getData());
        scount = new SPRVariable<int>(doc, pref + "[SCOUNT]", 0, this);
        for(int s=0; s<scount->getData(); s++){
            SPRSeparateSmenaModel *sm = new SPRSeparateSmenaModel(doc, id->getData(), s, this);
            smenas.append(sm);
        }
        addNewSmena();
   }
}

SPRSeparateResumeItemModel *SPRSeparatorLogsModel::findResumeById(int id)
{
    if(id < 0){
        SPRSeparateSmenaModel *sm = findSmenaById(id); // last smena
        if(sm){
            return sm->fingResumeById(id); // last resume
        } else {
            return nullptr;
        }
    } else {
        foreach(SPRSeparateSmenaModel *sm, smenas){
            SPRSeparateResumeItemModel *res = sm->fingResumeById(id);
            if(res){
                return res;
            }
        }
        return nullptr;
    }
}

void SPRSeparatorLogsModel::onSeparateDataReady(SPRHistoryModel2 *hist){
    if(hist){
        QVector<SPRHistoryModel2::histDataOneThread> *diff = hist->getDiffRecords();
        QVector<QVector<double>> vlast((*diff).size(), QVector<double>(3, 0));
        void *res = nullptr;
        for(int i=0; i<diff->size(); i++){
            vlast[i][0] = (*diff)[i].data.i_prd[0];
            vlast[i][1] = (*diff)[i].data.i_prd[1];
            vlast[i][2] = (*diff)[i].data.i_prd[2];
        }
        SPRSeparateSmenaModel *sm = findSmenaById(-1);
        if(!sm){
            sm = addNewSmena();
        }
        res = sm->updateResumeValues(&vlast);
        if(res){
            fullTableViewResumeData();
            emit doStore();
            saveAsAllData(fileName);
            emit resumesTableChanget();
        }
    }
}

void SPRSeparatorLogsModel::onSelectUser(SPRUserModel *mod)
{
    if(mod){
        currentUser = mod;
        SPRSeparateSmenaModel *sm = findSmenaById();
        if(!sm){
    //        onSeparateDataReady();
            emit doStore();
            addNewSmena();
        } else {
            SPRSeparateResumeItemModel *res = sm->fingResumeById();
            if(res && !res->isNullResume()){
    //            onSeparateDataReady();
                emit doStore();
                addNewSmena();
            } else {
                sm->setUid(mod->id->getData());
            }
        }
    }
}

void SPRSeparatorLogsModel::onSelectSmena(SPRSmenaModel *mod)
{
    if(mod){
        currentSmena = mod;
        SPRSeparateSmenaModel *sm = findSmenaById();
        if(!sm){
    //        onSeparateDataReady();
            emit doStore();
            addNewSmena();
        } else {
            SPRSeparateResumeItemModel *res = sm->fingResumeById();
            if(res && !res->isNullResume()){
    //            onSeparateDataReady();
                emit doStore();
                addNewSmena();
            } else {
                sm->setSid(mod->id->getData());
            }
        }
    }
}

void SPRSeparatorLogsModel::fullTableViewResumeData(){
    SPRSeparateSmenaModel *smLast = findSmenaById();
    SPRSeparateResumeItemModel *resLast = nullptr;
    if(smLast){
        resLast = smLast->fingResumeById();
    }
    foreach(SPRSeparateSmenaModel *sm, smenas){
        if(sm){
            foreach(SPRSeparateResumeItemModel *res, sm->getAllResumes()){
                ToTableViewResumeData *findRow = findFromTableByResume(res);
                bool newRow = false;
                if(!findRow){
                    newRow = true;
                    findRow = new ToTableViewResumeData();
                    findRow->res = res;
                    findRow->dtBegin = sm->getSmenaBegin();
                    findRow->dtEnd = (res->getBegin() == res->getEnd() || res == findResumeById(-1)) ?
                                QDateTime::currentDateTime() :
                                res->getEnd();

                    findRow->strResume = res->toTableViewStringResume();
                    findRow->prAllResume = res->toTableViewPercentResume();
                    findRow->countAllResume = res->toTableViewAllCountResume();

                    SPRUserModel *user = userSmenaModel ? userSmenaModel->findUserById(sm->getUid()) : nullptr;
                    findRow->userNicName = user ? user->nik->getData() : "";

                    SPRSmenaModel *smena = userSmenaModel ? userSmenaModel->findSmenaById(sm->getSid()) : nullptr;
                    findRow->smenaName = smena ? smena->name->getData() : "";
                }
                if(newRow){
                    tableViewResumeData.push_front(findRow);
                }
                if(res == resLast){
                    findRow->dtEnd = (res->getBegin() == res->getEnd() || res == findResumeById(-1)) ?
                                QDateTime::currentDateTime() :
                                res->getEnd();

                    findRow->strResume = res->toTableViewStringResume();
                    findRow->prAllResume = res->toTableViewPercentResume();
                    findRow->countAllResume = res->toTableViewAllCountResume();
                }
            }
        }
    }
}

void SPRSeparatorLogsModel::fullTableViewMessagesData()
{

    foreach(SPRSeparateSmenaModel *sm, smenas){
        foreach(SPRSeparateMsgModel *msg, sm->messages){
            ToTableViewMessagesData *findRow = findFromTableByMessage(msg, &tableViewMessageData);
            if(!findRow){
                findRow = new ToTableViewMessagesData();
                findRow->dt = msg->dt->getData();
                QString nikName = "";
                QString smenaName = "";
                if(userSmenaModel){
                    SPRUserModel *user = userSmenaModel->findUserById(sm->uid->getData());
                    nikName = user ? user->nik->getData() : "";

                    SPRSmenaModel *usSmena = userSmenaModel->findSmenaById(sm->sid->getData());
                    smenaName = usSmena ? usSmena->name->getData() : "";
                }
                findRow->userNicName = nikName;
                findRow->smenaName = smenaName;
                findRow->_commandId = msg->comId->getData();
                findRow->msg = msg;
                findRow->type = msg->type->getData();
                QString text;
                if(msg->comId->getData() == nocommand){
                    text = msg->msgText->getData();
                } else {
                    if(mapTCPCommandMessages.contains(msg->comId->getData())){
                        text = mapTCPCommandMessages[msg->comId->getData()];
                    }
                    text += msg->msgText->getData();
                }
                findRow->messageText = text;

                tableViewMessageData.push_front(findRow);

//                emit echoLogMessage(msg, this);
            }
        }
    }
}

QVector<SPRSeparatorLogsModel::ToTableViewResumeData*> *SPRSeparatorLogsModel::getTableViewResumeData()
{
    return &tableViewResumeData;
}

QVector<SPRSeparatorLogsModel::ToTableViewMessagesData*> *SPRSeparatorLogsModel::getTableViewMessagesData()
{
    return &tableViewMessageData;
}

SPRSeparatorLogsModel::ToTableViewResumeData *SPRSeparatorLogsModel::findFromTableByResume(SPRSeparateResumeItemModel *id)
{
    for(int i=0; i<tableViewResumeData.size();i++){
        if(tableViewResumeData[i]->res == id){
            return tableViewResumeData[i];
        }
    }
    return nullptr;
}

SPRSeparatorLogsModel::ToTableViewMessagesData *SPRSeparatorLogsModel::findFromTableByMessage(SPRSeparateMsgModel *msg, QVector<ToTableViewMessagesData *> *sTable){
    for(int i=0; i < sTable->size(); i++){
        if(sTable->at(i)->msg == msg){
            return sTable->at(i);
        }
    }
    return nullptr;
}

SPRMainModel *SPRSeparatorLogsModel::getMainModel()
{
    return mainModel;
}

void SPRSeparatorLogsModel::deleteSmena(SPRSeparateSmenaModel *sm)
{
    if(sm){
        sm->removeFromXML();
        smenas.removeAll(sm);
        delete sm;
    }
}
