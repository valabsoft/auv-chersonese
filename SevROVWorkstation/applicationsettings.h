#ifndef APPLICATIONSETTINGS_H
#define APPLICATIONSETTINGS_H

#include <QString>
#include <QDir>
#include <QSettings>
#include <QApplication>

#include "datastructure.h"

class ApplicationSettings
{
private:
    const std::string APP_VERSION = "1.2.1";
    QString _settingsFileName;
public:
    ApplicationSettings();
    QString getAppVersion();
    void load();
    void load(ControllerSettings &ctrset);

    int CAMERA_WIDTH = 1280;
    int CAMERA_HEIGHT = 960;

    const int CAMERA_VIEW_X0 = 10;
    const int CAMERA_VIEW_Y0 = 10;
    const int CAMERA_VIEW_BORDER_WIDTH = 10;
    const int CONTROL_PANEL_WIDTH = 120;
    const int TOOL_PANEL_WIDHT = 200;
    const int TOOL_PANEL_HEIGHT = 25;

    int CAMERA_FPS = 30;

    int VIDEO_TIMER_INTERVAL = 50;

    int CAMERA_MONO_ID = 0;
    int CAMERA_LEFT_ID = 1;
    int CAMERA_RIGHT_ID = 2;

    int JOYSTICK_ID = 0;
    int JOYSTICK_TIMER_INTERVAL = 100;
};

#endif // APPLICATIONSETTINGS_H
