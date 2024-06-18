#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QImage>
#include <QFileDialog>

#include "applicationsettings.h"
#include "sevrovcontroller.h"
#include "enumclasses.h"
#include "datastructure.h"
#include "toolwindow.h"

#include "sevrovxboxcontroller.h"
#include "sevrovlibrary.h"
#include "sevrovconnector.h"

#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
private:
    ApplicationSettings _appSet;
    SevROVController _sevROV;
    SevROVXboxController *_jsController;
    XboxGamepad _xbox;

    // Инструмент "Линейка"
    ToolWindow *_toolWindow;

    long _cnt; // Счетчик вызовов

    // Joystick related
    QTimer *_controlTimer;

    ///////////////////////////////////////////////////////////////////////////
    // OpenCV related
    QTimer *_videoTimer;

    // VA (22-05-2024) TODO: Оптимизировать кол-во переменных
    cv::VideoCapture *_webCamO; // Моно-камера
    cv::VideoCapture *_webCamL; // Стерео-камера (левая)
    cv::VideoCapture *_webCamR; // Стерео-камера (правая)

    cv::Mat _sourceMatO;
    cv::Mat _sourceMatL;
    cv::Mat _sourceMatR;

    cv::Mat _destinationMatO;
    cv::Mat _destinationMatL;
    cv::Mat _destinationMatR;

    QImage _imgCamO;
    QImage _imgCamL;
    QImage _imgCamR;
    ///////////////////////////////////////////////////////////////////////////

    void setupIcons();
    void setupWindowGeometry();
    void setupCameraViewLayout(CameraView layouttype = CameraView::MONO);
    void setupConnectedControlsStyle(bool isconnected = false);

    void setupCameraConnection(CameraConnection connection = CameraConnection::ON);

    void onVideoTimer();
    t_vuxyzrgb getCloud3DPoints(int rows, int cols, bool norm);
    std::vector<Cloud3DItem> getCloud3DPoints(std::string pathtofile);
    t_vuxyzrgb convertCloud3DPoints(std::vector<Cloud3DItem> cloud, bool);

    void moveWindowToCenter();
    void roundedRectangle(
        cv::Mat& src,
        cv::Point topLeft,
        cv::Point bottomRight,
        const cv::Scalar lineColor,
        const int thickness,
        const int lineType,
        const int cornerRadius);

    void onControlTimer();

    void OnButtonA(short value);
    void OnButtonB(short value);
    void OnButtonX(short value);
    void OnButtonY(short value);
    void OnButtonLBumper(short value);
    void OnButtonRBumper(short value);
    void OnButtonView(short value);
    void OnButtonMenu(short value);
    void OnDPad(short value);

    void OnAxisLStickX(short value);
    void OnAxisLStickY(short value);
    void OnAxisRStickX(short value);
    void OnAxisRStickY(short value);
    void OnAxisLTrigger(short value);
    void OnAxisRTrigger(short value);

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onStartStopButtonClicked();
    void onViewButtonClicked();
    void onScreenshotButtonClicked();
    void onSettingsButtonClicked();

private:
    Ui::MainWindow *ui;

Q_SIGNALS:
    void update_fps_value(QString fps);
};
#endif // MAINWINDOW_H
