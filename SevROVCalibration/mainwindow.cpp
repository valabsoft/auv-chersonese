#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    _appSet.load();

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

    qDebug() << "_appSet.CAMERA_WIDTH: " << _appSet.CAMERA_WIDTH;
    qDebug() << "_appSet.CAMERA_HEIGHT: " << _appSet.CAMERA_HEIGHT;

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

    // Панель счетчика (привязка к позиции окна камеры)
    ui->lbCounter->setGeometry(
        _appSet.CAMERA_VIEW_BORDER_WIDTH,
        ui->lbCameraL->y() + ui->lbCameraL->height() + _appSet.CAMERA_VIEW_BORDER_WIDTH,
        mainWindowRect.width() - _appSet.CAMERA_VIEW_BORDER_WIDTH * 2,
        70);
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

        // Формируем имя калибровочного файла
        std::string calibrationFileName = "";
        switch (_appSet.CALIBRATION_FILE_TYPE) {
        case CalibrationFileType::XML:
            calibrationFileName = _appSet.CALIBRATION_FILE_NAME + ".xml";
            break;
        case CalibrationFileType::YML:
            calibrationFileName = _appSet.CALIBRATION_FILE_NAME + ".yml";
            break;
        default:
            calibrationFileName = _appSet.CALIBRATION_FILE_NAME + ".xml";
            break;
        }

        std::filesystem::path calibrationFile(calibrationFileName);
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

    int nRet = MV_OK;
    MV_FRAME_OUT stOutFrame = {};

    switch (_appSet.CAMERA_TYPE)
    {
    case CameraType::IP:
        ///////////////////////////////////////////////////////////////////
        // Left Camera
        ///////////////////////////////////////////////////////////////////
        nRet = MV_CC_GetImageBuffer(handleL, &stOutFrame, 1000);
        if (nRet == MV_OK)
        {
            _sourceMatL = cv::Mat(stOutFrame.stFrameInfo.nHeight, stOutFrame.stFrameInfo.nWidth, CV_8U, stOutFrame.pBufAddr); // TODO: Почему H x W а не W x H ?
            cv::cvtColor(_sourceMatL, _sourceMatL, cv::COLOR_BayerRG2RGB);

            nRet = MV_CC_FreeImageBuffer(handleL, &stOutFrame);

        //    if(nRet != MV_OK)
        //        qDebug() << "ERROR: Left Camera - Free Image Buffer fail!";
        }
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
        nRet = MV_CC_GetImageBuffer(handleR, &stOutFrame, 1000);
        if (nRet == MV_OK)
        {
            _sourceMatR = cv::Mat(stOutFrame.stFrameInfo.nHeight, stOutFrame.stFrameInfo.nWidth, CV_8U, stOutFrame.pBufAddr); // TODO: Почему H x W а не W x H ?
            cv::cvtColor(_sourceMatR, _sourceMatR, cv::COLOR_BayerRG2RGB);

            nRet = MV_CC_FreeImageBuffer(handleR, &stOutFrame);

        //    if(nRet != MV_OK)
        //        qDebug() << "ERROR: Right Camera - Free Image Buffer fail!";
        }
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
        }

        // Запускаем таймер
        if (!_timer->isActive())
            _timer->start(_appSet.VIDEO_TIMER_INTERVAL);

        break;
    case ProcessStatus::OFF:

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

    MVCC_ENUMVALUE stEnumValue = {};
    MVCC_ENUMENTRY stEnumEntry = {};

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
