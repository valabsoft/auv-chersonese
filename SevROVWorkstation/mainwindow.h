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

    // Инструмент "Линейка"
    ToolWindow *_toolWindow;

    long _cnt; // Счетчик вызовов

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

    void setup_icons();
    void setup_window_geometry();
    void setup_camera_view_layout(CameraView layouttype = CameraView::MONO);
    void setup_connected_controls_style(bool isconnected = false);

    void setup_camera_connection(CameraConnection connection = CameraConnection::ON);

    void on_video_timer();
    t_vuxyzrgb get_cloud_3D_points(int rows, int cols, bool norm);
    std::vector<Cloud3DItem> get_cloud_3D_points(std::string pathtofile);
    t_vuxyzrgb convert_cloud_3D_points(std::vector<Cloud3DItem> cloud, bool);

    void move_window_to_center();
    void rounded_rectangle(
        cv::Mat& src,
        cv::Point topLeft,
        cv::Point bottomRight,
        const cv::Scalar lineColor,
        const int thickness,
        const int lineType,
        const int cornerRadius);

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pbStartStop_clicked();

    void on_pbView_clicked();

    void on_pbScreenshot_clicked();

private:
    Ui::MainWindow *ui;

Q_SIGNALS:
    void update_fps_value(QString fps);
};
#endif // MAINWINDOW_H
