QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    applicationsettings.cpp \
    calibration.cpp \
    main.cpp \
    mainwindow.cpp \
    settingswindow.cpp

HEADERS += \
    applicationsettings.h \
    calibration.h \
    enumclasses.h \
    mainwindow.h \
    settingswindow.h

FORMS += \
    mainwindow.ui \
    settingswindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

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
