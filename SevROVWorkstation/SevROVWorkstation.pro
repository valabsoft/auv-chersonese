QT       += core gui datavisualization network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    applicationsettings.cpp \
    camerascene.cpp \
    main.cpp \
    mainwindow.cpp \
    pointcloud/calibrate.cpp \
    pointcloud/disparity.cpp \
    pointcloud/three_dimensional_proc.cpp \
    settingswindow.cpp \
    sevrovcontroller.cpp \
    toolwindow.cpp

HEADERS += \
    applicationsettings.h \
    camerascene.h \
    datastructure.h \
    enumclasses.h \
    mainwindow.h \
    pointcloud/calibrate.h \
    pointcloud/disparity.h \
    pointcloud/three_dimensional_proc.h \
    settingswindow.h \
    sevrovcontroller.h \
    toolwindow.h

FORMS += \
    mainwindow.ui \
    settingswindow.ui \
    toolwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

# SevROV Library

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../SevROVLibrary/release/ -lSevROVLibrary
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../SevROVLibrary/debug/ -lSevROVLibrary
else:unix:!macx: LIBS += -L$$OUT_PWD/../SevROVLibrary/ -lSevROVLibrary

INCLUDEPATH += $$PWD/../SevROVLibrary
DEPENDPATH += $$PWD/../SevROVLibrary

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../SevROVLibrary/release/libSevROVLibrary.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../SevROVLibrary/debug/libSevROVLibrary.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../SevROVLibrary/release/SevROVLibrary.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../SevROVLibrary/debug/SevROVLibrary.lib
else:unix:!macx: PRE_TARGETDEPS += $$OUT_PWD/../SevROVLibrary/libSevROVLibrary.a

# OpenCV include path and libs

win32 {
    INCLUDEPATH += c:\opencv-4.8.0-build\install\include\

    LIBS += c:\opencv-4.8.0-build\bin\libopencv_core480.dll
    LIBS += c:\opencv-4.8.0-build\bin\libopencv_highgui480.dll
    LIBS += c:\opencv-4.8.0-build\bin\libopencv_imgcodecs480.dll
    LIBS += c:\opencv-4.8.0-build\bin\libopencv_features2d480.dll
    LIBS += c:\opencv-4.8.0-build\bin\libopencv_calib3d480.dll
    LIBS += c:\opencv-4.8.0-build\bin\libopencv_videoio480.dll
    LIBS += c:\opencv-4.8.0-build\bin\libopencv_imgproc480.dll
}
unix {
    INCLUDEPATH += /usr/include/opencv4
    LIBS += -L/usr/local/lib -lopencv_core -lopencv_highgui -lopencv_imgcodecs -lopencv_features2d -lopencv_calib3d -lopencv_videoio -lopencv_imgproc
}

RESOURCES += \
    SevROVWorkstation.qrc

# SDL Library
win32 {
    LIBS += -L C:/SDL2-2.28.2-mingw/x86_64-w64-mingw32/lib -lSDL2
    INCLUDEPATH += C:/SDL2-2.28.2-mingw/x86_64-w64-mingw32/include
}
unix
{
    LIBS += -L/usr/local/lib -lSDL2
    INCLUDEPATH += /usr/include/SDL2
}

# MVS Library
win32 {
    LIBS += -LC:/MVS/Development/Libraries/win64 -lMvCameraControl
    INCLUDEPATH += C:/MVS/Development/Includes
}
unix
{

}

QMAKE_CXXFLAGS_WARN_ON += -Wno-unknown-pragmas
