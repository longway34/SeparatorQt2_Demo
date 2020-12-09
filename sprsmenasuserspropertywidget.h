#ifndef SPRSMENASUSERSPROPERTYWIDGET_H
#define SPRSMENASUSERSPROPERTYWIDGET_H

#include <QDomDocument>

#include "ui_sprsmenasuserspropertywidget.h"

#include "models/spruserssmenamodel.h"
#include "isprwidget.h"

class SPRSmenasUsersPropertyWidget : public QWidget, public ISPRWidget
{
    Q_OBJECT

    SPRUsersSmenaModel *model;
    QDomDocument docUsers;

    SPRUserModel *selectedUser;
    SPRSmenaModel *selectedSmena;
public:
    Ui::SPRSmenasUsersPropertyWidget ui;
    explicit SPRSmenasUsersPropertyWidget(QWidget *parent = nullptr);

    virtual void setModelData(IModelVariable* _model);

    virtual void widgetsShow();
public:
    virtual ISPRModelData *getModelData();

    SPRUserModel *getSelectedUser() const;

    void setSelectedUser(SPRUserModel *value);

    void setSelectedSmena(SPRSmenaModel *value);
protected:
    virtual void onModelChanget(IModelVariable *);
    void setUserTableRow(int r, SPRUserModel *mod);

    void setSmenaTableRow(int r, SPRSmenaModel *mod);
protected slots:
    void onAvatarChanget(int);
    void onUsersItemChanget(int row, int col);
    void addUser(bool);
    void onDeleteUser(IModelVariable *user);
    void onNikNameEditing();
    void onCurrentUserItemChanget(QTableWidgetItem *current, QTableWidgetItem *);
    void onSelectUserButtomClicked(bool);
    void onCurrentUserChanget(SPRUserModel *user);

    void onSmenaItemChanget(int row, int col);
    void addSmena(bool);
    void onSmenaNameEditing();
    void onSelectSmenaButtomClicked(bool);
    void onCurrentSmenaItemChanget(QTableWidgetItem *current, QTableWidgetItem *);
    void onDeleteSmena(IModelVariable *smena);
    void onSmenaTimesChanget();
    void onCurrentSmenaChanget(SPRSmenaModel *smena);
signals:
    void selectUser(SPRUserModel*);
    void selectSmena(SPRSmenaModel*);
};

#endif // SPRSMENASUSERSPROPERTYWIDGET_H
