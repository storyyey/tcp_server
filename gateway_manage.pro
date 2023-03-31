requires(qtHaveModule(httpserver))

QT += core gui network httpserver xlsx sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11
LIBS += -lws2_32
RC_ICONS = exeicon.ico
# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    DeviceData.cpp \
    actionbar.cpp \
    arpscan.cpp \
    debugwidget.cpp \
    devicedb.cpp \
    devicestable.cpp \
    devicestablemenu.cpp \
    dhcpserver.cpp \
    httpserver.cpp \
    ipconfigrecordtable.cpp \
    ipsetdialog.cpp \
    main.cpp \
    mainwindow.cpp \
    networktest.cpp \
    onlyapp.cpp \
    operatexlsx.cpp \
    otherdialog.cpp \
    remarkdialog.cpp \
    searchdialog.cpp \
    setoption.cpp \
    tables.cpp \
    tablesearch.cpp \
    udpserver.cpp \
    upgradedialog.cpp \
    upgraderecordtable.cpp

HEADERS += \
    DeviceData.h \
    actionbar.h \
    arpscan.h \
    debugwidget.h \
    devicedb.h \
    devicestable.h \
    devicestablemenu.h \
    dhcpserver.h \
    httpserver.h \
    ipconfigrecordtable.h \
    ipsetdialog.h \
    mainwindow.h \
    networktest.h \
    onlyapp.h \
    operatexlsx.h \
    otherdialog.h \
    remarkdialog.h \
    searchdialog.h \
    setoption.h \
    tables.h \
    tablesearch.h \
    udpserver.h \
    upgradedialog.h \
    upgraderecordtable.h

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    icon.qrc

DISTFILES += \
    icon/add.png \
    icon/del.png \
    icon/network.png \
    icon/reboot.png


