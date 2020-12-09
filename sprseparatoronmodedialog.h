#ifndef SPRSEPARATORONMODEDIALOG_H
#define SPRSEPARATORONMODEDIALOG_H

#include "ui_sprseparatoronmodedialog.h"

typedef enum {cancel, onWithRA, onWithOutRA} SPRTypeOnSeparatorMode;

class SPRSeparatorOnModeDialog : public QDialog
{
    Q_OBJECT

    SPRTypeOnSeparatorMode returnMode;
public:
    explicit SPRSeparatorOnModeDialog(QWidget *parent = nullptr);

    SPRTypeOnSeparatorMode getReturnMode() const;

private:
    Ui::SPRSeparatorOnModeDialog ui;

    // QDialog interface
public slots:
//    virtual int exec();
    void onClickButton();
};

#endif // SPRSEPARATORONMODEDIALOG_H
