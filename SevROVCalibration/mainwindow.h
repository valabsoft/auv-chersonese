#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QScreen>
#include <QDebug>

#include "applicationsettings.h"
#include "enumclasses.h"
#include "settingswindow.h"
#include "calibration.h"

#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/calib3d.hpp>

#include "MvCameraControl.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void moveWindowToCenter();
    void setupWindowGeometry();
    void setupControlsStyle();

    void setupCameraConnection(ProcessStatus status);

    void onTimer();

    // Подготовка папок для сохранения результатов калибровки
    void folderPreparation();

    // Функции для инициализации модуля Hikrobot
    int MV_SDK_Initialization();
    int MV_SDK_Finalization();

private:
    Ui::MainWindow *ui;

    ApplicationSettings _appSet; // Уставки приложения

    ProcessStatus _videoCaptureStatus = ProcessStatus::OFF;
    ProcessStatus _calibrationStatus = ProcessStatus::OFF;

    QTimer *_timer;

    // Окно настроек
    SettingsWindow *_settingsWindow;

    cv::VideoCapture *_webCamL; // Стерео-камера (левая)
    cv::VideoCapture *_webCamR; // Стерео-камера (правая)

    cv::Mat _sourceMatL;
    cv::Mat _sourceMatR;

    cv::Mat _destinationMatL;
    cv::Mat _destinationMatR;

    QImage _imgCamL;
    QImage _imgCamR;

    int calibrationShotCounter = 0;
    int calibrationPauseCounter = 0;

    clock_t calibrationTimer;

    // IP camera related
    void* handleL = NULL;
    void* handleR = NULL;

private slots:
    void onVideoCaptureButtonClicked();
    void onCalibrationButtonClicked();
    void onCalcButtonClicked();
    void onSettingsButtonClicked();
    void onCloseButtonClicked();
};
#endif // MAINWINDOW_H
