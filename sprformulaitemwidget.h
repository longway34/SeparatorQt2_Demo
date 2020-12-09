#ifndef SPRFORMULAITEMWIDGET_H
#define SPRFORMULAITEMWIDGET_H

#include "ui_sprformulaitemwidget.h"
#include "isprwidget.h"
#include "models/isprmodeldata.h"
#include "models/sprformulaitemmodel.h"

class SPRFormulaItemWidget : public QWidget, public ISPRWidget
{
    Q_OBJECT

protected:
    SPRFormulaItemModel *model;
    bool visibleMinMax;

public:
    explicit SPRFormulaItemWidget(QWidget *parent = nullptr);
    Ui::SPRFormulaItemWidget ui;
    void setResultText(){ui.lResult->setText(tr("H")+QString(model->index));}
//    void setElements(const DefaultMapElements *elements);
    ISPRModelData *setModelData(ISPRModelData *value);

    void setVisibleMinMax(bool visible);
    bool isVisibleMinMax();

public slots:
    virtual void viewChange();

    // ISPRWidget interface
public slots:
    virtual void widgetsShow();

    // ISPRWidget interface
public slots:
    virtual ISPRModelData *getModelData();
    virtual void onModelChanget(IModelVariable *send);



    // ISPRWidget interface
protected:
};

#endif // SPRFORMULAITEMWIDGET_H
