#-------------------------------------------------
#
# Project created by QtCreator 2017-12-19T12:57:18
#
#-------------------------------------------------

QT       += core gui xml network
QT += charts
CONFIG   += qwt

win32:CONFIG += console
win32:TARGET  = ../XmlDOMRead

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Separator7
TEMPLATE = app

#QMAKE_CXXFLAGS_DEBUG += -g3 -pg
#QMAKE_LFLAGS_DEBUG += -pg -lgmon

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
    _types.cpp \
    colorbutton.cpp \
    ipvalidator.cpp \
    isprwidget.cpp \
    main.cpp \
    sprformulaelement.cpp \
    sprformulaitemwidget.cpp \
    sprporogswidget.cpp \
    sprsettingscontrolwidget.cpp \
    sprsettingsformulawidget.cpp \
    sprsettingsimswidget.cpp \
    sprsettingsmainwidget.cpp \
    sprsettingsporogswidget.cpp \
    sprsettingsrentgenautosetwidget.cpp \
    sprsettingsrentgencontrolwidget.cpp \
    sprsettingsrentgenmainwidget.cpp \
    sprsettingsrentgenwidget.cpp \
    sprspectrumbasedlisttable.cpp \
    sprspectrumlisttablewidget.cpp \
    sprspectrumtableadv.cpp \
    models/imodelvariable.cpp \
    models/isprmodeldata.cpp \
    models/sprformulaitemmodel.cpp \
    models/sprgrspectrumitemmodel.cpp \
    models/sprporogsmodel.cpp \
    models/sprsettingscontrolmodel.cpp \
    models/sprsettingsformulamodel.cpp \
    models/sprsettingsimsmodel.cpp \
    models/sprsettingsmainmodel.cpp \
    models/sprsettingsporogsmodel.cpp \
    models/sprsettingsrentgenmodel.cpp \
    models/sprsettingsrmtautosetmodel.cpp \
    verticallabel.cpp \
    sprsettingswidget.cpp \
    models/sprmainmodel.cpp \
    models/sprspectrumzonestablemodel.cpp \
    models/sprspectrumzonesmodel.cpp \
    models/sprgraphitem.cpp \
    models/sprspectrumitemmodel.cpp \
    models/sprspectrumlistitemsmodel.cpp \
    sprspectrumgraphicswidget.cpp \
    sprtestimswidget.cpp \
    tcp/ITCPCommand.cpp \
    tcp/ServerConnect.cpp \
    tcp/TCPTimeOutCommand.cpp \
    tcp/TCPCommand.cpp \
    tcp/tcptimeoutwigget.cpp \
    tcp/TCPCommandSet.cpp \
    tcp/tcpcommandrguup.cpp \
    tcp/tcpcommandrguupdown.cpp \
    tcp/tcpcommandgetspectrums.cpp \
    tcp/tcpstartsoptestimscommand.cpp \
    tcp/tcpcommandstartpitatel.cpp \
    tcp/tcpstartrudospuskcommand.cpp \
    tcp/tcpstoprudostuskcommand.cpp \
    tcp/tcpcommandstoppitatel.cpp \
    models/sprseparatemodel.cpp \
    sprseparatewidget.cpp \
    tcp/tcpgetspectrumsgistogramms.cpp \
    sprspectrumsmalltable.cpp \
    tcp/tcpteststartseparate.cpp \
    tcp/tcpteststopseparate.cpp \
    tcp/tcpseparatego.cpp \
    sprspectrumbaselisttable2.cpp \
    models/sprelementsmodel.cpp \
    variables/sprdatetimevariable.cpp \
    variables/sprenumvariable.cpp \
    variables/sprqcolorvariable.cpp \
    variables/sprqstringvariable.cpp \
    variables/sprvariable.cpp \
    variables/sprkprvariable.cpp \
    variables/sprbooleanvariable.cpp \
    firstcollumn2.cpp \
    sprelementpropertydialog.cpp \
    sprzonesranges.cpp \
    sprzonesrangestable.cpp \
    sprzonesrangestablewidget.cpp \
    sprspectrumlisttable.cpp \
    sprspectrumchoisetimedialog.cpp \
    sprdeucodestable.cpp \
    models/imainmodel.cpp \
    sprrentgenvatable.cpp \
    mainwindow.cpp \
    scrollbar.cpp \
    scrollzoomer.cpp \
    tcp/tcpstartseparate2.cpp \
    sprseparategistogrammwidget.cpp \
    sprseparatedetailstable.cpp \
    sprseparatedetailtablewidget.cpp \
    maintabwidget.cpp \
    sprseparatedetailssummarytable.cpp \
    sprporogsmoved.cpp \
    sprsettingsrentgenautosetdialog.cpp \
    tcp/tcpautosetrentgen.cpp \
    tcp/tcpcommandrguupdown2.cpp \
    tcp/tcpexpositiononoff.cpp \
    tcp/tcpcommandseparatoroff.cpp \
    tcp/tcpautosetcommandgo.cpp \
    isprsettingswidget.cpp \
    sprhitorygraphics.cpp \
    sprsepratemaintable.cpp \
    models/sprseparateoutputmodel.cpp \
    models/sprhistorymodel.cpp \
    sprpitatelcontrolwidget.cpp \
    sprseparatecontrolwidget.cpp \
    tcp/tcpcommandseparatoronfull.cpp \
    tcp/tcpcommandrentgersetup.cpp \
    aboutdialog.cpp \
    sprseparateforcomplexwidget.cpp \
    sprporogswidget2.cpp \
    sprspectrumlisttable2.cpp \
    sprzonesranges2.cpp \
    sprzonesrangestable2.cpp \
    sprzonesrangestablewidget2.cpp \
    sprspectrumbaselisttable3.cpp \
    sprsettingscontrolwidget2.cpp \
    sprseparatesummarygistogrammwidget.cpp \
    sprseparatesummarygistogramm.cpp \
    sprzonesranges3.cpp \
    sprzonesrangestable3.cpp \
    sprzonesrangestablewidget3.cpp \
    tcp/tcpgetspectrums2.cpp \
    tcp/tcpclearstates.cpp \
    sprrentgencontroltimestable.cpp \
    sprrentgencontroldifftable.cpp \
    sprzonesrange4.cpp \
    sprzonesthreadrangetable4.cpp \
    sprzonesrange5.cpp \
    sprzonestable5.cpp \
    sprsmilecombobox.cpp \
    sprsmenasuserspropertywidget.cpp \
    models/spruserssmenamodel.cpp \
    tcp/tcpseparatego2.cpp \
    models/sprhistorymodel2.cpp \
    models/sprseparatorlogsmodel.cpp \
    sprhistorylogswidget.cpp \
    tcp/tcpechologswidget.cpp \
    sprcomplexinfowidget.cpp \
    tcp/tcpgetsetthermo.cpp \
    sprseparatoronmodedialog.cpp \
    sprstonesizeschartwidget.cpp

HEADERS += \
    _types.h \
    colorbutton.h \
    ipvalidator.h \
    isprwidget.h \
    sprformulaelement.h \
    sprformulaitemwidget.h \
    sprporogswidget.h \
    sprsettingscontrolwidget.h \
    sprsettingsformulawidget.h \
    sprsettingsimswidget.h \
    sprsettingsmainwidget.h \
    sprsettingsporogswidget.h \
    sprsettingsrentgenautosetwidget.h \
    sprsettingsrentgencontrolwidget.h \
    sprsettingsrentgenmainwidget.h \
    sprsettingsrentgenwidget.h \
    sprspectrumbasedlisttable.h \
    sprspectrumtableadv.h \
    models/imodelvariable.h \
    models/isprmodeldata.h \
    models/sprformulaitemmodel.h \
    models/sprgrspectrumitemmodel.h \
    models/sprporogsmodel.h \
    models/sprsettingscontrolmodel.h \
    models/sprsettingsformulamodel.h \
    models/sprsettingsimsmodel.h \
    models/sprsettingsmainmodel.h \
    models/sprsettingsporogsmodel.h \
    models/sprsettingsrentgenmodel.h \
    models/sprsettingsrmtautosetmodel.h \
    tcp/TCPCommandSet.h \
    verticallabel.h \
    sprsettingswidget.h \
    models/sprmainmodel.h \
    models/sprspectrumzonestablemodel.h \
    models/sprspectrumzonesmodel.h \
    models/sprgraphitem.h \
    models/sprspectrumitemmodel.h \
    models/sprspectrumlistitemsmodel.h \
    sprspectrumgraphicswidget.h \
    sprtestimswidget.h \
    tcp/ServerConnect.h \
    tcp/ITCPCommand.h \
    tcp/TCPCommand.h \
    tcp/TCPTimeOutCommand.h \
    tcp/tcptimeoutwigget.h \
    tcp/tcpcommandrguup.h \
    tcp/tcpcommandrguupdown.h \
    tcp/tcpcommandgetspectrums.h \
    tcp/tcpstartsoptestimscommand.h \
    tcp/tcpcommandstartpitatel.h \
    tcp/tcpstartrudospuskcommand.h \
    tcp/tcpstoprudostuskcommand.h \
    tcp/tcpcommandstoppitatel.h \
    models/sprseparatemodel.h \
    sprseparatewidget.h \
    tcp/tcpgetspectrumsgistogramms.h \
    sprspectrumsmalltable.h \
    tcp/tcpteststartseparate.h \
    tcp/tcpteststopseparate.h \
    tcp/tcpseparatego.h \
    sprspectrumbaselisttable2.h \
    models/sprelementsmodel.h \
    variables/sprdatetimevariable.h \
    variables/sprenumvariable.h \
    variables/sprqcolorvariable.h \
    variables/sprqstringvariable.h \
    variables/sprvariable.h \
    variables/sprkprvariable.h \
    variables/sprbooleanvariable.h \
    firstcollumn2.h \
    sprelementpropertydialog.h \
    sprzonesranges.h \
    sprzonesrangestable.h \
    sprzonesrangestablewidget.h \
    sprspectrumlisttable.h \
    sprspectrumlisttablewidget.h \
    sprspectrumchoisetimedialog.h \
    sprdeucodestable.h \
    models/imainmodel.h \
    sprrentgenvatable.h \
    mainwindow.h \
    scrollbar.h \
    scrollzoomer.h \
    tcp/tcpstartseparate2.h \
    sprseparategistogrammwidget.h \
    sprseparatedetailstable.h \
    sprseparatedetailtablewidget.h \
    maintabwidget.h \
    sprseparatedetailssummarytable.h \
    sprporogsmoved.h \
    sprsettingsrentgenautosetdialog.h \
    tcp/tcpautosetrentgen.h \
    tcp/tcpcommandrguupdown2.h \
    tcp/tcpexpositiononoff.h \
    tcp/tcpcommandseparatoroff.h \
    tcp/tcpautosetcommandgo.h \
    isprsettingswidget.h \
    sprhitorygraphics.h \
    sprsepratemaintable.h \
    models/sprseparateoutputmodel.h \
    models/sprhistorymodel.h \
    sprpitatelcontrolwidget.h \
    sprseparatecontrolwidget.h \
    tcp/tcpcommandseparatoronfull.h \
    tcp/tcpcommandrentgersetup.h \
    aboutdialog.h \
    sprseparateforcomplexwidget.h \
    sprcomplextabwidget.h \
    sprporogswidget2.h \
    sprspectrumlisttable2.h \
    sprzonesranges2.h \
    sprzonesrangestable2.h \
    sprzonesrangestablewidget2.h \
    sprspectrumbaselisttable3.h \
    sprsettingscontrolwidget2.h \
    sprseparatesummarygistogrammwidget.h \
    sprseparatesummarygistogramm.h \
    sprzonesranges3.h \
    sprzonesrangestable3.h \
    sprzonesrangestablewidget3.h \
    tcp/tcpgetspectrums2.h \
    tcp/tcpclearstates.h \
    sprrentgencontroltimestable.h \
    sprrentgencontroldifftable.h \
    sprzonesrange4.h \
    sprzonesthreadrangetable4.h \
    sprzonesrange5.h \
    sprzonestable5.h \
    sprsmilecombobox.h \
    sprsmenasuserspropertywidget.h \
    models/spruserssmenamodel.h \
    tcp/tcpseparatego2.h \
    models/sprhistorymodel2.h \
    models/sprseparatorlogsmodel.h \
    sprhistorylogswidget.h \
    tcp/tcpechologswidget.h \
    sprcomplexinfowidget.h \
    tcp/tcpgetsetthermo.h \
    sprseparatoronmodedialog.h \
    sprstonesizeschartwidget.h

FORMS += \
    sprsettingsrentgenmainwidget.ui \
    sprsettingsrentgencontrolwidget.ui \
    sprsettingsrentgenautosetwidget.ui \
    sprformulaitemwidget.ui \
    sprsettingsformulawidget.ui \
    sprporogswidget.ui \
    sprsettingsporogswidget.ui \
    sprsettingsmainwidget.ui \
    sprsettingsrentgenwidget.ui \
    sprsettingsimswidget.ui \
    sprsettingscontrolwidget.ui \
    sprsettingswidget.ui \
    sprspectrumgraphicswidget.ui \
    sprtestimswidget.ui \
    tcp/tcptimeoutwigget.ui \
    sprseparatewidget.ui \
    firstcollumn2.ui \
    sprelementpropertydialog.ui \
    sprzonesrangestablewidget.ui \
    sprspectrumlisttablewidget.ui \
    sprspectrumchoisetimedialog.ui \
    mainwindow.ui \
    sprseparategistogrammwidget.ui \
    sprseparatedetailtablewidget.ui \
    maintabwidget.ui \
    sprsettingrentgenautosetdialog.ui \
    sprpitatelcontrolwidget.ui \
    sprseparatecontrolwidget.ui \
    aboutdialog.ui \
    sprseparateforcomplexwidget.ui \
    sprzonesrangestablewidget2.ui \
    sprsettingscontrolwidget2.ui \
    sprzonesrangestablewidget3.ui \
    sprsmenasuserspropertywidget.ui \
    sprhistorylogswidget.ui \
    tcp/tcpechologswidget.ui \
    sprcomplexinfowidget.ui \
    sprporogswidget2.ui \
    sprseparatoronmodedialog.ui

RESOURCES += \
    icons.qrc \
    defxmlsettings.qrc \
    defconfigxml.qrc \
    smiles.qrc \
    defusers.qrc

#DISTFILES += \
#    models.pri

#include(models/models.pri)
#include(tcp/tcp.pri)


unix:INCLUDEPATH += /usr/local/qwt-6.1.5/include
unix:LIBS += -L/usr/local/qwt-6.1.5/lib
#unix:LIBS += -l/usr/local/qwt-6.1.3/lib/libqwt.so

#unix: CONFIG += link_pkgconfig
#unix: PKGCONFIG += qwt

unix:!macx: LIBS += -L$$PWD/../../../../../../usr/local/qwt-6.1.5/lib/ -lqwt

#nix:!macx:INCLUDEPATH += $$PWD/../../../../../../usr/local/qwt-6.1.5/include
#nix:!macx:DEPENDPATH += $$PWD/../../../../../../usr/local/qwt-6.1.5/include

#win32: LIBS += -lpsapi
#win32: INCLUDEPATH += C:\Qwt-6.1.5\src

DISTFILES += \
    Readme.MD \
    icons/demo_images/main_screen.png \
    icons/demo_images/process_control_screen.png \
    icons/demo_images/setting_control_screen.png \
    icons/demo_images/spectrs_control_screen.png
