#include "sprsmenasuserspropertywidget.h"

#include <QFile>
#include <QFileInfo>
#include <QTimeEdit>

#include "firstcollumn2.h"
#include "sprsmilecombobox.h"

SPRUserModel *SPRSmenasUsersPropertyWidget::getSelectedUser() const
{
    return selectedUser;
}

void SPRSmenasUsersPropertyWidget::setSelectedSmena(SPRSmenaModel *value){
    if(value != selectedSmena){
        model->setCurrentSmena(value);
//        emit selectSmena(value);
    }
    selectedSmena = value;
    if(model && value){
        for(int r=0; r<ui.twSmenas->rowCount(); r++){
            FirstCollumn2 *fc = qobject_cast<FirstCollumn2*>(ui.twSmenas->cellWidget(r,0));
            if(fc){
                SPRSmenaModel *mod = qobject_cast<SPRSmenaModel*>(fc->getModelPrt());
                if(mod == value){
                    ui.twSmenas->setCurrentCell(r, 1);
                }
            }
        }
    }

}

void SPRSmenasUsersPropertyWidget::setSelectedUser(SPRUserModel *value)
{
    if(value != selectedUser){
        model->setCurrentUser(value);
//        emit selectUser(value);
    }
    selectedUser = value;
    if(model && value){
        for(int r=0; r<ui.twUsers->rowCount(); r++){
            FirstCollumn2 *fc = qobject_cast<FirstCollumn2*>(ui.twUsers->cellWidget(r,0));
            if(fc){
                SPRUserModel *mod = qobject_cast<SPRUserModel*>(fc->getModelPrt());
                if(mod == value){
                    ui.twUsers->setCurrentCell(r, 1);
                }
            }
        }
    }

}

SPRSmenasUsersPropertyWidget::SPRSmenasUsersPropertyWidget(QWidget *parent) :
    QWidget(parent), model(nullptr), selectedUser(nullptr), selectedSmena(nullptr)
{
    ui.setupUi(this);
    QString fNameIn = "Users.xml";
    bool isCreating = false;
    if(!QFileInfo::exists(fNameIn)){
        fNameIn = ":/defUsers/defUsers.xml";
        isCreating = true;
    }
    ISPRWidget::setDoc(fNameIn);

    setModelData(new SPRUsersSmenaModel(doc, nullptr));
    if(isCreating){
        model->saveAsAllData(QString("Users.xml"));
//        QFile out("Users.xml");
//        if(out.open(QIODevice::WriteOnly)){
//        QTextStream stream( &out );
//          stream << doc->toString();
//          out.close();
//        }
    }

    model->setFileName("Users.xml");
    ui.twUsers->setSizeAdjustPolicy(QAbstractItemView::SizeAdjustPolicy::AdjustToContents);
    ui.twUsers->horizontalHeader()->setSizeAdjustPolicy(QHeaderView::AdjustToContents);
    ui.twUsers->verticalHeader()->setSizeAdjustPolicy(QHeaderView::AdjustToContents);
    ui.twUsers->verticalHeader()->setVisible(false);
    ui.twUsers->setSelectionMode(QAbstractItemView::SingleSelection);
    ui.twUsers->setSelectionBehavior(QAbstractItemView::SelectRows);

    ui.twSmenas->setSizeAdjustPolicy(QAbstractItemView::SizeAdjustPolicy::AdjustToContents);
    ui.twSmenas->horizontalHeader()->setSizeAdjustPolicy(QHeaderView::AdjustToContents);
    ui.twSmenas->verticalHeader()->setSizeAdjustPolicy(QHeaderView::AdjustToContents);
    ui.twSmenas->verticalHeader()->setVisible(false);
    ui.twSmenas->setSelectionMode(QAbstractItemView::SingleSelection);
    ui.twSmenas->setSelectionBehavior(QAbstractItemView::SelectRows);

}

void SPRSmenasUsersPropertyWidget::setModelData(IModelVariable *_model)
{
    if(model != _model && _model){
        model = qobject_cast<SPRUsersSmenaModel*>(_model);
        if(model){
            connect(model, &IModelVariable::modelChanget, this, &SPRSmenasUsersPropertyWidget::onModelChanget);

            connect(model, &SPRUsersSmenaModel::currentSmenaChanget, this, &SPRSmenasUsersPropertyWidget::onCurrentSmenaChanget);
            connect(model, &SPRUsersSmenaModel::currentUserChanget, this, &SPRSmenasUsersPropertyWidget::onCurrentUserChanget);
//            selectedUser = model->users.at(0);
        }

        connect(ui.twUsers, &QTableWidget::cellChanged, this, &SPRSmenasUsersPropertyWidget::onUsersItemChanget);
        connect(ui.twUsers, &QTableWidget::currentItemChanged, this, &SPRSmenasUsersPropertyWidget::onCurrentUserItemChanget);

        connect(ui.twSmenas, &QTableWidget::cellChanged, this, &SPRSmenasUsersPropertyWidget::onSmenaItemChanget);
        connect(ui.twSmenas, &QTableWidget::currentItemChanged, this, &SPRSmenasUsersPropertyWidget::onCurrentSmenaItemChanget);


        connect(ui.bAddUser, &QPushButton::clicked, this, &SPRSmenasUsersPropertyWidget::addUser);
        connect(ui.leNik, &QLineEdit::editingFinished, this, &SPRSmenasUsersPropertyWidget::onNikNameEditing);
        connect(ui.bSelectUser, &QPushButton::clicked, this, &SPRSmenasUsersPropertyWidget::onSelectUserButtomClicked);

        connect(ui.bAddSmena, &QPushButton::clicked, this, &SPRSmenasUsersPropertyWidget::addSmena);
        connect(ui.leSmenaName, &QLineEdit::editingFinished, this, &SPRSmenasUsersPropertyWidget::onSmenaNameEditing);
        connect(ui.bSelectSmena, &QPushButton::clicked, this, &SPRSmenasUsersPropertyWidget::onSelectSmenaButtomClicked);

        widgetsShow();
    }

}

void SPRSmenasUsersPropertyWidget::onCurrentUserChanget(SPRUserModel *user){
    for(int row=0; row < ui.twUsers->rowCount(); row++){
        FirstCollumn2 *fc = qobject_cast<FirstCollumn2*>(ui.twUsers->cellWidget(row, 0));
        if(fc && fc->getModelPrt() == user){
            ui.twUsers->blockSignals(true);
            ui.twUsers->setCurrentCell(row, 1);
            ui.twUsers->blockSignals(false);
        }
    }
}

void SPRSmenasUsersPropertyWidget::onCurrentSmenaChanget(SPRSmenaModel *smena){
    for(int row=0; row < ui.twSmenas->rowCount(); row++){
        FirstCollumn2 *fc = qobject_cast<FirstCollumn2*>(ui.twSmenas->cellWidget(row, 0));
        if(fc && fc->getModelPrt() == smena){
            ui.twSmenas->blockSignals(true);
            ui.twSmenas->setCurrentCell(row, 1);
            ui.twSmenas->blockSignals(false);
        }
    }

}

void SPRSmenasUsersPropertyWidget::onSelectSmenaButtomClicked(bool){
    model->setCurrentSmena(selectedSmena);
//    emit selectSmena(selectedSmena);
    close();
}
void SPRSmenasUsersPropertyWidget::onSelectUserButtomClicked(bool){
    model->setCurrentUser(selectedUser);
//    emit selectUser(selectedUser);
    close();
}

void SPRSmenasUsersPropertyWidget::onCurrentSmenaItemChanget(QTableWidgetItem *current, QTableWidgetItem *){
    if(current){
        int row = ui.twSmenas->currentIndex().row();
        FirstCollumn2 *fc = qobject_cast<FirstCollumn2*>(ui.twSmenas->cellWidget(row, 0));
        if(fc){
            SPRSmenaModel *mod = qobject_cast<SPRSmenaModel*>(fc->getModelPrt());
            if(mod){
                selectedSmena = mod;
//                setSelectedSmena(mod);
            }
        }
    } else {
        selectedSmena = model->smenas.first();
//        setSelectedSmena(model->smenas.first());
    }

}

void SPRSmenasUsersPropertyWidget::onCurrentUserItemChanget(QTableWidgetItem *current, QTableWidgetItem *){
    if(current){
        int row = ui.twUsers->currentIndex().row();
        FirstCollumn2 *fc = qobject_cast<FirstCollumn2*>(ui.twUsers->cellWidget(row, 0));
        if(fc){
            SPRUserModel *mod = qobject_cast<SPRUserModel*>(fc->getModelPrt());
            if(mod){
                selectedUser = mod;
//                setSelectedUser(mod);
            }
        }
    } else {
        selectedUser = model->users.first();
//        setSelectedUser(model->users.first());
    }
}

void SPRSmenasUsersPropertyWidget::onSmenaNameEditing(){
    if(ui.leSmenaName->text().isEmpty()){
        ui.bAddSmena->setEnabled(false);
    } else {
        ui.bAddSmena->setEnabled(true);
    }
}

void SPRSmenasUsersPropertyWidget::onNikNameEditing(){
    if(ui.leNik->text().isEmpty()){
        ui.bAddUser->setEnabled(false);
    } else {
        ui.bAddUser->setEnabled(true);
    }
}

void SPRSmenasUsersPropertyWidget::onSmenaItemChanget(int row, int col){
    FirstCollumn2 *fc = qobject_cast<FirstCollumn2*>(ui.twSmenas->cellWidget(row, 0));
    if(fc){
        SPRSmenaModel *mod = qobject_cast<SPRSmenaModel*>(fc->getModelPrt());
        if(mod){
            uint id = mod->id->getData();
            QString name = ui.twSmenas->item(row, 1)->text();
            QTime begin = qobject_cast<QTimeEdit*>(ui.twSmenas->cellWidget(row, 2))->time();
            QTime end = qobject_cast<QTimeEdit*>(ui.twSmenas->cellWidget(row, 3))->time();

            mod->setData(id, name, begin, end);
        }
    }

}
void SPRSmenasUsersPropertyWidget::onUsersItemChanget(int row, int col)
{
    FirstCollumn2 *fc = qobject_cast<FirstCollumn2*>(ui.twUsers->cellWidget(row, 0));
    if(fc){
        SPRUserModel *user = qobject_cast<SPRUserModel*>(fc->getModelPrt());
        if(user){
            uint id = user->id->getData();
            QString nik = ui.twUsers->item(row, 1)->text();
            QString family = ui.twUsers->item(row, 2)->text();
            QString name = ui.twUsers->item(row, 3)->text();
            QString patronymic = ui.twUsers->item(row, 4)->text();
            QString avatar = user->avatar->getData();

            user->setData(id, nik, family, name, patronymic, avatar);
        }
    }
}
void SPRSmenasUsersPropertyWidget::addSmena(bool){
    QString name = ui.leSmenaName->text();
    QTime begin = ui.leBegin->time();
    QTime end = ui.leEnd->time();

    if(model){
        model->addSmena(name, begin, end);
    }
    ui.leSmenaName->clear(); ui.leBegin->setTime(QTime(0,0)); ui.leEnd->setTime(QTime(0,0));
}

void SPRSmenasUsersPropertyWidget::addUser(bool)
{
    QString nik = ui.leNik->text();
    QString family = ui.leFamily->text();
    QString name = ui.leName->text();
    QString patronymic = ui.leParonymic->text();
    QString avatar = ui.cbAvatar->currentData().value<QString>();

    if(model){
        model->addUser(nik, family, name, patronymic, avatar);
    }
    ui.leNik->clear(); ui.leFamily->clear(); ui.leName->clear(); ui.leParonymic->clear();
}

void SPRSmenasUsersPropertyWidget::onDeleteSmena(IModelVariable *smena){
    if(model){
        SPRSmenaModel *mod = qobject_cast<SPRSmenaModel*>(smena);
        if(mod){
            ui.leSmenaName->setText(mod->name->getData());
            ui.leBegin->setTime(mod->begin->getData());
            ui.leEnd->setTime(mod->end->getData());

            ui.twSmenas->setCurrentCell(0,1);
            model->deleteSmena(mod);
        }
    }

}

void SPRSmenasUsersPropertyWidget::onDeleteUser(IModelVariable *user)
{
    if(model){
        SPRUserModel *mod = qobject_cast<SPRUserModel*>(user);
        if(mod){
            ui.leNik->setText(mod->nik->getData());
            ui.leFamily->setText(mod->family->getData());
            ui.leName->setText(mod->name->getData());
            ui.leParonymic->setText(mod->patronymic->getData());
            int ind = ui.cbAvatar->findData(mod->avatar->getData());
            if(!(ind < 0)){
                ui.cbAvatar->setCurrentIndex(ind);
            }
            ui.twUsers->setCurrentCell(0,1);
            model->deleteUser(mod);
        }
    }
}

void SPRSmenasUsersPropertyWidget::widgetsShow()
{
    if(model){
        int rmCount = model->usersCount->getData();
        ui.twUsers->setRowCount(rmCount);

        int rc = 0; int sr = -1;
        foreach(SPRUserModel *user, model->users){
            setUserTableRow(rc, user);
            if(user == model->currentUser){
                sr = rc;
            }
            rc++;
        }
        if(sr >= 0){
            ui.twUsers->setCurrentCell(sr, 1);
        }

        rmCount = model->smenaCount->getData();
        ui.twSmenas->setRowCount(rmCount);
        rc = 0; sr = -1;
        foreach(SPRSmenaModel *smena, model->smenas){
            setSmenaTableRow(rc, smena);
            if(smena == model->currentSmena){
                sr = rc;
            }
            rc++;
        }
        if(sr >= 0){
            ui.twSmenas->setCurrentCell(sr, 1);
        }

        ui.twUsers->resizeRowsToContents();
        ui.twUsers->resizeColumnsToContents();

        if(!ui.leNik->text().isEmpty()){
            ui.bAddUser->setEnabled(true);
        } else {
            ui.bAddUser->setEnabled(false);
        }


    }
}

void SPRSmenasUsersPropertyWidget::setSmenaTableRow(int r, SPRSmenaModel *mod){
    FirstCollumn2 *fc = qobject_cast<FirstCollumn2*>(ui.twSmenas->cellWidget(r, 0));
    if(!fc){
        fc = new FirstCollumn2(nullptr);
        ui.twSmenas->setCellWidget(r,0, fc);
        connect(fc, &FirstCollumn2::deleteRow, this, &SPRSmenasUsersPropertyWidget::onDeleteSmena);
    }
    if(r == 0){
        fc->setParams(r, QString::number(mod->id->getData()), QColor(Qt::black), false, false, false, false);
    } else {
        fc->setParams(r, QString::number(mod->id->getData()), QColor(Qt::black), false, false, true, false);
    }
    fc->setModelPrt(mod);

    ui.twSmenas->blockSignals(true);
    QTableWidgetItem *twi = ui.twSmenas->item(r,1); // name
    if(!twi){
        twi = new QTableWidgetItem(mod->name->getData());
        ui.twSmenas->setItem(r, 1, twi);
    }
    twi->setText(mod->name->getData());

    QTimeEdit *te = qobject_cast<QTimeEdit*>(ui.twSmenas->cellWidget(r,2));
    if(!te){
        te = new QTimeEdit(mod->begin->getData());
        te->setObjectName("begin");
        QVariant vmod; vmod.setValue<SPRSmenaModel*>(mod);
        te->setProperty("mod", vmod);
        ui.twSmenas->setCellWidget(r, 2, te);
        connect(te, &QTimeEdit::editingFinished, this, &SPRSmenasUsersPropertyWidget::onSmenaTimesChanget);
    }
    te->setTime(mod->begin->getData());

    te = qobject_cast<QTimeEdit*>(ui.twSmenas->cellWidget(r,3));
    if(!te){
        te = new QTimeEdit(mod->end->getData());
        te->setObjectName("end");
        QVariant vmod; vmod.setValue<SPRSmenaModel*>(mod);
        te->setProperty("mod", vmod);
        ui.twSmenas->setCellWidget(r, 3, te);
        connect(te, &QTimeEdit::editingFinished, this, &SPRSmenasUsersPropertyWidget::onSmenaTimesChanget);
    }
    te->setTime(mod->end->getData());


    ui.twSmenas->blockSignals(false);
}

void SPRSmenasUsersPropertyWidget::onSmenaTimesChanget(){
    if(sender()){
        SPRSmenaModel *mod = sender()->property("mod").value<SPRSmenaModel*>();
        QTimeEdit *te = qobject_cast<QTimeEdit*>(sender());
        if(mod){
            uint id = mod->id->getData();
            QString name = mod->name->getData();
            QTime begin = mod->begin->getData();
            QTime end = mod->end->getData();
            QString type = sender()->objectName();
            if(!type.isEmpty() && !type.isNull() && mod && te){
                if(type == "begin"){
                    begin = te->time();
                } else if(type == "end"){
                    end = te->time();
                }
                mod->setData(id, name, begin, end);
            }
        } else {
            return;
        }
    }
}

void SPRSmenasUsersPropertyWidget::setUserTableRow(int r, SPRUserModel *mod){
    FirstCollumn2 *fc = qobject_cast<FirstCollumn2*>(ui.twUsers->cellWidget(r, 0));
    if(!fc){
        fc = new FirstCollumn2(nullptr);
        ui.twUsers->setCellWidget(r,0, fc);
        connect(fc, &FirstCollumn2::deleteRow, this, &SPRSmenasUsersPropertyWidget::onDeleteUser);
    }
    if(r == 0){
        fc->setParams(r, QString::number(mod->id->getData()), QColor(Qt::black), false, false, false, false);
    } else {
        fc->setParams(r, QString::number(mod->id->getData()), QColor(Qt::black), false, false, true, false);
    }
    fc->setModelPrt(mod);

    ui.twUsers->blockSignals(true);
    QTableWidgetItem *twi = ui.twUsers->item(r,1); // nik
    if(!twi){
        twi = new QTableWidgetItem(mod->nik->getData());
        ui.twUsers->setItem(r, 1, twi);
    }
    twi->setText(mod->nik->getData());

    twi = ui.twUsers->item(r,2); // family
    if(!twi){
        twi = new QTableWidgetItem(mod->family->getData());
        ui.twUsers->setItem(r, 2, twi);
    }
    twi->setText(mod->family->getData());

    twi = ui.twUsers->item(r,3); // name
    if(!twi){
        twi = new QTableWidgetItem(mod->name->getData());
        ui.twUsers->setItem(r, 3, twi);
    }
    twi->setText(mod->name->getData());

    twi = ui.twUsers->item(r,4); // patronymic
    if(!twi){
        twi = new QTableWidgetItem(mod->patronymic->getData());
        ui.twUsers->setItem(r, 4, twi);
    }
    twi->setText(mod->patronymic->getData());
    ui.twUsers->blockSignals(false);

    SPRSmileComboBox *cb = dynamic_cast<SPRSmileComboBox*>(ui.twUsers->cellWidget(r, 5));
    if(!cb){
        QVariant id; id.setValue<SPRUserModel*>(mod);
        cb = new SPRSmileComboBox(nullptr);
        cb->setProperty("user", id);
        connect(cb, SIGNAL(currentIndexChanged(int)), this, SLOT(onAvatarChanget(int)));
        cb->setIconSize(QSize(32, 32));
        ui.twUsers->setCellWidget(r, 5, cb);
    }
    cb->blockSignals(true);
    int in = cb->findData(QVariant(mod->avatar->getData()));
    if(in < 0 || in >= cb->model()->rowCount()){
        cb->setCurrentIndex(0);
    } else {
        cb->setCurrentIndex(in);
    }
    cb->blockSignals(false);
}

void SPRSmenasUsersPropertyWidget::onAvatarChanget(int){
    SPRSmileComboBox *cb = dynamic_cast<SPRSmileComboBox*>(sender());
    if(cb){
        QVariant i; i = cb->currentData();
        QString iconName = i.value<QString>();

        QVariant u; u = cb->property("user");
        SPRUserModel *mod = qobject_cast<SPRUserModel*>(u.value<SPRUserModel*>());
        if(mod){
            mod->avatar->setData(iconName);
        }
    }
}

ISPRModelData *SPRSmenasUsersPropertyWidget::getModelData()
{
    return model;
}

void SPRSmenasUsersPropertyWidget::onModelChanget(IModelVariable *)
{
    if(sender() == model){
        widgetsShow();
    }
}
