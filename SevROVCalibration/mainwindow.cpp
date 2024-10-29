#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Сигналы для кнопок
    connect(ui->pbStartStop, &QPushButton::clicked, this, &MainWindow::onStartStopButtonClicked);
    connect(ui->pbCalibration, &QPushButton::clicked, this, &MainWindow::onCalibrationButtonClicked);
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
    ui->pbStartStop->setGeometry(
        _appSet.CAMERA_VIEW_X0,
        mainWindowRect.height() - 50 - _appSet.CAMERA_VIEW_BORDER_WIDTH,
        100,
        50);

    ui->pbCalibration->setGeometry(
        ui->pbStartStop->x() + ui->pbStartStop->width() + _appSet.CAMERA_VIEW_BORDER_WIDTH,
        mainWindowRect.height() - 50 - _appSet.CAMERA_VIEW_BORDER_WIDTH,
        150,
        50);

    ui->pbSettings->setGeometry(
        ui->pbCalibration->x() + ui->pbCalibration->width() + _appSet.CAMERA_VIEW_BORDER_WIDTH,
        mainWindowRect.height() - 50 - _appSet.CAMERA_VIEW_BORDER_WIDTH,
        150,
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
}

void MainWindow::onStartStopButtonClicked()
{
    if (_processStatus == ProcessStatus::OFF)
    {
        _processStatus = ProcessStatus::ON;
        setupCameraConnection(_processStatus);

        // Обновляем надписи
        ui->pbStartStop->setText("STOP");
    }
    else
    {
        _processStatus = ProcessStatus::OFF;
        setupCameraConnection(_processStatus);

        ui->pbStartStop->setText("START");
        ui->lbInfo->setText("КАЛИБРОВКА");
    }
}

void MainWindow::onCalibrationButtonClicked()
{
    ;
}

void MainWindow::onSettingsButtonClicked()
{
    ;
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
}

void MainWindow:: setupCameraConnection(ProcessStatus connection)
{
    switch (connection)
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
