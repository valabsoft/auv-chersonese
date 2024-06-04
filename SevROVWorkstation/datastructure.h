#ifndef DATASTRUCTURE_H
#define DATASTRUCTURE_H

#include <vector>

#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>


// TODO: Вынести в отдельный модуль
class Point3D {
public:
    // Координаты вершины
    double X;
    double Y;
    double Z;
    // Кол-во точек возле вершины
    int NumberOfPoint;
    // Имя точки
    std::string Name;
    Point3D(double x, double y, double z, std::string name) {
        X = x;
        Y = y;
        Z = z;
        Name = name;
    }
    // Метод для установки кол-ва точек
    void setNumberOfPoint(int numberofpoints)
    {
        NumberOfPoint = numberofpoints;
    }
    // Переопределяем оператор для возможности сортировки
    bool operator< (const Point3D& p) {
        return NumberOfPoint < p.NumberOfPoint;
    }
};

// Структура данных 3D точек после кластеризации
struct Data3DItem
{
    std::vector<int> vu;         // 2D координаты точки на изображении
    std::vector<double> xyz;     // 3D координаты точки в пространсве
    std::vector<int> rgb;        // Цвет точки
    int cluster;            // Номер кастера
};

//struct Data3DVector
//{
//    vector<vector<int>> vu;       // 2D координаты точки на изображении
//    vector<vector<double>> xyz;   // 3D координаты точки в пространсве
//    vector<vector<int>> rgb;      // Цвет точки
//    vector<int> cluster;               // Номер кастера
//};


struct StateInformation         // информация о состоянии
{
    int SecondaryPowerSupply;   // вторичное питание
    int ConnectToCams;          // соединение с камерами
    int ImageAquisition;        // получение изображения
    int SuccessfulWork;         // статус успешной работы
    int FuctSuccessfulWork;     // факт успешной работы
};

struct t_vuxyzrgb               // Структура в формате std:vector
{
    int num_p;                  // Количество точек
    int num_cl;                 // Количество кластеров
    std::vector<int> num_p_cl;       // Количесво точек в кластере

    int SelectedClaster;        // Номер кластера, выбранного оператором

    // до кластеризаци сразу полсе обнаружения 3D точек
    int num_p0;                 // Количество точек
    std::vector<std::vector<int>> vu0;    // 2D координаты точки на изображении
    std::vector<std::vector<double>> xyz0;// 3D координаты точки на пространсве
    std::vector<std::vector<int>> rgb0;   // цвет 3D точки

    // после кластеризации
    std::vector<std::vector<int>> vu;     // 2D координаты точки на изображении
    std::vector<std::vector<double>> xyz; // 3D координаты точки на пространсве
    std::vector<std::vector<int>> rgb;    // цвет точки
    std::vector<int> cluster;        // Для каждой точки номер кастера

    std::vector<std::vector<size_t>> clusters;// Для каждого кластера номер точки
    std::vector<std::vector<int>> objRect2D;  // Координаты рамки выделяющей объект на изображении

    std::vector<std::vector<double>> objRect3D;   // Координаты рамки выделяющей объект в 3D пространсве

    std::vector<double> distance2Point3D;    // координат 2-х выбранных точек и рассояние между ними

    // {vu_min.x, vu_min.y, vu_max.x, vu_max.y, Center2D.x, Center2D.y, radius2D}
    // {xyz_min.x, xyz_min.y, xyz_min.z, xyz_max.x, xyz_max.y, xyz_max.z, Center3D.x, Center3D.y,Center3D.z, radius3D }

    //=================================================
    // Координаты в системе координат робота
    //=================================================

    // Матрица трансформации для перевода из координат камеры в кординаты робота
    cv::Mat RTmatrix_cam2robot_cv = cv::Mat(4, 4, CV_64F);

    // Матрица вращения для пересчета координат при поворете камеры поворотным устройсвтвом
    cv::Mat RTmatrix_camRotation_cv = cv::Mat(4, 4, CV_64F);

    std::vector<std::vector<double>> objRect3D_robot_vec;// Координаты рамки выделяющей объект в 3D пространсве

    //=================================================
    // для храниения, какого вида примитив у объекта
    // 0 - не определено
    // 1 - плоскость
    // 2 - сфера
    // 3 - цилиндр

    int num_prim;               // количесво примитивов, должно совпадать с num_cl, введено так как иногда не совпадаес из-за рассинхронизации
    std::vector<int> PrimitiveDetect;//  виды примитивов объектов (всех кластеров)
    std::vector<std::vector<double>> PrimitiveParameter;  //  параметры модели привитивов (всех кластеров)

    // VA 3-fdu-2023: Неизвестный тип данных Point3d
    // vector<vector<Point3d>>PrimitivePoints;     // векотор точек плоскоси (4 шт) и цилиндра (30 шт) для прорисовки их на изображении


    cv::Mat xyz_Disparity_AllPoints;    // Координаты всех найденных точек в виде изображения uv по слоям xyz

    cv::Size imgSize;                   // Размер изображения

    std::vector<double> limit_WorkingArea;   // Границы рабоче зоны для стереокамры СТЗ, за её пределами ничего не видит

    //=================================================
    // Габариты объекта
    //=================================================
    double ObjSize_circumscribedLength = -1;
    double ObjSize_circumscribedWidth = -1;
    double ObjSize_circumscribedHeight = -1;
    double ObjSize_ObjLength = -1;
    double ObjSize_ObjWidth = -1;
};

struct Cloud3DItem
{
    int screenX;
    int screenY;
    double worldX;
    double worldY;
    double worldZ;
};

#endif // DATASTRUCTURE_H
