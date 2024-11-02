#include "applicationsettings.h"

ApplicationSettings::ApplicationSettings() {}

QString ApplicationSettings::getAppVersion()
{
    return QString(APP_VERSION.c_str());
}

void ApplicationSettings::load()
{
    _settingsFileName = QApplication::applicationDirPath() + QDir::separator() + "settings.ini";
    QSettings settings(_settingsFileName, QSettings::IniFormat);

    // Camera Settings
    settings.beginGroup("/CAMERA_SETTINGS");
    CAMERA_WIDTH = settings.value("/Width", "640").toInt();
    CAMERA_HEIGHT = settings.value("/Height", "480").toInt();
    CAMERA_FPS = settings.value("/FPS", "30").toInt();

    CAMERA_LEFT_ID = settings.value("/LeftID", "1").toInt();
    CAMERA_RIGHT_ID = settings.value("/RightID", "2").toInt();

    VIDEO_TIMER_INTERVAL = settings.value("/TimerInterval", "50").toInt();
    CAMERA_TYPE = settings.value("/CameraTypeID", 0).toInt();

    NUMBER_OF_SHOTS = settings.value("/NumberOfShots", 30).toInt();
    SHOTS_INTERVAL = settings.value("/ShotsInterval", 3).toInt();

    // Calibration File Settings
    CALIBRATION_FILE_NAME = settings.value("/CalibrationFileName", "camera_calibration_parameters").toString().toStdString();
    switch (settings.value("/CalibrationFileType", 1).toInt()) {
    case 0:
        CALIBRATION_FILE_TYPE = CalibrationFileType::XML;
        break;
    case 1:
        CALIBRATION_FILE_TYPE = CalibrationFileType::YML;
        break;
    default:
        CALIBRATION_FILE_TYPE = CalibrationFileType::YML;
        break;
    }

    settings.endGroup();
}
