#include "sprseparatoronmodedialog.h"

#include <QPushButton>

SPRTypeOnSeparatorMode SPRSeparatorOnModeDialog::getReturnMode() const
{
    return returnMode;
}

SPRSeparatorOnModeDialog::SPRSeparatorOnModeDialog(QWidget *parent) :
    QDialog(parent)
{
    ui.setupUi(this);

    returnMode = cancel;

    connect(ui.bCancel, &QPushButton::clicked, this, &SPRSeparatorOnModeDialog::onClickButton);
    connect(ui.bOnFull, &QPushButton::clicked, this, &SPRSeparatorOnModeDialog::onClickButton);
    connect(ui.bOnLigth, &QPushButton::clicked, this, &SPRSeparatorOnModeDialog::onClickButton);

}


//int SPRSeparatorOnModeDialog::exec()
//{
//    returnMode = cancel;
//    return QDialog::exec();
//}

void SPRSeparatorOnModeDialog::onClickButton()
{
    if(sender() == ui.bCancel){
        returnMode = cancel;
        reject();
        return;
    }
    if(sender() == ui.bOnFull){
        returnMode = onWithRA;
        accept();
    }
    if(sender() == ui.bOnLigth){
        returnMode = onWithOutRA;
        accept();
    }
}
