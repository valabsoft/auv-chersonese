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
    ui->lbFPS->setVisible(false);

    _videoTimer = new QTimer(this);
    connect(_videoTimer, &QTimer::timeout, this, &MainWindow::onVideoTimer);

    _controlTimer = new QTimer(this);
    connect(_controlTimer, &QTimer::timeout, this, &MainWindow::onControlTimer);

    // QObject::connect(this, SIGNAL(updateCntValue(QString)), ui->lbFPS, SLOT(setText(QString)));

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

    _dataTelemetry.Roll = 0.0;
    _dataTelemetry.Pitch = 0.0;
    _dataTelemetry.Yaw = 0.0;
    _dataTelemetry.Heading = 0.0;
    _dataTelemetry.Depth = 0.0;

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
    if (_webCamL->isOpened())
        _webCamL->release();

    if (_webCamR->isOpened())
        _webCamR->release();

    if (_webCamL)
        delete _webCamL;

    if (_webCamR)
        delete _webCamR;

    if (_toolWindow)
        delete _toolWindow;

    delete _jsController;

    delete ui;
}

std::string generateFileName(std::string filename, std::string fileextension)
{
    using namespace std::chrono;
    auto timer = system_clock::to_time_t(system_clock::now());
    std::tm localTime = *std::localtime(&timer);
    std::ostringstream oss;
    std::string fileName = filename + "_%d%m%Y%H%M%S" + fileextension;
    oss << std::put_time(&localTime, fileName.c_str());
    // return filename + fileextension; // Возвращаем имя файла без таймстемпа
    return oss.str();
}
std::string generateUniqueLogFileName()
{
    struct tm currentTime;
    time_t nowTime = time(0);

#ifdef _WIN32
    localtime_s(&currentTime, &nowTime);
#else
    localtime_r(&nowTime, &currentTime);
#endif

    std::ostringstream outStringStream;
    std::string fullFileName = "%d-%m-%Y.log";
    outStringStream << std::put_time(&currentTime, fullFileName.c_str());
    return outStringStream.str();
}
void writeLog(std::string logText, LOGTYPE logType)
{
    //if (!IS_DEBUG_LOG_ENABLED)
    //    return;

    try
    {
        std::filesystem::path pathToLogDirectory = std::filesystem::current_path() / "log";
        std::filesystem::directory_entry directoryEntry{ pathToLogDirectory };

        // Проверяем существование папки log в рабочем каталоге
        bool isLogDirectoryExists = directoryEntry.exists();

        if (!isLogDirectoryExists)
        {
            // Если папка log не существует, создаем ее
            isLogDirectoryExists = std::filesystem::create_directory(pathToLogDirectory);
            if (!isLogDirectoryExists)
            {
                return;
            }
        }

        // Определяем тип записи
        std::string logTypeAbbreviation;
        switch (logType)
        {
        case LOGTYPE::DEBUG:
            logTypeAbbreviation = "DEBG";
            break;
        case LOGTYPE::ERROR:
            logTypeAbbreviation = "ERRR";
            break;
        case LOGTYPE::EXCEPTION:
            logTypeAbbreviation = "EXCP";
            break;
        case LOGTYPE::INFO:
            logTypeAbbreviation = "INFO";
            break;
        case LOGTYPE::WARNING:
            logTypeAbbreviation = "WARN";
            break;
        default:
            logTypeAbbreviation = "INFO";
            break;
        }

        // Определяем временную метку
        struct tm currentTime;
        time_t nowTime = time(0);

#ifdef _WIN32
        localtime_s(&currentTime, &nowTime);
#else
        localtime_r(&nowTime, &currentTime);
#endif

        std::ostringstream outStringStream;
        outStringStream << std::put_time(&currentTime, "%H:%M:%S");
        std::string logTime = outStringStream.str();

        // Генерируем уникальное имя файла в формате dd-mm-yyyy.log
        std::string logFileName = generateUniqueLogFileName();
        std::filesystem::path pathToLogFile = pathToLogDirectory / logFileName;

        std::ofstream logFile; // Идентификатор лог-файла

        if (std::filesystem::exists(pathToLogFile))
        {
            // Если файл лога существует, открываем файл для дозаписи и добавляем строку в конец
            logFile.open(pathToLogFile.c_str(), std::ios_base::app);
        }
        else
        {
            // Если файл лога не существует, создаем его и добавляем строчку
            logFile.open(pathToLogFile.c_str(), std::ios_base::out);
        }

        if (logFile.is_open())
        {
            logFile << logTime << " | " << logTypeAbbreviation << " | " << logText << std::endl;
            logFile.close();
        }
    }
    catch (...)
    {
        return;
    }
}

int MainWindow::MV_SDK_Initialization()
{
    handleL = NULL;
    handleR = NULL;

    // Initialize SDK
    int nRet = MV_OK;
    if (MV_OK != nRet)
        return 1;

    // Enumerate devices
    MV_CC_DEVICE_INFO_LIST stDeviceList;
    memset(&stDeviceList, 0, sizeof(MV_CC_DEVICE_INFO_LIST));
    nRet = MV_CC_EnumDevices(MV_GIGE_DEVICE | MV_USB_DEVICE, &stDeviceList);
    if (MV_OK != nRet)
        return 2;

    MVCC_ENUMVALUE stEnumValue = {0};
    MVCC_ENUMENTRY stEnumEntry = {0};

    ///////////////////////////////////////////////////////////////////////////
    // Left Camera Initialization
    ///////////////////////////////////////////////////////////////////////////

    nRet = MV_CC_CreateHandle(&handleL, stDeviceList.pDeviceInfo[_appSet.CAMERA_LEFT_ID]);
    if (MV_OK != nRet)
        return 11;

    // Left camera open device.
    nRet = MV_CC_OpenDevice(handleL);
    if (MV_OK != nRet)
        return 12;

    // Detect network optimal package size (only works for GigE cameras).
    if (stDeviceList.pDeviceInfo[_appSet.CAMERA_LEFT_ID]->nTLayerType == MV_GIGE_DEVICE)
    {
        int nPacketSize = MV_CC_GetOptimalPacketSize(handleL);
        if (nPacketSize > 0)
        {
            nRet = MV_CC_SetIntValue(handleL, "GevSCPSPacketSize", nPacketSize);
            if (nRet != MV_OK)
            {
                printf("Warning: Set Packet Size fail nRet [0x%x]!", nRet);
                writeLog("Set Packet Size fail!", LOGTYPE::WARNING);
            }
        }
        else
        {
            printf("Warning: Get Packet Size fail nRet [0x%x]!", nPacketSize);
            writeLog("Get Packet Size fail!", LOGTYPE::WARNING);
        }
    }

    // Get the symbol of the specified value of the enum type node.
    nRet = MV_CC_GetEnumValue(handleL, "PixelFormat", &stEnumValue);
    if (MV_OK != nRet)
        return 13;

    stEnumEntry.nValue = stEnumValue.nCurValue;
    nRet = MV_CC_GetEnumEntrySymbolic(handleL, "PixelFormat", &stEnumEntry);
    if (MV_OK != nRet)
        return 14;
    else
    {
        printf("PixelFormat:%s\n", stEnumEntry.chSymbolic);
        std::string pixelFormat(stEnumEntry.chSymbolic);
        writeLog("PixelFormat: " + pixelFormat, LOGTYPE::INFO);
    }
    ///////////////////////////////////////////////////////////////////////////
    // Start image acquisition
    nRet = MV_CC_StartGrabbing(handleL);
    if (MV_OK != nRet)
        return 15;

    if (stDeviceList.nDeviceNum < 1)
        return -1;

    ///////////////////////////////////////////////////////////////////////////
    // Right Camera Initialization
    ///////////////////////////////////////////////////////////////////////////

    nRet = MV_CC_CreateHandle(&handleR, stDeviceList.pDeviceInfo[_appSet.CAMERA_RIGHT_ID]);
    if (MV_OK != nRet)
        return 21;

    // Right camera open device.
    nRet = MV_CC_OpenDevice(handleR);
    if (MV_OK != nRet)
        return 22;

    // Detect network optimal package size (only works for GigE cameras).
    if (stDeviceList.pDeviceInfo[_appSet.CAMERA_RIGHT_ID]->nTLayerType == MV_GIGE_DEVICE)
    {
        int nPacketSize = MV_CC_GetOptimalPacketSize(handleR);
        if (nPacketSize > 0)
        {
            nRet = MV_CC_SetIntValue(handleR, "GevSCPSPacketSize", nPacketSize);
            if (nRet != MV_OK)
            {
                printf("Warning: Set Packet Size fail nRet [0x%x]!", nRet);
                writeLog("Set Packet Size fail!", LOGTYPE::WARNING);
            }
        }
        else
        {
            printf("Warning: Get Packet Size fail nRet [0x%x]!", nPacketSize);
            writeLog("Get Packet Size fail!", LOGTYPE::WARNING);
        }
    }

    // Get the symbol of the specified value of the enum type node.
    nRet = MV_CC_GetEnumValue(handleR, "PixelFormat", &stEnumValue);
    if (MV_OK != nRet)
        return 23;

    stEnumEntry.nValue = stEnumValue.nCurValue;
    nRet = MV_CC_GetEnumEntrySymbolic(handleR, "PixelFormat", &stEnumEntry);
    if (MV_OK != nRet)
        return 24;
    else
    {
        printf("PixelFormat:%s\n", stEnumEntry.chSymbolic);
        std::string pixelFormat(stEnumEntry.chSymbolic);
        writeLog("PixelFormat: " + pixelFormat, LOGTYPE::INFO);
    }
    ///////////////////////////////////////////////////////////////////////////    
    // Start image acquisition
    nRet = MV_CC_StartGrabbing(handleR);
    if (MV_OK != nRet)
        return 25;

    return 0;
}
int MainWindow::MV_SDK_Finalization()
{
    int nRet = MV_OK;

    ///////////////////////////////////////////////////////////////////////////
    // Close device -- Left
    if (handleL != NULL)
    {
        nRet = MV_CC_StopGrabbing(handleL);
        if (MV_OK != nRet)
            return 10;

        nRet = MV_CC_CloseDevice(handleL);
        if (MV_OK != nRet)
            return 11;

        // Destroy handle
        nRet = MV_CC_DestroyHandle(handleL);
        if (MV_OK != nRet)
            return 12;

        handleL = NULL;

        if (handleL != NULL)
        {
            MV_CC_DestroyHandle(handleL);
            handleL = NULL;
        }
    }
    ///////////////////////////////////////////////////////////////////////////
    // Close device -- Right
    if (handleR != NULL)
    {
        nRet = MV_CC_StopGrabbing(handleR);
        if (MV_OK != nRet)
            return 20;

        nRet = MV_CC_CloseDevice(handleR);
        if (MV_OK != nRet)
            return 21;

        // Destroy handle
        nRet = MV_CC_DestroyHandle(handleR);
        if (MV_OK != nRet)
            return 22;

        handleR = NULL;

        if (handleR != NULL)
        {
            MV_CC_DestroyHandle(handleR);
            handleR = NULL;
        }
    }
    ///////////////////////////////////////////////////////////////////////////

    return 0;
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

        if (_appSet.CAMERA_TYPE == CameraType::IP)
        {            
            int retCode = MV_SDK_Initialization();
            writeLog("MV_SDK_Initialization(): " + std::to_string(retCode), LOGTYPE::INFO);
            switch (retCode)
            {
            case -1:
                qDebug() <<  "ERROR: The only one camera found!";
                writeLog("setupCameraConnection(): ERROR: The only one camera found!", LOGTYPE::ERROR);
                break;
            case 1:
                qDebug() <<  "ERROR: Initialize SDK fail!";
                writeLog("setupCameraConnection(): ERROR: Initialize SDK fail!", LOGTYPE::ERROR);
                break;
            case 2:
                qDebug() <<  "ERROR: Enum Devices fail!";
                writeLog("setupCameraConnection(): ERROR: Enum Devices fail!", LOGTYPE::ERROR);
                break;

            case 11:
                qDebug() <<  "ERROR: Left Camera - Create Handle fail!";
                writeLog("setupCameraConnection(): ERROR: Left Camera - Create Handle fail!", LOGTYPE::ERROR);
                break;
            case 12:
                qDebug() <<  "ERROR: Left Camera - Open Device fail!";
                writeLog("setupCameraConnection(): Left Camera - Open Device fail!", LOGTYPE::ERROR);
                break;
            case 13:
                qDebug() <<  "ERROR: Left Camera - Get PixelFormat's value fail!";
                writeLog("setupCameraConnection(): Left Camera - Get PixelFormat's value fail!", LOGTYPE::ERROR);
                break;
            case 14:
                qDebug() <<  "ERROR: Left Camera - Get PixelFormat's symbol fail!";
                writeLog("setupCameraConnection(): Left Camera - Get PixelFormat's symbol fail!", LOGTYPE::ERROR);
                break;
            case 15:
                qDebug() << "ERROR: Left Camera - Start Grabbing fail!";
                writeLog("setupCameraConnection(): Left Camera - Start Grabbing fail!", LOGTYPE::ERROR);
                break;

            case 21:
                qDebug() <<  "ERROR: Right Camera - Create Handle fail!";
                writeLog("setupCameraConnection(): Right Camera - Create Handle fail!", LOGTYPE::ERROR);
                break;
            case 22:
                qDebug() <<  "ERROR: Right Camera - Open Device fail!";
                writeLog("setupCameraConnection(): Right Camera - Open Device fail!", LOGTYPE::ERROR);
                break;
            case 23:
                qDebug() <<  "ERROR: Right Camera - Get PixelFormat's value fail!";
                writeLog("setupCameraConnection(): Right Camera - Get PixelFormat's value fail!", LOGTYPE::ERROR);
                break;
            case 24:
                qDebug() <<  "ERROR: Right Camera - Get PixelFormat's symbol fail!";
                writeLog("setupCameraConnection(): Right Camera - Get PixelFormat's symbol fail!", LOGTYPE::ERROR);
                break;
            case 25:
                qDebug() << "ERROR: Right Camera - Start Grabbing fail!";
                writeLog("setupCameraConnection(): Right Camera - Start Grabbing fail!", LOGTYPE::ERROR);
                break;
            default:
                break;
            }
        }
        else if (_appSet.CAMERA_TYPE == CameraType::WEB)
        {
            _webCamL = new cv::VideoCapture(_appSet.CAMERA_LEFT_ID);
            _webCamR = new cv::VideoCapture(_appSet.CAMERA_RIGHT_ID);
            // TODO VA (23-05-2024): Проверить что FPS выставляется
            _webCamL->set(cv::CAP_PROP_FPS, _appSet.CAMERA_FPS);
            _webCamR->set(cv::CAP_PROP_FPS, _appSet.CAMERA_FPS);
        }

        // Запускаем таймер
        if (!_videoTimer->isActive())
            _videoTimer->start(_appSet.VIDEO_TIMER_INTERVAL);

        break;
    case CameraConnection::OFF:

        if (_appSet.CAMERA_TYPE == CameraType::IP)
        {
            int retCode = MV_SDK_Finalization();
            writeLog("MV_SDK_Finalization(): " + std::to_string(retCode), LOGTYPE::INFO);
            switch (retCode)
            {
            case 10:
                qDebug() <<  "ERROR: Left Camera - Stop Grabbing fail!";
                writeLog("setupCameraConnection(): Left Camera - Stop Grabbing fail!", LOGTYPE::ERROR);
                break;
            case 11:
                qDebug() <<  "ERROR: Left Camera - CloseDevice fail!";
                writeLog("setupCameraConnection(): Left Camera - CloseDevice fail!", LOGTYPE::ERROR);
                break;
            case 12:
                qDebug() <<  "ERROR: Left Camera - Destroy Handle fail!";
                writeLog("setupCameraConnection(): Left Camera - Destroy Handle fail!", LOGTYPE::ERROR);
                break;

            case 20:
                qDebug() <<  "ERROR: Right Camera - Stop Grabbing fail!";
                writeLog("setupCameraConnection(): Right Camera - Stop Grabbing fail!", LOGTYPE::ERROR);
                break;
            case 21:
                qDebug() <<  "ERROR: Right Camera - CloseDevice fail!";
                writeLog("setupCameraConnection(): Right Camera - CloseDevice fail!", LOGTYPE::ERROR);
                break;
            case 22:
                qDebug() <<  "ERROR: Right Camera - Destroy Handle fail!";
                writeLog("setupCameraConnection(): Right Camera - Destroy Handle fail!", LOGTYPE::ERROR);
                break;
            default:
                break;
            }
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

void recordVideo(std::vector<cv::Mat> frames, int recordInterval, cv::Size cameraResolution)
{
    writeLog("recordVideo() function call detected: ", LOGTYPE::DEBUG);

    cv::VideoWriter videoWriter;
    int fourccCode = cv::VideoWriter::fourcc('X', 'V', 'I', 'D');
    std::string fileExtension = ".avi";
    // Генерируем имя файла с привязкой к текущему времени
    std::string fileName = generateFileName("сhersonesos", fileExtension);
    int realFPS = (int)(frames.size() / recordInterval);
    videoWriter = cv::VideoWriter(fileName, fourccCode, realFPS , cameraResolution);

    writeLog("fileName: " + fileName, LOGTYPE::DEBUG);
    writeLog("realFPS: " + std::to_string(realFPS), LOGTYPE::DEBUG);
    writeLog("frames.size(): " + std::to_string(frames.size()), LOGTYPE::DEBUG);
    writeLog("recordInterval: " + std::to_string(recordInterval), LOGTYPE::DEBUG);
    writeLog("cameraResolution.width: " + std::to_string(cameraResolution.width), LOGTYPE::DEBUG);
    writeLog("cameraResolution.height: " + std::to_string(cameraResolution.height), LOGTYPE::DEBUG);
    writeLog("fourccCode: " + std::to_string(fourccCode), LOGTYPE::DEBUG);

    try
    {
        if (videoWriter.isOpened())
        {
            writeLog("Saving video started", LOGTYPE::DEBUG);
            for(auto& frame : frames)
            {
                videoWriter.write(frame);
            }
            // Освобождение объекта записи видеопотока
            videoWriter.release();
            writeLog("Saving video finished", LOGTYPE::DEBUG);
        }
        else
             writeLog("cv::VideoWriter NOT opened!", LOGTYPE::ERROR);
    }
    catch (...)
    {
        writeLog("EXCEPTION", LOGTYPE::EXCEPTION);
    }

    writeLog("==================================================", LOGTYPE::DEBUG);
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

    // VA (23-05-2024) Не работает...
    // double fps;
    // fps = _webCamO->get(cv::CAP_PROP_FPS);
    //Q_EMIT updateCntValue("CNT: " + QString::number(_cnt++));
    MV_FRAME_OUT stOutFrame = {0};

    switch (_sevROV.cameraView)
    {
    case CameraView::MONO:

        // Захват изображения в зависимости от типа камеры
        switch (_appSet.CAMERA_TYPE)
        {
        case CameraType::IP:
            nRet = MV_CC_GetImageBuffer(handleL, &stOutFrame, 1000);
            if (nRet == MV_OK)
            {
                // qDebug() << "Mono Camera - Get Image Buffer: Width[" << stOutFrame.stFrameInfo.nWidth << "], Height[" << stOutFrame.stFrameInfo.nHeight << "], FrameNum[" << stOutFrame.stFrameInfo.nFrameNum << "]";
                _sourceMatL = cv::Mat(stOutFrame.stFrameInfo.nHeight, stOutFrame.stFrameInfo.nWidth, CV_8U, stOutFrame.pBufAddr); // TODO: Почему H x W а не W x H ?
                cv::cvtColor(_sourceMatL, _sourceMatL, cv::COLOR_BayerRG2RGB);

                nRet = MV_CC_FreeImageBuffer(handleL, &stOutFrame);

                if(nRet != MV_OK)
                    qDebug() << "ERROR: Mono Camera - Free Image Buffer fail!";
            }
            else
                qDebug() << "ERROR: Mono Camera - Get Image fail!";
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

        // Цикл записи видеопотока в файл
        if ((clock() - timerStart) <= (VIDEO_FRAGMENT_DURATION * CLOCKS_PER_SEC))
        {
            _videoFrame = _sourceMatL.clone();
            if (true)
            {
                // Получаем текущие дату и время
                auto timer = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
                std::tm localTime = *std::localtime(&timer);
                std::ostringstream oss;
                std::string timeMask = "%d-%m-%Y %H:%M:%S";
                oss << std::put_time(&localTime, timeMask.c_str());

                // Вычисляем размер текста
                cv::Size txtSize = cv::getTextSize(oss.str(), cv::FONT_HERSHEY_SIMPLEX, 0.5, 1, 0);

                // Помещаем timestamp на кадр
                cv::putText(_videoFrame,
                            oss.str(),
                            cv::Point((cameraResolution.width - txtSize.width) / 2, cameraResolution.height - txtSize.height - 5),
                            cv::FONT_HERSHEY_SIMPLEX,
                            0.5,
                            cv::Scalar(255, 255, 255),
                            1,
                            cv::LINE_AA);
            }
            frames.push_back(_videoFrame.clone()); // Запоминаем фрейм
        }
        else
        {
            // Запускаем поток записи
            std::thread videoSaverThread(recordVideo, frames, VIDEO_FRAGMENT_DURATION, cameraResolution);
            // videoSaverThread.join(); // Будет пауза при сохранении
            videoSaverThread.detach(); // Открепляем поток от основного потока (паузы не будет вообще)

            frames.clear(); // Очищаем буфер фреймов
            timerStart = clock(); // Сбрасываем таймер записи
        }

        cv::resize(_sourceMatL, resizedMatL, cv::Size(_appSet.CAMERA_WIDTH, _appSet.CAMERA_HEIGHT));
        cv::cvtColor(resizedMatL, _destinationMatL, cv::COLOR_BGR2RGB);

#pragma region Draw Graphical Objects
        {
            ///////////////////////////////////////////////////////////////////
            // Отрисовка прицела
            ///////////////////////////////////////////////////////////////////
            //cv::rectangle(_destinationMatL,
            //              cv::Point(X0 - SIGHT_SIZE, Y0 - SIGHT_SIZE),
            //              cv::Point(X0 + SIGHT_SIZE, Y0 + SIGHT_SIZE),
            //              CV_RGB(255, 255, 255), 1, 0);

            _destinationMatL.copyTo(overlayImage);

            // Внешний контур
            roundedRectangle(_destinationMatL,
                             cv::Point(X0 - SIGHT_SIZE, Y0 - SIGHT_SIZE),
                             cv::Point(X0 + SIGHT_SIZE, Y0 + SIGHT_SIZE),
                             CV_RGB(0, 255, 255),
                             2,
                             cv::LINE_8,
                             10);

            // Рисочки внешнего контура
            cv::line(_destinationMatL,
                     cv::Point(X0, Y0 - SIGHT_SIZE),
                     cv::Point(X0, Y0 - SIGHT_SIZE + SIGHT_TICK),
                     CV_RGB(0, 255, 255),
                     1,
                     cv::LINE_8);
            cv::line(_destinationMatL,
                     cv::Point(X0, Y0 + SIGHT_SIZE),
                     cv::Point(X0, Y0 + SIGHT_SIZE - SIGHT_TICK),
                     CV_RGB(0, 255, 255),
                     1,
                     cv::LINE_8);
            cv::line(_destinationMatL,
                     cv::Point(X0 - SIGHT_SIZE, Y0 ),
                     cv::Point(X0 - SIGHT_SIZE + SIGHT_TICK, Y0),
                     CV_RGB(0, 255, 255),
                     1,
                     cv::LINE_8);
            cv::line(_destinationMatL,
                     cv::Point(X0 + SIGHT_SIZE, Y0 ),
                     cv::Point(X0 + SIGHT_SIZE - SIGHT_TICK, Y0),
                     CV_RGB(0, 255, 255),
                     1,
                     cv::LINE_8);

            // Рисочки внутреннего прицела
            cv::line(_destinationMatL,
                     cv::Point(X0 - SIGHT_DELTA, Y0),
                     cv::Point(X0 - SIGHT_DELTA - SIGHT_CROSS, Y0),
                     CV_RGB(255, 255, 255),
                     1,
                     cv::LINE_8);
            cv::line(_destinationMatL,
                     cv::Point(X0 + SIGHT_DELTA, Y0),
                     cv::Point(X0 + SIGHT_DELTA + SIGHT_CROSS, Y0),
                     CV_RGB(255, 255, 255),
                     1,
                     cv::LINE_8);

            cv::line(_destinationMatL,
                     cv::Point(X0, Y0 - SIGHT_DELTA),
                     cv::Point(X0, Y0 - SIGHT_DELTA - SIGHT_CROSS),
                     CV_RGB(255, 255, 255),
                     1,
                     cv::LINE_8);
            cv::line(_destinationMatL,
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
                cv::line(_destinationMatL,
                         cv::Point(XV0, YV0 + GRID_V_DELTA * 10 * (i - 1)),
                         cv::Point(XV0 + GRID_BIG_SIZE, YV0 + GRID_V_DELTA * 10 * (i - 1)),
                         CV_RGB(255, 255, 255),
                         2,
                         cv::LINE_8);

                for (int j = 1; j < 10; j++)
                {
                    cv::line(_destinationMatL,
                             cv::Point(XV0 + GRID_SMALL_SIZE, YV0 + GRID_V_DELTA * 10 * (i - 1) + j * GRID_V_DELTA),
                             cv::Point(XV0 + 2 * GRID_SMALL_SIZE, YV0 + GRID_V_DELTA * 10 * (i - 1) + j * GRID_V_DELTA),
                             CV_RGB(255, 255, 255),
                             1,
                             cv::LINE_8);
                }
            }
            // Завершающая
            cv::line(_destinationMatL,
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
                cv::line(_destinationMatL,
                         cv::Point(X0 + (X0 - XV0) - 30, YV0 + GRID_V_DELTA * 10 * (i - 1)),
                         cv::Point(X0 + (X0 - XV0) + GRID_BIG_SIZE - 30, YV0 + GRID_V_DELTA * 10 * (i - 1)),
                         CV_RGB(255, 255, 255),
                         2,
                         cv::LINE_8);

                for (int j = 1; j < 10; j++)
                {
                    cv::line(_destinationMatL,
                             cv::Point(X0 + (X0 - XV0) - 30, YV0 + GRID_V_DELTA * 10 * (i - 1) + j * GRID_V_DELTA),
                             cv::Point(X0 + (X0 - XV0) + GRID_SMALL_SIZE - 30, YV0 + GRID_V_DELTA * 10 * (i - 1) + j * GRID_V_DELTA),
                             CV_RGB(255, 255, 255),
                             1,
                             cv::LINE_8);
                }
            }
            // Завершающая
            cv::line(_destinationMatL,
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
                cv::line(_destinationMatL,
                         cv::Point(XH0 + GRID_H_DELTA * 10 * (i - 1), YH0),
                         cv::Point(XH0 + GRID_H_DELTA * 10 * (i - 1), YH0 + GRID_BIG_SIZE),
                         CV_RGB(255, 255, 255),
                         2,
                         cv::LINE_8);

                for (int j = 1; j < 10; j++)
                {
                    cv::line(_destinationMatL,
                             cv::Point(XH0 + GRID_H_DELTA * 10 * (i - 1) + j * GRID_H_DELTA, YH0 + GRID_SMALL_SIZE),
                             cv::Point(XH0 + GRID_H_DELTA * 10 * (i - 1) + j * GRID_H_DELTA , YH0 + 2 * GRID_SMALL_SIZE),
                             CV_RGB(255, 255, 255),
                             1,
                             cv::LINE_8);
                }
            }
            // Завершающая
            cv::line(_destinationMatL,
                     cv::Point(XH0 + GRID_H_DELTA * 10 * (GRID_H_MAX - 1), YH0),
                     cv::Point(XH0 + GRID_H_DELTA * 10 * (GRID_H_MAX - 1), YH0 + GRID_BIG_SIZE),
                     CV_RGB(255, 255, 255),
                     2,
                     cv::LINE_8);
            ///////////////////////////////////////////////////////////////////////
            // Табличка
            cv::rectangle(_destinationMatL,
                          cv::Point(XV0, _appSet.CAMERA_HEIGHT - 50),
                          cv::Point(X0 + (X0 - XV0) + GRID_BIG_SIZE - 30, _appSet.CAMERA_HEIGHT - 100),
                          CV_RGB(255, 255, 255), 2, cv::LINE_8);
            cv::rectangle(_destinationMatL,
                          cv::Point(XV0, _appSet.CAMERA_HEIGHT - 50),
                          cv::Point(X0 + (X0 - XV0) + GRID_BIG_SIZE - 30, _appSet.CAMERA_HEIGHT - 100),
                          CV_RGB(0, 0, 0), -1);

            ///////////////////////////////////////////////////////////////////////
            // Диагностика

            cv::putText(_destinationMatL,
                        "DIAGNOSTIC: " +
                            QTime::currentTime().toString("hh:mm:ss").toStdString(),
                        cv::Point(XV0 + 20, _appSet.CAMERA_HEIGHT - 65),
                        cv::FONT_HERSHEY_SIMPLEX,
                        1,
                        CV_RGB(255, 255, 255),
                        2);

            ///////////////////////////////////////////////////////////////////////
            // Левая информацияонная панель (CONTROL)
            cv::rectangle(_destinationMatL,
                          cv::Point(JSX0, JSY0),
                          cv::Point(JSX0 + JSWIDTH, JSY0 + JSTEXTDELTA + 12*13 + 10),
                          CV_RGB(255, 255, 255), 2, cv::LINE_8);
            cv::rectangle(_destinationMatL,
                          cv::Point(JSX0, JSY0),
                          cv::Point(JSX0 + JSWIDTH, JSY0 + JSTEXTDELTA + 12*13 + 10),
                          CV_RGB(0, 0, 0), -1);

            cv::line(_destinationMatL,
                     cv::Point(JSX0 + 5, JSY0 + 10),
                     cv::Point(JSX0 + JSWIDTH - 5, JSY0 + 10),
                     CV_RGB(255, 255, 255),
                     1,
                     cv::LINE_8);

            cv::line(_destinationMatL,
                     cv::Point(JSX0 + 5, JSY0 + 27),
                     cv::Point(JSX0 + JSWIDTH - 5, JSY0 + 27),
                     CV_RGB(255, 255, 255),
                     1,
                     cv::LINE_8);

            // Текстовка
            cv::putText(_destinationMatL,
                        "CONTROL",
                        cv::Point(JSX0 + 10, JSY0 + 24),
                        cv::FONT_HERSHEY_PLAIN,
                        1,
                        CV_RGB(255, 255, 255),
                        1);

            // HorizontalVectorX
            cv::putText(_destinationMatL,
                        "HRZV.X: " + QString::number(_dataControl.HorizontalVectorX, 'f', 2).toStdString(),
                        cv::Point(JSX0 + 10, JSY0 + JSTEXTDELTA + 12*1),
                        cv::FONT_HERSHEY_PLAIN,
                        1,
                        CV_RGB(255, 255, 255),
                        1);
            // HorizontalVectorY
            cv::putText(_destinationMatL,
                        "HRZV.Y: " + QString::number(_dataControl.HorizontalVectorY, 'f', 2).toStdString(),
                        cv::Point(JSX0 + 10, JSY0 + JSTEXTDELTA + 12*2),
                        cv::FONT_HERSHEY_PLAIN,
                        1,
                        CV_RGB(255, 255, 255),
                        1);
            // VericalThrust
            cv::putText(_destinationMatL,
                        "VERT.TH: " + QString::number(_dataControl.VericalThrust, 'f', 2).toStdString(),
                        cv::Point(JSX0 + 10, JSY0 + JSTEXTDELTA + 12*3),
                        cv::FONT_HERSHEY_PLAIN,
                        1,
                        CV_RGB(255, 255, 255),
                        1);
            // AngularVelocityZ
            cv::putText(_destinationMatL,
                        "ANGV.Z: " + QString::number(_dataControl.AngularVelocityZ, 'f', 2).toStdString(),
                        cv::Point(JSX0 + 10, JSY0 + JSTEXTDELTA + 12*4),
                        cv::FONT_HERSHEY_PLAIN,
                        1,
                        CV_RGB(255, 255, 255),
                        1);
            // PowerTarget
            cv::putText(_destinationMatL,
                        "PWR.TRG: " + QString::number(_dataControl.PowerTarget, 'f', 2).toStdString(),
                        cv::Point(JSX0 + 10, JSY0 + JSTEXTDELTA + 12*5),
                        cv::FONT_HERSHEY_PLAIN,
                        1,
                        CV_RGB(255, 255, 255),
                        1);
            // ManipulatorState
            cv::putText(_destinationMatL,
                        "MNP.ST: " + QString::number(_dataControl.ManipulatorState).toStdString(),
                        cv::Point(JSX0 + 10, JSY0 + JSTEXTDELTA + 12*6),
                        cv::FONT_HERSHEY_PLAIN,
                        1,
                        CV_RGB(255, 255, 255),
                        1);
            // ManipulatorRotate
            cv::putText(_destinationMatL,
                        "MNP.ROT: " + QString::number(_dataControl.ManipulatorRotate, 'f', 2).toStdString(),
                        cv::Point(JSX0 + 10, JSY0 + JSTEXTDELTA + 12*7),
                        cv::FONT_HERSHEY_PLAIN,
                        1,
                        CV_RGB(255, 255, 255),
                        1);
            // CameraRotate
            cv::putText(_destinationMatL,
                        "CAM.ROT: " + QString::number(_dataControl.CameraRotate).toStdString(),
                        cv::Point(JSX0 + 10, JSY0 + JSTEXTDELTA + 12*8),
                        cv::FONT_HERSHEY_PLAIN,
                        1,
                        CV_RGB(255, 255, 255),
                        1);
            // ResetInitialization
            cv::putText(_destinationMatL,
                        "RESET.INI: " + QString::number(_dataControl.ResetInitialization).toStdString(),
                        cv::Point(JSX0 + 10, JSY0 + JSTEXTDELTA + 12*9),
                        cv::FONT_HERSHEY_PLAIN,
                        1,
                        CV_RGB(255, 255, 255),
                        1);
            // LightsState
            cv::putText(_destinationMatL,
                        "LIGHT: " + QString::number(_dataControl.LightsState).toStdString(),
                        cv::Point(JSX0 + 10, JSY0 + JSTEXTDELTA + 12*10),
                        cv::FONT_HERSHEY_PLAIN,
                        1,
                        CV_RGB(255, 255, 255),
                        1);
            // RollInc
            cv::putText(_destinationMatL,
                        "ROLL.INC: " + QString::number(_dataControl.RollInc).toStdString(),
                        cv::Point(JSX0 + 10, JSY0 + JSTEXTDELTA + 12*11),
                        cv::FONT_HERSHEY_PLAIN,
                        1,
                        CV_RGB(255, 255, 255),
                        1);
            // PitchInc
            cv::putText(_destinationMatL,
                        "PITCH.INC: " + QString::number(_dataControl.PitchInc).toStdString(),
                        cv::Point(JSX0 + 10, JSY0 + JSTEXTDELTA + 12*12),
                        cv::FONT_HERSHEY_PLAIN,
                        1,
                        CV_RGB(255, 255, 255),
                        1);
            // ResetPosition
            cv::putText(_destinationMatL,
                        "RESET.POS: " + QString::number(_dataControl.ResetPosition).toStdString(),
                        cv::Point(JSX0 + 10, JSY0 + JSTEXTDELTA + 12*13),
                        cv::FONT_HERSHEY_PLAIN,
                        1,
                        CV_RGB(255, 255, 255),
                        1);

            /*
        cv::putText(_destinationMatL,
                    "JOYSTICK INFO",
                    cv::Point(JSX0 + 10, JSY0 + 24),
                    cv::FONT_HERSHEY_PLAIN,
                    1,
                    CV_RGB(255, 255, 255),
                    1);

        cv::putText(_destinationMatL,
                    "LStickX: " + QString::number(_xbox.LStickX).toStdString(),
                    cv::Point(JSX0 + 10, JSY0 + JSTEXTDELTA + 12*1),
                    cv::FONT_HERSHEY_PLAIN,
                    1,
                    CV_RGB(255, 255, 255),
                    1);
        cv::putText(_destinationMatL,
                    "LStickY: " + QString::number(_xbox.LStickY).toStdString(),
                    cv::Point(JSX0 + 10, JSY0 + JSTEXTDELTA + 12*2),
                    cv::FONT_HERSHEY_PLAIN,
                    1,
                    CV_RGB(255, 255, 255),
                    1);
        cv::putText(_destinationMatL,
                    "RStickX: " + QString::number(_xbox.RStickX).toStdString(),
                    cv::Point(JSX0 + 10, JSY0 + JSTEXTDELTA + 12*3),
                    cv::FONT_HERSHEY_PLAIN,
                    1,
                    CV_RGB(255, 255, 255),
                    1);
        cv::putText(_destinationMatL,
                    "RStickY: " + QString::number(_xbox.RStickY).toStdString(),
                    cv::Point(JSX0 + 10, JSY0 + JSTEXTDELTA + 12*4),
                    cv::FONT_HERSHEY_PLAIN,
                    1,
                    CV_RGB(255, 255, 255),
                    1);
        cv::putText(_destinationMatL,
                    "LTrigger: " + QString::number(_xbox.LTrigger).toStdString(),
                    cv::Point(JSX0 + 10, JSY0 + JSTEXTDELTA + 12*5),
                    cv::FONT_HERSHEY_PLAIN,
                    1,
                    CV_RGB(255, 255, 255),
                    1);
        cv::putText(_destinationMatL,
                    "RTrigger: " + QString::number(_xbox.RTrigger).toStdString(),
                    cv::Point(JSX0 + 10, JSY0 + JSTEXTDELTA + 12*6),
                    cv::FONT_HERSHEY_PLAIN,
                    1,
                    CV_RGB(255, 255, 255),
                    1);

        cv::putText(_destinationMatL,
                    "A: " + QString::number(_xbox.A).toStdString(),
                    cv::Point(JSX0 + 10, JSY0 + JSTEXTDELTA + 12*7),
                    cv::FONT_HERSHEY_PLAIN,
                    1,
                    CV_RGB(255, 255, 255),
                    1);
        cv::putText(_destinationMatL,
                    "B: " + QString::number(_xbox.B).toStdString(),
                    cv::Point(JSX0 + 10, JSY0 + JSTEXTDELTA + 12*8),
                    cv::FONT_HERSHEY_PLAIN,
                    1,
                    CV_RGB(255, 255, 255),
                    1);
        cv::putText(_destinationMatL,
                    "X: " + QString::number(_xbox.X).toStdString(),
                    cv::Point(JSX0 + 10, JSY0 + JSTEXTDELTA + 12*9),
                    cv::FONT_HERSHEY_PLAIN,
                    1,
                    CV_RGB(255, 255, 255),
                    1);
        cv::putText(_destinationMatL,
                    "Y: " + QString::number(_xbox.Y).toStdString(),
                    cv::Point(JSX0 + 10, JSY0 + JSTEXTDELTA + 12*10),
                    cv::FONT_HERSHEY_PLAIN,
                    1,
                    CV_RGB(255, 255, 255),
                    1);
        cv::putText(_destinationMatL,
                    "LBumper: " + QString::number(_xbox.LBumper).toStdString(),
                    cv::Point(JSX0 + 10, JSY0 + JSTEXTDELTA + 12*11),
                    cv::FONT_HERSHEY_PLAIN,
                    1,
                    CV_RGB(255, 255, 255),
                    1);
        cv::putText(_destinationMatL,
                    "RBumper: " + QString::number(_xbox.RBumper).toStdString(),
                    cv::Point(JSX0 + 10, JSY0 + JSTEXTDELTA + 12*12),
                    cv::FONT_HERSHEY_PLAIN,
                    1,
                    CV_RGB(255, 255, 255),
                    1);
        cv::putText(_destinationMatL,
                    "D-Pad: " + QString::number(_xbox.DPad).toStdString(),
                    cv::Point(JSX0 + 10, JSY0 + JSTEXTDELTA + 12*13),
                    cv::FONT_HERSHEY_PLAIN,
                    1,
                    CV_RGB(255, 255, 255),
                    1);

        */

            ////////////////////////////////////////////////////////////////////
            // Телеметрия

            TMX0 = X0 + (X0 - XV0) - 30;
            cv::rectangle(_destinationMatL,
                          cv::Point(TMX0, TMY0),
                          cv::Point(TMX0 + TMWIDTH, TMY0 + TMTEXTDELTA + 12*13 + 10),
                          CV_RGB(255, 255, 255), 2, cv::LINE_8);
            cv::rectangle(_destinationMatL,
                          cv::Point(TMX0, TMY0),
                          cv::Point(TMX0 + TMWIDTH, TMY0 + TMTEXTDELTA + 12*13 + 10),
                          CV_RGB(0, 0, 0), -1);

            cv::line(_destinationMatL,
                     cv::Point(TMX0 + 5, TMY0 + 10),
                     cv::Point(TMX0 + TMWIDTH - 5, TMY0 + 10),
                     CV_RGB(255, 255, 255),
                     1,
                     cv::LINE_8);
            cv::line(_destinationMatL,
                     cv::Point(TMX0 + 5, TMY0 + 27),
                     cv::Point(TMX0 + TMWIDTH - 5, TMY0 + 27),
                     CV_RGB(255, 255, 255),
                     1,
                     cv::LINE_8);

            cv::putText(_destinationMatL,
                        "TELEMETRY",
                        cv::Point(TMX0 + 10, TMY0 + 24),
                        cv::FONT_HERSHEY_PLAIN,
                        1,
                        CV_RGB(255, 255, 255),
                        1);

            cv::putText(_destinationMatL,
                        "Roll: " + QString::number(_dataTelemetry.Roll, 'f', 2).toStdString(),
                        cv::Point(TMX0 + 10, TMY0 + TMTEXTDELTA + 12*1),
                        cv::FONT_HERSHEY_PLAIN,
                        1,
                        CV_RGB(255, 255, 255),
                        1);
            cv::putText(_destinationMatL,
                        "Pitch: " + QString::number(_dataTelemetry.Pitch, 'f', 2).toStdString(),
                        cv::Point(TMX0 + 10, TMY0 + TMTEXTDELTA + 12*2),
                        cv::FONT_HERSHEY_PLAIN,
                        1,
                        CV_RGB(255, 255, 255),
                        1);
            cv::putText(_destinationMatL,
                        "Yaw: " + QString::number(_dataTelemetry.Yaw, 'f', 2).toStdString(),
                        cv::Point(TMX0 + 10, TMY0 + TMTEXTDELTA + 12*3),
                        cv::FONT_HERSHEY_PLAIN,
                        1,
                        CV_RGB(255, 255, 255),
                        1);
            cv::putText(_destinationMatL,
                        "Heading: " + QString::number(_dataTelemetry.Heading, 'f', 2).toStdString(),
                        cv::Point(TMX0 + 10, TMY0 + TMTEXTDELTA + 12*4),
                        cv::FONT_HERSHEY_PLAIN,
                        1,
                        CV_RGB(255, 255, 255),
                        1);
            cv::putText(_destinationMatL,
                        "Depth: " + QString::number(_dataTelemetry.Depth, 'f', 2).toStdString(),
                        cv::Point(TMX0 + 10, TMY0 + TMTEXTDELTA + 12*5),
                        cv::FONT_HERSHEY_PLAIN,
                        1,
                        CV_RGB(255, 255, 255),
                        1);

            ///////////////////////////////////////////////////////////////////////
            // Склейка
            cv::addWeighted(overlayImage, alpha, _destinationMatL, 1 - alpha, 0, transparencyiImage);
            ///////////////////////////////////////////////////////////////////////

            _imgCamL = QImage((uchar*) transparencyiImage.data,
                              transparencyiImage.cols,
                              transparencyiImage.rows,
                              transparencyiImage.step,
                              QImage::Format_RGB888);


            //_imgCamO = QImage((uchar*) _destinationMatL.data,
            //                  _destinationMatL.cols,
            //                  _destinationMatL.rows,
            //                  _destinationMatL.step,
            //                  QImage::Format_RGB888);

            ui->lbCamera->setPixmap(QPixmap::fromImage(_imgCamL));
        }
#pragma endregion

        break;
    case CameraView::STEREO:
        switch (_appSet.CAMERA_TYPE)
        {
        case CameraType::IP:
            ///////////////////////////////////////////////////////////////////
            // Left Camera
            ///////////////////////////////////////////////////////////////////
            nRet = MV_CC_GetImageBuffer(handleL, &stOutFrame, 1000);
            if (nRet == MV_OK)
            {
                // qDebug() << "Left Camera - Get Image Buffer: Width[" << stOutFrame.stFrameInfo.nWidth << "], Height[" << stOutFrame.stFrameInfo.nHeight << "], FrameNum[" << stOutFrame.stFrameInfo.nFrameNum << "]";
                _sourceMatL = cv::Mat(stOutFrame.stFrameInfo.nHeight, stOutFrame.stFrameInfo.nWidth, CV_8U, stOutFrame.pBufAddr); // TODO: Почему H x W а не W x H ?
                cv::cvtColor(_sourceMatL, _sourceMatL, cv::COLOR_BayerRG2RGB);

                nRet = MV_CC_FreeImageBuffer(handleL, &stOutFrame);

                if(nRet != MV_OK)
                    qDebug() << "ERROR: Left Camera - Free Image Buffer fail!";
            }
            else
                qDebug() << "ERROR: Left Camera - Get Image fail!";
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
            nRet = MV_CC_GetImageBuffer(handleR, &stOutFrame, 1000);
            if (nRet == MV_OK)
            {
                // qDebug() << "Right Camera - Get Image Buffer: Width[" << stOutFrame.stFrameInfo.nWidth << "], Height[" << stOutFrame.stFrameInfo.nHeight << "], FrameNum[" << stOutFrame.stFrameInfo.nFrameNum << "]";
                _sourceMatR = cv::Mat(stOutFrame.stFrameInfo.nHeight, stOutFrame.stFrameInfo.nWidth, CV_8U, stOutFrame.pBufAddr); // TODO: Почему H x W а не W x H ?
                cv::cvtColor(_sourceMatR, _sourceMatR, cv::COLOR_BayerRG2RGB);

                nRet = MV_CC_FreeImageBuffer(handleR, &stOutFrame);

                if(nRet != MV_OK)
                    qDebug() << "ERROR: Right Camera - Free Image Buffer fail!";
            }
            else
                qDebug() << "ERROR: Right Camera - Get Image fail!";
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

void MainWindow::videoRecorderInitialization()
{
    if (frames.size() > 0)
    {
        frames.clear(); // Сброс буфера кадров
        frames.shrink_to_fit(); // Requests the container to reduce its capacity to fit its size
    }

    cameraResolution.height = _appSet.CAMERA_HEIGHT;
    cameraResolution.width = _appSet.CAMERA_WIDTH;
    cameraFPS = _appSet.CAMERA_FPS;
    int bufferSize = cameraFPS * VIDEO_FRAGMENT_DURATION; // VIDEO_FRAGMENT_DURATION = 30 секунд - интервал записи одного ролика
    frames.reserve((size_t)bufferSize); // Резервируем длину вектора под хранение фреймов
    timerStart = clock(); // Запоминаем время начала записи
}

void MainWindow::onStartStopButtonClicked()
{
    // Логируем настройки приложения
    writeLog("onStartStopButtonClicked()", LOGTYPE::INFO);
    writeLog("SETTINGS ===>", LOGTYPE::INFO);
    writeLog("Application Version: " + _appSet.getAppVersion().toStdString(), LOGTYPE::INFO);
    writeLog("CAMERA_FPS: " + std::to_string(_appSet.CAMERA_FPS), LOGTYPE::INFO);
    writeLog("CAMERA_WIDTH: " + std::to_string(_appSet.CAMERA_WIDTH), LOGTYPE::INFO);
    writeLog("CAMERA_HEIGHT: " + std::to_string(_appSet.CAMERA_HEIGHT), LOGTYPE::INFO);
    writeLog("CAMERA_FPS: " + std::to_string(_appSet.CAMERA_FPS), LOGTYPE::INFO);
    writeLog("CAMERA_LEFT_ID: " + std::to_string(_appSet.CAMERA_LEFT_ID), LOGTYPE::INFO);
    writeLog("CAMERA_RIGHT_ID: " + std::to_string(_appSet.CAMERA_RIGHT_ID), LOGTYPE::INFO);
    writeLog("CAMERA_TYPE: " + std::to_string(_appSet.CAMERA_TYPE), LOGTYPE::INFO);
    writeLog("VIDEO_TIMER_INTERVAL: " + std::to_string(_appSet.VIDEO_TIMER_INTERVAL), LOGTYPE::INFO);
    writeLog("==================================================", LOGTYPE::INFO);

    // Меняем состояние флага
    _sevROV.isConnected = !_sevROV.isConnected;
    // _cnt = 0; // Сбрасываем счетчик
    // Q_EMIT updateCntValue("CNT: " + QString::number(_cnt++));

    // Меняем иконку на кнопке
    if (_sevROV.isConnected) // Connection ON
    {
        ui->pbStartStop->setIcon(QIcon(":/img/on_button_icon.png"));
        ui->pbStartStop->setIconSize(QSize(64, 64));

        writeLog("setupCameraConnection(CameraConnection::ON)", LOGTYPE::INFO);
        setupCameraConnection(CameraConnection::ON);

        // Joyjstick
        _jsController->OpenJoystick(_appSet.JOYSTICK_ID);
        _jsController->isRunning = true;
        _jsController->start(); // Запуск процесса в поток

        _controlTimer->start(_appSet.JOYSTICK_TIMER_INTERVAL);

        videoRecorderInitialization();
    }
    else // Connection OFF
    {
        ui->pbStartStop->setIcon(QIcon(":/img/off_button_icon.png"));
        ui->pbStartStop->setIconSize(QSize(64, 64));

        writeLog("setupCameraConnection(CameraConnection::OFF)", LOGTYPE::INFO);
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

    cv::Mat imageL;
    cv::Mat imageR;

    int nRet = MV_OK;
    MV_FRAME_OUT stOutFrame = {0};

    switch (_appSet.CAMERA_TYPE)
    {
    case CameraType::IP:
        nRet = MV_CC_GetImageBuffer(handleL, &stOutFrame, 1000);
        if (nRet == MV_OK)
        {
            // qDebug() << "Left Camera - Get Image Buffer: Width[" << stOutFrame.stFrameInfo.nWidth << "], Height[" << stOutFrame.stFrameInfo.nHeight << "], FrameNum[" << stOutFrame.stFrameInfo.nFrameNum << "]";
            imageL = cv::Mat(stOutFrame.stFrameInfo.nHeight, stOutFrame.stFrameInfo.nWidth, CV_8U, stOutFrame.pBufAddr); // TODO: Почему H x W а не W x H ?
            cv::cvtColor(imageL, imageL, cv::COLOR_BayerRG2RGB);

            nRet = MV_CC_FreeImageBuffer(handleL, &stOutFrame);

            if(nRet != MV_OK)
                qDebug() << "ERROR: Left Camera - Free Image Buffer fail!";
        }
        else
            qDebug() << "ERROR: Left Camera - Get Image fail!";

        ///////////////////////////////////////////////////////////////////////
        // Right Camera
        ///////////////////////////////////////////////////////////////////////
        nRet = MV_CC_GetImageBuffer(handleR, &stOutFrame, 1000);
        if (nRet == MV_OK)
        {
            // qDebug() << "Right Camera - Get Image Buffer: Width[" << stOutFrame.stFrameInfo.nWidth << "], Height[" << stOutFrame.stFrameInfo.nHeight << "], FrameNum[" << stOutFrame.stFrameInfo.nFrameNum << "]";
            imageR = cv::Mat(stOutFrame.stFrameInfo.nHeight, stOutFrame.stFrameInfo.nWidth, CV_8U, stOutFrame.pBufAddr); // TODO: Почему H x W а не W x H ?
            cv::cvtColor(imageR, imageR, cv::COLOR_BayerRG2RGB);

            nRet = MV_CC_FreeImageBuffer(handleR, &stOutFrame);

            if(nRet != MV_OK)
                qDebug() << "ERROR: Right Camera - Free Image Buffer fail!";
        }
        else
            qDebug() << "ERROR: Right Camera - Get Image fail!";
        ///////////////////////////////////////////////////////////////////////
        break;
    case CameraType::WEB:
        _webCamL->read(imageL);
        _webCamR->read(imageR);
        break;
    default:
        break;
    }

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
    _toolWindow->setDataCloud3D(imageL, data);
    _toolWindow->setWindowTitle("ТНПА :: AРМ Оператора :: " + _appSet.getAppVersion());

    // Центрировать инструментальную панель
    QRect screenGeometry = QGuiApplication::screens()[0]->geometry();
    int x = (screenGeometry.width() - _toolWindow->width()) / 2;
    int y = (screenGeometry.height() - _toolWindow->height()) / 2;

    _toolWindow->move(x, y);
    _toolWindow->exec();

    ///////////////////////////////////////////////////////////////////////////
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
