#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QScreen>

#include "applicationsettings.h"
#include "enumclasses.h"

#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc/imgproc.hpp>

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

    void setupCameraConnection(ProcessStatus connection);

    void onTimer();

private:
    Ui::MainWindow *ui;
    ApplicationSettings _appSet; // Уставки приложения
    ProcessStatus _processStatus = ProcessStatus::OFF;
    QTimer *_timer;

    cv::VideoCapture *_webCamL; // Стерео-камера (левая)
    cv::VideoCapture *_webCamR; // Стерео-камера (правая)

    cv::Mat _sourceMatL;
    cv::Mat _sourceMatR;

    cv::Mat _destinationMatL;
    cv::Mat _destinationMatR;

    QImage _imgCamL;
    QImage _imgCamR;

private slots:
    void onStartStopButtonClicked();
    void onCalibrationButtonClicked();
    void onSettingsButtonClicked();
    void onCloseButtonClicked();
};
#endif // MAINWINDOW_H
