QT       += core gui 3dcore 3dlogic 3dextras 3dinput

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    cabstractentitymodel.cpp \
    cmovableplane.cpp \
    cmovableplanedelegate.cpp \
    cmovableplanemodel.cpp \
    cobjinspectorfactorymethod.cpp \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    cabstractentitymodel.h \
    cmovableplane.h \
    cmovableplanedelegate.h \
    cmovableplanemodel.h \
    cobjinspectorfactorymethod.h \
    mainwindow.h \
    movableplanemodel_types.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    res.qrc

DISTFILES += \
    eng.geom \
    eng.vert \
    engv2.geom \
    engv2.vert \
    v8_engine.obj
