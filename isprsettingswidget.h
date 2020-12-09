#ifndef ISPRSETTINGSWIDGET_H
#define ISPRSETTINGSWIDGET_H

#include "isprwidget.h"

class ISPRSettingsWidget
{
public:
    ISPRSettingsWidget();

    virtual void setMasterMode(bool value) = 0;

};

#endif // ISPRSETTINGSWIDGET_H
