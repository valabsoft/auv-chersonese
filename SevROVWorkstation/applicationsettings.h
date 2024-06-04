#ifndef APPLICATIONSETTINGS_H
#define APPLICATIONSETTINGS_H

#include <QString>

class ApplicationSettings
{
private:
    const std::string APP_VERSION = "1.2.1";
public:
    ApplicationSettings();
    QString getAppVersion();
    void load();

    //const int CAMERA_WIDTH = 640;
    //const int CAMERA_HEIGHT = 480;
    const int CAMERA_WIDTH = 1280;
    const int CAMERA_HEIGHT = 960;
    //const int CAMERA_WIDTH = 1920;
    //const int CAMERA_HEIGHT = 1080;
    const int CAMERA_VIEW_X0 = 10;
    const int CAMERA_VIEW_Y0 = 10;
    const int CAMERA_VIEW_BORDER_WIDTH = 10;
    const int CONTROL_PANEL_WIDTH = 120;
    const int TOOL_PANEL_WIDHT = 200;
    const int TOOL_PANEL_HEIGHT = 25;

    const int CAMERA_FPS = 30;

    const int VIDEO_TIMER_INTERVAL = 50;

    const int CAMERA_ID = 0;
    const int CAMERA_LEFT_ID = 0;
    const int CAMERA_RIGHT_ID = 0;
};

#endif // APPLICATIONSETTINGS_H
