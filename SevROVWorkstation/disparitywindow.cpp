#include "disparitywindow.h"
#include "ui_disparitywindow.h"

DisparityWindow::DisparityWindow(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::DisparityWindow)
{
    ui->setupUi(this);

    _appSet.load();

    setupWindowGeometry();
    setupControlsStyle();
}

DisparityWindow::~DisparityWindow()
{
    delete ui;
}

void DisparityWindow::moveWindowToCenter()
{
    auto primaryScreen = QGuiApplication::primaryScreen(); // Главный экран
    QRect primaryScreenRect = primaryScreen->availableGeometry(); // Размер главного экрана
    QPoint primaryScreenRectCenter = primaryScreenRect.center();
    primaryScreenRectCenter.setX(primaryScreenRectCenter.x() - (this->width()/2));
    primaryScreenRectCenter.setY(primaryScreenRectCenter.y() - (this->height()/2));
    move(primaryScreenRectCenter);
}

void DisparityWindow::setupWindowGeometry()
{
    // Установка размера главного окна// Установка размера главного окна
    int windowWidth = 640 + _appSet.CAMERA_VIEW_BORDER_WIDTH * 2 + 20;
    int windowHeight = 480 + _appSet.CAMERA_VIEW_BORDER_WIDTH * 2 + 70;

    // Фиксируем размер окна и убираем иконку ресайза
    setFixedSize(QSize(windowWidth, windowHeight));

    moveWindowToCenter(); // Центрируем окно

    // Геометрия окон камер
    ui->lbDisparity->setGeometry(
        _appSet.CAMERA_VIEW_X0 + 10,
        _appSet.CAMERA_VIEW_Y0,
        640,
        480);

    QRect mainWindowRect = this->geometry();

    ui->sbMinDisparity->setGeometry(
        _appSet.CAMERA_VIEW_X0 + 10,
        mainWindowRect.height() - _appSet.CAMERA_VIEW_BORDER_WIDTH - ui->sbMinDisparity->height(),
        ui->sbMinDisparity->width(),
        ui->sbMinDisparity->height());

    ui->sbNumDisparities->setGeometry(
        ui->sbMinDisparity->x() + ui->sbMinDisparity->width() + _appSet.CAMERA_VIEW_BORDER_WIDTH,
        mainWindowRect.height() - _appSet.CAMERA_VIEW_BORDER_WIDTH - ui->sbNumDisparities->height(),
        ui->sbNumDisparities->width(),
        ui->sbNumDisparities->height());

    ui->sbBlockSIze->setGeometry(
        ui->sbNumDisparities->x() + ui->sbNumDisparities->width() + _appSet.CAMERA_VIEW_BORDER_WIDTH,
        mainWindowRect.height() - _appSet.CAMERA_VIEW_BORDER_WIDTH - ui->sbBlockSIze->height(),
        ui->sbBlockSIze->width(),
        ui->sbBlockSIze->height());

    ui->sbLambda->setGeometry(
        ui->sbBlockSIze->x() + ui->sbBlockSIze->width() + _appSet.CAMERA_VIEW_BORDER_WIDTH,
        mainWindowRect.height() - _appSet.CAMERA_VIEW_BORDER_WIDTH - ui->sbLambda->height(),
        ui->sbLambda->width(),
        ui->sbLambda->height());

    ui->sbSigma->setGeometry(
        ui->sbLambda->x() + ui->sbLambda->width() + _appSet.CAMERA_VIEW_BORDER_WIDTH,
        mainWindowRect.height() - _appSet.CAMERA_VIEW_BORDER_WIDTH - ui->sbSigma->height(),
        ui->sbSigma->width(),
        ui->sbSigma->height());

    ui->cbColorMap->setGeometry(
        ui->sbSigma->x() + ui->sbSigma->width() + _appSet.CAMERA_VIEW_BORDER_WIDTH,
        mainWindowRect.height() - _appSet.CAMERA_VIEW_BORDER_WIDTH - ui->cbColorMap->height(),
        ui->cbColorMap->width(),
        ui->cbColorMap->height());

    ui->cbDisparityType->setGeometry(
        ui->cbColorMap->x() + ui->cbColorMap->width() + _appSet.CAMERA_VIEW_BORDER_WIDTH,
        mainWindowRect.height() - _appSet.CAMERA_VIEW_BORDER_WIDTH - ui->cbDisparityType->height(),
        ui->cbDisparityType->width(),
        ui->cbDisparityType->height());

    ui->lbMinDisparity->setGeometry(
        ui->sbMinDisparity->x(),
        ui->sbMinDisparity->y() - ui->lbMinDisparity->height() - 5,
        ui->lbMinDisparity->width(),
        ui->lbMinDisparity->height());

    ui->lbNumDisparity->setGeometry(
        ui->sbNumDisparities->x(),
        ui->sbNumDisparities->y() - ui->lbNumDisparity->height() - 5,
        ui->lbNumDisparity->width(),
        ui->lbNumDisparity->height());

    ui->lbBlockSize->setGeometry(
        ui->sbBlockSIze->x(),
        ui->sbBlockSIze->y() - ui->lbBlockSize->height() - 5,
        ui->lbBlockSize->width(),
        ui->lbBlockSize->height());

    ui->lbLambda->setGeometry(
        ui->sbLambda->x(),
        ui->sbLambda->y() - ui->lbLambda->height() - 5,
        ui->lbLambda->width(),
        ui->lbLambda->height());

    ui->lbSigma->setGeometry(
        ui->sbSigma->x(),
        ui->sbSigma->y() - ui->lbSigma->height() - 5,
        ui->lbSigma->width(),
        ui->lbSigma->height());

    ui->lbColor->setGeometry(
        ui->cbColorMap->x(),
        ui->cbColorMap->y() - ui->lbColor->height() - 5,
        ui->lbColor->width(),
        ui->lbColor->height());

    ui->lbDisparityType->setGeometry(
        ui->cbDisparityType->x(),
        ui->cbDisparityType->y() - ui->lbDisparityType->height() - 5,
        ui->lbDisparityType->width(),
        ui->lbDisparityType->height());
}

void DisparityWindow::setupControlsStyle()
{
    QString QLabelCameraStyle = "QLabel {"
                                "border-style: solid;"
                                "border-width: 1px;"
                                "border-color: #F0BE50;"
                                "}";

    QString QSpinBoxStyle = "QSpinBox {"
                            "border-style: solid;"
                            "border-width: 1px;"
                            "border-color: silver;"
                            "color: silver;"
                            "}";

    QString QComboBoxStyle = "QComboBox {"
                             "border-style: solid;"
                             "border-width: 1px;"
                             "border-color: silver;"
                             "color: silver;"
                             "}"
                             "QListView { color: silver; }";

    QString QLabelTextStyle = "QLabel {"
                              "color: silver;"
                              "}";

    ui->lbDisparity->setStyleSheet(QLabelCameraStyle);

    ui->sbMinDisparity->setStyleSheet(QSpinBoxStyle);
    ui->sbNumDisparities->setStyleSheet(QSpinBoxStyle);
    ui->sbBlockSIze->setStyleSheet(QSpinBoxStyle);
    ui->sbLambda->setStyleSheet(QSpinBoxStyle);
    ui->sbSigma->setStyleSheet(QSpinBoxStyle);

    ui->cbColorMap->setStyleSheet(QComboBoxStyle);
    ui->cbDisparityType->setStyleSheet(QComboBoxStyle);
    ui->lbMinDisparity->setStyleSheet(QLabelTextStyle);
    ui->lbNumDisparity->setStyleSheet(QLabelTextStyle);
    ui->lbBlockSize->setStyleSheet(QLabelTextStyle);
    ui->lbLambda->setStyleSheet(QLabelTextStyle);
    ui->lbSigma->setStyleSheet(QLabelTextStyle);
    ui->lbColor->setStyleSheet(QLabelTextStyle);
    ui->lbDisparityType->setStyleSheet(QLabelTextStyle);

}

int DisparityWindow::disparityMap(cv::Mat &map,
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
                                  bool showImages)
{
    ///////////////////////////////////////////////////////////////////////
    // Преобразование в GrayScale
    cv::Mat imageLeftGS;
    cv::Mat imageRightGS;
    cv::Mat imageDisparityLeft;
    cv::Mat imageDisparityRight;

    cv::cvtColor(imageLeft, imageLeftGS, cv::COLOR_BGR2GRAY);
    cv::cvtColor(imageRight, imageRightGS, cv::COLOR_BGR2GRAY);

    ///////////////////////////////////////////////////////////////////////
    // Построение карты диспаратности (базовый метод)
    cv::Ptr<cv::StereoBM> leftMatcher = cv::StereoBM::create(numDisparities, blockSize);
    cv::Ptr<cv::StereoMatcher> rightMatcher = cv::ximgproc::createRightMatcher(leftMatcher);

    leftMatcher->compute(imageLeftGS, imageRightGS, imageDisparityLeft);
    rightMatcher->compute(imageRightGS, imageLeftGS, imageDisparityRight);

    cv::Mat disparityLeft;
    cv::Mat disparityRight;

    imageDisparityLeft.convertTo(disparityLeft, CV_32F, 1.0);
    imageDisparityRight.convertTo(disparityRight, CV_32F, 1.0);

    disparityLeft = (disparityLeft / 16.0f - (float)minDisparity) / ((float)numDisparities);
    disparityRight = (disparityRight / 16.0f - (float)minDisparity) / ((float)numDisparities);

    if (showImages)
    {
        cv::namedWindow("Image Left", cv::WINDOW_AUTOSIZE);
        cv::imshow("Image Left", imageLeft);

        cv::namedWindow("Image Right", cv::WINDOW_AUTOSIZE);
        cv::imshow("Image Right", imageRight);

        cv::namedWindow("Disparity", cv::WINDOW_AUTOSIZE);
        cv::imshow("Disparity", disparityLeft);
    }

    ///////////////////////////////////////////////////////////////////////
    // Расцвечивание карты диспаратности
    cv::Mat disparityTMP;
    disparityLeft.convertTo(disparityTMP, CV_8UC3, 255.0);

    // Запоминаем результат преобразования
    cv::Mat disparity = disparityTMP.clone();

    cv::Mat heatmap;

    cv::applyColorMap(disparityTMP, heatmap, colorMap);

    if (showImages)
    {
        cv::namedWindow("Heatmap", cv::WINDOW_AUTOSIZE);
        cv::imshow("Heatmap", heatmap);
    }

    ///////////////////////////////////////////////////////////////////////
    // Построение карты диспаратности (фильтрация)
    cv::Ptr<cv::ximgproc::DisparityWLSFilter> prtWLSFilter = cv::ximgproc::createDisparityWLSFilter(leftMatcher);

    prtWLSFilter->setLambda(lambda);
    prtWLSFilter->setSigmaColor(sigma);

    cv::Mat filteredDisparityT;

    // Создание фильтра сглаживания
    prtWLSFilter->filter(imageDisparityLeft, imageLeft, filteredDisparityT, imageDisparityRight);

    cv::Mat filteredDisparity;
    cv::ximgproc::getDisparityVis(filteredDisparityT, filteredDisparity, 1.0);

    if (showImages)
    {
        cv::namedWindow("Disparity Filtered", cv::WINDOW_AUTOSIZE);
        cv::imshow("Disparity Filtered", filteredDisparity);
    }

    cv::Mat filteredHeatmap;
    cv::applyColorMap(filteredDisparity, filteredHeatmap, colorMap);

    if (showImages)
    {
        cv::namedWindow("Heatmap Filtered", cv::WINDOW_AUTOSIZE);
        cv::imshow("Heatmap Filtered", filteredHeatmap);
    }

    if (saveToFile)
    {

        auto currentPath = std::filesystem::current_path();
        auto disparityPath = currentPath / "disparity";

        if (std::filesystem::is_directory(disparityPath))
        {
            std::filesystem::remove_all(disparityPath);
        }
        std::filesystem::create_directory(disparityPath);

        std::filesystem::path fileDisparity("disparity\\disparity-basic.jpg");
        std::filesystem::path fileHeatmap("disparity\\heatmap-basic.jpg");
        std::filesystem::path fileDisparityFiltered("disparity\\disparity-filtered.jpg");
        std::filesystem::path fileHeatmapFiltered("disparity\\heatmap-filtered.jpg");

        auto pathDisparity = currentPath / fileDisparity;
        auto pathHeatmap = currentPath / fileHeatmap;
        auto pathDisparityFiltered = currentPath / fileDisparityFiltered;
        auto pathHeatmapFiltered = currentPath / fileHeatmapFiltered;

        cv::imwrite(pathDisparity.u8string(), disparity);
        cv::imwrite(pathHeatmap.u8string(), heatmap);
        cv::imwrite(pathDisparityFiltered.u8string(), filteredDisparity);
        cv::imwrite(pathHeatmapFiltered.u8string(), filteredHeatmap);
    }

    switch (disparityType)
    {
    case DISPARITY_TYPE::BASIC_DISPARITY:
        disparity.copyTo(map);
        break;
    case DISPARITY_TYPE::BASIC_HEATMAP:
        heatmap.copyTo(map);
        break;
    case DISPARITY_TYPE::FILTERED_DISPARITY:
        filteredDisparity.copyTo(map);
        break;
    case DISPARITY_TYPE::FILTERED_HEATMAP:
        filteredHeatmap.copyTo(map);
        break;
    case DISPARITY_TYPE::ALL:
        filteredHeatmap.copyTo(map);
        break;
    default:
        filteredHeatmap.copyTo(map);
        break;
    }

    return EXIT_SUCCESS;
}

void DisparityWindow::onStereoCaptured(const cv::Mat &frameL, const cv::Mat &frameR)
{
    cv::Mat disparity;

    int minDisparity = 32;
    int numDisparities = 16 * 10;
    int blockSize = 15;
    double lambda = 5000.0;
    double sigma = 3;
    int colorMap = cv::COLORMAP_TURBO;
    DISPARITY_TYPE disparityType = DISPARITY_TYPE::FILTERED_HEATMAP;

    minDisparity = ui->sbMinDisparity->value();
    numDisparities = ui->sbNumDisparities->value();
    blockSize = ui->sbBlockSIze->value();
    lambda = ui->sbLambda->value();
    sigma = ui->sbSigma->value();

    switch (ui->cbColorMap->currentIndex()) {
    case 0:
        colorMap = cv::COLORMAP_JET;
        break;
    case 1:
        colorMap = cv::COLORMAP_VIRIDIS;
        break;
    case 2:
        colorMap = cv::COLORMAP_TURBO;
        break;
    case 3:
        colorMap = cv::COLORMAP_HOT;
        break;
    default:
        colorMap = cv::COLORMAP_TURBO;
        break;
    }

    switch (ui->cbDisparityType->currentIndex()) {
    case 0:
        disparityType = DISPARITY_TYPE::BASIC_DISPARITY;
        break;
    case 1:
        disparityType = DISPARITY_TYPE::BASIC_HEATMAP;
        break;
    case 2:
        disparityType = DISPARITY_TYPE::FILTERED_DISPARITY;
        break;
    case 3:
        disparityType = DISPARITY_TYPE::FILTERED_HEATMAP;
        break;
    default:
        disparityType = DISPARITY_TYPE::FILTERED_HEATMAP;
        break;
    }


    if (disparityMap(disparity,
                     frameL,
                     frameR,
                     minDisparity,
                     numDisparities,
                     blockSize,
                     lambda,
                     sigma,
                     disparityType,
                     colorMap,
                     false,
                     false) == EXIT_SUCCESS)
    {
        cv::Mat resized;
        cv::Mat destination;
        QImage image;

        cv::resize(disparity, resized, cv::Size(640, 480));
        cv::cvtColor(resized, destination, cv::COLOR_BGR2RGB);

        image = QImage((uchar*) destination.data,
                       destination.cols,
                       destination.rows,
                       destination.step,
                       QImage::Format_RGB888);

        ui->lbDisparity->setPixmap(QPixmap::fromImage(image));
    }
}
