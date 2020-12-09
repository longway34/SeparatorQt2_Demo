#ifndef ISPRMODELDATA_H
#define ISPRMODELDATA_H

#include <QObject>
#include <QWidget>
#include <QList>
//#include <QDomDocument>
#include <QFile>
#include <iostream>
#include "models/imodelvariable.h"

//#include "models/isprvariable.h"

class ISPRModelData : public IModelVariable
{
    Q_OBJECT

protected:
    QDomDocument document;
    QString fileName;
    QString filePath;

//    QList<IModelVariable *> myVariables;
//    QDomDocument *doc;
//    QDomElement root;
    virtual bool saveAs(QString fname="");
public:
    ISPRModelData(){}
    ISPRModelData(IModelVariable *parent): IModelVariable(parent){}
    ISPRModelData(QString fname, IModelVariable *parent = nullptr);
    ISPRModelData(QDomDocument *_doc, IModelVariable *parent = nullptr);

    QDomDocument *getDoc() const;
    void setDoc(QDomDocument *_doc, IModelVariable *parent = nullptr);
    void setDoc(QString fname, IModelVariable *parent = nullptr);

    QString getFileName() const;

//    IModelVariable *getMainModel();
//    QList<IModelVariable *> getVariables() const;
//    void setVariables(const QList<IModelVariable *> &value);

//    bool isVariables(IModelVariable* variable){
//        myVariables.contains(variable);
//    }

//    QList<IModelVariable *> *addVariables(IModelVariable* variable){
//        if(!isVariables(variable)){
//            myVariables.append(variable);
//        }
//        return &myVariables;
//    }

//    QList<IModelVariable *> *addVariables(QList<IModelVariable*> listVariable){
//        foreach(IModelVariable *var, listVariable){
//            addVariables(var);
//        }
//        return &myVariables;
//    }

//    IModelVariable *

    void setFileName(const QString &value);

public slots:
    virtual bool saveAsAllData(QString fname = "");
    virtual bool saveAsOperateData(QString fname = "");

};

#endif // ISPRMODELDATA_H
