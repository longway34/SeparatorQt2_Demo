#ifndef SPRSPECTRUMBASELISTTABLE3_H
#define SPRSPECTRUMBASELISTTABLE3_H

#include <QObject>
#include <QWidget>
#include "sprspectrumlisttable2.h"

class SPRSpectrumBaseListTable3 : public SPRSpectrumListTable2
{
    QStringList hHeaders;
public:
    explicit SPRSpectrumBaseListTable3(QWidget *parent = nullptr);
    virtual void widgetsShow();
};

#endif // SPRSPECTRUMBASELISTTABLE3_H
