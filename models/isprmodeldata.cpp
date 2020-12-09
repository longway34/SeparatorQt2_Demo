#include <QDebug>
#include "isprmodeldata.h"
#include <QFile>
#include <QFileInfo>

QDomDocument *ISPRModelData::getDoc() const
{
    return doc;
}

void ISPRModelData::setDoc(QDomDocument *_doc, IModelVariable *parent)
{
    if(_doc){
        doc = _doc;
        document = *doc;
    }
    setMVParent(parent);
}

void ISPRModelData::setDoc(QString fname, IModelVariable *parent)
{
    fileName = "";
    if(!fname.isEmpty()) {fileName = fname;}
    bool fileCreate = false;
    if(QFile::exists(fname)){
        QFile in(fileName);
        if(in.open(QIODevice::ReadOnly)){
            QFileInfo fi(fname);
            fileName = fi.absoluteFilePath();
            if(!document.setContent(&in)){
                qDebug() << "Error!!! read content from " << fname << "new file created now...";
                doc = new QDomDocument("SEPARATOR");

                document = *doc;
            }
        }
    } else {
        doc = new QDomDocument("SEPARATOR");
        document = *doc;

        fileCreate = true;
    }
    setDoc(&document, parent);

    if(!fileName.isEmpty()){
        QFile out(fname);
        if(out.open(QIODevice::WriteOnly)){
            QTextStream stream( &out );
            stream << document.toString().toUtf8();

            out.close();
        }
    }
}


QString ISPRModelData::getFileName() const
{
    return fileName;
}


//QList<IModelVariable *> ISPRModelData::getMyVariables() const
//{
//    return myVariables;
//}

//void ISPRModelData::setMyVariables(const QList<IModelVariable *> &value)
//{
//    myVariables = value;
//}

void ISPRModelData::setFileName(const QString &value)
{
    fileName = value;
}

ISPRModelData::ISPRModelData(QString fname, IModelVariable *parent): IModelVariable(nullptr, parent)
{
    setDoc(fname, parent);
}

ISPRModelData::ISPRModelData(QDomDocument *_doc, IModelVariable *parent): IModelVariable(_doc, parent)
{
    setDoc(_doc, parent);
}

bool ISPRModelData::saveAs(QString fname)
{
    if(!fname.isEmpty()){ fileName = fname; }
        QFile fout(fileName);
        if(fout.open(QIODevice::WriteOnly)){
//            QTextCodec *codec = QTextCodec::codecForName("UTF-8");
//            struct QTextCodec::ConverterState state;

//            QTextCodec::setCodecForLocale(codec);

            qint64 len = strlen(doc->toString().toStdString().c_str());
            QByteArray baOut = doc->toString().toUtf8();

            QTextStream out(&fout);
//            out << doc->toString();
            out << baOut;
            fout.flush();
            fout.close();
            baOut.clear();
            return true;
//            delete codec;
        }
    return false;
}

bool ISPRModelData::saveAsAllData(QString fname)
{
    emit doStore();
    return saveAs(fname);
}

bool ISPRModelData::saveAsOperateData(QString fname){
    return saveAs(fname);
}
