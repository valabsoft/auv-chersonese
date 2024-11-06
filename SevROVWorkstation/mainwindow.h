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
#include "disparitywindow.h"
#include "videostreaming.h"

#include "sevrovxboxcontroller.h"
#include "sevrovlibrary.h"
#include "sevrovconnector.h"

#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "MvCameraControl.h"

#include "pointcloud/three_dimensional_proc.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

enum class LOGTYPE
{
    DEBUG,		// Отладка			DEBG
    ERR,		// Ошибка			ERRR // ERROR - конфликтует с библиотекой стримера
    EXCEPTION,	// Исключение		EXCP
    INFO,		// Информация		INFO
    WARNING		// Предупреждение	WARN
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

    // Окно карты диспаратности
    DisparityWindow *_disparityWindow;

    // long _cnt; // Счетчик вызовов

    // Joystick related
    QTimer *_controlTimer;

    ///////////////////////////////////////////////////////////////////////////
    // OpenCV related
    QTimer *_videoTimer;

    cv::VideoCapture *_webCamL; // Стерео-камера (левая)
    cv::VideoCapture *_webCamR; // Стерео-камера (правая)

    cv::Mat _sourceMatL;
    cv::Mat _sourceMatR;
    cv::Mat _videoFrame;

    cv::Mat _destinationMatL;
    cv::Mat _destinationMatR;

    QImage _imgCamL;
    QImage _imgCamR;
    VideoStreaming *_leftCamStreaming;
    ///////////////////////////////////////////////////////////////////////////
    // IP camera related
    void* handleL = NULL;
    void* handleR = NULL;
    ///////////////////////////////////////////////////////////////////////////

    void setupIcons();
    void setupWindowGeometry();
    void setupCameraViewLayout(CameraView layouttype = CameraView::MONO);
    void setupConnectedControlsStyle(bool isconnected = false);

    void setupCameraConnection(CameraConnection connection = CameraConnection::ON);

    void onVideoTimer();
    t_vuxyzrgb getCloud3DPoints(int rows, int cols, bool norm);
    std::vector<Cloud3DItem> getCloud3DPoints(std::string pathtofile);
    t_vuxyzrgb convertCloud3DPoints(std::vector<Cloud3DItem> cloud);

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

    // Функции для инициализации модуля Hikrobot
    int MV_SDK_Initialization();
    int MV_SDK_Finalization();

    std::vector<cv::Mat> frames; // Буфер для хранения фреймов
    cv::Size cameraResolution; // Разрешение камеры
    int cameraFPS; // FPS камеры
    clock_t timerStart; // Таймер начала записи
    void videoRecorderInitialization();
    void writeLog(std::string logText, LOGTYPE logType);
    std::string generateUniqueLogFileName();
    void recordVideo(std::vector<cv::Mat> frames, int recordInterval, cv::Size cameraResolution);
    std::string generateFileName(std::string filename, std::string fileextension);

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
    void onDisparityButtonClicked();
    void onAcousticButtonClicked();

private:
    Ui::MainWindow *ui;

signals:
    // void updateCntValue(QString fps);
    void onStereoCaptured(const cv::Mat &frameL, const cv::Mat &frameR);
};
#endif // MAINWINDOW_H
