#include "sprmainmodel.h"
#include <QDir>

#include "models/sprhistorymodel2.h"
#include "models/sprseparatorlogsmodel.h"

void SPRMainModel::setDoc(QDomDocument *value)
{
    doc = value;

    elementsModel = new SPRElementsModel(doc, this);

    if(settingsMainModel) {delete settingsMainModel; settingsMainModel = nullptr;}
    settingsMainModel = new SPRSettingsMainModel(doc, this);

    if(settingsPorogsModel) {delete settingsPorogsModel; settingsPorogsModel = nullptr;}
    settingsPorogsModel = new SPRSettingsPorogsModel(doc, this);
    settingsPorogsModel->setThreads(settingsMainModel->getThreadsVariable());

    if(settingsRentgenModel) {delete settingsRentgenModel; settingsRentgenModel = nullptr;}
    settingsRentgenModel = new SPRSettingsRentgenModel(doc, this);

    settingsRentgenModel->setThreads(settingsMainModel->getThreadsVariable());
    settingsRentgenModel->setRentgens(settingsMainModel->getRentgensVariable());

    if(settingsControlModel) {delete settingsControlModel; settingsControlModel = nullptr;}
    settingsControlModel = new SPRSettingsControlModel(doc, this);

    if(settingsFormulaModel) {delete settingsFormulaModel; settingsFormulaModel = nullptr;}
    settingsFormulaModel = new SPRSettingsFormulaModel(doc, elementsModel, settingsPorogsModel->getConditions(), this);
//    settingsFormulaModel->setCondition(settingsPorogsModel->getConditions());

    if(spectrumZonesTableModel) {delete spectrumZonesTableModel; spectrumZonesTableModel = nullptr;}
    spectrumZonesTableModel = new SPRSpectrumZonesTableModel(doc, elementsModel, this);
    spectrumZonesTableModel->setThreads(settingsMainModel->getThreadsVariable());

    if(spectrumListItemsModel) {delete spectrumListItemsModel; spectrumListItemsModel = nullptr;}
    spectrumListItemsModel = new SPRSpectrumListItemsModel(this, nullptr, false, this);
//    spectrumListItemsModel->setSpectrumsfName(settingsMainModel->getSpectrumFileName());

    kSpectrums = new SPRSpectrumListItemsModel(this,
                        spectrumListItemsModel->getSpectrumsModelBase(), true, nullptr);

    if(separateModel){delete separateModel; separateModel = nullptr;}
    separateModel = new SPRSeparateModel(doc, this, nullptr);

    if(separateOutputModel){delete separateOutputModel; separateOutputModel = nullptr;}
    separateOutputModel = new SPRSeparateOutputModel(this, this->getThreadsList(), nullptr);

    if(historyModel){delete historyModel; historyModel = nullptr;}
    historyModel = new SPRHistoryModel(this, kSpectrums);

    if(historyModel2) {delete historyModel2; historyModel2 = nullptr;}
    historyModel2 = new SPRHistoryModel2(this, kSpectrums);

    if(settingsIMSModel) {delete settingsIMSModel; settingsIMSModel = nullptr;}
    settingsIMSModel = new SPRSettingsIMSModel(doc, this);
    settingsIMSModel->setIms(settingsMainModel->getImsVariable());
    settingsIMSModel->setHistoryModel(historyModel2);

    //    if(server) delete server;
//    server = new ServerConnect(settingsMainModel->getIpAddressVariable()->getData(), settingsMainModel->getIpPortVariable()->getData(), settingsMainModel->getIpAddressVariable(), settingsMainModel->getIpPortVariable());

    emit modelChanget(this);
}

SPRQStringVariable *SPRMainModel::getSpectrumFName()
{
    return settingsMainModel->getSpectrumFileNameVariable();
}

void SPRMainModel::setSpectrumFName(QString value)
{
    settingsMainModel->spectrumFileName->setData(value);
    spectrumListItemsModel->store();
    spectrumListItemsModel->saveAsAllData(this->settingsMainModel->spectrumFileName->getData());
//    separateModel->store();
//    separateModel->saveAsAllData(this->settingsMainModel->spectrumFileName->getData());
}

void SPRMainModel::setServerConnecting(bool _connecting){
    if(getServer()){
        getServer()->setConnected(_connecting);
    }
}

ServerConnect *SPRMainModel::getServer() const
{
    return settingsMainModel->getServer();
}

void SPRMainModel::setServer(ServerConnect *value)
{
    settingsMainModel->setServer(value);
}

SPRSeparateModel *SPRMainModel::getSeparateModel() const
{
    return separateModel;
}

void SPRMainModel::setSeparateModel(SPRSeparateModel *value)
{
    separateModel = value;
}

void SPRMainModel::setFName(const QString &value)
{

    fileName = value;
}

SPRElementsModel *SPRMainModel::getElementsModel() const
{
    return elementsModel;
}

SPRSeparateOutputModel *SPRMainModel::getSeparateOutputModel() const
{
    return separateOutputModel;
}

SPRThreadList SPRMainModel::getThreadsList()
{
    SPRThreadList res;
    if(getThreads())
        for(uint8_t th = 0; th<getThreads()->getData(); th++){
            res << th;
        }
    else
        for(uint8_t th = 0; th<MAX_SPR_MAIN_THREADS; th++){
            res << th;
        }

    return res;
}

SPRSpectrumListItemsModel *SPRMainModel::getKSpectrums() const
{
    return kSpectrums;
}

SPRHistoryModel *SPRMainModel::getHistoryModel() const
{
    return historyModel;
}

void SPRMainModel::setLogModel(SPRSeparatorLogsModel *value)
{
    logModel = value;

    if(getServer()){
        getServer()->setLogModel(value);
    }
    if(getHistoryModel2()){
        getHistoryModel2()->setLogModel(logModel);
    }

}

bool SPRMainModel::isSettingsChanged()
{
    return settingsChanged;
}

void SPRMainModel::setSettingsChanget(bool value)
{
    settingsChanged = value;
}

SPRMainModel::SPRMainModel(QDomDocument *_doc, ISPRModelData *parent): ISPRModelData(_doc, parent),
//    server(nullptr),
//    spectrumFName(nullptr),
    settingsMainModel(nullptr),
    settingsControlModel(nullptr),
    settingsFormulaModel(nullptr),
    settingsIMSModel(nullptr),
    settingsPorogsModel(nullptr),
    settingsRentgenModel(nullptr),
    spectrumZonesTableModel(nullptr),
    spectrumListItemsModel(nullptr),
    kSpectrums(nullptr),
    elementsModel(nullptr),
//    server(nullptr),
    separateModel(nullptr),
    separateOutputModel(nullptr),
    historyModel(nullptr),
    historyModel2(nullptr),
    errorsParsingXml(false),
    settingsChanged(false)

{
    setObjectName("MainModel");
    setDoc(_doc);
}

SPRMainModel::SPRMainModel(QString docFName, ISPRModelData *parent): ISPRModelData(docFName, parent),
//    spectrumFName(nullptr),
//    server(nullptr),
    settingsMainModel(nullptr),
    settingsControlModel(nullptr),
    settingsFormulaModel(nullptr),
    settingsIMSModel(nullptr),
    settingsPorogsModel(nullptr),
    settingsRentgenModel(nullptr),
    spectrumZonesTableModel(nullptr),
    spectrumListItemsModel(nullptr),
    kSpectrums(nullptr),
    elementsModel(nullptr),
//    server(nullptr),
    separateModel(nullptr),
    separateOutputModel(nullptr),
    historyModel(nullptr),
    historyModel2(nullptr),
    errorsParsingXml(true),
    settingsChanged(false)

{
    setObjectName("MainModel");
    filePath = "";

    bool createNewDoc = false;
//    bool createNewLogDoc = false;

    if(!QFile::exists(docFName)){
        docFName = ":/def/defSettings.xml";
        createNewDoc = true;
    }

//    if(!QFile::exists(logFName)){
//        logFName = ":/def/defLogs.xml";
//        createNewLogDoc = true;
//    }

    QFile in(docFName);
    if(in.open(QIODevice::ReadOnly)){
        if(document.setContent(&in)){
            doc = &document;
            in.close();
            errorsParsingXml = false;
        }
    }

    if(createNewDoc){
        docFName = "defSeparator.xml";
        QFile out(docFName);
        if(out.open(QIODevice::WriteOnly)){
        QTextStream stream( &out );
          stream << doc->toString();

          out.close();
        }

    }
    if(doc){
        setDoc(doc);
        setFName(docFName);
    }

//    QFile inLog(logFName);
//    if(inLog.open(QIODevice::ReadOnly)){
//        logModel = new SPRSeparatorLogsModel(logFName, 0, this, nullptr);
//        setProperty("delete_logModel", QVariant(true));
//    }

}

SPRMainModel::~SPRMainModel()
{
    if(settingsControlModel) delete settingsControlModel; settingsControlModel = nullptr;
    if(settingsFormulaModel) delete settingsFormulaModel; settingsFormulaModel = nullptr;
    if(settingsIMSModel) delete settingsIMSModel; settingsIMSModel = nullptr;
    if(settingsRentgenModel) delete settingsRentgenModel; settingsRentgenModel = nullptr;
    if(spectrumZonesTableModel) delete spectrumZonesTableModel; spectrumZonesTableModel = nullptr;
    if(settingsPorogsModel) delete settingsPorogsModel; settingsPorogsModel = nullptr;
    if(settingsMainModel) delete settingsMainModel; settingsMainModel = nullptr;
    if(elementsModel) delete elementsModel; elementsModel = nullptr;
    if(separateModel) delete separateModel; separateModel = nullptr;
    if(separateOutputModel) delete separateOutputModel; separateOutputModel = nullptr;
    if(historyModel) delete historyModel; historyModel = nullptr;
    if(historyModel2) delete historyModel2; historyModel2 = nullptr;
//    if(spectrumFName) delete spectrumFName; spectrumFName = nullptr;
//    if(server) delete server;
}

QDomDocument *SPRMainModel::getDoc() const
{
    return doc;
}


void SPRMainModel::onModelChanget(IModelVariable *send)
{
    settingsChanged = true;
    IModelVariable::onModelChanget(send);
}

bool SPRMainModel::saveAs(QString fname)
{
    bool res = ISPRModelData::saveAs(fname);
    if(res){
        settingsChanged = false;
    }
}
