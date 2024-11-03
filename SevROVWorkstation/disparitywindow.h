#ifndef DISPARITYWINDOW_H
#define DISPARITYWINDOW_H

#include <QDialog>
#include <QScreen>
#include "applicationsettings.h"

#include <opencv2/calib3d.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/ximgproc.hpp>

namespace Ui {
class DisparityWindow;
}

class DisparityWindow : public QDialog
{
    Q_OBJECT

public:
    explicit DisparityWindow(QWidget *parent = nullptr);
    ~DisparityWindow();

    ApplicationSettings _appSet; // Уставки приложения

    void moveWindowToCenter();
    void setupWindowGeometry();
    void setupControlsStyle();

    int disparityMap(cv::Mat &map,
                     const cv::Mat& imageLeft,
                     const cv::Mat& imageRight,
                     int minDisparity,
                     int numDisparities,
                     int blockSize,
                     double lambda,
                     double sigma,
                     DISPARITY_TYPE disparityType,
                     int colorMap,
                     bool saveToFile,
                     bool showImages);

public slots:
    void onStereoCaptured(const cv::Mat &frameL, const cv::Mat &frameR);

private:
    Ui::DisparityWindow *ui;
};

#endif // DISPARITYWINDOW_H
