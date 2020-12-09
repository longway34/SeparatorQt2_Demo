#ifndef IMODELVARIABLE_H
#define IMODELVARIABLE_H

#include <QObject>
#include <QStringList>
#include <QtXml>
//#include <QDomDocument>
//#include <QDomElement>
//#include <QDomNode>
//#include <QDomAttr>
//#include <QDomCharacterData>
//#include <QDomText>
//#include <QDomNode>
#include <QDebug>
#include <QByteArray>

//#include "models/isprmodeldata.h"
class FindData {
    QString xpath;
    QStringList nextXPath;
    QString tagName;
    QString keyName;
    QString keyValue;
    QString attributeName;
    QDomNode findedNode;
    QString defValue;
    FindData *next;
    int orderNum;
public:
    ~FindData()
    {
        if(next) {
            delete next;
        }
//        if(next) delete next;
    }
    void init(QStringList _xpath, QString _defValue);
    FindData(QStringList xpath, QString _defValue=QString()){
        init(xpath, _defValue);
    }

    QDomElement find(QDomNode *parent);

    void toDebug(QStringList params = {});

    QDomElement create(QDomNode *parent, int findOrderNum);
    QDomNode getFindedNode();
};

class SPRMainModel;

class IModelVariable: public QObject
{
    Q_OBJECT

    QString value;
    QDomNode xmlNode;
    IModelVariable *mvparent;

    QList<IModelVariable*> myChildren;

    void fromXml(){
        if(!xmlNode.isNull()){
            if(xmlNode.nodeValue() != value){
//                QString v = xmlNode.nodeValue();
//                QString v1 = QString::fromUtf8(xmlNode.nodeValue().toStdString().c_str());
                value = xmlNode.nodeValue();
//                emit modelChanget(this);
            }
        }
    }

    void mconnect(IModelVariable *_mvparent){
        if(_mvparent && _mvparent != mvparent){
            if(mvparent){
                disconnect(mvparent, SIGNAL(doStore()), this, SLOT(store()));
                disconnect(mvparent, SIGNAL(doRestore()), this, SLOT(restore()));
                disconnect(this, SIGNAL(modelChanget(IModelVariable*)), mvparent, SLOT(onModelChanget(IModelVariable*)));
                mvparent->getChildren().removeAll(this);
            }
            mvparent = _mvparent;
            connect(mvparent, SIGNAL(doStore()), this, SLOT(store()));
            connect(mvparent, SIGNAL(doRestore()), this, SLOT(restore()));
            connect(this, SIGNAL(modelChanget(IModelVariable*)), mvparent, SLOT(onModelChanget(IModelVariable*)));
            mvparent->addChildren(this);
        }
    }
protected:
    QDomDocument *doc;

    QList<IModelVariable*> addChildren(IModelVariable* child);
    IModelVariable(IModelVariable *_mvparent): mvparent(nullptr), myChildren(), doc(nullptr){
//        myChildren.append(this);
        mconnect(_mvparent);
    }

    IModelVariable(): mvparent(nullptr), myChildren(), doc(nullptr){
//        myChildren.append(this);
    }
    IModelVariable(QDomDocument *parent, IModelVariable *_mvparent=nullptr): mvparent(nullptr), myChildren(), doc(parent) {
//        myChildren.append(this);
        mconnect(_mvparent);
    }

    IModelVariable(QDomDocument *parent, QString xpath, QString defValue, IModelVariable *_mvparent=nullptr): mvparent(nullptr), myChildren(), doc(parent){
//        myChildren.append(this);
        Init(parent, xpath, defValue, _mvparent);
    }
    void Init(QDomDocument *parent, QString xpath, QString defValue, IModelVariable *mvparent=nullptr);

public:
    ~IModelVariable();


    virtual SPRMainModel *getMainModel();

    void setMVParent(IModelVariable *_mvparent){
        mconnect(_mvparent);
    }

    void toXml(){
        if(!xmlNode.isNull())
            xmlNode.setNodeValue(value);
    }

    void removeFromXML();

    virtual QList<IModelVariable*> getChildren() const;

    bool isMyChildren(IModelVariable* var);

    QString getData(){
        return value;
    }
    void setData(QString _value){
        if(value != _value){
            value = _value;
            emit modelChanget(this);
        }
    }

    void toDebug(QDomNode element=QDomNode()){
        if(element.isNull()) element = xmlNode;

        if(element.isAttr()) element = element.toAttr().parentNode();
        if(element.isText()) element = element.toText().parentNode();
        if(element.isElement()){
            qDebug() << "tag    :" << element.toElement().tagName();
            QDomNamedNodeMap attrs = element.toElement().attributes();
            if(attrs.size()>0){
                qDebug() << "***********************";
                qDebug() << "tag : "<<element.toElement().tagName() << " = " << element.toElement().text();
                qDebug() << "****** atributes ******";
                for(int i=0; i<attrs.size(); i++) {
                    qDebug() << i << " " << attrs.item(i).nodeName() << " : "<< attrs.item(i).nodeValue();
                }
            }
        }
        qDebug() << "value :"<<value;
        qDebug() << "";
    }

    QString toString(){
        return getData();
    }
public slots:
    virtual void store(){
        toXml();
        emit doStore();
    }
    virtual void restore(){
        QString old = value;
        fromXml();
        emit doRestore();
        if(old != value){
            emit modelChanget(this);
        }
    }
    virtual void onModelChanget(IModelVariable* send){
        emit modelChanget(send);
    }
signals:
    void doStore();
    void doRestore();
    void modelChanget(IModelVariable *);
};
#endif // IMODELVARIABLE_H
