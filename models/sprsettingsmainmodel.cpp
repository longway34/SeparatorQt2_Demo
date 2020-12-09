#include "sprsettingsmainmodel.h"
#include "models/sprseparatorlogsmodel.h"

ServerConnect *SPRSettingsMainModel::getServer() const
{
    return server;
}

void SPRSettingsMainModel::setServer(ServerConnect *value)
{
    server = value;
}

SPRSettingsMainModel::SPRSettingsMainModel(QObject *parent)
{
    name = nullptr;
    ipAddress = nullptr;
    ipPort = nullptr;
    rentgens = nullptr;
    ims = nullptr;
    threads = nullptr;
    typePRAM = nullptr;
    typeThermo = nullptr;
    color = nullptr;
    ignoreRGUErrors = nullptr;
    ignoreDoors = nullptr;
    igroreCriticalErrors = nullptr;
    ignoreRentgenErrors = nullptr;
}

SPRSettingsMainModel::SPRSettingsMainModel(QDomDocument *_doc, ISPRModelData *parent):
    ISPRModelData(_doc, parent),
    server(nullptr),
    name(nullptr),
    ipAddress(nullptr),
    ipPort(nullptr),
    rentgens(nullptr),
    ims(nullptr),
    threads(nullptr),
    typePRAM(nullptr),
    typeThermo(nullptr),
    color(nullptr),
    ignoreRGUErrors(nullptr),
    ignoreDoors(nullptr),
    igroreCriticalErrors(nullptr),
    ignoreRentgenErrors(nullptr)
{
    name = new SPRQStringVariable(doc, SPR_SETTINGS_MAIN_NAME_XPATH, DEF_SPR_MAIN_NAME,this);
    setProperty("delete_name_variable", QVariant(true));
    ipAddress = new SPRQStringVariable(doc, SPR_SETTINGS_MAIN_ADDRESS_XPATH, DEF_SPR_MAIN_ADDRESS,this);
    ipAddress->setObjectName("ipAddress");
    setProperty("delete_ip_address_variable", QVariant(true));
    ipPort = new SPRVariable<uint>(doc, SPR_SETTINGS_MAIN_PORT_XPATH, DEF_SPR_MAIN_PORT,this);
    setProperty("delete_ip_port_variable", QVariant(true));
    rentgens = new SPRVariable<uint>(doc, SPR_SETTINGS_MAIN_RENTGENS_XPATH, DEF_SPR_MAIN_RENTGENS,this);
    setProperty("delete_rentgens_variable", QVariant(true));
    threads = new SPRVariable<uint>(doc,SPR_SETTINGS_MAIN_THREADS_XPATH, DEF_SPR_MAIN_THREADS, this);
    setProperty("delete_threads_variable", QVariant(true));
    ims = new SPRVariable<uint>(doc, SPR_SETTINGS_MAIN_IMS_XPATH, DEF_SPR_MAIN_IMS,this);
    setProperty("delete_ims_variable", QVariant(true));
    typePRAM = new SPREnumVariable<TypePRAM>(doc, SPR_SETTINGS_MAIN_TYPE_PRAM_XPATH, DEF_SPR_MAIN_TYPE_PRAM,this);
    typeThermo = new SPREnumVariable<TypeThermo>(doc, SPR_SETTINGS_MAIN_TYPE_THERMO_XPATH, DEF_SPR_MAIN_TYPE_THERMO,this);
    color = new SPRQColorVariable(doc, SPR_SETTINGS_MAIN_COLOR_XPATH, DEF_SPR_MAIN_COLOR,this);
    setProperty("delete_color_variable", QVariant(true));
    spectrumFileName = new SPRQStringVariable(doc, SPR_SETTINGS_MAIN_SPECTRUM_FNAME_XPATH, DEF_SPR_MAIN_SETTINGS_FNAME+DEF_SPR_MAIN_SPECTRUM_FNAME_SUFFIX, this);

    server = new ServerConnect(ipAddress->getData(), ipPort->getData());
    setProperty("delete_server_variable", QVariant(true));
    server->setVName(ipAddress);
    server->setVPort(ipPort);

    ignoreRGUErrors = new SPRBooleanVariable(doc, SPR_SETTINGS_MAIN_IGNORE_RGU_XPATH, DEF_SPR_MAIN_SETTINGS_IGNORE_RGU, this);
    ignoreDoors = new SPRBooleanVariable(doc, SPR_SETTINGS_MAIN_IGNORE_DOOR_XPATH, DEF_SPR_MAIN_SETTINGS_IGNORE_DOOR, this);
    igroreCriticalErrors = new SPRBooleanVariable(doc, SPR_SETTINGS_MAIN_STOP_ON_CRITICAL_MOVE_XPATH, DEF_SPR_MAIN_SETTINGS_STOP_ON_CRITICAL_MOVE, this);
    ignoreRentgenErrors = new SPRBooleanVariable(doc, SPR_SETTINGS_MAIN_IRNORE_RENTGEN_ERRORS_XPATH, DEF_SPR_MAIN_SETTINGS_IRNORE_RENTGEN_ERRORS, this);

}

SPRSettingsMainModel::~SPRSettingsMainModel()
{
    if(name && property("delete_name_variable").toBool()) delete name; name = nullptr;
    if(ipAddress && property("delete_ip_address_variable").toBool()) delete ipAddress; ipAddress = nullptr;
    if(ipPort && property("delete_ip_port_variable").toBool()) delete ipPort; ipPort = nullptr;
    if(rentgens && QVariant(property("delete_rentgens")).toBool()) delete rentgens; rentgens = nullptr;
    if(ims && property("delete_ims_variable").toBool()) delete ims; ims = nullptr;
    if(typePRAM) delete typePRAM; typePRAM = nullptr;
    if(typeThermo) delete typeThermo; typeThermo = nullptr;
    if(color && property("delete_color_variable").toBool()) delete color; color = nullptr;
    if(threads && QVariant(property("delete_threads")).toBool()) delete threads; threads = nullptr;

    if(ignoreRGUErrors) delete ignoreRGUErrors; ignoreRGUErrors = nullptr;
    if(ignoreDoors) delete ignoreDoors; ignoreDoors = nullptr;
    if(igroreCriticalErrors) delete igroreCriticalErrors; igroreCriticalErrors = nullptr;
    if(ignoreRentgenErrors) delete ignoreRentgenErrors; ignoreRentgenErrors = nullptr;


    if(server && property("delete_server_variable").toBool()) delete server; server = nullptr;
}

void SPRSettingsMainModel::setImsVariable(SPRVariable<uint> *value)
{
    setProperty("delete_ims_variable", QVariant(false));
    ims = value;
}

SPRQColorVariable *SPRSettingsMainModel::getColorVariable() const
{
    return color;
}

void SPRSettingsMainModel::setColorVariable(SPRQColorVariable *value)
{
    setProperty("delete_color_variable", QVariant(false));
    color = value;
}

void SPRSettingsMainModel::setNameVariable(SPRQStringVariable *value)
{
    setProperty("delete_name_variable", QVariant(false));
    name = value;
}

SPRVariable<uint> *SPRSettingsMainModel::getIpPortVariable() const
{
    return ipPort;
}

void SPRSettingsMainModel::setIpPortVariable(SPRVariable<uint> *value)
{
    setProperty("delete_ip_port_variable", QVariant(false));
    ipPort = value;
}

SPRQStringVariable *SPRSettingsMainModel::getIpAddressVariable() const
{
    return ipAddress;
}

void SPRSettingsMainModel::setIpAddressVariable(SPRQStringVariable *value)
{
    setProperty("delete_ip_address_variable", QVariant(false));
    ipAddress = value;
}

SPRQStringVariable *SPRSettingsMainModel::getNameVariable() const
{
    return name;
}

SPRQStringVariable *SPRSettingsMainModel::getSpectrumFileNameVariable()
{
    return spectrumFileName;
}

SPRVariable<uint> *SPRSettingsMainModel::getThreadsVariable() const
{
    return threads;
}

SPRVariable<uint> *SPRSettingsMainModel::getImsVariable() const
{
    return ims;
}

SPRVariable<uint> *SPRSettingsMainModel::getRentgensVariable() const
{
    return rentgens;
}
