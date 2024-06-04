QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    mainwindow.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../SevROVLibrary/release/ -lSevROVLibrary
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../SevROVLibrary/debug/ -lSevROVLibrary
else:unix: LIBS += -L$$OUT_PWD/../SevROVLibrary/ -lSevROVLibrary

INCLUDEPATH += $$PWD/../SevROVLibrary
DEPENDPATH += $$PWD/../SevROVLibrary

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../SevROVLibrary/release/libSevROVLibrary.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../SevROVLibrary/debug/libSevROVLibrary.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../SevROVLibrary/release/SevROVLibrary.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../SevROVLibrary/debug/SevROVLibrary.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../SevROVLibrary/libSevROVLibrary.a

RESOURCES += \
    resource.qrc
