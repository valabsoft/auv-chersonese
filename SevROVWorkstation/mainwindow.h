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
#include "settingswindow.h"

#include "sevrovxboxcontroller.h"
#include "sevrovlibrary.h"
#include "sevrovconnector.h"

#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "pointcloud/three_dimensional_proc.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

// Данные телеметрии
struct DataControl
{
    float HorizontalVectorX;
    float HorizontalVectorY;
    float VericalThrust;
    float PowerTarget;
    float AngularVelocityZ;
    float ManipulatorState;
    float ManipulatorRotate;
    float CameraRotate;
    int8_t ResetInitialization;
    int8_t LightsState;
    int8_t StabilizationState;
    float RollInc;
    float PitchInc;
    int8_t ResetPosition;
    float RollKp;
    float RollKi;
    float RollKd;
    float PitchKp;
    float PitchKi;
    float PitchKd;
    float YawKp;
    float YawKi;
    float YawKd;
    float DepthKp;
    float DepthKi;
    float DepthKd;
    int8_t UpdatePID;
};

// Данные управления
struct DataTelemetry
{
    float Roll;
    float Pitch;
    float Yaw;
    float Heading;
    float Depth;
    float RollSetPoint;
    float PitchSetPoint;
};

class ControllerSettings
{
public:
    ControllerSettings() {
        powerLimit = 0.0;
        rollStabilization = false;
        pitchStabilization = false;
        yawStabilization = false;
        depthStabilization = false;

        updatePID = false;

        rollPID.setKp(0.1);
        rollPID.setKi(0.1);
        rollPID.setKd(0.1);

        pitchPID.setKp(0.1);
        pitchPID.setKi(0.1);
        pitchPID.setKd(0.1);

        yawPID.setKp(0.1);
        yawPID.setKi(0.1);
        yawPID.setKd(0.1);

        depthPID.setKp(0.1);
        depthPID.setKi(0.1);
        depthPID.setKd(0.1);
    };

    float powerLimit;
    bool rollStabilization;
    bool pitchStabilization;
    bool yawStabilization;
    bool depthStabilization;
    bool updatePID;

    SevROVPIDController rollPID;
    SevROVPIDController pitchPID;
    SevROVPIDController yawPID;
    SevROVPIDController depthPID;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT
private:    
    ApplicationSettings _appSet; // Уставки приложения
    ControllerSettings _ctrSet; // Уставки контроллера
    SevROVController _sevROV;
    SevROVXboxController *_jsController;
    SevROVConnector _rovConnector;
    XboxGamepad _xbox;

    DataControl _dataControl;
    DataTelemetry _dataTelemetry;

    // Инструмент "Линейка"
    ToolWindow *_toolWindow;

    // Окно настроек
    SettingsWindow *_settingsWindow;

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

public slots:
    void onSocketProcessTelemetryDatagram();
    void onSocketConnect();
    void onSocketDisconnect();

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
