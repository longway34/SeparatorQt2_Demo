#include "spruserssmenamodel.h"

/**
 * @brief SPRSmenaModel::SPRSmenaModel
 * @param _doc
 * @param _id
 * @param _mparent
 */
SPRSmenaModel::SPRSmenaModel(QDomDocument *_doc, uint _id, IModelVariable *_mparent):
    ISPRModelData(_doc, _mparent),
    id(nullptr), name(nullptr), begin(nullptr), end(nullptr)
{
    QString xpath_prefix = tr("USERS/SHIFT[@ID=%1]").arg(_id);

    id = new SPRVariable<uint>(doc, xpath_prefix+"[ID]", _id, this);
    name = new SPRQStringVariable(doc, xpath_prefix + "[NAME]", "", this);
    begin = new SPRTimeVariable(doc, xpath_prefix + "[START]", QTime(0,0), this, QString("HH:mm"));
    end = new SPRTimeVariable(doc, xpath_prefix + "[FINISH]", QTime(0,0), this, QString("HH:mm"));
}

SPRSmenaModel::~SPRSmenaModel()
{
    if(id) delete id;
    if(name) delete name;
    if(begin) delete begin;
    if(end) delete end;
}

void SPRSmenaModel::setData(uint _id, QString _name, QTime _begin, QTime _end)
{
    this->blockSignals(true);
    id->setData(_id); name->setData(_name); begin->setData(_begin); end->setData(_end);
    this->blockSignals(false);

    saveAsAllData();
    emit modelChanget(this);
}

/**
 * @brief SPRUserModel::SPRUserModel
 * @param doc
 * @param mparent
 */

SPRUserModel::SPRUserModel(QDomDocument *_doc, uint _id, IModelVariable *_mparent):
    ISPRModelData(_doc, _mparent),
    id(nullptr), nik(nullptr), family(nullptr), name(nullptr), patronymic(nullptr), avatar(nullptr)
{
    QString xpath_prefix = tr("USERS/USER[@ID=%1]").arg(_id);

    id = new SPRVariable<uint>(doc, xpath_prefix+"[ID]", _id, this);
    nik = new SPRQStringVariable(doc, xpath_prefix+"[NIK]", "anonymouse", this);
    family = new SPRQStringVariable(doc, xpath_prefix+"[FAMILY]", "", this);
    name = new SPRQStringVariable(doc, xpath_prefix + "[NAME]", "", this);
    patronymic = new SPRQStringVariable(doc, xpath_prefix + "[PATRONYMIC]", "", this);
    avatar = new SPRQStringVariable(doc, xpath_prefix + "[AVATAR]", DEF_AVATAR_ICON, this);

}
SPRUserModel::~SPRUserModel(){
    if(id) delete id;
    if(nik) delete nik;
    if(family) delete family;
    if(patronymic) delete patronymic;
    if(avatar) delete avatar;
}

void SPRUserModel::setData(uint _id, QString _nik, QString _family, QString _name, QString _patronymic, QString _avatar)
{
    this->blockSignals(true);
    id->setData(_id); nik->setData(_nik); family->setData(_family); name->setData(_name); patronymic->setData(_patronymic); avatar->setData(_avatar);
    this->blockSignals(false);

    saveAsAllData();
    emit modelChanget(this);
}
/**
 * @brief SPRUsersSmenaModel::SPRUsersSmenaModel
 */
SPRSmenaModel *SPRUsersSmenaModel::getCurrentSmena() const
{
    return currentSmena;
}

SPRUserModel *SPRUsersSmenaModel::getCurrentUser() const
{
    return currentUser;
}

void SPRUsersSmenaModel::setCurrentSmena(SPRSmenaModel *value)
{
    currentSmena = value;
    emit currentSmenaChanget(currentSmena);
    emit modelChanget(this);
}

void SPRUsersSmenaModel::setCurrentUser(SPRUserModel *value)
{
    currentUser = value;
    emit currentUserChanget(currentUser);
    emit modelChanget(this);
}

SPRUsersSmenaModel::SPRUsersSmenaModel(QDomDocument *_doc, IModelVariable *_mparent):
    ISPRModelData (_doc, _mparent), currentSmena(nullptr), currentUser(nullptr)
{
    usersCount = new SPRVariable<uint>(doc, QString("USERS[UCOUNT]"), 1, this);
    smenaCount = new SPRVariable<uint>(doc, QString("USERS[SCOUNT]"), 1, this);
    uint c = usersCount->getData();
    for(uint u=0; u<c; u++){
        SPRUserModel *mod = new SPRUserModel(doc, u, this);
        if(mod){
            users.append(mod);
        }
    }
    uint s = smenaCount->getData();
    for(uint sm=0; sm<s; sm++){
        SPRSmenaModel *mod = new SPRSmenaModel(doc, sm, this);
        if(mod){
            smenas.append(mod);
        }
    }
}

SPRUsersSmenaModel::~SPRUsersSmenaModel(){
    qDeleteAll(users);
    if(usersCount) delete usersCount;

    qDeleteAll(smenas);
    if(smenaCount) delete smenaCount;
}

void SPRUsersSmenaModel::addSmena(QString _name, QTime _begin, QTime _end){
    uint newId = 0;
    for(int i=0; i<smenas.size(); i++){
        if(newId < smenas[i]->id->getData()){ newId = smenas[i]->id->getData();}
    }
    newId++;

    SPRSmenaModel *mod = new SPRSmenaModel(doc, newId, this);
    mod->setData(newId, _name, _begin, _end);
    smenas.append(mod);

    smenaCount->setData(smenas.size());
    saveAsAllData();
    emit modelChanget(mod);

}

void SPRUsersSmenaModel::deleteSmena(IModelVariable *smena)
{
    SPRSmenaModel *mod = qobject_cast<SPRSmenaModel*>(smena);
    if(mod){
        smenas.removeAll(mod);
        smenaCount->setData(smenas.size());
        saveAsAllData();
        emit modelChanget(this);
    }
}

void SPRUsersSmenaModel::addUser(QString _nik, QString _family, QString _name, QString _patronymic, QString _avatar)
{
    uint newId = 0;
    for(int i=0; i<users.size(); i++){
        if(newId < users[i]->id->getData()){ newId = users[i]->id->getData();}
        if(_nik == users[i]->nik->getData()) {_nik = _nik + QString::number(abs(qrand()%10));}
    }
    newId++;
    SPRUserModel *mod = new SPRUserModel(doc, newId, this);
    mod->setData(newId, _nik, _family, _name, _patronymic, _avatar);
    users.append(mod);

    usersCount->setData(users.size());
    saveAsAllData();
    emit modelChanget(mod);
}

void SPRUsersSmenaModel::deleteUser(IModelVariable *user)
{
    SPRUserModel *mod = qobject_cast<SPRUserModel*>(user);
    if(mod && users.contains(mod) && mod->id->getData() != 0){
        users.removeAll(mod);
        delete mod;

        usersCount->setData(users.size());
        saveAsAllData();
        emit modelChanget(this);
    }

}

SPRUserModel *SPRUsersSmenaModel::findUserById(int id)
{
    foreach(SPRUserModel *mod, users){
        if(mod->id->getData() == id){
            return mod;
        }
    }
    return nullptr;
}

SPRSmenaModel *SPRUsersSmenaModel::findSmenaById(int id)
{
    foreach(SPRSmenaModel *mod, smenas){
        if(mod->id->getData() == id){
            return mod;
        }
    }
    return nullptr;
}



void SPRUsersSmenaModel::onModelChanget(IModelVariable *send)
{
    IModelVariable::onModelChanget(send);
    saveAsAllData();
}

