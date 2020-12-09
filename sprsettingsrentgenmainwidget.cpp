#include "sprsettingsrentgenmainwidget.h"

#include "models/sprmainmodel.h"
#include "tcp/ServerConnect.h"


SPRSettingsRentgenMainWidget::SPRSettingsRentgenMainWidget(QWidget *parent) :
    QWidget(parent), model(nullptr)
{
    ui.setupUi(this);

}

ISPRModelData *SPRSettingsRentgenMainWidget::setModelData(ISPRModelData *data)
{
    if(data){
        if(model){
            disconnect(model, SIGNAL(modelChanget(IModelVariable*)), this, SLOT(onModelChanget(IModelVariable*)));
        }
        model = (SPRSettingsRentgenModel*)data;
        connect(model, SIGNAL(modelChanget(IModelVariable*)), this, SLOT(onModelChanget(IModelVariable*)));

        ui.tDEUCode->setModelData(model);
        ui.tTubeVA->setModelData(model);
    }
    return data;
}

void SPRSettingsRentgenMainWidget::widgetsShow()
{
}

ISPRModelData *SPRSettingsRentgenMainWidget::getModelData()
{
    return model;
}


void SPRSettingsRentgenMainWidget::setThreads(SPRVariable<uint> *value)
{
    model->setThreads(value);
}

void SPRSettingsRentgenMainWidget::setRentgens(SPRVariable<uint> *value)
{
    model->setRentgens(value);
}

void SPRSettingsRentgenMainWidget::onModelChanget(IModelVariable *var)
{
    if(sender() == model || var == model){
        SPRMainModel *mm = model->getMainModel();
        if(mm){
            ServerConnect *server = mm->getServer();
            if(server){
                server->clearState(spr_state_codes_is_stated);
            }
        }
    }
}

void SPRSettingsRentgenMainWidget::setMasterMode(bool value)
{
    this->setEnabled(value);
}
