#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QScreen>
#include <QDir>

#include <fstream>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Сигналы для кнопок
    connect(ui->pbStartStop, &QPushButton::clicked, this, &MainWindow::onStartStopButtonClicked);
    connect(ui->pbView, &QPushButton::clicked, this, &MainWindow::onViewButtonClicked);
    connect(ui->pbScreenshot, &QPushButton::clicked, this, &MainWindow::onScreenshotButtonClicked);
    connect(ui->pbSettings, &QPushButton::clicked, this, &MainWindow::onSettingsButtonClicked);

    // Загрузка настроек
    _appSet.load(_ctrSet);

    // TODO VA (23-05-2024): После подключения общей библиотеки, использовать
    // setWindowTitle("ТНПА :: Контроль :: " + QString(APP_VERSION.c_str()));
    setWindowTitle("ТНПА :: AРМ Оператора :: " + _appSet.getAppVersion());

    // Устанавливаем геометрию окна и основных элементов
    setupWindowGeometry();

    // Установка иконок
    setupIcons();

    // Layout по умолчанию - одиночная камера
    setupCameraViewLayout(CameraView::MONO);
    setupConnectedControlsStyle(false);

    // Цвет фона главного окна приложения
    this->setStyleSheet("background-color: black;");

    QFont fontLabel("GOST type A", 8, QFont::Bold);
    ui->lbFPS->setStyleSheet("background-color : black; color : silver;");
    ui->lbFPS->setFont(fontLabel);

    _videoTimer = new QTimer(this);
    connect(_videoTimer, &QTimer::timeout, this, &MainWindow::onVideoTimer);

    _controlTimer = new QTimer(this);
    connect(_controlTimer, &QTimer::timeout, this, &MainWindow::onControlTimer);

    QObject::connect(this, SIGNAL(updateCntValue(QString)), ui->lbFPS, SLOT(setText(QString)));

    // Работа с джойстиком
    _jsController = new SevROVXboxController();

    // Кнопки
    connect(_jsController, &SevROVXboxController::OnButtonA, this, &MainWindow::OnButtonA);
    connect(_jsController, &SevROVXboxController::OnButtonB, this, &MainWindow::OnButtonB);
    connect(_jsController, &SevROVXboxController::OnButtonX, this, &MainWindow::OnButtonX);
    connect(_jsController, &SevROVXboxController::OnButtonY, this, &MainWindow::OnButtonY);
    connect(_jsController, &SevROVXboxController::OnButtonLBumper, this, &MainWindow::OnButtonLBumper);
    connect(_jsController, &SevROVXboxController::OnButtonRBumper, this, &MainWindow::OnButtonRBumper);
    connect(_jsController, &SevROVXboxController::OnButtonView, this, &MainWindow::OnButtonView);
    connect(_jsController, &SevROVXboxController::OnButtonMenu, this, &MainWindow::OnButtonMenu);
    connect(_jsController, &SevROVXboxController::OnDPad, this, &MainWindow::OnDPad);

    // Оси
    connect(_jsController, &SevROVXboxController::OnAxisLStickX, this, &MainWindow::OnAxisLStickX);
    connect(_jsController, &SevROVXboxController::OnAxisLStickY, this, &MainWindow::OnAxisLStickY);
    connect(_jsController, &SevROVXboxController::OnAxisRStickX, this, &MainWindow::OnAxisRStickX);
    connect(_jsController, &SevROVXboxController::OnAxisRStickY, this, &MainWindow::OnAxisRStickY);
    connect(_jsController, &SevROVXboxController::OnAxisLTrigger, this, &MainWindow::OnAxisLTrigger);
    connect(_jsController, &SevROVXboxController::OnAxisRTrigger, this, &MainWindow::OnAxisRTrigger);

    // Зеркалим данные
    _xbox.A = 0;
    _xbox.B = 0;
    _xbox.X = 0;
    _xbox.Y = 0;
    _xbox.LBumper = 0;
    _xbox.RBumper = 0;
    _xbox.View = 0;
    _xbox.Menu = 0;
    _xbox.DPad = 0;
    _xbox.LStickX = 0;
    _xbox.LStickY = 0;
    _xbox.RStickX = 0;
    _xbox.RStickY = 0;
    _xbox.LTrigger = -32768;
    _xbox.RTrigger = -32768;

    // Создаем коннектор с AUV. Клиент должен уметь писать управление и читать телеметрию
    _rovConnector.setMode(SevROVConnector::Mode::CONTROL_WRITE |
                         SevROVConnector::Mode::TELEMETRY_READ);


    connect(&_rovConnector, SIGNAL(OnConnected()), this, SLOT(onSocketConnect()));
    connect(&_rovConnector, SIGNAL(OnDisconnected()), this, SLOT(onSocketDisconnect()));
    connect(&_rovConnector, SIGNAL(OnProcessTelemetryDatagram()), this, SLOT(onSocketProcessTelemetryDatagram()));
}

MainWindow::~MainWindow()
{
    // Освобождение ресурсов видео таймер
    if (!_videoTimer->isActive())
        _videoTimer->stop();

    if (_videoTimer)
        delete _videoTimer;

    // Освобождение ресурсов таймер управления
    if (!_controlTimer->isActive())
        _controlTimer->stop();

    if (_controlTimer)
        delete _controlTimer;

    // Веб-камеры

    if (_webCamO->isOpened())
        _webCamO->release();
    if (_webCamL->isOpened())
        _webCamL->release();
    if (_webCamR->isOpened())
        _webCamR->release();

    if (_webCamO)
        delete _webCamO;

    if (_webCamL)
        delete _webCamL;

    if (_webCamR)
        delete _webCamR;

    if (_toolWindow)
        delete _toolWindow;

    delete _jsController;

    delete ui;
}

void MainWindow::setupIcons()
{
    // Иконка главного окна
    setWindowIcon(QIcon(":/img/sevrov.png"));

    // Установка иконок для кнопок
    ui->pbSettings->setIcon(QIcon(":/img/settings_icon.png"));
    ui->pbSettings->setIconSize(QSize(64, 64));
    ui->pbStartStop->setIcon(QIcon(":/img/off_button_icon.png"));
    ui->pbStartStop->setIconSize(QSize(64, 64));
    ui->pbView->setIcon(QIcon(":/img/display_icon.png"));
    ui->pbView->setIconSize(QSize(64, 64));
    ui->pbScreenshot->setIcon(QIcon(":/img/camera_icon.png"));
    ui->pbScreenshot->setIconSize(QSize(64, 64));
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
    int windowWidth = _appSet.CAMERA_WIDTH + _appSet.CONTROL_PANEL_WIDTH + _appSet.CAMERA_VIEW_BORDER_WIDTH * 4;
    int windowHeight = _appSet.CAMERA_HEIGHT + _appSet.CAMERA_VIEW_BORDER_WIDTH * 2;

    // Фиксируем размер окна и убираем иконку ресайза
    setFixedSize(QSize(windowWidth, windowHeight));

    // Центрируем окно в пределах экрана
    moveWindowToCenter();

    QRect mainWindowRect = this->geometry();

    // Геометрия окон камер
    ui->lbCamera->setGeometry(
        _appSet.CAMERA_VIEW_X0,
        _appSet.CAMERA_VIEW_Y0,
        _appSet.CAMERA_WIDTH,
        _appSet.CAMERA_HEIGHT);

    ui->lbCameraL->setGeometry(
        _appSet.CAMERA_VIEW_X0,
        (mainWindowRect.height() - _appSet.CAMERA_HEIGHT / 2) / 2,
        _appSet.CAMERA_WIDTH / 2,
        _appSet.CAMERA_HEIGHT / 2);

    ui->lbCameraR->setGeometry(
        _appSet.CAMERA_VIEW_X0 + _appSet.CAMERA_WIDTH / 2 + _appSet.CAMERA_VIEW_BORDER_WIDTH,
        (mainWindowRect.height() - _appSet.CAMERA_HEIGHT / 2) / 2,
        _appSet.CAMERA_WIDTH / 2,
        _appSet.CAMERA_HEIGHT / 2);

    // Позицианируем панель управления
    ui->gbControlButtons->setGeometry(
        mainWindowRect.width() - _appSet.CONTROL_PANEL_WIDTH - _appSet.CAMERA_VIEW_BORDER_WIDTH,
        _appSet.CAMERA_VIEW_Y0,
        _appSet.CONTROL_PANEL_WIDTH,
        mainWindowRect.height() - _appSet.CAMERA_VIEW_BORDER_WIDTH * 2);
}

void MainWindow::setupCameraViewLayout(CameraView layouttype)
{
    switch (layouttype)
    {
    case CameraView::MONO:
        ui->lbCamera->setVisible(true);
        ui->lbCameraL->setVisible(false);
        ui->lbCameraR->setVisible(false);
        break;
    case CameraView::STEREO:
        ui->lbCamera->setVisible(false);
        ui->lbCameraL->setVisible(true);
        ui->lbCameraR->setVisible(true);
        break;
    }
}

void MainWindow::setupConnectedControlsStyle(bool isconnected)
{
    if (isconnected)
    {
        // Обводка внешней границы окон камер
        ui->lbCamera->setStyleSheet("QLabel {"
                                    "border-style: solid;"
                                    "border-width: 1px;"
                                    "border-color: #F0BE50; "
                                    "}");
        ui->lbCameraL->setStyleSheet("QLabel {"
                                     "border-style: solid;"
                                     "border-width: 1px;"
                                     "border-color: #F0BE50; "
                                     "}");
        ui->lbCameraR->setStyleSheet("QLabel {"
                                     "border-style: solid;"
                                     "border-width: 1px;"
                                     "border-color: #F0BE50; "
                                     "}");

        // Обводка внешней границы блока управления
        ui->gbControlButtons->setStyleSheet("QGroupBox {"
                                            "border-style: solid;"
                                            "border-width: 1px;"
                                            "border-color: #F0BE50; "
                                            "}");
    }
    else
    {
        // Обводка внешней границы окон камер
        ui->lbCamera->setStyleSheet("QLabel {"
                                    "border-style: solid;"
                                    "border-width: 1px;"
                                    "border-color: silver; "
                                    "}");
        ui->lbCameraL->setStyleSheet("QLabel {"
                                     "border-style: solid;"
                                     "border-width: 1px;"
                                     "border-color: silver; "
                                     "}");
        ui->lbCameraR->setStyleSheet("QLabel {"
                                     "border-style: solid;"
                                     "border-width: 1px;"
                                     "border-color: silver; "
                                     "}");

        // Обводка внешней границы блока управления
        ui->gbControlButtons->setStyleSheet("QGroupBox {"
                                            "border-style: solid;"
                                            "border-width: 1px;"
                                            "border-color: silver; "
                                            "}");
    }
}

void MainWindow::setupCameraConnection(CameraConnection connection)
{
    switch (connection)
    {
    case CameraConnection::ON:

        // Выделяем ресурсы
        //switch (_sevROV.cameraView)
        //{
        //case CameraView::MONO:
        //    _webCam = new cv::VideoCapture(camID);
        //    break;
        //case CameraView::STEREO:
        //    _webCamL = new cv::VideoCapture(camIDL);
        //    _webCamR = new cv::VideoCapture(camIDR);
        //    break;
        //default:
        //    break;
        //}

        _webCamO = new cv::VideoCapture(_appSet.CAMERA_MONO_ID);
        _webCamL = new cv::VideoCapture(_appSet.CAMERA_LEFT_ID);
        _webCamR = new cv::VideoCapture(_appSet.CAMERA_RIGHT_ID);

        // TODO VA (23-05-2024): Оно работает вообще?
        _webCamO->set(cv::CAP_PROP_FPS, _appSet.CAMERA_FPS);
        _webCamL->set(cv::CAP_PROP_FPS, _appSet.CAMERA_FPS);
        _webCamR->set(cv::CAP_PROP_FPS, _appSet.CAMERA_FPS);

        // Запускаем таймер
        if (!_videoTimer->isActive())
            _videoTimer->start(_appSet.VIDEO_TIMER_INTERVAL);

        break;
    case CameraConnection::OFF:

        // Освобождаем ресурсы
        if (_webCamO->isOpened())
            _webCamO->release();
        if (_webCamL->isOpened())
            _webCamL->release();
        if (_webCamR->isOpened())
            _webCamR->release();

        // Остановка таймера
        if (!_videoTimer->isActive())
            _videoTimer->stop();

        // Стереть старое изображение
        QPixmap pixmap;
        QColor color;

        color = QColor(0, 0, 0, 255);
        pixmap = QPixmap(ui->lbCamera->size());
        pixmap.fill(color);
        ui->lbCamera->setPixmap(pixmap);

        pixmap = QPixmap(ui->lbCameraL->size());
        pixmap.fill(color);
        ui->lbCameraL->setPixmap(pixmap);
        ui->lbCameraR->setPixmap(pixmap);

        // Останавливаем таймер
        if (_videoTimer->isActive())
            _videoTimer->stop();

        break;
    }
}

// https://stackoverflow.com/questions/18973103/how-to-draw-a-rounded-rectangle-rectangle-with-rounded-corners-with-opencv
void MainWindow::roundedRectangle(
    cv::Mat& src,
    cv::Point topLeft,
    cv::Point bottomRight,
    const cv::Scalar lineColor,
    const int thickness,
    const int lineType,
    const int cornerRadius)
{
     // Сorners:
     // p1 - p2
     // |     |
     // p4 - p3

    cv::Point p1 = topLeft;
    cv::Point p2 = cv::Point(bottomRight.x, topLeft.y);
    cv::Point p3 = bottomRight;
    cv::Point p4 = cv::Point(topLeft.x, bottomRight.y);

    // Draw Straight Lines
    cv::line(src, cv::Point(p1.x + cornerRadius, p1.y), cv::Point(p2.x - cornerRadius, p2.y), lineColor, thickness, lineType);
    cv::line(src, cv::Point(p2.x, p2.y + cornerRadius), cv::Point(p3.x, p3.y - cornerRadius), lineColor, thickness, lineType);
    cv::line(src, cv::Point(p4.x + cornerRadius, p4.y), cv::Point(p3.x-cornerRadius, p3.y), lineColor, thickness, lineType);
    cv::line(src, cv::Point(p1.x, p1.y + cornerRadius), cv::Point(p4.x, p4.y - cornerRadius), lineColor, thickness, lineType);

    // Draw Arcs
    cv::ellipse(src, p1 + cv::Point(cornerRadius, cornerRadius), cv::Size(cornerRadius, cornerRadius), 180.0, 0, 90, lineColor, thickness, lineType);
    cv::ellipse(src, p2 + cv::Point(-cornerRadius, cornerRadius), cv::Size(cornerRadius, cornerRadius), 270.0, 0, 90, lineColor, thickness, lineType);
    cv::ellipse(src, p3 + cv::Point(-cornerRadius, -cornerRadius), cv::Size(cornerRadius, cornerRadius), 0.0, 0, 90, lineColor, thickness, lineType);
    cv::ellipse(src, p4 + cv::Point(cornerRadius, -cornerRadius), cv::Size(cornerRadius, cornerRadius), 90.0, 0, 90, lineColor, thickness, lineType);
}

void MainWindow::onVideoTimer()
{
    int X0 = _appSet.CAMERA_WIDTH / 2;
    int Y0 = _appSet.CAMERA_HEIGHT / 2;


    int GRID_SMALL_SIZE = 15;
    int GRID_BIG_SIZE = 30;

    int GRID_V_DELTA = _appSet.CAMERA_HEIGHT / 75;
    int GRID_V_MAX = 5;

    int GRID_H_DELTA = _appSet.CAMERA_WIDTH / 100;
    int GRID_H_MAX = 5;

    int XV0 = _appSet.CAMERA_WIDTH / 5;
    int YV0 = _appSet.CAMERA_HEIGHT / 2 - GRID_V_DELTA * 2 * 10;

    int XH0 = _appSet.CAMERA_WIDTH / 2 - GRID_H_DELTA * 2 * 10;
    int YH0 = _appSet.CAMERA_HEIGHT / 10;

    int SIGHT_SIZE = 50;
    int SIGHT_TICK = 10;
    int SIGHT_CROSS = 20;
    int SIGHT_DELTA = 5;

    int JS_DELTA = 20;
    int JSX0 = JS_DELTA;
    int JSY0 = JS_DELTA;
    int JSWIDTH = XV0 + 2 * GRID_SMALL_SIZE - JS_DELTA;
    int JSTEXTDELTA = 30;

    int TM_DELTA = 20;
    int TMX0 = JS_DELTA;
    int TMY0 = JS_DELTA;
    int TMWIDTH = XV0 + 2 * GRID_SMALL_SIZE - TM_DELTA;
    int TMTEXTDELTA = 30;

    cv::Mat overlayImage;
    cv::Mat transparencyiImage;
    double alpha = 0.5;

    cv::Mat resizedMatO;
    cv::Mat resizedMatL;
    cv::Mat resizedMatR;

    int nRet = MV_OK;
    void* handle = NULL;

    // VA (23-05-2024) Не работает...
    // double fps;
    // fps = _webCamO->get(cv::CAP_PROP_FPS);

    //Q_EMIT updateCntValue("CNT: " + QString::number(_cnt++));


    //******************************************************
    if (MV_OK != nRet)
    {
        printf("Initialize SDK fail! nRet [0x%x]\n", nRet);
    }

    // Enumerate devices.
    MV_CC_DEVICE_INFO_LIST stDeviceList;
    memset(&stDeviceList, 0, sizeof(MV_CC_DEVICE_INFO_LIST));
    nRet = MV_CC_EnumDevices(MV_GIGE_DEVICE | MV_USB_DEVICE, &stDeviceList);
    if (MV_OK != nRet)
    {
        printf("Enum Devices fail! nRet [0x%x]\n", nRet);
    }


    unsigned int nIndex = 0;

    MVCC_ENUMVALUE stEnumValue = {0};
    MVCC_ENUMENTRY stEnumEntry = {0};
    MV_FRAME_OUT stOutFrame = {0};
    unsigned char* pData = NULL;
    MV_CC_DEVICE_INFO* pDeviceInfo;


    //******************************************************

    switch (_sevROV.cameraView)
    {
    case CameraView::MONO:
/*
        if (stDeviceList.nDeviceNum > 0)
        {
            for (unsigned int i = 0; i < stDeviceList.nDeviceNum; i++)
            {
                //printf("[device %d]:\n", i);
                pDeviceInfo = stDeviceList.pDeviceInfo[i];
                if (NULL == pDeviceInfo)
                {
                    break;
                }

                if (pDeviceInfo[i].SpecialInfo.stGigEInfo.chUserDefinedName == "LCamera")
                {
                    nIndex = 0;
                    break;
                }
            }
        }
        else
        {
            printf("Find No Devices!\n");
        }
*/
        //_webCamO->read(_sourceMatO);  //тут должно записывать кадр!
        nRet = MV_CC_CreateHandle(&handle, stDeviceList.pDeviceInfo[nIndex]);
        if (MV_OK != nRet)
        {
            printf("Create Handle fail! nRet [0x%x]\n", nRet);
            break;
        }

        // Open device.
        nRet = MV_CC_OpenDevice(handle);
        if (MV_OK != nRet)
        {
            printf("Open Device fail! nRet [0x%x]\n", nRet);
            break;
        }

        // Detect network optimal package size (only works for GigE cameras).
        if (stDeviceList.pDeviceInfo[nIndex]->nTLayerType == MV_GIGE_DEVICE)
        {
            int nPacketSize = MV_CC_GetOptimalPacketSize(handle);
            if (nPacketSize > 0)
            {
                nRet = MV_CC_SetIntValue(handle,"GevSCPSPacketSize",nPacketSize);
                if(nRet != MV_OK)
                {
                    printf("Warning: Set Packet Size fail nRet [0x%x]!", nRet);
                }
            }
            else
            {
                printf("Warning: Get Packet Size fail nRet [0x%x]!", nPacketSize);
            }
        }
        // Get the symbol of the specified value of the enum type node.

        nRet = MV_CC_GetEnumValue(handle, "PixelFormat", &stEnumValue);
        if (MV_OK != nRet)
        {
            printf("Get PixelFormat's value fail! nRet [0x%x]\n", nRet);
            break;
        }

        stEnumEntry.nValue = stEnumValue.nCurValue;
        nRet = MV_CC_GetEnumEntrySymbolic(handle, "PixelFormat", &stEnumEntry);
        if (MV_OK != nRet)
        {
            printf("Get PixelFormat's symbol fail! nRet [0x%x]\n", nRet);
            break;
        }
        else
        {
            printf("PixelFormat:%s\n", stEnumEntry.chSymbolic);
        }

        // Start image acquisition.
        nRet = MV_CC_StartGrabbing(handle);
        if (MV_OK != nRet)
        {
            printf("Start Grabbing fail! nRet [0x%x]\n", nRet);
            break;
        }

        nRet = MV_CC_GetImageBuffer(handle, &stOutFrame, 1000);
        if (nRet == MV_OK)
        {
            printf("Get Image Buffer: Width[%d], Height[%d], FrameNum[%d]\n",
                   stOutFrame.stFrameInfo.nWidth, stOutFrame.stFrameInfo.nHeight, stOutFrame.stFrameInfo.nFrameNum);

            //pData = stOutFrame.pBufAddr;
            //std::cout << stOutFrame.pBufAddr << std::endl;
            //cv::cvtColor(nData, pData, cv::COLOR_GRAY2RGB);
            _sourceMatO = cv::Mat(stOutFrame.stFrameInfo.nHeight, stOutFrame.stFrameInfo.nWidth, CV_8UC1, stOutFrame.pBufAddr);
            //cv::imwrite("D:/Image_Mat.png", srcImage);


            nRet = MV_CC_FreeImageBuffer(handle, &stOutFrame);
            if(nRet != MV_OK)
            {
                printf("Free Image Buffer fail! nRet [0x%x]\n", nRet);
            }
        }
        else
        {
            printf("Get Image fail! nRet [0x%x]\n", nRet);
        }


        //******************************************************

        if (_sourceMatO.empty())
            return;

        cv::resize(_sourceMatO, resizedMatO, cv::Size(_appSet.CAMERA_WIDTH, _appSet.CAMERA_HEIGHT));

        cv::cvtColor(resizedMatO, _destinationMatO, cv::COLOR_BGR2RGB);

        ///////////////////////////////////////////////////////////////////////
        // Отрисовка прицела
        ///////////////////////////////////////////////////////////////////////
        //cv::rectangle(_destinationMatO,
        //              cv::Point(X0 - SIGHT_SIZE, Y0 - SIGHT_SIZE),
        //              cv::Point(X0 + SIGHT_SIZE, Y0 + SIGHT_SIZE),
        //              CV_RGB(255, 255, 255), 1, 0);

        _destinationMatO.copyTo(overlayImage);

        // Внешний контур
        roundedRectangle(_destinationMatO,
                          cv::Point(X0 - SIGHT_SIZE, Y0 - SIGHT_SIZE),
                          cv::Point(X0 + SIGHT_SIZE, Y0 + SIGHT_SIZE),
                          CV_RGB(0, 255, 255),
                          2,
                          cv::LINE_8,
                          10);

        // Рисочки внешнего контура
        cv::line(_destinationMatO,
                 cv::Point(X0, Y0 - SIGHT_SIZE),
                 cv::Point(X0, Y0 - SIGHT_SIZE + SIGHT_TICK),
                 CV_RGB(0, 255, 255),
                 1,
                 cv::LINE_8);
        cv::line(_destinationMatO,
                 cv::Point(X0, Y0 + SIGHT_SIZE),
                 cv::Point(X0, Y0 + SIGHT_SIZE - SIGHT_TICK),
                 CV_RGB(0, 255, 255),
                 1,
                 cv::LINE_8);
        cv::line(_destinationMatO,
                 cv::Point(X0 - SIGHT_SIZE, Y0 ),
                 cv::Point(X0 - SIGHT_SIZE + SIGHT_TICK, Y0),
                 CV_RGB(0, 255, 255),
                 1,
                 cv::LINE_8);
        cv::line(_destinationMatO,
                 cv::Point(X0 + SIGHT_SIZE, Y0 ),
                 cv::Point(X0 + SIGHT_SIZE - SIGHT_TICK, Y0),
                 CV_RGB(0, 255, 255),
                 1,
                 cv::LINE_8);

        // Рисочки внутреннего прицела
        cv::line(_destinationMatO,
                 cv::Point(X0 - SIGHT_DELTA, Y0),
                 cv::Point(X0 - SIGHT_DELTA - SIGHT_CROSS, Y0),
                 CV_RGB(255, 255, 255),
                 1,
                 cv::LINE_8);
        cv::line(_destinationMatO,
                 cv::Point(X0 + SIGHT_DELTA, Y0),
                 cv::Point(X0 + SIGHT_DELTA + SIGHT_CROSS, Y0),
                 CV_RGB(255, 255, 255),
                 1,
                 cv::LINE_8);

        cv::line(_destinationMatO,
                 cv::Point(X0, Y0 - SIGHT_DELTA),
                 cv::Point(X0, Y0 - SIGHT_DELTA - SIGHT_CROSS),
                 CV_RGB(255, 255, 255),
                 1,
                 cv::LINE_8);
        cv::line(_destinationMatO,
                 cv::Point(X0, Y0 + SIGHT_DELTA),
                 cv::Point(X0, Y0 + SIGHT_DELTA + SIGHT_CROSS),
                 CV_RGB(255, 255, 255),
                 1,
                 cv::LINE_8);

        ///////////////////////////////////////////////////////////////////////
        // Риски вертикальные (левые)
        ///////////////////////////////////////////////////////////////////////
        for (int i = 1; i < GRID_V_MAX; i++)
        {
            cv::line(_destinationMatO,
                     cv::Point(XV0, YV0 + GRID_V_DELTA * 10 * (i - 1)),
                     cv::Point(XV0 + GRID_BIG_SIZE, YV0 + GRID_V_DELTA * 10 * (i - 1)),
                     CV_RGB(255, 255, 255),
                     2,
                     cv::LINE_8);

            for (int j = 1; j < 10; j++)
            {
                cv::line(_destinationMatO,
                         cv::Point(XV0 + GRID_SMALL_SIZE, YV0 + GRID_V_DELTA * 10 * (i - 1) + j * GRID_V_DELTA),
                         cv::Point(XV0 + 2 * GRID_SMALL_SIZE, YV0 + GRID_V_DELTA * 10 * (i - 1) + j * GRID_V_DELTA),
                         CV_RGB(255, 255, 255),
                         1,
                         cv::LINE_8);
            }
        }
        // Завершающая
        cv::line(_destinationMatO,
                 cv::Point(XV0, YV0 + GRID_V_DELTA * 10 * (GRID_V_MAX - 1)),
                 cv::Point(XV0 + GRID_BIG_SIZE, YV0 + GRID_V_DELTA * 10 * (GRID_V_MAX - 1)),
                 CV_RGB(255, 255, 255),
                 2,
                 cv::LINE_8);


        ///////////////////////////////////////////////////////////////////////
        // Риски вертикальные (правые)
        ///////////////////////////////////////////////////////////////////////
        for (int i = 1; i < GRID_V_MAX; i++)
        {
            cv::line(_destinationMatO,
                     cv::Point(X0 + (X0 - XV0) - 30, YV0 + GRID_V_DELTA * 10 * (i - 1)),
                     cv::Point(X0 + (X0 - XV0) + GRID_BIG_SIZE - 30, YV0 + GRID_V_DELTA * 10 * (i - 1)),
                     CV_RGB(255, 255, 255),
                     2,
                     cv::LINE_8);

            for (int j = 1; j < 10; j++)
            {
                cv::line(_destinationMatO,
                         cv::Point(X0 + (X0 - XV0) - 30, YV0 + GRID_V_DELTA * 10 * (i - 1) + j * GRID_V_DELTA),
                         cv::Point(X0 + (X0 - XV0) + GRID_SMALL_SIZE - 30, YV0 + GRID_V_DELTA * 10 * (i - 1) + j * GRID_V_DELTA),
                         CV_RGB(255, 255, 255),
                         1,
                         cv::LINE_8);
            }
        }
        // Завершающая
        cv::line(_destinationMatO,
                 cv::Point(X0 + (X0 - XV0) - 30, YV0 + GRID_V_DELTA * 10 * (GRID_V_MAX - 1)),
                 cv::Point(X0 + (X0 - XV0) + GRID_BIG_SIZE - 30, YV0 + GRID_V_DELTA * 10 * (GRID_V_MAX - 1)),
                 CV_RGB(255, 255, 255),
                 2,
                 cv::LINE_8);


        ///////////////////////////////////////////////////////////////////////
        // Риски горизонтальные
        ///////////////////////////////////////////////////////////////////////
        for (int i = 1; i < GRID_H_MAX; i++)
        {
            cv::line(_destinationMatO,
                     cv::Point(XH0 + GRID_H_DELTA * 10 * (i - 1), YH0),
                     cv::Point(XH0 + GRID_H_DELTA * 10 * (i - 1), YH0 + GRID_BIG_SIZE),
                     CV_RGB(255, 255, 255),
                     2,
                     cv::LINE_8);

            for (int j = 1; j < 10; j++)
            {
                cv::line(_destinationMatO,
                         cv::Point(XH0 + GRID_H_DELTA * 10 * (i - 1) + j * GRID_H_DELTA, YH0 + GRID_SMALL_SIZE),
                         cv::Point(XH0 + GRID_H_DELTA * 10 * (i - 1) + j * GRID_H_DELTA , YH0 + 2 * GRID_SMALL_SIZE),
                         CV_RGB(255, 255, 255),
                         1,
                         cv::LINE_8);
            }
        }
        // Завершающая
        cv::line(_destinationMatO,
                 cv::Point(XH0 + GRID_H_DELTA * 10 * (GRID_H_MAX - 1), YH0),
                 cv::Point(XH0 + GRID_H_DELTA * 10 * (GRID_H_MAX - 1), YH0 + GRID_BIG_SIZE),
                 CV_RGB(255, 255, 255),
                 2,
                 cv::LINE_8);
        ///////////////////////////////////////////////////////////////////////
        // Табличка
        cv::rectangle(_destinationMatO,
                      cv::Point(XV0, _appSet.CAMERA_HEIGHT - 50),
                      cv::Point(X0 + (X0 - XV0) + GRID_BIG_SIZE - 30, _appSet.CAMERA_HEIGHT - 100),
                      CV_RGB(255, 255, 255), 2, cv::LINE_8);
        cv::rectangle(_destinationMatO,
                      cv::Point(XV0, _appSet.CAMERA_HEIGHT - 50),
                      cv::Point(X0 + (X0 - XV0) + GRID_BIG_SIZE - 30, _appSet.CAMERA_HEIGHT - 100),
                      CV_RGB(0, 0, 0), -1);

        ///////////////////////////////////////////////////////////////////////
        // Диагностика

        cv::putText(_destinationMatO,
                    "DIAGNOSTIC: " +
                        QTime::currentTime().toString("hh:mm:ss").toStdString(),
                    cv::Point(XV0 + 20, _appSet.CAMERA_HEIGHT - 65),
                    cv::FONT_HERSHEY_SIMPLEX,
                    1,
                    CV_RGB(255, 255, 255),
                    2);

        ///////////////////////////////////////////////////////////////////////
        // Левая информацияонная панель (CONTROL)
        cv::rectangle(_destinationMatO,
                      cv::Point(JSX0, JSY0),
                      cv::Point(JSX0 + JSWIDTH, JSY0 + JSTEXTDELTA + 12*13 + 10),
                      CV_RGB(255, 255, 255), 2, cv::LINE_8);
        cv::rectangle(_destinationMatO,
                      cv::Point(JSX0, JSY0),
                      cv::Point(JSX0 + JSWIDTH, JSY0 + JSTEXTDELTA + 12*13 + 10),
                      CV_RGB(0, 0, 0), -1);

        cv::line(_destinationMatO,
                 cv::Point(JSX0 + 5, JSY0 + 10),
                 cv::Point(JSX0 + JSWIDTH - 5, JSY0 + 10),
                 CV_RGB(255, 255, 255),
                 1,
                 cv::LINE_8);

        cv::line(_destinationMatO,
                 cv::Point(JSX0 + 5, JSY0 + 27),
                 cv::Point(JSX0 + JSWIDTH - 5, JSY0 + 27),
                 CV_RGB(255, 255, 255),
                 1,
                 cv::LINE_8);

        // Текстовка
        cv::putText(_destinationMatO,
                    "CONTROL",
                    cv::Point(JSX0 + 10, JSY0 + 24),
                    cv::FONT_HERSHEY_PLAIN,
                    1,
                    CV_RGB(255, 255, 255),
                    1);

        // HorizontalVectorX
        cv::putText(_destinationMatO,
                    "HRZV.X: " + QString::number(_dataControl.HorizontalVectorX, 'f', 2).toStdString(),
                    cv::Point(JSX0 + 10, JSY0 + JSTEXTDELTA + 12*1),
                    cv::FONT_HERSHEY_PLAIN,
                    1,
                    CV_RGB(255, 255, 255),
                    1);
        // HorizontalVectorY
        cv::putText(_destinationMatO,
                    "HRZV.Y: " + QString::number(_dataControl.HorizontalVectorY, 'f', 2).toStdString(),
                    cv::Point(JSX0 + 10, JSY0 + JSTEXTDELTA + 12*2),
                    cv::FONT_HERSHEY_PLAIN,
                    1,
                    CV_RGB(255, 255, 255),
                    1);
        // VericalThrust
        cv::putText(_destinationMatO,
                    "VERT.TH: " + QString::number(_dataControl.VericalThrust, 'f', 2).toStdString(),
                    cv::Point(JSX0 + 10, JSY0 + JSTEXTDELTA + 12*3),
                    cv::FONT_HERSHEY_PLAIN,
                    1,
                    CV_RGB(255, 255, 255),
                    1);        
        // AngularVelocityZ
        cv::putText(_destinationMatO,
                    "ANGV.Z: " + QString::number(_dataControl.AngularVelocityZ, 'f', 2).toStdString(),
                    cv::Point(JSX0 + 10, JSY0 + JSTEXTDELTA + 12*4),
                    cv::FONT_HERSHEY_PLAIN,
                    1,
                    CV_RGB(255, 255, 255),
                    1);
        // PowerTarget
        cv::putText(_destinationMatO,
                    "PWR.TRG: " + QString::number(_dataControl.PowerTarget, 'f', 2).toStdString(),
                    cv::Point(JSX0 + 10, JSY0 + JSTEXTDELTA + 12*5),
                    cv::FONT_HERSHEY_PLAIN,
                    1,
                    CV_RGB(255, 255, 255),
                    1);
        // ManipulatorState
        cv::putText(_destinationMatO,
                    "MNP.ST: " + QString::number(_dataControl.ManipulatorState).toStdString(),
                    cv::Point(JSX0 + 10, JSY0 + JSTEXTDELTA + 12*6),
                    cv::FONT_HERSHEY_PLAIN,
                    1,
                    CV_RGB(255, 255, 255),
                    1);
        // ManipulatorRotate
        cv::putText(_destinationMatO,
                    "MNP.ROT: " + QString::number(_dataControl.ManipulatorRotate, 'f', 2).toStdString(),
                    cv::Point(JSX0 + 10, JSY0 + JSTEXTDELTA + 12*7),
                    cv::FONT_HERSHEY_PLAIN,
                    1,
                    CV_RGB(255, 255, 255),
                    1);
        // CameraRotate
        cv::putText(_destinationMatO,
                    "CAM.ROT: " + QString::number(_dataControl.CameraRotate).toStdString(),
                    cv::Point(JSX0 + 10, JSY0 + JSTEXTDELTA + 12*8),
                    cv::FONT_HERSHEY_PLAIN,
                    1,
                    CV_RGB(255, 255, 255),
                    1);
        // ResetInitialization
        cv::putText(_destinationMatO,
                    "RESET.INI: " + QString::number(_dataControl.ResetInitialization).toStdString(),
                    cv::Point(JSX0 + 10, JSY0 + JSTEXTDELTA + 12*9),
                    cv::FONT_HERSHEY_PLAIN,
                    1,
                    CV_RGB(255, 255, 255),
                    1);
        // LightsState
        cv::putText(_destinationMatO,
                    "LIGHT: " + QString::number(_dataControl.LightsState).toStdString(),
                    cv::Point(JSX0 + 10, JSY0 + JSTEXTDELTA + 12*10),
                    cv::FONT_HERSHEY_PLAIN,
                    1,
                    CV_RGB(255, 255, 255),
                    1);
        // RollInc
        cv::putText(_destinationMatO,
                    "ROLL.INC: " + QString::number(_dataControl.RollInc).toStdString(),
                    cv::Point(JSX0 + 10, JSY0 + JSTEXTDELTA + 12*11),
                    cv::FONT_HERSHEY_PLAIN,
                    1,
                    CV_RGB(255, 255, 255),
                    1);
        // PitchInc
        cv::putText(_destinationMatO,
                    "PITCH.INC: " + QString::number(_dataControl.PitchInc).toStdString(),
                    cv::Point(JSX0 + 10, JSY0 + JSTEXTDELTA + 12*12),
                    cv::FONT_HERSHEY_PLAIN,
                    1,
                    CV_RGB(255, 255, 255),
                    1);
        // ResetPosition
        cv::putText(_destinationMatO,
                    "RESET.POS: " + QString::number(_dataControl.ResetPosition).toStdString(),
                    cv::Point(JSX0 + 10, JSY0 + JSTEXTDELTA + 12*13),
                    cv::FONT_HERSHEY_PLAIN,
                    1,
                    CV_RGB(255, 255, 255),
                    1);

        /*

        cv::putText(_destinationMatO,
                    "JOYSTICK INFO",
                    cv::Point(JSX0 + 10, JSY0 + 24),
                    cv::FONT_HERSHEY_PLAIN,
                    1,
                    CV_RGB(255, 255, 255),
                    1);

        cv::putText(_destinationMatO,
                    "LStickX: " + QString::number(_xbox.LStickX).toStdString(),
                    cv::Point(JSX0 + 10, JSY0 + JSTEXTDELTA + 12*1),
                    cv::FONT_HERSHEY_PLAIN,
                    1,
                    CV_RGB(255, 255, 255),
                    1);
        cv::putText(_destinationMatO,
                    "LStickY: " + QString::number(_xbox.LStickY).toStdString(),
                    cv::Point(JSX0 + 10, JSY0 + JSTEXTDELTA + 12*2),
                    cv::FONT_HERSHEY_PLAIN,
                    1,
                    CV_RGB(255, 255, 255),
                    1);
        cv::putText(_destinationMatO,
                    "RStickX: " + QString::number(_xbox.RStickX).toStdString(),
                    cv::Point(JSX0 + 10, JSY0 + JSTEXTDELTA + 12*3),
                    cv::FONT_HERSHEY_PLAIN,
                    1,
                    CV_RGB(255, 255, 255),
                    1);
        cv::putText(_destinationMatO,
                    "RStickY: " + QString::number(_xbox.RStickY).toStdString(),
                    cv::Point(JSX0 + 10, JSY0 + JSTEXTDELTA + 12*4),
                    cv::FONT_HERSHEY_PLAIN,
                    1,
                    CV_RGB(255, 255, 255),
                    1);
        cv::putText(_destinationMatO,
                    "LTrigger: " + QString::number(_xbox.LTrigger).toStdString(),
                    cv::Point(JSX0 + 10, JSY0 + JSTEXTDELTA + 12*5),
                    cv::FONT_HERSHEY_PLAIN,
                    1,
                    CV_RGB(255, 255, 255),
                    1);
        cv::putText(_destinationMatO,
                    "RTrigger: " + QString::number(_xbox.RTrigger).toStdString(),
                    cv::Point(JSX0 + 10, JSY0 + JSTEXTDELTA + 12*6),
                    cv::FONT_HERSHEY_PLAIN,
                    1,
                    CV_RGB(255, 255, 255),
                    1);

        cv::putText(_destinationMatO,
                    "A: " + QString::number(_xbox.A).toStdString(),
                    cv::Point(JSX0 + 10, JSY0 + JSTEXTDELTA + 12*7),
                    cv::FONT_HERSHEY_PLAIN,
                    1,
                    CV_RGB(255, 255, 255),
                    1);
        cv::putText(_destinationMatO,
                    "B: " + QString::number(_xbox.B).toStdString(),
                    cv::Point(JSX0 + 10, JSY0 + JSTEXTDELTA + 12*8),
                    cv::FONT_HERSHEY_PLAIN,
                    1,
                    CV_RGB(255, 255, 255),
                    1);
        cv::putText(_destinationMatO,
                    "X: " + QString::number(_xbox.X).toStdString(),
                    cv::Point(JSX0 + 10, JSY0 + JSTEXTDELTA + 12*9),
                    cv::FONT_HERSHEY_PLAIN,
                    1,
                    CV_RGB(255, 255, 255),
                    1);
        cv::putText(_destinationMatO,
                    "Y: " + QString::number(_xbox.Y).toStdString(),
                    cv::Point(JSX0 + 10, JSY0 + JSTEXTDELTA + 12*10),
                    cv::FONT_HERSHEY_PLAIN,
                    1,
                    CV_RGB(255, 255, 255),
                    1);
        cv::putText(_destinationMatO,
                    "LBumper: " + QString::number(_xbox.LBumper).toStdString(),
                    cv::Point(JSX0 + 10, JSY0 + JSTEXTDELTA + 12*11),
                    cv::FONT_HERSHEY_PLAIN,
                    1,
                    CV_RGB(255, 255, 255),
                    1);
        cv::putText(_destinationMatO,
                    "RBumper: " + QString::number(_xbox.RBumper).toStdString(),
                    cv::Point(JSX0 + 10, JSY0 + JSTEXTDELTA + 12*12),
                    cv::FONT_HERSHEY_PLAIN,
                    1,
                    CV_RGB(255, 255, 255),
                    1);
        cv::putText(_destinationMatO,
                    "D-Pad: " + QString::number(_xbox.DPad).toStdString(),
                    cv::Point(JSX0 + 10, JSY0 + JSTEXTDELTA + 12*13),
                    cv::FONT_HERSHEY_PLAIN,
                    1,
                    CV_RGB(255, 255, 255),
                    1);

        */

        ///////////////////////////////////////////////////////////////////////
        // Телеметрия

        TMX0 = X0 + (X0 - XV0) - 30;
        cv::rectangle(_destinationMatO,
                      cv::Point(TMX0, TMY0),
                      cv::Point(TMX0 + TMWIDTH, TMY0 + TMTEXTDELTA + 12*13 + 10),
                      CV_RGB(255, 255, 255), 2, cv::LINE_8);
        cv::rectangle(_destinationMatO,
                      cv::Point(TMX0, TMY0),
                      cv::Point(TMX0 + TMWIDTH, TMY0 + TMTEXTDELTA + 12*13 + 10),
                      CV_RGB(0, 0, 0), -1);

        cv::line(_destinationMatO,
                 cv::Point(TMX0 + 5, TMY0 + 10),
                 cv::Point(TMX0 + TMWIDTH - 5, TMY0 + 10),
                 CV_RGB(255, 255, 255),
                 1,
                 cv::LINE_8);
        cv::line(_destinationMatO,
                 cv::Point(TMX0 + 5, TMY0 + 27),
                 cv::Point(TMX0 + TMWIDTH - 5, TMY0 + 27),
                 CV_RGB(255, 255, 255),
                 1,
                 cv::LINE_8);

        cv::putText(_destinationMatO,
                    "TELEMETRY",
                    cv::Point(TMX0 + 10, TMY0 + 24),
                    cv::FONT_HERSHEY_PLAIN,
                    1,
                    CV_RGB(255, 255, 255),
                    1);

        cv::putText(_destinationMatO,
                    "Roll: " + QString::number(_dataTelemetry.Roll, 'f', 2).toStdString(),
                    cv::Point(TMX0 + 10, TMY0 + TMTEXTDELTA + 12*1),
                    cv::FONT_HERSHEY_PLAIN,
                    1,
                    CV_RGB(255, 255, 255),
                    1);
        cv::putText(_destinationMatO,
                    "Pitch: " + QString::number(_dataTelemetry.Pitch, 'f', 2).toStdString(),
                    cv::Point(TMX0 + 10, TMY0 + TMTEXTDELTA + 12*2),
                    cv::FONT_HERSHEY_PLAIN,
                    1,
                    CV_RGB(255, 255, 255),
                    1);
        cv::putText(_destinationMatO,
                    "Yaw: " + QString::number(_dataTelemetry.Yaw, 'f', 2).toStdString(),
                    cv::Point(TMX0 + 10, TMY0 + TMTEXTDELTA + 12*3),
                    cv::FONT_HERSHEY_PLAIN,
                    1,
                    CV_RGB(255, 255, 255),
                    1);
        cv::putText(_destinationMatO,
                    "Heading: " + QString::number(_dataTelemetry.Heading, 'f', 2).toStdString(),
                    cv::Point(TMX0 + 10, TMY0 + TMTEXTDELTA + 12*4),
                    cv::FONT_HERSHEY_PLAIN,
                    1,
                    CV_RGB(255, 255, 255),
                    1);
        cv::putText(_destinationMatO,
                    "Depth: " + QString::number(_dataTelemetry.Depth, 'f', 2).toStdString(),
                    cv::Point(TMX0 + 10, TMY0 + TMTEXTDELTA + 12*5),
                    cv::FONT_HERSHEY_PLAIN,
                    1,
                    CV_RGB(255, 255, 255),
                    1);
        /*
        cv::putText(_destinationMatO,
                    "Depth: " + QString::number(_xbox.RTrigger).toStdString(),
                    cv::Point(TMX0 + 10, TMY0 + TMTEXTDELTA + 12*6),
                    cv::FONT_HERSHEY_PLAIN,
                    1,
                    CV_RGB(255, 255, 255),
                    1);

        cv::putText(_destinationMatO,
                    "A: " + QString::number(_xbox.A).toStdString(),
                    cv::Point(TMX0 + 10, TMY0 + TMTEXTDELTA + 12*7),
                    cv::FONT_HERSHEY_PLAIN,
                    1,
                    CV_RGB(255, 255, 255),
                    1);
        cv::putText(_destinationMatO,
                    "B: " + QString::number(_xbox.B).toStdString(),
                    cv::Point(TMX0 + 10, TMY0 + TMTEXTDELTA + 12*8),
                    cv::FONT_HERSHEY_PLAIN,
                    1,
                    CV_RGB(255, 255, 255),
                    1);
        cv::putText(_destinationMatO,
                    "X: " + QString::number(_xbox.X).toStdString(),
                    cv::Point(TMX0 + 10, TMY0 + TMTEXTDELTA + 12*9),
                    cv::FONT_HERSHEY_PLAIN,
                    1,
                    CV_RGB(255, 255, 255),
                    1);
        cv::putText(_destinationMatO,
                    "Y: " + QString::number(_xbox.Y).toStdString(),
                    cv::Point(TMX0 + 10, TMY0 + TMTEXTDELTA + 12*10),
                    cv::FONT_HERSHEY_PLAIN,
                    1,
                    CV_RGB(255, 255, 255),
                    1);
        cv::putText(_destinationMatO,
                    "LBumper: " + QString::number(_xbox.LBumper).toStdString(),
                    cv::Point(TMX0 + 10, TMY0 + TMTEXTDELTA + 12*11),
                    cv::FONT_HERSHEY_PLAIN,
                    1,
                    CV_RGB(255, 255, 255),
                    1);
        cv::putText(_destinationMatO,
                    "RBumper: " + QString::number(_xbox.RBumper).toStdString(),
                    cv::Point(TMX0 + 10, TMY0 + TMTEXTDELTA + 12*12),
                    cv::FONT_HERSHEY_PLAIN,
                    1,
                    CV_RGB(255, 255, 255),
                    1);
        //cv::putText(_destinationMatO,
        //            "View: " + QString::number(_xbox.View).toStdString(),
        //            cv::Point(JSX0 + 10, JSY0 + JSTEXTDELTA + 12*13),
        //            cv::FONT_HERSHEY_PLAIN,
        //            1,
        //            CV_RGB(255, 255, 255),
        //            1);
        //cv::putText(_destinationMatO,
        //            "Menu: " + QString::number(_xbox.Menu).toStdString(),
        //            cv::Point(JSX0 + 10, JSY0 + JSTEXTDELTA + 12*14),
        //            cv::FONT_HERSHEY_PLAIN,
        //            1,
        //            CV_RGB(255, 255, 255),
        //            1);
        cv::putText(_destinationMatO,
                    "D-Pad: " + QString::number(_xbox.DPad).toStdString(),
                    cv::Point(TMX0 + 10, TMY0 + TMTEXTDELTA + 12*13),
                    cv::FONT_HERSHEY_PLAIN,
                    1,
                    CV_RGB(255, 255, 255),
                    1);
        */

        ///////////////////////////////////////////////////////////////////////
        // Склейка
        cv::addWeighted(overlayImage, alpha, _destinationMatO, 1 - alpha, 0, transparencyiImage);
        ///////////////////////////////////////////////////////////////////////

        _imgCamO = QImage((uchar*) transparencyiImage.data,
                          transparencyiImage.cols,
                          transparencyiImage.rows,
                          transparencyiImage.step,
                          QImage::Format_RGB888);


        //_imgCamO = QImage((uchar*) _destinationMatO.data,
        //                  _destinationMatO.cols,
        //                  _destinationMatO.rows,
        //                  _destinationMatO.step,
        //                  QImage::Format_RGB888);

        ui->lbCamera->setPixmap(QPixmap::fromImage(_imgCamO));

        //******************************************************
        nRet = MV_CC_StopGrabbing(handle);
        if (MV_OK != nRet)
        {
            printf("Stop Grabbing fail! nRet [0x%x]\n", nRet);
            break;
        }

        // Close device.
        nRet = MV_CC_CloseDevice(handle);
        if (MV_OK != nRet)
        {
            printf("ClosDevice fail! nRet [0x%x]\n", nRet);
            break;
        }

        // Destroy handle.
        nRet = MV_CC_DestroyHandle(handle);
        if (MV_OK != nRet)
        {
            printf("Destroy Handle fail! nRet [0x%x]\n", nRet);
            break;
        }
        handle = NULL;

        if (handle != NULL)
        {
            MV_CC_DestroyHandle(handle);
            handle = NULL;
        }
        //******************************************************

        break;
    case CameraView::STEREO:
        ///////////////////////////////////////////////////////////////////////
        // Left Camera



        nRet = MV_CC_CreateHandle(&handle, stDeviceList.pDeviceInfo[0]);
        if (MV_OK != nRet)
        {
            printf("Create Handle fail! nRet [0x%x]\n", nRet);
            break;
        }

        // Open device.
        nRet = MV_CC_OpenDevice(handle);
        if (MV_OK != nRet)
        {
            printf("Open Device fail! nRet [0x%x]\n", nRet);
            break;
        }

        // Detect network optimal package size (only works for GigE cameras).
        if (stDeviceList.pDeviceInfo[nIndex]->nTLayerType == MV_GIGE_DEVICE)
        {
            int nPacketSize = MV_CC_GetOptimalPacketSize(handle);
            if (nPacketSize > 0)
            {
                nRet = MV_CC_SetIntValue(handle,"GevSCPSPacketSize",nPacketSize);
                if(nRet != MV_OK)
                {
                    printf("Warning: Set Packet Size fail nRet [0x%x]!", nRet);
                }
            }
            else
            {
                printf("Warning: Get Packet Size fail nRet [0x%x]!", nPacketSize);
            }
        }
        // Get the symbol of the specified value of the enum type node.

        nRet = MV_CC_GetEnumValue(handle, "PixelFormat", &stEnumValue);
        if (MV_OK != nRet)
        {
            printf("Get PixelFormat's value fail! nRet [0x%x]\n", nRet);
            break;
        }

        stEnumEntry.nValue = stEnumValue.nCurValue;
        nRet = MV_CC_GetEnumEntrySymbolic(handle, "PixelFormat", &stEnumEntry);
        if (MV_OK != nRet)
        {
            printf("Get PixelFormat's symbol fail! nRet [0x%x]\n", nRet);
            break;
        }
        else
        {
            printf("PixelFormat:%s\n", stEnumEntry.chSymbolic);
        }

        // Start image acquisition.
        nRet = MV_CC_StartGrabbing(handle);
        if (MV_OK != nRet)
        {
            printf("Start Grabbing fail! nRet [0x%x]\n", nRet);
            break;
        }

        nRet = MV_CC_GetImageBuffer(handle, &stOutFrame, 1000);
        if (nRet == MV_OK)
        {
            printf("Get Image Buffer: Width[%d], Height[%d], FrameNum[%d]\n",
                   stOutFrame.stFrameInfo.nWidth, stOutFrame.stFrameInfo.nHeight, stOutFrame.stFrameInfo.nFrameNum);

            //pData = stOutFrame.pBufAddr;
            //std::cout << stOutFrame.pBufAddr << std::endl;
            //cv::cvtColor(nData, pData, cv::COLOR_GRAY2RGB);
            _sourceMatL = cv::Mat(stOutFrame.stFrameInfo.nHeight, stOutFrame.stFrameInfo.nWidth, CV_8UC1, stOutFrame.pBufAddr);
            //cv::imwrite("D:/Image_Mat.png", srcImage);


            nRet = MV_CC_FreeImageBuffer(handle, &stOutFrame);
            if(nRet != MV_OK)
            {
                printf("Free Image Buffer fail! nRet [0x%x]\n", nRet);
            }
        }
        else
        {
            printf("Get Image fail! nRet [0x%x]\n", nRet);
        }


        //_webCamL->read(_sourceMatL);

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

        //******************************************************
        nRet = MV_CC_StopGrabbing(handle);
        if (MV_OK != nRet)
        {
            printf("Stop Grabbing fail! nRet [0x%x]\n", nRet);
            break;
        }

        // Close device.
        nRet = MV_CC_CloseDevice(handle);
        if (MV_OK != nRet)
        {
            printf("ClosDevice fail! nRet [0x%x]\n", nRet);
            break;
        }

        // Destroy handle.
        nRet = MV_CC_DestroyHandle(handle);
        if (MV_OK != nRet)
        {
            printf("Destroy Handle fail! nRet [0x%x]\n", nRet);
            break;
        }
        handle = NULL;

        if (handle != NULL)
        {
            MV_CC_DestroyHandle(handle);
            handle = NULL;
        }
        //******************************************************


        ///////////////////////////////////////////////////////////////////////
        // Right Camera

        nRet = MV_CC_CreateHandle(&handle, stDeviceList.pDeviceInfo[1]);
        if (MV_OK != nRet)
        {
            printf("Create Handle fail! nRet [0x%x]\n", nRet);
            break;
        }

        // Open device.
        nRet = MV_CC_OpenDevice(handle);
        if (MV_OK != nRet)
        {
            printf("Open Device fail! nRet [0x%x]\n", nRet);
            break;
        }

        // Detect network optimal package size (only works for GigE cameras).
        if (stDeviceList.pDeviceInfo[nIndex]->nTLayerType == MV_GIGE_DEVICE)
        {
            int nPacketSize = MV_CC_GetOptimalPacketSize(handle);
            if (nPacketSize > 0)
            {
                nRet = MV_CC_SetIntValue(handle,"GevSCPSPacketSize",nPacketSize);
                if(nRet != MV_OK)
                {
                    printf("Warning: Set Packet Size fail nRet [0x%x]!", nRet);
                }
            }
            else
            {
                printf("Warning: Get Packet Size fail nRet [0x%x]!", nPacketSize);
            }
        }
        // Get the symbol of the specified value of the enum type node.

        nRet = MV_CC_GetEnumValue(handle, "PixelFormat", &stEnumValue);
        if (MV_OK != nRet)
        {
            printf("Get PixelFormat's value fail! nRet [0x%x]\n", nRet);
            break;
        }

        stEnumEntry.nValue = stEnumValue.nCurValue;
        nRet = MV_CC_GetEnumEntrySymbolic(handle, "PixelFormat", &stEnumEntry);
        if (MV_OK != nRet)
        {
            printf("Get PixelFormat's symbol fail! nRet [0x%x]\n", nRet);
            break;
        }
        else
        {
            printf("PixelFormat:%s\n", stEnumEntry.chSymbolic);
        }

        // Start image acquisition.
        nRet = MV_CC_StartGrabbing(handle);
        if (MV_OK != nRet)
        {
            printf("Start Grabbing fail! nRet [0x%x]\n", nRet);
            break;
        }

        nRet = MV_CC_GetImageBuffer(handle, &stOutFrame, 1000);
        if (nRet == MV_OK)
        {
            printf("Get Image Buffer: Width[%d], Height[%d], FrameNum[%d]\n",
                   stOutFrame.stFrameInfo.nWidth, stOutFrame.stFrameInfo.nHeight, stOutFrame.stFrameInfo.nFrameNum);

            //pData = stOutFrame.pBufAddr;
            //std::cout << stOutFrame.pBufAddr << std::endl;
            //cv::cvtColor(nData, pData, cv::COLOR_GRAY2RGB);
            _sourceMatR = cv::Mat(stOutFrame.stFrameInfo.nHeight, stOutFrame.stFrameInfo.nWidth, CV_8UC1, stOutFrame.pBufAddr);
            //cv::imwrite("D:/Image_Mat.png", srcImage);


            nRet = MV_CC_FreeImageBuffer(handle, &stOutFrame);
            if(nRet != MV_OK)
            {
                printf("Free Image Buffer fail! nRet [0x%x]\n", nRet);
            }
        }
        else
        {
            printf("Get Image fail! nRet [0x%x]\n", nRet);
        }


        //_webCamR->read(_sourceMatR);

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

        //******************************************************
        nRet = MV_CC_StopGrabbing(handle);
        if (MV_OK != nRet)
        {
            printf("Stop Grabbing fail! nRet [0x%x]\n", nRet);
            break;
        }

        // Close device.
        nRet = MV_CC_CloseDevice(handle);
        if (MV_OK != nRet)
        {
            printf("ClosDevice fail! nRet [0x%x]\n", nRet);
            break;
        }

        // Destroy handle.
        nRet = MV_CC_DestroyHandle(handle);
        if (MV_OK != nRet)
        {
            printf("Destroy Handle fail! nRet [0x%x]\n", nRet);
            break;
        }
        handle = NULL;

        if (handle != NULL)
        {
            MV_CC_DestroyHandle(handle);
            handle = NULL;
        }
        //******************************************************


        break;
    default:
        break;
    }
}

t_vuxyzrgb MainWindow:: getCloud3DPoints(int rows, int cols, bool norm = true)
{
    t_vuxyzrgb data;

    // Путь к папке с данными
    auto dataPath = QDir::cleanPath(qApp->applicationDirPath() +
                                    QDir::separator() + "data");
    // qDebug() << "Data path : " << dataPath;

    // Чтение данных
    auto fileName = QFileDialog::getOpenFileName(this,
                                                 tr("Open Data File"),
                                                 dataPath,
                                                 tr("TXT Files (*.txt)"));
    QFile file(fileName);
    QStringList lineData;
    Data3DItem data3DItem;

    int vuX;
    int vuY;
    double xyzX;
    double xyzY;
    double xyzZ;
    int rgbR;
    int rgbG;
    int rgbB;
    int clst;

    int Xmin = INT_MAX;
    int Xmax = INT_MIN;
    int Ymin = INT_MAX;
    int Ymax = INT_MIN;

    if (file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QTextStream in(&file);
        while (!in.atEnd()) {

            // Строка данных
            lineData = in.readLine().split("\t");

            // Парсинг
            vuX = lineData[0].trimmed().toInt();
            vuY = lineData[1].trimmed().toInt();

            if (vuX > Xmax)
                Xmax = vuX;
            if (vuX < Xmin)
                Xmin = vuX;

            if (vuY > Ymax)
                Ymax = vuY;
            if (vuY < Ymin)
                Ymin = vuY;

            xyzX = lineData[2].trimmed().toDouble();
            xyzY = lineData[3].trimmed().toDouble();
            xyzZ = lineData[4].trimmed().toDouble();

            rgbR = lineData[5].trimmed().toInt();
            rgbG = lineData[6].trimmed().toInt();
            rgbB = lineData[7].trimmed().toInt();

            clst = lineData[8].trimmed().toInt();

            // Очистка структуры
            data3DItem.vu.clear();
            data3DItem.vu.clear();

            data3DItem.xyz.clear();
            data3DItem.xyz.clear();
            data3DItem.xyz.clear();

            data3DItem.rgb.clear();
            data3DItem.rgb.clear();
            data3DItem.rgb.clear();

            // Заполнение структуры
            data3DItem.vu.push_back(vuX);
            data3DItem.vu.push_back(vuY);

            data3DItem.xyz.push_back(xyzX);
            data3DItem.xyz.push_back(xyzY);
            data3DItem.xyz.push_back(xyzZ);

            data3DItem.rgb.push_back(rgbR);
            data3DItem.rgb.push_back(rgbG);
            data3DItem.rgb.push_back(rgbB);

            data3DItem.cluster = clst;

            // Накопление данных
            data.vu.push_back(data3DItem.vu);
            data.xyz.push_back(data3DItem.xyz);
            data.rgb.push_back(data3DItem.rgb);
            data.cluster.push_back(data3DItem.cluster);
        }


        if (norm)
        {
            for (size_t i = 0; i < data.vu.size(); i++)
            {
                if (Xmax != Xmin)
                    data.vu.at(i).at(1) = cols * (data.vu.at(i).at(1) - Xmin) /
                                          (Xmax - Xmin);
                if (Ymax != Ymin)
                    data.vu.at(i).at(0) = rows * (data.vu.at(i).at(0) - Ymin) /
                                          (Ymax - Ymin);
            }
        }

    }

    return data;
}

std::vector<Cloud3DItem> MainWindow::getCloud3DPoints(std::string pathtofile)
{
    std::vector<Cloud3DItem> cloud;

    Cloud3DItem item;

    int screenX;
    int screenY;
    double worldX;
    double worldY;
    double worldZ;

    std::string datarow;

    // TODO: Add path checking
    std::ifstream infile(pathtofile);

    // Чтения файла
    if (infile.is_open())
    {
        while (infile >> screenX >> screenY >> worldX >> worldY >> worldZ)
        {
            // VA 07-07-2024: Разобраться, почему необходимо
            // переставлять местами X и Y для координат экрана
            item.screenX = screenY;
            item.screenY = screenX;
            item.worldX = worldX;
            item.worldY = worldY;
            item.worldZ = worldZ;

            cloud.push_back(item);
        }

        infile.close();
    }

    return cloud;
}

t_vuxyzrgb MainWindow::convertCloud3DPoints(std::vector<Cloud3DItem> cloud)
{
    t_vuxyzrgb data;
    Data3DItem data3DItem;

    int vuX;
    int vuY;
    double xyzX;
    double xyzY;
    double xyzZ;
    int rgbR;
    int rgbG;
    int rgbB;
    int clst;

    int Xmin = INT_MAX;
    int Xmax = INT_MIN;
    int Ymin = INT_MAX;
    int Ymax = INT_MIN;

    for (auto &item : cloud) // access by reference to avoid copying
    {
        vuX = item.screenX;
        vuY = item.screenY;
        xyzX = item.worldX;
        xyzY = item.worldY;
        xyzZ = item.worldZ;

        ///////////////////////////////////////////////////////////////////////
        // Min Max
        ///////////////////////////////////////////////////////////////////////

        if (vuX > Xmax)
            Xmax = vuX;
        if (vuX < Xmin)
            Xmin = vuX;

        if (vuY > Ymax)
            Ymax = vuY;
        if (vuY < Ymin)
            Ymin = vuY;

        ///////////////////////////////////////////////////////////////////////

        rgbR = 0;
        rgbG = 0;
        rgbB = 0;

        clst = 0;

        // Очистка структуры
        data3DItem.vu.clear();
        data3DItem.vu.clear();

        data3DItem.xyz.clear();
        data3DItem.xyz.clear();
        data3DItem.xyz.clear();

        data3DItem.rgb.clear();
        data3DItem.rgb.clear();
        data3DItem.rgb.clear();

        // Заполнение структуры
        data3DItem.vu.push_back(vuX);
        data3DItem.vu.push_back(vuY);

        data3DItem.xyz.push_back(xyzX);
        data3DItem.xyz.push_back(xyzY);
        data3DItem.xyz.push_back(xyzZ);

        data3DItem.rgb.push_back(rgbR);
        data3DItem.rgb.push_back(rgbG);
        data3DItem.rgb.push_back(rgbB);

        data3DItem.cluster = clst;

        // Накопление данных
        data.vu.push_back(data3DItem.vu);
        data.xyz.push_back(data3DItem.xyz);
        data.rgb.push_back(data3DItem.rgb);
        data.cluster.push_back(data3DItem.cluster);
    }

    return data;
}

void MainWindow::onControlTimer()
{
    // Пересчитываем состояние джойстика в управление
    SevROVLibrary::XboxToControlData(_xbox,
                                     _ctrSet.powerLimit,
                                     _ctrSet.rollStabilization,
                                     _ctrSet.pitchStabilization,
                                     _ctrSet.yawStabilization,
                                     _ctrSet.depthStabilization,
                                     _ctrSet.rollPID,
                                     _ctrSet.pitchPID,
                                     _ctrSet.yawPID,
                                     _ctrSet.depthPID,
                                     _ctrSet.updatePID,
                                     &_rovConnector.control);

    // Считываем текущие сигналы контроля
    _dataControl.HorizontalVectorX = _rovConnector.control.getHorizontalVectorX();
    _dataControl.HorizontalVectorY = _rovConnector.control.getHorizontalVectorY();
    _dataControl.VericalThrust = _rovConnector.control.getVericalThrust();
    _dataControl.PowerTarget = _rovConnector.control.getPowerTarget();
    _dataControl.AngularVelocityZ = _rovConnector.control.getAngularVelocityZ();

    _dataControl.ManipulatorState = _rovConnector.control.getManipulatorState();
    _dataControl.ManipulatorRotate = _rovConnector.control.getManipulatorRotate();
    _dataControl.CameraRotate = _rovConnector.control.getCameraRotate();

    _dataControl.ResetInitialization = _rovConnector.control.getResetInitialization();
    _dataControl.LightsState = _rovConnector.control.getLightsState();
    _dataControl.StabilizationState = _rovConnector.control.getStabilizationState();

    _dataControl.RollInc = _rovConnector.control.getRollInc();
    _dataControl.PitchInc = _rovConnector.control.getPitchInc();

    _dataControl.ResetPosition = _rovConnector.control.getResetPosition();

    _dataControl.RollKp = _rovConnector.control.getRollKp();
    _dataControl.RollKi = _rovConnector.control.getRollKi();
    _dataControl.RollKd = _rovConnector.control.getRollKd();

    _dataControl.PitchKp = _rovConnector.control.getPitchKp();
    _dataControl.PitchKi = _rovConnector.control.getPitchKi();
    _dataControl.PitchKd = _rovConnector.control.getPitchKd();

    _dataControl.YawKp = _rovConnector.control.getYawKp();
    _dataControl.YawKi = _rovConnector.control.getYawKi();
    _dataControl.YawKd = _rovConnector.control.getYawKd();

    _dataControl.DepthKp = _rovConnector.control.getDepthKp();
    _dataControl.DepthKi = _rovConnector.control.getDepthKi();
    _dataControl.DepthKd = _rovConnector.control.getDepthKd();

    _dataControl.UpdatePID = _rovConnector.control.getUpdatePID();


    // При соединении уже задали IP и Port, которые будут использоваться для записи датаграммы
    if (_rovConnector.getIsConnected())
        _rovConnector.writeControlDatagram();

    // Сброс флага обновления параметров ПИД-контроллера
    if (_ctrSet.updatePID)
        _ctrSet.updatePID = false;
}

void MainWindow::onStartStopButtonClicked()
{
    // Меняем состояние флага
    _sevROV.isConnected = !_sevROV.isConnected;
    _cnt = 0; // Сбрасываем счетчик
    Q_EMIT updateCntValue("CNT: " + QString::number(_cnt++));

    // Меняем иконку на кнопке
    if (_sevROV.isConnected)
    {
        ui->pbStartStop->setIcon(QIcon(":/img/on_button_icon.png"));
        ui->pbStartStop->setIconSize(QSize(64, 64));

        setupCameraConnection(CameraConnection::ON);

        // Joyjstick
        _jsController->OpenJoystick(_appSet.JOYSTICK_ID);
        _jsController->isRunning = true;
        _jsController->start(); // Запуск процесса в поток

        _controlTimer->start(_appSet.JOYSTICK_TIMER_INTERVAL);
    }
    else
    {
        ui->pbStartStop->setIcon(QIcon(":/img/off_button_icon.png"));
        ui->pbStartStop->setIconSize(QSize(64, 64));

        setupCameraConnection(CameraConnection::OFF);

        // Joystick
        _jsController->CloseJoystick();
        _jsController->isRunning = false;
        _jsController->quit();

        _controlTimer->stop();
    }

    // Будем использовать connectToHost и disconnectFromHost
    if (_rovConnector.getIsConnected())
    {
        // Отключаемся
        _rovConnector.disconnectFromHost();
    }
    else
    {
        // Запоминаем IP и Port сервера
        _rovConnector.setIP(_appSet.ROV_IP);
        _rovConnector.setPort(_appSet.ROV_PORT);

        // Соединяемся с хостом
        _rovConnector.connectToHost(_appSet.ROV_IP, _appSet.ROV_PORT);

        // Отправка служебной последовательности
        // stream << (std::byte)0xAA;
        // stream << (std::byte)0xFF;
        if (_rovConnector.getIsConnected())
            _rovConnector.writeConnectDatagram();
    }

    setupConnectedControlsStyle(_sevROV.isConnected);
}

void MainWindow::onViewButtonClicked()
{
    // Если нет соединения - выход
    if (!_sevROV.isConnected)
        return;

    switch (_sevROV.cameraView)
    {
    case CameraView::MONO:
        ui->pbView->setIcon(QIcon(":/img/video_icon.png"));
        ui->pbView->setIconSize(QSize(64, 64));
        _sevROV.cameraView = CameraView::STEREO;

        break;
    case CameraView::STEREO:
        ui->pbView->setIcon(QIcon(":/img/display_icon.png"));
        ui->pbView->setIconSize(QSize(64, 64));
        _sevROV.cameraView = CameraView::MONO;
        break;
    default:
        break;
    }

    setupCameraViewLayout(_sevROV.cameraView);
}

void MainWindow::onScreenshotButtonClicked()
{
    // Создаем инструмент Линейка
    _toolWindow = new ToolWindow(this);    

    // Get current Image from camera
    cv::Mat image;
    _webCamO->read(image);

    // FOR DEBUG ONLY
    // Ресайз картинки под размер камеры
    cv::Mat image_resized;
    cv::resize(image,
               image_resized,
               cv::Size(_appSet.CAMERA_WIDTH, _appSet.CAMERA_HEIGHT),
               0,
               0,
               cv::INTER_LINEAR);

    // Для облака 3D-точек используем стереопару
    cv::Mat imageL;
    cv::Mat imageR;

    _webCamL->read(imageL);
    _webCamR->read(imageR);

    std::string file_calibration_parameters =
        (QCoreApplication::applicationDirPath() + "/camera_calibration_parameters.yml").toStdString();

    stereo_output_par_t calib_par = read_stereo_params(file_calibration_parameters);

    // Поск 3D точек и сохранение их в формате x y 3d_x 3d_y 3d_z
    std::vector<std::vector<double>> coords3d = point3d_finder(imageL, imageR, calib_par);

    // Запись найденных точек в файл
    std::string file_cloud_3D = (QCoreApplication::applicationDirPath() + "/cloud_3D.txt").toStdString();
    write_coords_file(coords3d, file_cloud_3D);

    // Загрузка данных
    std::vector<Cloud3DItem> cloud = getCloud3DPoints(file_cloud_3D);

    // Конвертация в старый формат
    t_vuxyzrgb data = convertCloud3DPoints(cloud);

    _toolWindow->setupWindowGeometry();
    // TODO: Переделать под новый формат данных
    // _toolWindow->set_data_cloud_3D(image_resized, cloud);
    _toolWindow->setDataCloud3D(image_resized, data);
    _toolWindow->setWindowTitle("ТНПА :: AРМ Оператора :: " + _appSet.getAppVersion());

    // Центрировать инструментальную панель
    QRect screenGeometry = QGuiApplication::screens()[0]->geometry();
    int x = (screenGeometry.width() - _toolWindow->width()) / 2;
    int y = (screenGeometry.height() - _toolWindow->height()) / 2;

    _toolWindow->move(x, y);
    _toolWindow->exec();

    // Очищаем ресурсы
    delete _toolWindow;
    ///////////////////////////////////////////////////////////////////////////
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
        _appSet.load(_ctrSet);
        _ctrSet.updatePID = _settingsWindow->getUpdatePID();
    }

    delete _settingsWindow;
}

void MainWindow::OnButtonA(short value)
{
    //ui->edA->setText(QString::number(value));
    _xbox.A = value;
}
void MainWindow::OnButtonB(short value)
{
    //ui->edB->setText(QString::number(value));
    _xbox.B = value;
}
void MainWindow::OnButtonX(short value)
{
    //ui->edX->setText(QString::number(value));
    _xbox.X = value;
}
void MainWindow::OnButtonY(short value)
{
    //ui->edY->setText(QString::number(value));
    _xbox.Y = value;
}
void MainWindow::OnButtonLBumper(short value)
{
    //ui->edLBumper->setText(QString::number(value));
    _xbox.LBumper = value;
}
void MainWindow::OnButtonRBumper(short value)
{
    //ui->edRBumper->setText(QString::number(value));
    _xbox.RBumper = value;
}
void MainWindow::OnButtonView(short value)
{
    //ui->edView->setText(QString::number(value));
    _xbox.View = value;
}
void MainWindow::OnButtonMenu(short value)
{
    //ui->edMenu->setText(QString::number(value));
    _xbox.Menu = value;
}
void MainWindow::OnDPad(short value)
{
    //ui->edDPad->setText(QString::number(value));
    _xbox.DPad = value;
}
void MainWindow::OnAxisLStickX(short value)
{
    //ui->edLStickX->setText(QString::number(value));
    _xbox.LStickX = value;
}
void MainWindow::OnAxisLStickY(short value)
{
    //ui->edLStickY->setText(QString::number(value));
    _xbox.LStickY = value;
}
void MainWindow::OnAxisRStickX(short value)
{
    //ui->edRStickX->setText(QString::number(value));
    _xbox.RStickX = value;
}
void MainWindow::OnAxisRStickY(short value)
{
    //ui->edRStickY->setText(QString::number(value));
    _xbox.RStickY = value;
}
void MainWindow::OnAxisLTrigger(short value)
{
    //ui->edLTrigger->setText(QString::number(value));
    _xbox.LTrigger = value;
}
void MainWindow::OnAxisRTrigger(short value)
{
    //ui->edRTrigger->setText(QString::number(value));
    _xbox.RTrigger = value;
}

void MainWindow::onSocketProcessTelemetryDatagram()
{
    qDebug() << "Telemetry Datagram Received...";

    // Проверяем, разрешено ли коннектору читать телеметрию
    if ((_rovConnector.getMode() & SevROVConnector::Mode::TELEMETRY_READ)
        == SevROVConnector::Mode::TELEMETRY_READ)
    {
        _dataTelemetry.Roll = _rovConnector.telemetry.getRoll();
        _dataTelemetry.Pitch = _rovConnector.telemetry.getPitch();
        _dataTelemetry.Yaw = _rovConnector.telemetry.getYaw();
        _dataTelemetry.Heading = _rovConnector.telemetry.getHeading();
        _dataTelemetry.Depth = _rovConnector.telemetry.getDepth();
        _dataTelemetry.RollSetPoint = _rovConnector.telemetry.getRollSetPoint();
        _dataTelemetry.PitchSetPoint = _rovConnector.telemetry.getPitchSetPoint();
    }
}
void MainWindow::onSocketConnect()
{
    qDebug() << "Socket connected successfully";
}
void MainWindow::onSocketDisconnect()
{
    qDebug() << "Socket disconnected successfully";
}
