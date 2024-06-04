#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QScreen>
#include <QDir>

#include "sevrovlibrary.h"

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
    setFixedSize(QSize(590, 650));
    // statusBar()->setSizeGripEnabled(false);
    // Центрируем окно в пределах экрана
    move(screen()->geometry().center() - frameGeometry().center());
    setWindowTitle("ТНПА :: Симулятор :: " + QString(APP_VERSION.c_str()));

    // Сервер должен уметь читать сигналы управления и писать телеметрию в ответ
    rovConnector.setMode(SevROVConnector::Mode::CONTROL_READ |
                         SevROVConnector::Mode::TELEMETRY_WRITE);

    connect(&rovConnector, SIGNAL(OnConnected()), this, SLOT(OnSocketConnect()));
    connect(&rovConnector, SIGNAL(OnDisconnected()), this, SLOT(OnSocketDisconnect()));
    connect(&rovConnector, SIGNAL(OnProcessControlDatagram()), this, SLOT(OnSocketProcessControlDatagram()));
    // connect(&rovConnector, SIGNAL(OnProcessConnectDatagram()), this, SLOT(OnSocketProcessConnectDatagram()));
    connect(&rovConnector, &SevROVConnector::OnProcessConnectDatagram, this, &MainWindow::OnSocketProcessConnectDatagram);

    loadSettings();
}

MainWindow::~MainWindow()
{
    saveSettings();
    delete ui;
}


void MainWindow::on_pushButtonConnect_clicked()
{
    // Для сервера используем вариант с bind() для уставноки соединения
    if (rovConnector.getIsConnected())
    {
        // Прекращаем прослушку IP и порта
        rovConnector.closeConnection();
    }
    else
    {
        // Начинаем прослушку заданного IP и порта
        rovConnector.openConnection(ui->edIP->text(),
                                    ui->edPort->text().toInt());
    }
}

void MainWindow::OnSocketConnect()
{
    qDebug() << "Socket connected successfully";
    ui->pushButtonConnect->setText("РАЗЪЕДИНИТЬ");
}
void MainWindow::OnSocketDisconnect()
{
    qDebug() << "Socket disconnected successfully";
    ui->pushButtonConnect->setText("СОЕДИНИТЬ");
}
void MainWindow::OnSocketProcessControlDatagram()
{
    qDebug() << "Control Datagram Received...";

    // Проверяем, разрешено ли коннектору читать телеметрию
    if ((rovConnector.getMode() & SevROVConnector::Mode::CONTROL_READ)
        == SevROVConnector::Mode::CONTROL_READ)
    {
        // rovConnector.control.printDebugInfo();

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

        // Имитируем изменение телеметрии
        rovConnector.telemetry.setRoll(QRandomGenerator::global()->bounded(256));
        rovConnector.telemetry.setPitch(QRandomGenerator::global()->bounded(256));
        rovConnector.telemetry.setYaw(QRandomGenerator::global()->bounded(256));
        rovConnector.telemetry.setHeading(QRandomGenerator::global()->bounded(256));
        rovConnector.telemetry.setDepth(QRandomGenerator::global()->bounded(256));
        rovConnector.telemetry.setRollSetPoint(QRandomGenerator::global()->bounded(256));
        rovConnector.telemetry.setPitchSetPoint(QRandomGenerator::global()->bounded(256));

        ui->edRoll->setText(QString::number(rovConnector.telemetry.getRoll(), 'f', 2));
        ui->edPitch->setText(QString::number(rovConnector.telemetry.getPitch(), 'f', 2));
        ui->edYaw->setText(QString::number(rovConnector.telemetry.getYaw(), 'f', 2));
        ui->edHeading->setText(QString::number(rovConnector.telemetry.getHeading(), 'f', 2));
        ui->edDepthAUV->setText(QString::number(rovConnector.telemetry.getDepth(), 'f', 2));

        ui->edRollSetPoint->setText(QString::number(rovConnector.telemetry.getRollSetPoint(), 'f', 2));
        ui->edPitchSetPoint->setText(QString::number(rovConnector.telemetry.getPitchSetPoint(), 'f', 2));
    }
}

void MainWindow::OnSocketProcessConnectDatagram(QString ip, int port)
{
    qDebug() << "Connect Datagram Received..." << ip.toStdString().c_str() << ":" << port;
}

void MainWindow::loadSettings()
{
    QSettings settings(settingsFileName, QSettings::IniFormat);

    settings.beginGroup("/CONNECTION");
    ui->edIP->setText(settings.value("/IP", "127.0.0.1").toString());
    ui->edPort->setText(settings.value("/Port", "1234").toString());
    settings.endGroup();
}
void MainWindow::saveSettings()
{
    QSettings settings(settingsFileName, QSettings::IniFormat);

    settings.beginGroup("/CONNECTION");
    settings.setValue("/IP", ui->edIP->text());
    settings.setValue("/Port", ui->edPort->text());
    settings.endGroup();
}
