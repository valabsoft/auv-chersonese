#ifndef TOOLWINDOW_H
#define TOOLWINDOW_H

#include <QDialog>
#include <Q3DScatter>

#include "camerascene.h"

//#include <opencv2/opencv.hpp>
//#include <opencv2/core.hpp>
//#include <opencv2/videoio.hpp>
//#include <opencv2/highgui.hpp>
//#include <opencv2/imgproc/imgproc.hpp>

#include "datastructure.h"
#include "applicationsettings.h"

#include <algorithm>
#include <iterator>

namespace Ui {
class ToolWindow;
}

class ToolWindow : public QDialog
{
    Q_OBJECT

public:
    // explicit ToolWindow(cv::Mat image, t_vuxyzrgb data, QWidget *parent = nullptr);
    explicit ToolWindow(QWidget *parent = nullptr);
    ~ToolWindow();

    enum ToolMode {Mode2D, Mode3D};
    void setMode(ToolMode mode);
    void setDataCloud3D(cv::Mat image, t_vuxyzrgb data);
    // void setDataCloud3D(cv::Mat image, t_vuxyzrgb data, std::vector<Cloud3DItem> cloud);
    ToolMode getMode();
    void clearData();

    void setupWindowGeometry();

public slots:
    void updateInfoA(double X, double Y, double Z, double D);

private slots:
    void onClustersItemSelectionChanged();
    void on2DButtonClicked();
    void on3DButtonClicked();
    void onSaveButtonClicked();
    void onDeleteButtonClicked();

private:
    Ui::ToolWindow *ui;
    ApplicationSettings _appSet;
    cv::Mat _source;
    cv::Mat _destination;
    QImage _imgcam;
    t_vuxyzrgb _allPoints;
    t_vuxyzrgb _clusterPoints;

    ToolMode _toolMode;

    Q3DScatter *_graph3D;
    QScatter3DSeries *_series3D;
    QWidget *_container3D;

    std::vector<int> getClusterIDs(t_vuxyzrgb points);

    CameraScene *_cameraScene;
    QRect _originalSize;

    double _geometryL;
    double _geometryW;
    double _geometryH;
    double _geometryLength;
    double _geometryWidth;
    double _geometryDistance;

    double getDistance(Point3D p1, Point3D p2);
    size_t getSumCount(std::vector<double> X,
                       std::vector<double> Y,
                       std::vector<double> Z,
                       Point3D MN, Point3D M0);

    double getNpLinalgNorm(std::vector<double> a);

    std::vector<double> cross(std::vector<double> a,
                              std::vector<double> b);

    std::vector<double> substr(std::vector<double> a,
                               std::vector<double> b);

    double linesegDist(std::vector<double> p,
                        std::vector<double> a,
                        std::vector<double> b);

    void calculateSizes(t_vuxyzrgb data,     // Входной массив точек кластера
                        double* L,           // Выход - Длина
                        double* W,           // Выход - Ширина
                        double* H,           // Выход - Высота
                        double* Length,      // Выход - Длина осевой линии
                        double* Width,       // Выход - Ширина осевой линии
                        double* Distance);   // Выход - Расстояние до центра масс

    void setupIcons();
    void setupСontrolsStyle();
    void moveWindowToCenter();
};

#endif // TOOLWINDOW_H
