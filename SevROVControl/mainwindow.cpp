#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QScreen>
#include <QDir>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setWindowIcon(QIcon(":/logo/img/sevrov.png"));

    // Путь к файлу настроек
    settingsFileName = QApplication::applicationDirPath() +
                       QDir::separator() + "settings.ini";

    // Фиксируем размер окна и убираем иконку ресайза
    setFixedSize(QSize(1015, 690));

    // statusBar()->setSizeGripEnabled(false);
    // Центрируем окно в пределах экрана
    move(screen()->geometry().center() - frameGeometry().center());
    setWindowTitle("ТНПА :: Контроль :: " + QString(APP_VERSION.c_str()));

    ui->edAUVConnection->setStyleSheet("* { background-color: rgba(0, 0, 0, 0); }");
    ui->lblCamera->setStyleSheet("QLabel {"
                                 "border-style: solid;"
                                 "border-width: 1px;"
                                 "border-color: silver; "
                                 "}");

    // Создаем объект джойстик-контроллера и получаем список доступных джойстиков
    jsController = new SevROVXboxController();
    QList<QString> jsList = jsController->GetJoystickList();
    for (int i = 0; i < jsList.count(); i++)
        ui->cbJoystickList->addItem(jsList[i]);

    ui->btnJoystick->setEnabled(jsList.count() > 0);

    // Кнопки
    connect(jsController, &SevROVXboxController::OnButtonA,
            this, &MainWindow::OnButtonA);
    connect(jsController, &SevROVXboxController::OnButtonB,
            this, &MainWindow::OnButtonB);
    connect(jsController, &SevROVXboxController::OnButtonX,
            this, &MainWindow::OnButtonX);
    connect(jsController, &SevROVXboxController::OnButtonY,
            this, &MainWindow::OnButtonY);
    connect(jsController, &SevROVXboxController::OnButtonLBumper,
            this, &MainWindow::OnButtonLBumper);
    connect(jsController, &SevROVXboxController::OnButtonRBumper,
            this, &MainWindow::OnButtonRBumper);
    connect(jsController, &SevROVXboxController::OnButtonView,
            this, &MainWindow::OnButtonView);
    connect(jsController, &SevROVXboxController::OnButtonMenu,
            this, &MainWindow::OnButtonMenu);
    connect(jsController, &SevROVXboxController::OnDPad,
            this, &MainWindow::OnDPad);

    // Оси
    connect(jsController, &SevROVXboxController::OnAxisLStickX,
            this, &MainWindow::OnAxisLStickX);
    connect(jsController, &SevROVXboxController::OnAxisLStickY,
            this, &MainWindow::OnAxisLStickY);
    connect(jsController, &SevROVXboxController::OnAxisRStickX,
            this, &MainWindow::OnAxisRStickX);
    connect(jsController, &SevROVXboxController::OnAxisRStickY,
            this, &MainWindow::OnAxisRStickY);
    connect(jsController, &SevROVXboxController::OnAxisLTrigger,
            this, &MainWindow::OnAxisLTrigger);
    connect(jsController, &SevROVXboxController::OnAxisRTrigger,
            this, &MainWindow::OnAxisRTrigger);

    // Начальные значения
    ui->edA->setText(QString::number(0));
    ui->edB->setText(QString::number(0));
    ui->edX->setText(QString::number(0));
    ui->edY->setText(QString::number(0));
    ui->edLBumper->setText(QString::number(0));
    ui->edRBumper->setText(QString::number(0));
    ui->edView->setText(QString::number(0));
    ui->edMenu->setText(QString::number(0));
    ui->edDPad->setText(QString::number(0));
    ui->edLStickX->setText(QString::number(0));
    ui->edLStickY->setText(QString::number(0));
    ui->edRStickX->setText(QString::number(0));
    ui->edRStickY->setText(QString::number(0));
    ui->edLTrigger->setText(QString::number(-32768));
    ui->edRTrigger->setText(QString::number(-32768));

    // Зеркалим данные
    xbox.A = 0;
    xbox.B = 0;
    xbox.X = 0;
    xbox.Y = 0;
    xbox.LBumper = 0;
    xbox.RBumper = 0;
    xbox.View = 0;
    xbox.Menu = 0;
    xbox.DPad = 0;
    xbox.LStickX = 0;
    xbox.LStickY = 0;
    xbox.RStickX = 0;
    xbox.RStickY = 0;
    xbox.LTrigger = -32768;
    xbox.RTrigger = -32768;

    controlTimer = new QTimer(this);
    connect(controlTimer, &QTimer::timeout, this, &MainWindow::OnControlTimer);

    videoTimer = new QTimer(this);
    connect(videoTimer, &QTimer::timeout, this, &MainWindow::OnVideoTimer);

    // Создаем коннектор с AUV. Клиент должен уметь писать управление и читать телеметрию
    rovConnector.setMode(SevROVConnector::Mode::CONTROL_WRITE |
                         SevROVConnector::Mode::TELEMETRY_READ);


    connect(&rovConnector, SIGNAL(OnConnected()), this, SLOT(OnSocketConnect()));
    connect(&rovConnector, SIGNAL(OnDisconnected()), this, SLOT(OnSocketDisconnect()));
    connect(&rovConnector, SIGNAL(OnProcessTelemetryDatagram()), this, SLOT(OnSocketProcessTelemetryDatagram()));

    ui->gbAxis->setEnabled(jsConnected);
    ui->gbButtons->setEnabled(jsConnected);
    ui->gbPID->setEnabled(jsConnected);
    ui->gbControl->setEnabled(jsConnected);

    ui->gbTelemetry->setEnabled(false);
    ui->edAUVConnection->setEnabled(false);

    loadSettings();
}

MainWindow::~MainWindow()
{    
    if (controlTimer->isActive())
        controlTimer->stop();

    if (videoTimer->isActive())
        videoTimer->stop();

    saveSettings();

    delete controlTimer;
    delete videoTimer;

    delete webcam;

    delete jsController;
    delete ui;
}


void MainWindow::on_btnJoystick_clicked()
{
    if (!jsConnected)
    {
        jsController->OpenJoystick(ui->cbJoystickList->currentIndex());

        jsController->isRunning = true;
        jsController->start(); // Запуск процесса в поток
        ui->btnJoystick->setText("ОТКЛЮЧИТЬСЯ ОТ ДЖОЙСТИКА");
        jsConnected = true;

        // IP b Port будут использоваться только для записи датаграммы
        // Мы поймем, что ответ пришел, если поднимется событие OnProcessTelemetryDatagram
        // rovConnector.setIP(ui->edIP->text());
        // rovConnector.setPort(ui->edPort->text().toInt());

        controlTimer->start(100);
    }
    else
    {

        jsController->CloseJoystick();        
        jsController->isRunning = false;
        jsController->quit();
        ui->btnJoystick->setText("ПОДКЛЮЧИТЬСЯ К ДЖОЙСТИКУ");
        jsConnected = false;

        controlTimer->stop();
    }

    ui->gbAxis->setEnabled(jsConnected);
    ui->gbButtons->setEnabled(jsConnected);
    ui->gbPID->setEnabled(jsConnected);
    ui->gbControl->setEnabled(jsConnected);
}

void MainWindow::OnButtonA(short value)
{
    ui->edA->setText(QString::number(value));
    xbox.A = value;
}
void MainWindow::OnButtonB(short value)
{
    ui->edB->setText(QString::number(value));    
    xbox.B = value;
}
void MainWindow::OnButtonX(short value)
{
    ui->edX->setText(QString::number(value));
    xbox.X = value;
}
void MainWindow::OnButtonY(short value)
{
    ui->edY->setText(QString::number(value));
    xbox.Y = value;
}
void MainWindow::OnButtonLBumper(short value)
{
    ui->edLBumper->setText(QString::number(value));
    xbox.LBumper = value;
}
void MainWindow::OnButtonRBumper(short value)
{
    ui->edRBumper->setText(QString::number(value));
    xbox.RBumper = value;
}
void MainWindow::OnButtonView(short value)
{
    ui->edView->setText(QString::number(value));
    xbox.View = value;
}
void MainWindow::OnButtonMenu(short value)
{
    ui->edMenu->setText(QString::number(value));
    xbox.Menu = value;
}
void MainWindow::OnDPad(short value)
{
    ui->edDPad->setText(QString::number(value));
    xbox.DPad = value;
}
void MainWindow::OnAxisLStickX(short value)
{
    ui->edLStickX->setText(QString::number(value));
    xbox.LStickX = value;
}
void MainWindow::OnAxisLStickY(short value)
{
    ui->edLStickY->setText(QString::number(value));
    xbox.LStickY = value;
}
void MainWindow::OnAxisRStickX(short value)
{
    ui->edRStickX->setText(QString::number(value));
    xbox.RStickX = value;
}
void MainWindow::OnAxisRStickY(short value)
{
    ui->edRStickY->setText(QString::number(value));
    xbox.RStickY = value;
}
void MainWindow::OnAxisLTrigger(short value)
{
    ui->edLTrigger->setText(QString::number(value));
    xbox.LTrigger = value;
}
void MainWindow::OnAxisRTrigger(short value)
{
    ui->edRTrigger->setText(QString::number(value));
    xbox.RTrigger = value;
}
void MainWindow::OnControlTimer()
{
    // Получаем текущие настройки регулятора
    float powerLimit = ui->sbPowerLimit->value();
    bool rollStabilization = ui->cbRollStab->isChecked();
    bool pitchStabilization = ui->cbPitchStab->isChecked();
    bool yawStabilization = ui->cbYawStab->isChecked();
    bool depthStabilization = ui->cbDepthStab->isChecked();

    SevROVPIDController rollPID;
    rollPID.setKp(ui->sbRollKp->value());
    rollPID.setKi(ui->sbRollKi->value());
    rollPID.setKd(ui->sbRollKd->value());

    SevROVPIDController pitchPID;
    pitchPID.setKp(ui->sbPitchKp->value());
    pitchPID.setKi(ui->sbPitchKi->value());
    pitchPID.setKd(ui->sbPitchKd->value());

    SevROVPIDController yawPID;
    yawPID.setKp(ui->sbYawKp->value());
    yawPID.setKi(ui->sbYawKi->value());
    yawPID.setKd(ui->sbYawKd->value());

    SevROVPIDController depthPID;
    depthPID.setKp(ui->sbDepthKp->value());
    depthPID.setKi(ui->sbDepthKi->value());
    depthPID.setKd(ui->sbDepthKd->value());

    // Пересчитываем состояние джойстика в управление
    SevROVLibrary::XboxToControlData(xbox,
                                     powerLimit,
                                     rollStabilization,
                                     pitchStabilization,
                                     yawStabilization,
                                     depthStabilization,
                                     rollPID,
                                     pitchPID,
                                     yawPID,
                                     depthPID,
                                     updatePID,
                                     &rovConnector.control);

    // Отображаем на панели сигналы контроля
    ui->edHorizontalVectorX->setText(QString::number(rovConnector.control.getHorizontalVectorX(), 'f', 2));
    ui->edHorizontalVectorY->setText(QString::number(rovConnector.control.getHorizontalVectorY(), 'f', 2));
    ui->edVericalThrust->setText(QString::number(rovConnector.control.getVericalThrust(), 'f', 2));
    ui->edPowerTarget->setText(QString::number(rovConnector.control.getPowerTarget(), 'f', 2));
    ui->edAngularVelocityZ->setText(QString::number(rovConnector.control.getAngularVelocityZ(), 'f', 2));

    ui->edManipulatorState->setText(QString::number(rovConnector.control.getManipulatorState()));
    ui->edManipulatorRotate->setText(QString::number(rovConnector.control.getManipulatorRotate(), 'f', 2));
    ui->edCameraRotate->setText(QString::number(rovConnector.control.getCameraRotate()));

    ui->edResetInitialization->setText(QString::number(rovConnector.control.getResetInitialization()));
    ui->edLightsState->setText(QString::number(rovConnector.control.getLightsState()));
    ui->edStabilizationState->setText(QString::number(rovConnector.control.getStabilizationState()));

    ui->edRollInc->setText(QString::number(rovConnector.control.getRollInc()));
    ui->edPitchInc->setText(QString::number(rovConnector.control.getPitchInc()));

    ui->edResetPosition->setText(QString::number(rovConnector.control.getResetPosition()));

    ui->edRollKp->setText(QString::number(rovConnector.control.getRollKp()));
    ui->edRollKi->setText(QString::number(rovConnector.control.getRollKi()));
    ui->edRollKd->setText(QString::number(rovConnector.control.getRollKd()));

    ui->edPitchKp->setText(QString::number(rovConnector.control.getPitchKp()));
    ui->edPitchKi->setText(QString::number(rovConnector.control.getPitchKi()));
    ui->edPitchKd->setText(QString::number(rovConnector.control.getPitchKd()));

    ui->edYawKp->setText(QString::number(rovConnector.control.getYawKp()));
    ui->edYawKi->setText(QString::number(rovConnector.control.getYawKi()));
    ui->edYawKd->setText(QString::number(rovConnector.control.getYawKd()));

    ui->edDepthKp->setText(QString::number(rovConnector.control.getDepthKp()));
    ui->edDepthKi->setText(QString::number(rovConnector.control.getDepthKi()));
    ui->edDepthKd->setText(QString::number(rovConnector.control.getDepthKd()));

    ui->edUpdatePID->setText(QString::number(rovConnector.control.getUpdatePID()));


    // При соединении уже задали IP и Port, которые будут использоваться для записи датаграммы
    if (rovConnector.getIsConnected())
        rovConnector.writeControlDatagram();

    // Сброс флага обновления параметров ПИД-контроллера
    if (updatePID)
        updatePID = false;
}

void MainWindow::on_btnAUV_clicked()
{

    // Для клиента держать постоянным открытым соедниение не нужно
    // For UDP sockets, after binding, the signal QUdpSocket::readyRead()
    // is emitted whenever a UDP datagram arrives on the specified
    // address and port. Thus, this function is useful to write UDP servers.
    // Т.е. bind нужно использовать только дла написания сервера.
    // для клиента держать соединение с сервером получается не обязательно


    //if (rovConnector.getIsConnected())
    //{
    //    rovConnector.closeConnection();
    //}
    //else
    //{
    //    // Создаем коннектор: Определяем IP и номер порта сервера
    //    rovConnector.openConnection(ui->edIP->text(),
    //                                ui->edPort->text().toInt());
    //}

    // Будем использовать connectToHost и disconnectFromHost
    if (rovConnector.getIsConnected())
    {
        // Отключаемся
        rovConnector.disconnectFromHost();
    }
    else
    {
        // Запоминаем IP и Port сервера
        rovConnector.setIP(ui->edIP->text());
        rovConnector.setPort(ui->edPort->text().toInt());

        // Соединяемся с хостом
        rovConnector.connectToHost(ui->edIP->text(),
                                   ui->edPort->text().toInt());

        if (rovConnector.getIsConnected())
            rovConnector.writeConnectDatagram();
    }
}

void MainWindow::OnSocketConnect()
{
    ui->gbTelemetry->setEnabled(true);

    qDebug() << "Socket connected successfully";
    ui->btnAUV->setText("ОТКЛЮЧИТЬСЯ ОТ ТНПА");
    ui->edAUVConnection->setText("Соединение с ТНПА установлено");
}
void MainWindow::OnSocketDisconnect()
{
    ui->gbTelemetry->setEnabled(false);

    qDebug() << "Socket disconnected successfully";
    ui->btnAUV->setText("ПОДКЛЮЧИТЬСЯ К ТНПА");
    ui->edAUVConnection->setText("Соединение с ТНПА разорвано");
}
void MainWindow::OnSocketProcessTelemetryDatagram()
{
    qDebug() << "Telemetry Datagram Received...";

    // Проверяем, разрешено ли коннектору читать телеметрию
    if ((rovConnector.getMode() & SevROVConnector::Mode::TELEMETRY_READ)
        == SevROVConnector::Mode::TELEMETRY_READ)
    {
        ui->edRoll->setText(QString::number(rovConnector.telemetry.getRoll(), 'f', 2));
        ui->edPitch->setText(QString::number(rovConnector.telemetry.getPitch(), 'f', 2));
        ui->edYaw->setText(QString::number(rovConnector.telemetry.getYaw(), 'f', 2));
        ui->edHeading->setText(QString::number(rovConnector.telemetry.getHeading(), 'f', 2));
        ui->edDepthAUV->setText(QString::number(rovConnector.telemetry.getDepth(), 'f', 2));
        ui->edRollSetPoint->setText(QString::number(rovConnector.telemetry.getRollSetPoint(), 'f', 2));
        ui->edPitchSetPoint->setText(QString::number(rovConnector.telemetry.getPitchSetPoint(), 'f', 2));

        // rovConnector.telemetry.printDebugInfo();
    }
}

void MainWindow::on_pbPIDUpdate_clicked()
{
    updatePID = true;
}

void MainWindow::loadSettings()
{
    QSettings settings(settingsFileName, QSettings::IniFormat);

    settings.beginGroup("/CONNECTION");
    ui->edIP->setText(settings.value("/IP", "127.0.0.1").toString());
    ui->edPort->setText(settings.value("/Port", "1234").toString());
    settings.endGroup();

    settings.beginGroup("/CONTROLLER");

    // Power Limit
    ui->sbPowerLimit->setValue(settings.value("/PowerLimit", 0.1).toDouble());
    ui->sbRollKp->setValue(settings.value("/RollKp", 0.1).toDouble());
    ui->sbRollKi->setValue(settings.value("/RollKi", 0.1).toDouble());
    ui->sbRollKd->setValue(settings.value("/RollKd", 0.1).toDouble());
    ui->cbRollStab->setChecked(settings.value("/RollStab", false).toBool());
    // Pitch PID
    ui->sbPitchKp->setValue(settings.value("/PitchKp", 0.1).toDouble());
    ui->sbPitchKi->setValue(settings.value("/PitchKi", 0.1).toDouble());
    ui->sbPitchKd->setValue(settings.value("/PitchKd", 0.1).toDouble());
    ui->cbPitchStab->setChecked(settings.value("/PitchStab", false).toBool());
    // Yaw PID
    ui->sbYawKp->setValue(settings.value("/YawKp", 0.1).toDouble());
    ui->sbYawKi->setValue(settings.value("/YawKi", 0.1).toDouble());
    ui->sbYawKd->setValue(settings.value("/YawKd", 0.1).toDouble());
    ui->cbYawStab->setChecked(settings.value("/YawStab", false).toBool());
    // Depth PID
    ui->sbDepthKp->setValue(settings.value("/DepthKp", 0.1).toDouble());
    ui->sbDepthKi->setValue(settings.value("/DepthKi", 0.1).toDouble());
    ui->sbDepthKd->setValue(settings.value("/DepthKd", 0.1).toDouble());
    ui->cbDepthStab->setChecked(settings.value("/DepthStab", false).toBool());

    settings.endGroup();
}
void MainWindow::saveSettings()
{
    QSettings settings(settingsFileName, QSettings::IniFormat);

    settings.beginGroup("/CONNECTION");
    settings.setValue("/IP", ui->edIP->text());
    settings.setValue("/Port", ui->edPort->text());
    settings.endGroup();

    settings.beginGroup("/CONTROLLER");

    // Power Limit
    settings.setValue("/PowerLimit", ui->sbPowerLimit->value());
    settings.setValue("/RollKp", ui->sbRollKp->value());
    settings.setValue("/RollKi", ui->sbRollKi->value());
    settings.setValue("/RollKd", ui->sbRollKd->value());
    settings.setValue("/RollStab", ui->cbRollStab->isChecked());
    // Pitch PID
    settings.setValue("/PitchKp", ui->sbPitchKp->value());
    settings.setValue("/PitchKi", ui->sbPitchKi->value());
    settings.setValue("/PitchKd", ui->sbPitchKd->value());
    settings.setValue("/PitchStab", ui->cbPitchStab->isChecked());
    // Yaw PID
    settings.setValue("/YawKp", ui->sbYawKp->value());
    settings.setValue("/YawKi", ui->sbYawKi->value());
    settings.setValue("/YawKd", ui->sbYawKd->value());
    settings.setValue("/YawStab", ui->cbYawStab->isChecked());
    // Depth PID
    settings.setValue("/DepthKp", ui->sbDepthKp->value());
    settings.setValue("/DepthKi", ui->sbDepthKi->value());
    settings.setValue("/DepthKd", ui->sbDepthKd->value());
    settings.setValue("/DepthStab", ui->cbDepthStab->isChecked());

    settings.endGroup();
}


void MainWindow::on_btnCamera_clicked()
{
    if (!cvConnected)
    {
        ui->btnCamera->setText("ОТКЛЮЧИТЬСЯ ОТ КАМЕРЫ");
        cvConnected = true;

        if (ui->edIPCamera->text().length() == 1)
        {
            int camID = ui->edIPCamera->text().toInt();
            webcam = new cv::VideoCapture(camID);
        }
        else
            webcam = new cv::VideoCapture(ui->edIPCamera->text().toStdString());

        if (!videoTimer->isActive())
            videoTimer->start(10);
    }
    else
    {
        ui->btnCamera->setText("ПОДКЛЮЧИТЬСЯ К КАМЕРЕ");
        cvConnected = false;

        if (webcam->isOpened())
            webcam->release();

        if (!videoTimer->isActive())
            videoTimer->stop();

        QPixmap pixmap;
        QColor color;

        color = QColor(255, 255, 255, 255);
        pixmap = QPixmap(ui->lblCamera->size());
        pixmap.fill(color);

        ui->lblCamera->setPixmap(pixmap);
        ui->lblCamera->setText("CAMERA");
    }
}

void MainWindow::OnVideoTimer()
{
    webcam->read(source);

    if (source.empty())
        return;

    cv::Mat resized;
    cv::resize(source, resized, cv::Size(640, 480));

    // Image preprocessing
    cv::cvtColor(resized, destination, cv::COLOR_BGR2RGB);

    imgcam = QImage((uchar*) destination.data,
                    destination.cols,
                    destination.rows,
                    destination.step,
                    QImage::Format_RGB888);

    // Show QImage using QLabel
    ui->lblCamera->setPixmap(QPixmap::fromImage(imgcam));
}

