#ifndef SPRSPECTRUMCHOISETIMEDIALOG_H
#define SPRSPECTRUMCHOISETIMEDIALOG_H

#include "ui_sprspectrumchoisetimedialog.h"
#include "variables/sprvariable.h"
#include "sprsettingsrentgenwidget.h"
#include "models/sprmainmodel.h"

class SPRSpectrumChoiseTimeDialog : public QDialog
{
    Q_OBJECT

    SPRMainModel *model;
    SPRSettingsRentgenWidget *rentgenWidget;
    int threads;
    bool masterMode;

public:

    explicit SPRSpectrumChoiseTimeDialog(QWidget *parent = nullptr);

    SPRSpectrumChoiseTimeDialog(SPRMainModel *_model, QWidget *parent = nullptr);
    QList<uint8_t> getThreads();
    double getTime();
    void setModelData(SPRMainModel *_model = nullptr);
    bool isRentgenOn();
    bool isRentgenOff();
    bool isRGUDown();
    QString getPrefix();
    void setMasterMode(bool value);
    bool isMasterMode();
public slots:
    void onShowRengenParams(bool);
    void onAllThreadClick(bool value);
    void setWithRentgenMode(bool onOff);
private:
    Ui::SPRSpectrumChoiseTimeDialog ui;
};

#endif // SPRSPECTRUMCHOISETIMEDIALOD_H
