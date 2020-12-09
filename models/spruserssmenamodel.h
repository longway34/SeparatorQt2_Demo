#ifndef SPRUSERSSMENAMODEL_H
#define SPRUSERSSMENAMODEL_H

#include <QObject>
#include <QWidget>

#include <QDomDocument>
#include "models/isprmodeldata.h"

#include "variables/sprqstringvariable.h"
#include "variables/sprvariable.h"
#include "variables/sprdatetimevariable.h"

#define DEF_AVATAR_ICON    ":/smiles/icons/Smiles/831758b0039b.png"


class SPRSmenaModel :public ISPRModelData
{
    Q_OBJECT

public:
    SPRSmenaModel(QDomDocument *_doc, uint _id, IModelVariable* _mparent = nullptr);
    ~SPRSmenaModel();

    void setData(uint _id, QString _name, QTime _begin, QTime _end);

    SPRVariable<uint> *id;
    SPRQStringVariable *name;
    SPRTimeVariable *begin;
    SPRTimeVariable *end;
};

class SPRUserModel : public ISPRModelData
{
    Q_OBJECT

public:
    SPRUserModel(QDomDocument *_doc, uint _id, IModelVariable *_mparent = nullptr);
    ~SPRUserModel();

    void setData(uint _id, QString _nik, QString _family="", QString _name="", QString _patronymic="", QString _avatar = DEF_AVATAR_ICON);
    SPRVariable<uint> *id;
    SPRQStringVariable *nik;
    SPRQStringVariable *family;
    SPRQStringVariable *name;
    SPRQStringVariable *patronymic;
    SPRQStringVariable *avatar;
};


class SPRUsersSmenaModel : public ISPRModelData
{
    Q_OBJECT

public:
    SPRVariable<uint> *usersCount;
    SPRVariable<uint> *smenaCount;
    QList<SPRUserModel*> users;
    QList<SPRSmenaModel*> smenas;

    SPRSmenaModel *currentSmena;
    SPRUserModel *currentUser;

    SPRUsersSmenaModel(QDomDocument *_doc, IModelVariable *_mparent);
    ~SPRUsersSmenaModel();

    void addUser(QString _nik, QString _family="", QString _name="", QString _patronymic="", QString _avatar = DEF_AVATAR_ICON);
    void deleteUser(IModelVariable *user);

    SPRUserModel *findUserById(int id);
    SPRSmenaModel *findSmenaById(int id);

    // IModelVariable interface
    void addSmena(QString _name, QTime _begin, QTime _end);
    void deleteSmena(IModelVariable *smena);
    SPRSmenaModel *getCurrentSmena() const;

    SPRUserModel *getCurrentUser() const;

    void setCurrentSmena(SPRSmenaModel *value);

    void setCurrentUser(SPRUserModel *value);

public slots:
    virtual void onModelChanget(IModelVariable *send);

signals:
    void currentUserChanget(SPRUserModel*);
    void currentSmenaChanget(SPRSmenaModel*);
};

#endif // SPRUSERSSMENAMODEL_H
