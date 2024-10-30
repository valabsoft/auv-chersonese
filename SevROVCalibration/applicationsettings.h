#ifndef APPLICATIONSETTINGS_H
#define APPLICATIONSETTINGS_H

#include <QString>
#include <QDir>
#include <QSettings>
#include <QApplication>
#include <QDebug>

#include "enumclasses.h"

class ApplicationSettings
{
private:
    const std::string APP_VERSION = "1.3.3";
    QString _settingsFileName;
public:
    ApplicationSettings();
    QString getAppVersion();
    void load();

    int CAMERA_WIDTH = 640;
    int CAMERA_HEIGHT = 480;

    const int CAMERA_VIEW_X0 = 10;
    const int CAMERA_VIEW_Y0 = 10;
    const int CAMERA_VIEW_BORDER_WIDTH = 10;
    const int CONTROL_PANEL_WIDTH = 120;
    const int CONTROL_PANEL_HEIGHT = 60;
    const int TOOL_PANEL_WIDHT = 200;
    const int TOOL_PANEL_HEIGHT = 25;

    int CAMERA_FPS = 30;

    int VIDEO_TIMER_INTERVAL = 50;

    int CAMERA_LEFT_ID = 1;
    int CAMERA_RIGHT_ID = 2;

    int CAMERA_TYPE = CameraType::WEB;

    int NUMBER_OF_SHOTS = 30;
    int SHOTS_INTERVAL = 3;
};

#endif // APPLICATIONSETTINGS_H
