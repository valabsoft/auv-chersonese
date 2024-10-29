#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Сигналы для кнопок
    connect(ui->pbVideoCapture, &QPushButton::clicked, this, &MainWindow::onVideoCaptureButtonClicked);
    connect(ui->pbCalibration, &QPushButton::clicked, this, &MainWindow::onCalibrationButtonClicked);
    connect(ui->pbCalc, &QPushButton::clicked, this, &MainWindow::onCalcButtonClicked);
    connect(ui->pbSettings, &QPushButton::clicked, this, &MainWindow::onSettingsButtonClicked);
    connect(ui->pbClose, &QPushButton::clicked, this, &MainWindow::onCloseButtonClicked);

    // Заголовок окна
    setWindowTitle("ТНПА :: Калибровка :: " + _appSet.getAppVersion());

    // Устанавливаем геометрию окна и основных элементов
    setupWindowGeometry();
    setupControlsStyle();

    // Таймер
    _timer = new QTimer(this);
    connect(_timer, &QTimer::timeout, this, &MainWindow::onTimer);

    _appSet.load();
}

MainWindow::~MainWindow()
{
    // Освобождение ресурсов видео таймер
    if (!_timer->isActive())
        _timer->stop();

    if (_timer)
        delete _timer;

    // Веб-камеры
    if (_webCamL->isOpened())
        _webCamL->release();

    if (_webCamR->isOpened())
        _webCamR->release();

    if (_webCamL)
        delete _webCamL;

    if (_webCamR)
        delete _webCamR;

    delete ui;
}

void MainWindow::moveWindowToCenter()
{
    auto primaryScreen = QGuiApplication::primaryScreen(); // Главный экран
    QRect primaryScreenRect = primaryScreen->availableGeometry(); // Размер главного экрана
    QPoint primaryScreenRectCenter = primaryScreenRect.center();
    primaryScreenRectCenter.setX(primaryScreenRectCenter.x() - (this->width()/2));
    primaryScreenRectCenter.setY(primaryScreenRectCenter.y() - (this->height()/2));
    move(primaryScreenRectCenter);
}

void MainWindow::setupWindowGeometry()
{
    // Установка размера главного окна// Установка размера главного окна
    int windowWidth = _appSet.CAMERA_WIDTH + _appSet.CAMERA_VIEW_BORDER_WIDTH * 3;
    int windowHeight = _appSet.CAMERA_HEIGHT + _appSet.CAMERA_VIEW_BORDER_WIDTH * 3;

    // Фиксируем размер окна и убираем иконку ресайза
    setFixedSize(QSize(windowWidth, windowHeight));

    // Центрируем окно в пределах экрана
    moveWindowToCenter();

    QRect mainWindowRect = this->geometry();


    // Позиционируем кнопки
    ui->pbVideoCapture->setGeometry(
        _appSet.CAMERA_VIEW_X0,
        mainWindowRect.height() - 50 - _appSet.CAMERA_VIEW_BORDER_WIDTH,
        170,
        50);

    ui->pbCalibration->setGeometry(
        ui->pbVideoCapture->x() + ui->pbVideoCapture->width() + _appSet.CAMERA_VIEW_BORDER_WIDTH,
        mainWindowRect.height() - 50 - _appSet.CAMERA_VIEW_BORDER_WIDTH,
        150,
        50);

    ui->pbCalc->setGeometry(
        ui->pbCalibration->x() + ui->pbCalibration->width() + _appSet.CAMERA_VIEW_BORDER_WIDTH,
        mainWindowRect.height() - 50 - _appSet.CAMERA_VIEW_BORDER_WIDTH,
        80,
        50);

    ui->pbSettings->setGeometry(
        ui->pbCalc->x() + ui->pbCalc->width() + _appSet.CAMERA_VIEW_BORDER_WIDTH,
        mainWindowRect.height() - 50 - _appSet.CAMERA_VIEW_BORDER_WIDTH,
        100,
        50);

    ui->pbClose->setGeometry(
        mainWindowRect.width() - 100 - _appSet.CAMERA_VIEW_BORDER_WIDTH,
        mainWindowRect.height() - 50 - _appSet.CAMERA_VIEW_BORDER_WIDTH,
        100,
        50);

    // Позиционируем информационную панель
    ui->lbInfo->setGeometry(
        _appSet.CAMERA_VIEW_BORDER_WIDTH,
        _appSet.CAMERA_VIEW_BORDER_WIDTH,
        mainWindowRect.width() - _appSet.CAMERA_VIEW_BORDER_WIDTH * 2,
        100);

    ui->lbCounter->setGeometry(
        _appSet.CAMERA_VIEW_BORDER_WIDTH,
        ui->pbVideoCapture->y() - _appSet.CAMERA_VIEW_BORDER_WIDTH - 70,
        mainWindowRect.width() - _appSet.CAMERA_VIEW_BORDER_WIDTH * 2,
        70);

    // Геометрия окон камер
    ui->lbCameraL->setGeometry(
        _appSet.CAMERA_VIEW_X0,
        // (mainWindowRect.height() - _appSet.CAMERA_HEIGHT / 2 - 50 - _appSet.CAMERA_VIEW_BORDER_WIDTH) / 2,
        ui->lbInfo->y() + ui->lbInfo->height() + _appSet.CAMERA_VIEW_BORDER_WIDTH,
        _appSet.CAMERA_WIDTH / 2,
        _appSet.CAMERA_HEIGHT / 2);

    ui->lbCameraR->setGeometry(
        _appSet.CAMERA_VIEW_X0 + _appSet.CAMERA_WIDTH / 2 + _appSet.CAMERA_VIEW_BORDER_WIDTH,
        // (mainWindowRect.height() - _appSet.CAMERA_HEIGHT / 2 - 50 - _appSet.CAMERA_VIEW_BORDER_WIDTH) / 2,
        ui->lbInfo->y() + ui->lbInfo->height() + _appSet.CAMERA_VIEW_BORDER_WIDTH,
        _appSet.CAMERA_WIDTH / 2,
        _appSet.CAMERA_HEIGHT / 2);
}

void MainWindow::setupControlsStyle()
{
    ui->lbCameraL->setStyleSheet("QLabel {"
                                 "border-style: solid;"
                                 "border-width: 1px;"
                                 "border-color: #1A5276;"
                                 "}");
    ui->lbCameraR->setStyleSheet("QLabel {"
                                 "border-style: solid;"
                                 "border-width: 1px;"
                                 "border-color: #1A5276;"
                                 "}");

    ui->lbInfo->setStyleSheet("QLabel {"
                              "border-style: solid;"
                              "border-width: 1px;"
                              "border-color: #1A5276;"
                              "}");

    ui->lbCounter->setStyleSheet("QLabel {"
                              "border-style: solid;"
                              "border-width: 1px;"
                              "border-color: #1A5276;"
                              "}");
}

void MainWindow::onVideoCaptureButtonClicked()
{
    if (_videoCaptureStatus == ProcessStatus::OFF)
    {
        _videoCaptureStatus = ProcessStatus::ON;
        setupCameraConnection(_videoCaptureStatus);

        // Обновляем надписи
        ui->pbVideoCapture->setText("VIDEO CAPTURE ON");
        ui->lbInfo->setText("ПОДГОТОВКА");
    }
    else
    {
        // Если процесс калибровки запущен - отключаем его
        if (_calibrationStatus == ProcessStatus::ON)
        {
            ui->pbCalibration->click();
        }

        _videoCaptureStatus = ProcessStatus::OFF;
        setupCameraConnection(_videoCaptureStatus);

        ui->pbVideoCapture->setText("VIDEO CAPTURE OFF");
        ui->lbInfo->setText("");
    }
}

void MainWindow::onCalibrationButtonClicked()
{
    if (_calibrationStatus == ProcessStatus::OFF)
    {
        folderPreparation();

        _calibrationStatus = ProcessStatus::ON;

        // Обновляем надписи
        ui->pbCalibration->setText("CALIBRATION ON");
    }
    else
    {
        _calibrationStatus = ProcessStatus::OFF;

        ui->pbCalibration->setText("CALIBRATION OFF");
    }

    // Сброс параметров калибровки
    calibrationShotCounter = 0;
    calibrationPauseCounter = 0;
    calibrationTimer = clock();

    ui->lbCounter->setText(QString::number(calibrationShotCounter) + " / " + QString::number(_appSet.NUMBER_OF_SHOTS));
}

void MainWindow::onCalcButtonClicked()
{
    try
    {
        ui->lbInfo->setText("ИДЕТ РАСЧЕТ ПАРАМЕТРОВ");

        CalibrationConfig config;
        std::filesystem::path configFile("config.dat");

        auto currentPath = std::filesystem::current_path();
        auto configPath = currentPath / configFile;

        // Чтение конфигуарции процедуры калибровки
        readCalibrartionConfigFile(configPath.u8string(), config);

        CalibrationParametersMono monoParL;
        CalibrationParametersMono monoParR;
        CalibrationParametersStereo stereoPar;

        std::vector<cv::String> imagesLeft;
        std::vector<cv::String> imagesRight;

        std::filesystem::path calibrationFile("calibration.xml");
        auto calibrationPath = currentPath / calibrationFile;
        std::string folderName = "calibration-images";

        std::filesystem::path leftFramePath = currentPath / folderName / "L";
        std::filesystem::path rightFramePath = currentPath / folderName / "R";

        cameraCalibrationStereo(imagesLeft, imagesRight, leftFramePath.u8string() + "/", rightFramePath.u8string() + "/", stereoPar, config.keypoints_c, config.keypoints_r, config.square_size);
        writeCalibrationParametersStereo(calibrationPath.u8string(), stereoPar);

        ui->lbInfo->setText("РАСЧЕТ ЗАКОНЧЕН");
    }
    catch (...)
    {
        ui->lbInfo->setText("!!! ОШИБКА КАЛИБРОВКИ !!!");
    }
}

void MainWindow::onSettingsButtonClicked()
{
    // Если вызвать конструктор SettingsWindow(this),
    // то копируются стили главного окна, поэтому вызываем с NULL
    _settingsWindow = new SettingsWindow(NULL);

    // Центрировать инструментальную панель
    QRect screenGeometry = QGuiApplication::screens()[0]->geometry();
    int x = (screenGeometry.width() - _settingsWindow->width()) / 2;
    int y = (screenGeometry.height() - _settingsWindow->height()) / 2;

    _settingsWindow->setWindowTitle("ТНПА :: Настройки :: " + _appSet.getAppVersion());

    _settingsWindow->move(x, y);

    if (_settingsWindow->exec() == QDialog::Accepted)
    {
        _appSet.load();
    }

    delete _settingsWindow;
}

void MainWindow::onCloseButtonClicked()
{
    close();
}

void MainWindow::onTimer()
{
    cv::Mat resizedMatL;
    cv::Mat resizedMatR;

    // int nRet = MV_OK;

    switch (_appSet.CAMERA_TYPE)
    {
    case CameraType::IP:
        ///////////////////////////////////////////////////////////////////
        // Left Camera
        ///////////////////////////////////////////////////////////////////
        //nRet = MV_CC_GetImageBuffer(handleL, &stOutFrame, 1000);
        //if (nRet == MV_OK)
        //{
        //    _sourceMatL = cv::Mat(stOutFrame.stFrameInfo.nHeight, stOutFrame.stFrameInfo.nWidth, CV_8U, stOutFrame.pBufAddr); // TODO: Почему H x W а не W x H ?
        //    cv::cvtColor(_sourceMatL, _sourceMatL, cv::COLOR_BayerRG2RGB);
        //
        //    nRet = MV_CC_FreeImageBuffer(handleL, &stOutFrame);
        //
        //    if(nRet != MV_OK)
        //        qDebug() << "ERROR: Left Camera - Free Image Buffer fail!";
        //}
        //else
        //    qDebug() << "ERROR: Left Camera - Get Image fail!";
        ///////////////////////////////////////////////////////////////////
        break;
    case CameraType::WEB:
        _webCamL->read(_sourceMatL);
        break;
    default:
        break;
    }

    if (_sourceMatL.empty())
        return;

    cv::resize(_sourceMatL, resizedMatL, cv::Size(_appSet.CAMERA_WIDTH / 2, _appSet.CAMERA_HEIGHT / 2));
    cv::cvtColor(resizedMatL, _destinationMatL, cv::COLOR_BGR2RGB);

    _imgCamL = QImage((uchar*) _destinationMatL.data,
                      _destinationMatL.cols,
                      _destinationMatL.rows,
                      _destinationMatL.step,
                      QImage::Format_RGB888);

    ui->lbCameraL->setPixmap(QPixmap::fromImage(_imgCamL));

    switch (_appSet.CAMERA_TYPE)
    {
    case CameraType::IP:
        //nRet = MV_CC_GetImageBuffer(handleR, &stOutFrame, 1000);
        //if (nRet == MV_OK)
        //{
        //    _sourceMatR = cv::Mat(stOutFrame.stFrameInfo.nHeight, stOutFrame.stFrameInfo.nWidth, CV_8U, stOutFrame.pBufAddr); // TODO: Почему H x W а не W x H ?
        //    cv::cvtColor(_sourceMatR, _sourceMatR, cv::COLOR_BayerRG2RGB);
        //
        //    nRet = MV_CC_FreeImageBuffer(handleR, &stOutFrame);
        //
        //    if(nRet != MV_OK)
        //        qDebug() << "ERROR: Right Camera - Free Image Buffer fail!";
        //}
        //else
        //    qDebug() << "ERROR: Right Camera - Get Image fail!";
        ///////////////////////////////////////////////////////////////////
        break;
    case CameraType::WEB:
        _webCamR->read(_sourceMatR);
        break;
    default:
        break;
    }

    if (_sourceMatR.empty())
        return;

    cv::resize(_sourceMatR, resizedMatR, cv::Size(_appSet.CAMERA_WIDTH / 2, _appSet.CAMERA_HEIGHT / 2));
    cv::cvtColor(resizedMatR, _destinationMatR, cv::COLOR_BGR2RGB);

    _imgCamR = QImage((uchar*) _destinationMatR.data,
                      _destinationMatR.cols,
                      _destinationMatR.rows,
                      _destinationMatR.step,
                      QImage::Format_RGB888);

    ui->lbCameraR->setPixmap(QPixmap::fromImage(_imgCamR));

    if (_calibrationStatus == ProcessStatus::ON)
    {
        // Основной цикл грабинга
        {
            if (clock() - calibrationTimer < _appSet.SHOTS_INTERVAL * CLOCKS_PER_SEC)
            {
                int diff = _appSet.SHOTS_INTERVAL - (clock() - calibrationTimer) / CLOCKS_PER_SEC;
                ui->lbInfo->setText(QString::number(std::round(diff)));
            }
            else
            {
                calibrationTimer = clock();
                calibrationShotCounter++;
                ui->lbCounter->setText(QString::number(calibrationShotCounter) + " / " + QString::number(_appSet.NUMBER_OF_SHOTS));

                std::string fullPathToLeftFrameImage;
                std::string fullPathToRightFrameImage;
                std::string folderName = "calibration-images";
                auto currentPath = std::filesystem::current_path();
                std::filesystem::path leftFramePath = currentPath / folderName / "L";
                std::filesystem::path rightFramePath = currentPath / folderName / "R";


                fullPathToLeftFrameImage = (leftFramePath / (std::to_string(calibrationShotCounter) + ".png")).u8string();
                fullPathToRightFrameImage = (rightFramePath / (std::to_string(calibrationShotCounter) + ".png")).u8string();

                cv::imwrite(fullPathToLeftFrameImage, _sourceMatL);
                cv::imwrite(fullPathToRightFrameImage, _sourceMatR);
            }
        }

        if (calibrationShotCounter == _appSet.NUMBER_OF_SHOTS)
        {
            ui->lbInfo->setText("ВЫПОЛНЕНО");
            ui->pbCalibration->click();
        }
    }
}

void MainWindow::setupCameraConnection(ProcessStatus status)
{
    switch (status)
    {
    case ProcessStatus::ON:

        if (_appSet.CAMERA_TYPE == CameraType::IP)
        {
            //int retCode = MV_SDK_Initialization();
            //switch (retCode)
            //{
            //case -1:
            //    qDebug() <<  "ERROR: The only one camera found!";
            //    break;

            //case 1:
            //    qDebug() <<  "ERROR: Initialize SDK fail!";
            //    break;
            //case 2:
            //    qDebug() <<  "ERROR: Enum Devices fail!";
            //    break;

            //case 11:
            //    qDebug() <<  "ERROR: Left Camera - Create Handle fail!";
            //    break;
            //case 12:
            //    qDebug() <<  "ERROR: Left Camera - Open Device fail!";
            //    break;
            //case 13:
            //    qDebug() <<  "ERROR: Left Camera - Get PixelFormat's value fail!";
            //    break;
            //case 14:
            //    qDebug() <<  "ERROR: Left Camera - Get PixelFormat's symbol fail!";
            //    break;
            //case 15:
            //    qDebug() << "ERROR: Left Camera - Start Grabbing fail!";
            //    break;

            //case 21:
            //    qDebug() <<  "ERROR: Right Camera - Create Handle fail!";
            //    break;
            //case 22:
            //    qDebug() <<  "ERROR: Right Camera - Open Device fail!";
            //    break;
            //case 23:
            //    qDebug() <<  "ERROR: Right Camera - Get PixelFormat's value fail!";
            //    break;
            //case 24:
            //    qDebug() <<  "ERROR: Right Camera - Get PixelFormat's symbol fail!";
            //    break;
            //case 25:
            //    qDebug() << "ERROR: Right Camera - Start Grabbing fail!";
            //    break;
            //default:
            //    break;
            //}
        }
        else if (_appSet.CAMERA_TYPE == CameraType::WEB)
        {
            _webCamL = new cv::VideoCapture(_appSet.CAMERA_LEFT_ID);
            _webCamR = new cv::VideoCapture(_appSet.CAMERA_RIGHT_ID);
        }

        // Запускаем таймер
        if (!_timer->isActive())
            _timer->start(_appSet.VIDEO_TIMER_INTERVAL);

        break;
    case ProcessStatus::OFF:

        if (_appSet.CAMERA_TYPE == CameraType::IP)
        {
            //int retCode = MV_SDK_Finalization();
            //switch (retCode)
            //{
            //case 10:
            //    qDebug() <<  "ERROR: Left Camera - Stop Grabbing fail!";
            //    break;
            //case 11:
            //    qDebug() <<  "ERROR: Left Camera - CloseDevice fail!";
            //    break;
            //case 12:
            //    qDebug() <<  "ERROR: Left Camera - Destroy Handle fail!";
            //    break;

            //case 20:
            //    qDebug() <<  "ERROR: Right Camera - Stop Grabbing fail!";
            //    break;
            //case 21:
            //    qDebug() <<  "ERROR: Right Camera - CloseDevice fail!";
            //    break;
            //case 22:
            //    qDebug() <<  "ERROR: Right Camera - Destroy Handle fail!";
            //    break;
            //default:
            //    break;
            //}
        }
        else if (_appSet.CAMERA_TYPE == CameraType::WEB)
        {
            // Освобождаем ресурсы
            if (_webCamL->isOpened())
                _webCamL->release();

            if (_webCamR->isOpened())
                _webCamR->release();
        }

        // Остановка таймера
        if (!_timer->isActive())
            _timer->stop();

        // Стереть старое изображение
        QPixmap pixmap;
        QColor color;

        color = QColor(0, 0, 0, 255);
        pixmap = QPixmap(ui->lbCameraL->size());
        pixmap.fill(color);
        ui->lbCameraL->setPixmap(pixmap);
        ui->lbCameraR->setPixmap(pixmap);

        // Останавливаем таймер
        if (_timer->isActive())
            _timer->stop();

        break;
    }
}

void MainWindow::folderPreparation()
{
    std::string folderName = "calibration-images";
    auto currentPath = std::filesystem::current_path();
    std::filesystem::path leftFramePath = currentPath / folderName / "L";
    std::filesystem::path rightFramePath = currentPath / folderName / "R";

    if (!std::filesystem::is_directory(currentPath / folderName))
    {
        std::filesystem::create_directory(currentPath / folderName);

        if (std::filesystem::is_directory(leftFramePath))
        {
            std::filesystem::remove_all(leftFramePath);
        }
        std::filesystem::create_directory(leftFramePath);

        if (std::filesystem::is_directory(rightFramePath))
        {
            std::filesystem::remove_all(rightFramePath);
        }
        std::filesystem::create_directory(rightFramePath);
    }
    else
    {
        if (std::filesystem::is_directory(leftFramePath))
        {
            std::filesystem::remove_all(leftFramePath);
        }
        std::filesystem::create_directory(leftFramePath);

        if (std::filesystem::is_directory(rightFramePath))
        {
            std::filesystem::remove_all(rightFramePath);
        }
        std::filesystem::create_directory(rightFramePath);
    }
}
