//#define _CRTDBG_MAP_ALLOC
//#include <stdlib.h>
//#include <crtdbg.h>


#include "maintabwidget.h"
#include "mainwindow.h"

#include <QApplication>
#include <QMainWindow>

#include "models/sprspectrumlistitemsmodel.h"
#include "models/sprelementsmodel.h"

//#include "models/sprhistorymodel2.h"

#include <QDomDocument>
#include <QFile>
#include <QDir>
#include <QDateTime>

void myMyssageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg){
    switch(type){
    case QtInfoMsg:
        std::cerr << QString("Info: %1 (%2:%3, %4)\n").arg(msg).arg(context.file).arg(context.line).arg(context.function).toStdString();
        break;
    case QtDebugMsg:
//        std::cerr << QString("Debug: %1 (%2:%3, %4)\n").arg(msg).arg(context.file).arg(context.line).arg(context.function).toStdString();
        std::cerr << QString(msg).toStdString() << std::endl;
        break;
    case QtWarningMsg:
        std::cerr << QString("Warn: %1 (%2:%3, %4)\n").arg(msg).arg(context.file).arg(context.line).arg(context.function).toStdString();
        break;
    case QtCriticalMsg:
        std::cerr << QString("Crit: %1 (%2:%3, %4)\n").arg(msg).arg(context.file).arg(context.line).arg(context.function).toStdString();
        break;
    case QtFatalMsg:
        std::cerr << QString("Fatal: %1 (%2:%3, %4)\n").arg(msg).arg(context.file).arg(context.line).arg(context.function).toStdString();
        abort();
    }
}

int main(int argc, char *argv[])
{

    qInstallMessageHandler(myMyssageOutput);
//_CrtSetDbgFlag();
//_CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );

QApplication a(argc, argv);

    a.setOrganizationName("Separator7");

    QString fName = "Separator.xml";
    for(int i=0; i<argc; i++){
        QString sarg = argv[i];
        if(sarg.toUpper() == "-F"){
            if(argc > i){
                fName = argv[i+1];
            }
        }
    }


    MainWindow w;
    w.setWindowModality(Qt::WindowModal);
    w.show();

    return a.exec();

//    _CrtDumpMemoryLeaks();
}
