#ifndef SPRFORMULAELEMENT_H
#define SPRFORMULAELEMENT_H

#include <QObject>
#include <QWidget>
#include <QLabel>
#include <QMenu>

#include "_types.h"
#include "variables/sprenumvariable.h"
#include "models/sprelementsmodel.h"
#include "isprwidget.h"


class SPRFormulaElement : public QLabel, public ISPRWidget
{
    Q_OBJECT

    SPRElementsModel *elements;
    SPREnumVariable<EnumElements> *element;
public:
    SPRFormulaElement(QWidget *parent = nullptr);
    virtual ~SPRFormulaElement();
    void setElement(SPREnumVariable<EnumElements> *variable);
    void setElementsModel(SPRElementsModel *_elements);
    void setCurrentElement(EnumElements element);

public slots:
    void onChangeCurrentElement(bool trigger);
    virtual void widgetsShow();
signals:
    void changeCurrentElement(EnumElements element);
protected:

public:
    virtual ISPRModelData *setModelData(ISPRModelData *data);
    virtual ISPRModelData *getModelData();

protected:
    virtual void onModelChanget(IModelVariable *variable);
    void setActionsElement();
};

#endif // SPRFORMULAELEMENT_H
