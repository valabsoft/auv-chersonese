#include "toolwindow.h"
#include "qdatetime.h"
#include "qdir.h"
#include "ui_toolwindow.h"

#include <fstream>
#include <QListWidgetItem>
#include <QRadioButton>
#include <QGraphicsPixmapItem>
#include <QScreen>
#include <QMessageBox>

ToolWindow::ToolWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ToolWindow)
{
    ui->setupUi(this);

    // Инициализация указателей
    cameraScene = nullptr;
    series3D = nullptr;
    graph3D = nullptr;
    container3D = nullptr;

    // Установка иконок и стилей
    setup_icons();
    setup_controls_style();
}

void ToolWindow::move_window_to_center()
{
    auto primaryScreen = QGuiApplication::primaryScreen(); // Главный экран
    QRect primaryScreenRect = primaryScreen->availableGeometry(); // Размер главного экрана

    QPoint primaryScreenRectCenter = primaryScreenRect.center();
    primaryScreenRectCenter.setX(primaryScreenRectCenter.x() - (this->width()/2));
    primaryScreenRectCenter.setY(primaryScreenRectCenter.y() - (this->height()/2));
    move(primaryScreenRectCenter);
}

void ToolWindow::setup_window_geometry()
{
    int windowWidth = _appSet.CAMERA_WIDTH + _appSet.TOOL_PANEL_WIDHT + _appSet.CAMERA_VIEW_BORDER_WIDTH * 3;
    int windowHeight = _appSet.CAMERA_HEIGHT + _appSet.TOOL_PANEL_HEIGHT + _appSet.CAMERA_VIEW_BORDER_WIDTH * 3;

    // Фиксируем размер окна и убираем иконку ресайза
    setFixedSize(QSize(windowWidth, windowHeight));

    ui->graphicsView->setFixedWidth(_appSet.CAMERA_WIDTH);
    ui->graphicsView->setFixedHeight(_appSet.CAMERA_HEIGHT);

    // Центрируем окно в пределах экрана
    move_window_to_center();
}

void ToolWindow::set_data_cloud_3D(cv::Mat image, t_vuxyzrgb data, std::vector<Cloud3DItem> cloud)
{
    // Check the source image
    if (image.empty())
        return;

    // Image copy
    source = image.clone();

    // Data copy
    allPoints = data;

    // Get unique clusters IDs
    std::vector<int> clusterIDs;
    clusterIDs.push_back(0); // Для датасета Олега у нас только один кластер

    // Image preprocessing
    cv::cvtColor(source, destination, cv::COLOR_BGR2RGB);
    imgcam = QImage((uchar*) destination.data,
                    destination.cols,
                    destination.rows,
                    destination.step,
                    QImage::Format_RGB888);

    cameraScene = new CameraScene(imgcam);
    ui->graphicsView->setScene(cameraScene);
    // https://stackoverflow.com/questions/7772080/tracking-mouse-move-in-qgraphicsscene-class
    ui->graphicsView->setMouseTracking(true);

    // Добавляем слот-сигнал
    QObject::connect(cameraScene, &CameraScene::updateInfo, this, &ToolWindow::updateInfoA);

    ///////////////////////////////////////////////////////////////////////////
    // Создаем объекты для работы с 3D-графиком
    graph3D = new Q3DScatter();
    series3D = new QScatter3DSeries();

    series3D->setItemSize(0.2f);
    series3D->setMeshSmooth(true);

    graph3D->axisX()->setTitle("X");
    graph3D->axisY()->setTitle("Y");
    graph3D->axisZ()->setTitle("Z");

    series3D->setItemLabelFormat(
        QStringLiteral("@xTitle: @xLabel @yTitle: @yLabel @zTitle: @zLabel"));

    graph3D->setShadowQuality(QAbstract3DGraph::ShadowQualitySoftLow);
    graph3D->scene()->activeCamera()->setCameraPreset(Q3DCamera::CameraPresetFront);

    graph3D->addSeries(series3D);

    container3D = QWidget::createWindowContainer(graph3D);
    ///////////////////////////////////////////////////////////////////////////

    // Checkbox list generationi
    for (int i : clusterIDs)
    {
        // Checkbox List
        // QListWidgetItem *item = new QListWidgetItem;
        // item->setText("Claster " + QString::number(i + 1));
        // item->setCheckState(Qt::Unchecked);
        // ui->lswClusters->addItem(item);

        // Radiobutton list
        QListWidgetItem *item = new QListWidgetItem(ui->lswClusters);
        ui->lswClusters->setItemWidget(
            item,
            new QRadioButton(QString("Cluster %1").arg(i)));
    }

    // Check the first item
    if (ui->lswClusters->count() > 0)
    {
        auto firstItem =
            static_cast<QRadioButton*>(
                ui->lswClusters->itemWidget(ui->lswClusters->item(0)));
        firstItem->setChecked(true);

        ui->lswClusters->item(0)->setSelected(true);
    }

    // Установка темы
    Q3DTheme *currentTheme = graph3D->activeTheme();
    currentTheme->setBackgroundEnabled(false);
    currentTheme->setType(Q3DTheme::ThemeArmyBlue);

    setMode(ToolMode::Mode2D);

    // Для фиксации кнопок с правой стороны (сбивает выравнивание сцены)
    // ui->verticalLayoutBtn->setAlignment(Qt::AlignRight);

    ui->btnDelete->setVisible(false);
}

void ToolWindow::set_data_cloud_3D(cv::Mat image, t_vuxyzrgb data)
{
    // Подгоняем размер сцены под размер изображения на входе
    ui->graphicsView->setFixedWidth(image.cols);
    ui->graphicsView->setFixedHeight(image.rows);

    // Центрируем окно в пределах экрана
    move(screen()->geometry().center() - frameGeometry().center());

    // Запоминаем текущую геометрию
    originalSize = this->geometry();

    // Фиксируем форму и запрещаем изменение размеров пользователем
    //this->layout()->setSizeConstraint(QLayout::SetFixedSize);
    //this->ui->verticalLayout->setAlignment(Qt::AlignCenter);
    //this->ui->verticalLayoutBtn->setAlignment(Qt::AlignRight);

    // Check the source image
    if (image.empty())
        return;

    // Image copy
    source = image.clone();

    // Data copy
    allPoints = data;
    // Get unique clusters IDs
    std::vector<int> clusterIDs = getClusterIDs(allPoints);


    // Image preprocessing
    cv::cvtColor(source, destination, cv::COLOR_BGR2RGB);
    imgcam = QImage((uchar*) destination.data,
                    destination.cols,
                    destination.rows,
                    destination.step,
                    QImage::Format_RGB888);

    cameraScene = new CameraScene(imgcam);
    ui->graphicsView->setScene(cameraScene);
    // https://stackoverflow.com/questions/7772080/tracking-mouse-move-in-qgraphicsscene-class
    ui->graphicsView->setMouseTracking(true);

    // Добавляем слот-сигнал
    QObject::connect(cameraScene, &CameraScene::updateInfo, this, &ToolWindow::updateInfoA);

    ///////////////////////////////////////////////////////////////////////////
    // Создаем объекты для работы с 3D-графиком
    graph3D = new Q3DScatter();
    series3D = new QScatter3DSeries();

    series3D->setItemSize(0.2f);
    series3D->setMeshSmooth(true);

    graph3D->axisX()->setTitle("X");
    graph3D->axisY()->setTitle("Y");
    graph3D->axisZ()->setTitle("Z");

    series3D->setItemLabelFormat(
        QStringLiteral("@xTitle: @xLabel @yTitle: @yLabel @zTitle: @zLabel"));

    graph3D->setShadowQuality(QAbstract3DGraph::ShadowQualitySoftLow);
    graph3D->scene()->activeCamera()->setCameraPreset(Q3DCamera::CameraPresetFront);

    graph3D->addSeries(series3D);

    container3D = QWidget::createWindowContainer(graph3D);
    ///////////////////////////////////////////////////////////////////////////

    // Checkbox list generationi
    for (int i : clusterIDs)
    {
        // Checkbox List
        // QListWidgetItem *item = new QListWidgetItem;
        // item->setText("Claster " + QString::number(i + 1));
        // item->setCheckState(Qt::Unchecked);
        // ui->lswClusters->addItem(item);

        // Radiobutton list
        QListWidgetItem *item = new QListWidgetItem(ui->lswClusters);
        ui->lswClusters->setItemWidget(
            item,
            new QRadioButton(QString("Cluster %1").arg(i)));
    }

    // Check the first item
    if (ui->lswClusters->count() > 0)
    {
        auto firstItem =
            static_cast<QRadioButton*>(
                ui->lswClusters->itemWidget(ui->lswClusters->item(0)));
        firstItem->setChecked(true);

        ui->lswClusters->item(0)->setSelected(true);
    }

    // Установка темы
    Q3DTheme *currentTheme = graph3D->activeTheme();
    currentTheme->setBackgroundEnabled(false);
    currentTheme->setType(Q3DTheme::ThemeArmyBlue);

    setMode(ToolMode::Mode2D);

    // Для фиксации кнопок с правой стороны (сбивает выравнивание сцены)
    // ui->verticalLayoutBtn->setAlignment(Qt::AlignRight);

    ui->btnDelete->setVisible(false);
}

ToolWindow::~ToolWindow()
{
    if (cameraScene != nullptr)
        delete cameraScene;

    if (series3D != nullptr)
        delete series3D;

    if (graph3D != nullptr)
        delete graph3D;

    if (container3D != nullptr)
        delete container3D;

    delete ui;
}
////////////////////////////////////////////////////////////////////////////////
// Функции для рассчета геометрии точек
////////////////////////////////////////////////////////////////////////////////
double ToolWindow::get_distance(Point3D p1, Point3D p2)
{
    return std::sqrt((p2.X - p1.X) * (p2.X - p1.X) +
                     (p2.Y - p1.Y) * (p2.Y - p1.Y) +
                     (p2.Z - p1.Z) * (p2.Z - p1.Z));
}

size_t ToolWindow::get_sum_count(std::vector<double> X,
                                 std::vector<double> Y,
                                 std::vector<double> Z,
                                 Point3D MN, Point3D M0)
{
    double max_dist = get_distance(MN, M0);
    size_t N = 0;
    for (size_t i = 0; i < X.size(); i++) {
        double dist = get_distance(MN, Point3D(X[i], Y[i], Z[i], "M"));
        if (dist < max_dist / 2)
            N += 1;
    }
    return N;
}

double ToolWindow::get_np_linalg_norm(std::vector<double> a)
{
    double res = 0;
    for (size_t i = 0; i < a.size(); i++) {
        res += std::pow(a[i], 2);
    }
    return std::sqrt(std::abs(res));
}

std::vector<double> ToolWindow::cross(std::vector<double> a,
                                      std::vector<double> b)
{
    double x1 = a[1] * b[2] - a[2] * b[1];
    double y1 = a[2] * b[0] - a[0] * b[2];
    double z1 = a[0] * b[1] - a[1] * b[0];
    std::vector<double> res = { x1, y1, z1 };
    return res;
}

std::vector<double> ToolWindow::substr(std::vector<double> a,
                                       std::vector<double> b)
{
    std::vector<double> res = a;
    for (size_t i = 0; i < a.size(); i++) {
        res[i] -= b[i];
    }
    return res;
}

double ToolWindow::lineseg_dist(std::vector<double> p,
                                std::vector<double> a,
                                std::vector<double> b)
{
    std::vector<double> AB = substr(b, a);
    std::vector<double> AC = substr(p, a);
    double area = get_np_linalg_norm(cross(AB, AC));
    double CD = area / get_np_linalg_norm(AB);
    return CD;
}

void ToolWindow::calculate_sizes(t_vuxyzrgb data,
                                 double* L,
                                 double* W,
                                 double* H,
                                 double* Length,
                                 double* Width,
                                 double* Distance)
{
    // Формируем набор точек для выполнения расчетов
    std::vector<double> X;
    std::vector<double> Y;
    std::vector<double> Z;

    for (size_t i = 0; i < data.cluster.size(); i++)
    {
        X.push_back(data.xyz.at(i).at(0));
        Y.push_back(data.xyz.at(i).at(1));
        Z.push_back(data.xyz.at(i).at(2));
    }

    // Определение максимумов и минимумов для вычисление граничных точек
    double Xmin = *min_element(X.begin(), X.end());
    double Xmax = *max_element(X.begin(), X.end());

    double Ymin = *min_element(Y.begin(), Y.end());
    double Ymax = *max_element(Y.begin(), Y.end());

    double Zmin = *min_element(Z.begin(), Z.end());
    double Zmax = *max_element(Z.begin(), Z.end());

    // Центр масс
    double X0 = Xmin + (Xmax - Xmin) / 2.0;
    double Y0 = Ymin + (Ymax - Ymin) / 2.0;
    double Z0 = Zmin + (Zmax - Zmin) / 2.0;

    // Граничные точки
    Point3D M0 = Point3D(X0, Y0, Z0, "M0");
    Point3D M1 = Point3D(Xmin, Ymin, Zmin, "M1");
    Point3D M2 = Point3D(Xmax, Ymin, Zmin, "M2");
    Point3D M3 = Point3D(Xmax, Ymax, Zmin, "M3");
    Point3D M4 = Point3D(Xmin, Ymax, Zmin, "M4");
    Point3D M5 = Point3D(Xmin, Ymin, Zmax, "M5");
    Point3D M6 = Point3D(Xmax, Ymin, Zmax, "M6");
    Point3D M7 = Point3D(Xmax, Ymax, Zmax, "M7");
    Point3D M8 = Point3D(Xmin, Ymax, Zmax, "M8");

    // Поиск граничных точек, через которые пройдет ось
    size_t m1 = get_sum_count(X, Y, Z, M1, M0);
    size_t m2 = get_sum_count(X, Y, Z, M2, M0);
    size_t m3 = get_sum_count(X, Y, Z, M3, M0);
    size_t m4 = get_sum_count(X, Y, Z, M4, M0);
    size_t m5 = get_sum_count(X, Y, Z, M5, M0);
    size_t m6 = get_sum_count(X, Y, Z, M6, M0);
    size_t m7 = get_sum_count(X, Y, Z, M7, M0);
    size_t m8 = get_sum_count(X, Y, Z, M8, M0);

    // Устанавливаем кол-во точек около габаритной точки
    M1.setNumberOfPoint(m1);
    M2.setNumberOfPoint(m2);
    M3.setNumberOfPoint(m3);
    M4.setNumberOfPoint(m4);
    M5.setNumberOfPoint(m5);
    M6.setNumberOfPoint(m6);
    M7.setNumberOfPoint(m7);
    M8.setNumberOfPoint(m8);

    // Формируем список
    std::list<Point3D> M;
    M.push_back(M1);
    M.push_back(M2);
    M.push_back(M3);
    M.push_back(M4);
    M.push_back(M5);
    M.push_back(M6);
    M.push_back(M7);
    M.push_back(M8);

    // Сортировка
    M.sort();
    M.reverse();

    // Формируем список осевых точек
    std::list<Point3D> P;

    // Первая точка берется из отсортированного списка MD
    P.push_back(M.front());
    auto& P0 = M.front();

    // Вспомогательные переменные
    double m0_dist;
    double p0_dist;
    bool same_x;
    bool same_y;
    bool same_z;
    bool same_xyz;

    // Начинаем проверку со второй точки
    for (auto it = std::next(M.begin()); it != M.end(); ++it)
    {
        m0_dist = get_distance(*it, M0);  // Расстояние от текущей точки до центра масс
        p0_dist = get_distance(*it, P0);  // Расстояние между точками P0 и текущей

        // Проверяем, не лежат ли точки в одной плоскости
        same_x = it->X == P0.X;
        same_y = it->Y == P0.Y;
        same_z = it->Z == P0.Z;
        same_xyz = !(same_x || same_y || same_z);
        // if (m0_dist < p0_dist)
        if ((m0_dist < p0_dist) && same_xyz)
        {
            // Добавляем найденную точку в список
            P.push_back(*it);
            // Если нужная точка найдена, прекращаем перебор
            break;
        }
    }
    auto& P1 = P.back();

    // Вычисление длины
    double length = sqrt(pow(P1.X - P0.X, 2) +
                         pow(P1.Y - P0.Y, 2) +
                         pow(P1.Z - P0.Z, 2));

    // Вычисление ширины
    std::vector<double> dists;
    for (size_t i = 0; i < X.size(); i++) {
        std::vector<double> a = { P0.X, P0.Y, P0.Z };
        std::vector<double> b = { P1.X, P1.Y, P1.Z };
        std::vector<double> p = { X[i], Y[i], Z[i] };
        dists.push_back(lineseg_dist(p, a, b));
    }
    double max_dist = *max_element(dists.begin(), dists.end()) * 2;

    // Вывод габаритов параллелепипеда, где (L-длина W-ширина H-высота )
    *L = get_distance(M1, M2);
    *W = get_distance(M2, M3);
    *H = get_distance(M3, M7);

    *Length = length;
    *Width = max_dist;

    // Расчет расстояния до камеры
    Point3D CaM0 = Point3D(0, 0, 0, "CaM0");
    *Distance = get_distance(CaM0, M0);
}
////////////////////////////////////////////////////////////////////////////////
std::vector<int> ToolWindow::getClusterIDs(t_vuxyzrgb points)
{
    std::vector<int> clusterIDs = points.cluster;
    std::vector<int>::iterator it;
    std::sort(clusterIDs.begin(), clusterIDs.end());

    it = std::unique(clusterIDs.begin(), clusterIDs.end());
    clusterIDs.resize(std::distance(clusterIDs.begin(), it));

    return clusterIDs;
}

void ToolWindow::on_lswClusters_itemSelectionChanged()
{
    auto selectedItem = static_cast<QRadioButton*>(
        ui->lswClusters->itemWidget(ui->lswClusters->selectedItems().first()));
    QString seletedText = selectedItem->text();
    int ID = seletedText.remove("Cluster ").toInt();

    // Remove old data
    clusterPoints.cluster.clear();
    clusterPoints.rgb.clear();
    clusterPoints.vu.clear();
    clusterPoints.xyz.clear();

    // Filter only selected cluster data
    for (size_t i = 0; i < allPoints.cluster.size(); i++)
    {
        if (allPoints.cluster.at(i) == ID)
        {
            clusterPoints.cluster.push_back(allPoints.cluster.at(i));
            clusterPoints.rgb.push_back(allPoints.rgb.at(i));
            clusterPoints.vu.push_back(allPoints.vu.at(i));
            clusterPoints.xyz.push_back(allPoints.xyz.at(i));
        }
    }

    // Debug Information
    qDebug() << "============================================================";
    qDebug() << "Selected Cluster ID: " << ID << "(" <<
        clusterPoints.cluster.size() << "/" << allPoints.cluster.size() << ")";
    qDebug() << "============================================================";

    cameraScene->removeRule(); // Удаляем старую линейку (если была создана)
    cameraScene->set3DPoints(clusterPoints);

    ///////////////////////////////////////////////////////////////////////////
    // Обновляем набор точек для рисования 3D
    ///////////////////////////////////////////////////////////////////////////

    // Удалить старые точки
    series3D->dataProxy()->removeItems(0, series3D->dataProxy()->itemCount());

    // Получить новые точки
    QScatterDataArray data;
    for (size_t i = 0; i < clusterPoints.cluster.size(); i++)
    {
        data << QVector3D(clusterPoints.xyz.at(i).at(0),
                          clusterPoints.xyz.at(i).at(1),
                          clusterPoints.xyz.at(i).at(2));
    }

    // Предобратока облака точек (удаление выбросов)

    // Рассчет геометрии точек
    double L, W, H, Length, Width, Distance;
    calculate_sizes(clusterPoints, &L, &W, &H, &Length, &Width, &Distance);

    qDebug() << "L: " << L;
    qDebug() << "W: " << W;
    qDebug() << "H: " << H;
    qDebug() << "Length: " << Length;
    qDebug() << "Width: " << Width;
    qDebug() << "Distance: " << Distance;

    geometryL = L;
    geometryW = W;
    geometryH = H;
    geometryLength = Length;
    geometryWidth = Width;
    geometryDistance = Distance;

    ui->lbInfo->setText("L:\t\t" + QString::number(L, 'f', 1) + "\n" +
                        "W:\t\t" + QString::number(W, 'f', 1) + "\n" +
                        "H:\t\t" + QString::number(H, 'f', 1) + "\n" +
                        "Length:\t\t" + QString::number(Length, 'f', 1) + "\n" +
                        "Width:\t\t" + QString::number(Width, 'f', 1) + "\n" +
                        "Distance:\t\t" + QString::number(Distance, 'f', 1));

    // Передать точки в объект Series
    series3D->dataProxy()->addItems(data);

    ///////////////////////////////////////////////////////////////////////////
}

void ToolWindow::on_btn2D_clicked()
{
    if (getMode() == ToolWindow::Mode3D)
    {
        container3D->setVisible(false);

        ui->verticalLayout->removeWidget(container3D);
        ui->verticalLayout->addWidget(ui->graphicsView);

        ui->graphicsView->setVisible(true);
        ui->btnDelete->setVisible(false);

        setMode(ToolMode::Mode2D);
    }
}

void ToolWindow::on_btn3D_clicked()
{
    if (getMode() == ToolWindow::Mode2D)
    {
        ui->graphicsView->setVisible(false);

        ui->verticalLayout->removeWidget(ui->graphicsView);
        ui->verticalLayout->addWidget(container3D);

        container3D->setVisible(true);
        ui->btnDelete->setVisible(true);

        // Восстанавливаем геометрию -- Костыль для фиксированной формы
        //this->setFixedHeight(originalSize.height());
        //this->setFixedWidth(originalSize.width());
        //this->adjustSize();

        setMode(ToolMode::Mode3D);
    }
}

void ToolWindow::setMode(ToolMode mode)
{
    toolMode = mode;
}

ToolWindow::ToolMode ToolWindow::getMode()
{
    return toolMode;
}

void ToolWindow::on_btnSave_clicked()
{
    // Создаем папку output, если она не существует
    QDir dir("output");
    if (!dir.exists())
        dir.mkpath(".");

    // Текущие дата и время
    QDateTime date = QDateTime::currentDateTime();
    QString timeStamp = date.toString("dd_MM_yyyy_hh_mm_ss");

    // Генерируем имя файла и сохраняем сцену снимка с линейкой
    QString sceneImage = QString("output") + QDir::separator() +
                         QString("scene_" + timeStamp + ".png");
    // Захват сцены снимка с линейкой
    //QPixmap sceneMap = ui->graphicsView->grab(
    //    ui->graphicsView->sceneRect().toRect());

    // Захват только картинки без учета других компонент
    QPixmap sceneMap = ui->graphicsView->grab(ui->graphicsView->contentsRect());
    sceneMap.save(sceneImage);



    // Генерируем имя файла и сохраняем сцену 3D графика с линейкой
    QString chartImage = QString("output") + QDir::separator() +
                         QString("chart_" + timeStamp + ".png");
    QImage graph3DImage = graph3D->renderToImage();
    QPixmap chartMap;
    chartMap.convertFromImage(graph3DImage);
    chartMap.save(chartImage);

    // Сохранение результатов в файл
    QString odometry = QString("output") + QDir::separator() +
                       QString("odometry_" + timeStamp + ".txt");

    std::ofstream odomFile(odometry.toStdString(), std::ofstream::out | std::ofstream::trunc);
    odomFile << "L:\t\t" + QString::number(geometryL, 'f', 1).toStdString() << std::endl;
    odomFile << "H:\t\t" + QString::number(geometryH, 'f', 1).toStdString() << std::endl;
    odomFile << "W:\t\t" + QString::number(geometryW, 'f', 1).toStdString() << std::endl;
    odomFile << "Length:\t\t" + QString::number(geometryLength, 'f', 1).toStdString() << std::endl;
    odomFile << "Width:\t\t" + QString::number(geometryWidth, 'f', 1).toStdString() << std::endl;
    odomFile << "Distance:\t" + QString::number(geometryDistance, 'f', 1).toStdString() << std::endl;
    odomFile.close();

    QMessageBox msgBox;
    msgBox.setWindowTitle("Информация");
    msgBox.setText(QString("Экспорт результатов завершен:\n") +
                   QString(sceneImage + "\n") +
                   QString(chartImage + "\n") +
                   QString(odometry + "\n"));

    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.exec();

}

void ToolWindow::updateInfoA(double X, double Y, double Z, double D)
{
    QString text = "X: " + QString::number(X, 'f', 1) + "; " +
                   "Y: " + QString::number(Y, 'f', 1) + "; " +
                   "Z: " + QString::number(Z, 'f', 1) + "; " +
                   "D: " + QString::number(D, 'f', 1);

    // Если точка не выделена, стираем информацию о предыдущем выделении
    if ((X == 0) & (Y == 0) & (Z == 0) & (D == 0))
        text = "";

    ui->lineEditInfo->setText(text);
}

void ToolWindow::on_btnDelete_clicked()
{
    int ind = series3D->selectedItem();
    if (ind == -1)
        return;

    // Удалить выделенную точку
    series3D->dataProxy()->removeItems(ind, 1);

    auto data = series3D->dataProxy()->array();
    t_vuxyzrgb newcluster;

    for (size_t i = 0; i < (size_t)data->size(); i++)
    {
        std::vector<double> xyz;

        xyz.push_back(data->at(i).x());
        xyz.push_back(data->at(i).y());
        xyz.push_back(data->at(i).z());

        newcluster.xyz.push_back(xyz);
        // Добавляем фиктивный признак принадлежности кластеру
        newcluster.cluster.push_back(1);
    }

    // Рассчет геометрии точек

    double L, W, H, Length, Width, Distance;
    calculate_sizes(newcluster, &L, &W, &H, &Length, &Width, &Distance);

    qDebug() << "L: " << L;
    qDebug() << "W: " << W;
    qDebug() << "H: " << H;
    qDebug() << "Length: " << Length;
    qDebug() << "Width: " << Width;
    qDebug() << "Distance: " << Distance;

    geometryL = L;
    geometryW = W;
    geometryH = H;
    geometryLength = Length;
    geometryWidth = Width;
    geometryDistance = Distance;

    ui->lbInfo->setText("L:\t\t" + QString::number(L, 'f', 1) + "\n" +
                        "W:\t\t" + QString::number(W, 'f', 1) + "\n" +
                        "H:\t\t" + QString::number(H, 'f', 1) + "\n" +
                        "Length:\t\t" + QString::number(Length, 'f', 1) + "\n" +
                        "Width:\t\t" + QString::number(Width, 'f', 1) + "\n" +
                        "Distance:\t\t" + QString::number(Distance, 'f', 1));
}

void ToolWindow::setup_icons()
{
    ui->btn2D->setIcon(QIcon(":/img/ruler_2d_icon.png"));
    ui->btn2D->setIconSize(QSize(64, 64));
    ui->btn3D->setIcon(QIcon(":/img/ruler_3d_icon.png"));
    ui->btn3D->setIconSize(QSize(64, 64));
    ui->btnSave->setIcon(QIcon(":/img/download_icon.png"));
    ui->btnSave->setIconSize(QSize(32, 32));
    ui->btnDelete->setIcon(QIcon(":/img/remove_icon.png"));
    ui->btnDelete->setIconSize(QSize(32, 32));
}

void ToolWindow::setup_controls_style()
{
    ui->graphicsView->setStyleSheet("QGraphicsView {"
                                    "border-style: solid;"
                                    "border-width: 1px;"
                                    "border-color: #F0BE50; "
                                    "}");
    //ui->lswClusters->setStyleSheet("QListWidget {"
    //                               "border-style: solid;"
    //                               "border-width: 1px;"
    //                               "border-color: #F0BE50; "
    //                               "}");

    ui->lswClusters->setStyleSheet("border-style: solid; border-width: 1px; border-color: #F0BE50; ");

    QFont fontBold12("GOST type A", 12, QFont::Bold);
    QFont fontNormal12("GOST type A", 12, QFont::Bold);

    ui->lbInfo->setStyleSheet("background-color : black; color : silver;");
    ui->lbInfo->setFont(fontBold12);

    ui->lswClusters->setStyleSheet("background-color : black; color : silver;");
    ui->lswClusters->setFont(fontNormal12);

    ui->lineEditInfo->setStyleSheet("border-style: solid; border-width: 1px; border-color: #F0BE50; background-color: black; color: silver;");
    ui->lineEditInfo->setFont(fontNormal12);

}

void ToolWindow::clear_data()
{
    ui->lswClusters->clear();
}
