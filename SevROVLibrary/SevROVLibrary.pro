QT -= gui
QT += network

TEMPLATE = lib
CONFIG += staticlib

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    sevrovconnector.cpp \
    sevrovcontroldata.cpp \
    sevrovdata.cpp \
    sevrovlibrary.cpp \
    sevrovpidcontroller.cpp \
    sevrovtelemetrydata.cpp \
    sevrovxboxcontroller.cpp

HEADERS += \
    sevrovconnector.h \
    sevrovcontroldata.h \
    sevrovdata.h \
    sevrovlibrary.h \
    sevrovpidcontroller.h \
    sevrovtelemetrydata.h \
    sevrovxboxcontroller.h

# Default rules for deployment.
unix {
    target.path = $$[QT_INSTALL_PLUGINS]/generic
}
!isEmpty(target.path): INSTALLS += target


win32 {
    LIBS += -L C:/SDL2-2.28.2-mingw/x86_64-w64-mingw32/lib -lSDL2
    INCLUDEPATH += C:/SDL2-2.28.2-mingw/x86_64-w64-mingw32/include
}
unix
{
    LIBS += -L/usr/local/lib -lSDL2
    INCLUDEPATH += /usr/include/SDL2
}
