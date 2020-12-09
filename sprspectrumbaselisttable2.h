#ifndef SPRSPECTRUMBASELISTTABLE2_H
#define SPRSPECTRUMBASELISTTABLE2_H

#include <QObject>
#include <QWidget>
#include "sprspectrumlisttable.h"

class SPRSpectrumBaseListTable2 : public SPRSpectrumListTable
{
    QStringList hHeaders;
public:
    explicit SPRSpectrumBaseListTable2(QWidget *parent = nullptr);
    virtual void widgetsShow();
};

#endif // SPRSPECTRUMBASELISTTABLE2_H
