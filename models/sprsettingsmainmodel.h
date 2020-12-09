#ifndef SPRSETTINGSMAINMODEL_H
#define SPRSETTINGSMAINMODEL_H

#include <QObject>
#include "_types.h"
#include "models/isprmodeldata.h"
#include "variables/sprqcolorvariable.h"
#include "variables/sprdatetimevariable.h"
#include "variables/sprqstringvariable.h"
#include "variables/sprenumvariable.h"
#include "variables/sprvariable.h"
#include "variables/sprbooleanvariable.h"
#include "tcp/ServerConnect.h"
#include "tcp/TCPCommand.h"

class SPRSettingsMainModel : public ISPRModelData
{
    ServerConnect *server;
public:
    SPRSettingsMainModel(QObject *parent=nullptr);
    SPRSettingsMainModel(QDomDocument *_doc, ISPRModelData *parent = nullptr);
    virtual ~SPRSettingsMainModel();

    SPRQStringVariable *name;
    SPRQStringVariable *spectrumFileName;
    SPRQStringVariable *ipAddress;
    SPRVariable<uint> *ipPort;
    SPRVariable<uint> *rentgens;
    SPRVariable<uint> *ims;
    SPRVariable<uint> *threads;
//    SPRVariable<uint> *ims_row_count;
    SPREnumVariable<TypePRAM> *typePRAM;
    SPREnumVariable<TypeThermo> *typeThermo;
    SPRQColorVariable *color;

    SPRBooleanVariable *ignoreRGUErrors;
    SPRBooleanVariable *ignoreDoors;
    SPRBooleanVariable *igroreCriticalErrors;
    SPRBooleanVariable *ignoreRentgenErrors;

//    SPRDateVariable *date;
//    SPRQStringVariable *version;

    SPRVariable<uint> *getRentgensVariable() const;
    SPRVariable<uint> *getThreadsVariable() const;
    SPRVariable<uint> *getImsVariable() const;
    SPRQStringVariable *getSpectrumFileNameVariable();
    SPRQStringVariable *getNameVariable() const;
    SPRQStringVariable *getIpAddressVariable() const;
    void setIpAddressVariable(SPRQStringVariable *value);
    SPRVariable<uint> *getIpPortVariable() const;
    void setIpPortVariable(SPRVariable<uint> *value);
    void setNameVariable(SPRQStringVariable *value);
    ServerConnect *getServer() const;
    void setServer(ServerConnect *value);
    SPRQColorVariable *getColorVariable() const;
    void setColorVariable(SPRQColorVariable *value);
    SPRVariable<uint> *getIms() const;
    void setImsVariable(SPRVariable<uint> *value);
};

#endif // SPRSETTINGSMAINMODEL_H
