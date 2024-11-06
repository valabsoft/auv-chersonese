#include "settingswindow.h"
#include "ui_settingswindow.h"

SettingsWindow::SettingsWindow(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::SettingsWindow)
{
    ui->setupUi(this);

    // Сброс стилей (не работает, передают parrent = NULL)

    this->setStyleSheet(styleSheet());
    this->setStyleSheet("");

    connect(ui->cbCameraResolution, &QComboBox::currentIndexChanged, this,
            &SettingsWindow::onCameraResolutionComboboxCurrentIndexChanged);
    onCameraResolutionComboboxCurrentIndexChanged(0);

    connect(ui->pbOK, &QPushButton::clicked, this, &SettingsWindow::onOKButtonClicked);
    connect(ui->pbCancel, &QPushButton::clicked, this, &SettingsWindow::onCancelButtonClicked);
    connect(ui->pbPIDUpdate, &QPushButton::clicked, this, &SettingsWindow::onPIDUpdateClicked);

    // Загрузка настроек
    _settingsFileName = QApplication::applicationDirPath() + QDir::separator() + "settings.ini";
    loadSettings();
    updatePID = false;
}

SettingsWindow::~SettingsWindow()
{
    delete ui;
}

void SettingsWindow::onCameraResolutionComboboxCurrentIndexChanged(int index)
{
    // QString currentResolution = ui->cbCameraResolution->currentText();
    QString currentResolution = ui->cbCameraResolution->itemText(index);
    QStringList resolution = currentResolution.split(" x ");

    //int W = resolution[0].toInt();
    //int H = resolution[1].toInt();

    ui->leW->setText(resolution[0]);
    ui->leH->setText(resolution[1]);
}

void SettingsWindow::onOKButtonClicked()
{
    // Сохранить настройки
    saveSettings();
    // Закрыть форму
    this->accept();
    // this->close();
}

void SettingsWindow::onCancelButtonClicked()
{
    // Закрыть форму
    this->reject();
    // this->close();
}

void SettingsWindow::loadSettings()
{
    QSettings settings(_settingsFileName, QSettings::IniFormat);

    settings.beginGroup("/CAMERA_SETTINGS");
    ui->cbCameraResolution->setCurrentIndex(settings.value("/ResulutionIndex", 0).toInt());
    ui->leW->setText(settings.value("/Width", 640).toString());
    ui->leH->setText(settings.value("/Height", 480).toString());
    ui->leFPS->setText(settings.value("/FPS", 30).toString());    
    ui->cbCameraLeftID->setCurrentIndex(settings.value("/LeftID", 1).toInt());
    ui->cbCameraRightID->setCurrentIndex(settings.value("/RightID", 2).toInt());
    ui->leVideoTimerInterval->setText(settings.value("/TimerInterval", 50).toString());
    ui->cbCameraTypeID->setCurrentIndex(settings.value("/CameraTypeID", 0).toInt());
    ui->cbVideoRecording->setChecked(settings.value("/isRecordingEnabled", true).toBool());
    ui->leVideoRecordingLength->setText(settings.value("/VideoRecordingLength", 60).toString());
    ui->leStoredVideoFilesLimit->setText(settings.value("/StoredVideoFilesLimit", 100).toString());
    ui->cbLeftCameraStreaming->setChecked(settings.value("/isLeftCameraStreamingEnabled", false).toBool());
    ui->leLeftCameraStreamingPort->setText(settings.value("/LeftCameraStreamingPort", 8080).toString());
    settings.endGroup();

    settings.beginGroup("/JOYSTICK");
    ui->cbJoystickID->setCurrentIndex(settings.value("/JoystickID", 0).toInt());
    ui->leJoystickTimerInterval->setText(settings.value("/TimerInterval", 100).toString());
    settings.endGroup();

    settings.beginGroup("/CONTROLLER");

    ui->sbRollKp->setValue(settings.value("/rollKp", 0.1).toDouble());
    ui->sbRollKi->setValue(settings.value("/rollKi", 0.1).toDouble());
    ui->sbRollKd->setValue(settings.value("/rollKd", 0.1).toDouble());
    ui->cbRollStab->setChecked(settings.value("/rollStabilization", false).toBool());

    ui->sbPitchKp->setValue(settings.value("/pitchKp", 0.1).toDouble());
    ui->sbPitchKi->setValue(settings.value("/pitchKi", 0.1).toDouble());
    ui->sbPitchKd->setValue(settings.value("/pitchKd", 0.1).toDouble());
    ui->cbPitchStab->setChecked(settings.value("/pitchStabilization", false).toBool());

    ui->sbYawKp->setValue(settings.value("/yawKp", 0.1).toDouble());
    ui->sbYawKi->setValue(settings.value("/yawKi", 0.1).toDouble());
    ui->sbYawKd->setValue(settings.value("/yawKd", 0.1).toDouble());
    ui->cbYawStab->setChecked(settings.value("/yawStabilization", false).toBool());

    ui->sbDepthKp->setValue(settings.value("/depthKp", 0.1).toDouble());
    ui->sbDepthKi->setValue(settings.value("/depthKi", 0.1).toDouble());
    ui->sbDepthKd->setValue(settings.value("/depthKd", 0.1).toDouble());
    ui->cbDepthStab->setChecked(settings.value("/depthStabilization", false).toBool());

    ui->sbPowerLimit->setValue(settings.value("/powerLimit", 0.1).toDouble());

    settings.endGroup();

    settings.beginGroup("/ROV");
    ui->leIP->setText(settings.value("/IP", "127.0.0.1").toString());
    ui->lePort->setText(settings.value("/Port", 1234).toString());
    settings.endGroup();
}

void SettingsWindow::saveSettings()
{
    QSettings settings(_settingsFileName, QSettings::IniFormat);

    settings.beginGroup("/CAMERA_SETTINGS");
    settings.setValue("/ResulutionIndex", ui->cbCameraResolution->currentIndex());
    settings.setValue("/Width", ui->leW->text());
    settings.setValue("/Height", ui->leH->text());
    settings.setValue("/FPS", ui->leFPS->text());    
    settings.setValue("/LeftID", ui->cbCameraLeftID->currentIndex());
    settings.setValue("/RightID", ui->cbCameraRightID->currentIndex());
    settings.setValue("/TimerInterval", ui->leVideoTimerInterval->text());
    settings.setValue("/CameraTypeID", ui->cbCameraTypeID->currentIndex());
    settings.setValue("/isRecordingEnabled", ui->cbVideoRecording->isChecked());
    settings.setValue("/VideoRecordingLength", ui->leVideoRecordingLength->text());
    settings.setValue("/StoredVideoFilesLimit", ui->leStoredVideoFilesLimit->text());
    settings.setValue("/isLeftCameraStreamingEnabled", ui->cbLeftCameraStreaming->isChecked());
    settings.setValue("/LeftCameraStreamingPort", ui->leLeftCameraStreamingPort->text());
    settings.endGroup();

    settings.beginGroup("/JOYSTICK");
    settings.setValue("/JoystickID", ui->cbJoystickID->currentIndex());
    settings.setValue("/TimerInterval", ui->leJoystickTimerInterval->text());
    settings.endGroup();

    settings.beginGroup("/CONTROLLER");

    settings.setValue("/rollKp", ui->sbRollKp->value());
    settings.setValue("/rollKi", ui->sbRollKi->value());
    settings.setValue("/rollKd", ui->sbRollKd->value());
    settings.setValue("/rollStabilization", ui->cbRollStab->isChecked());

    settings.setValue("/pitchKp", ui->sbPitchKp->value());
    settings.setValue("/pitchKi", ui->sbPitchKi->value());
    settings.setValue("/pitchKd", ui->sbPitchKd->value());
    settings.setValue("/pitchStabilization", ui->cbPitchStab->isChecked());

    settings.setValue("/yawKp", ui->sbYawKp->value());
    settings.setValue("/yawKi", ui->sbYawKi->value());
    settings.setValue("/yawKd", ui->sbYawKd->value());
    settings.setValue("/yawStabilization", ui->cbYawStab->isChecked());

    settings.setValue("/depthKp", ui->sbDepthKp->value());
    settings.setValue("/depthKi", ui->sbDepthKi->value());
    settings.setValue("/depthKd", ui->sbDepthKd->value());
    settings.setValue("/depthStabilization", ui->cbDepthStab->isChecked());

    settings.setValue("/powerLimit", ui->sbPowerLimit->value());

    settings.endGroup();

    settings.beginGroup("/ROV");
    settings.setValue("/IP", ui->leIP->text());
    settings.setValue("/Port", ui->lePort->text());
    settings.endGroup();
}

void SettingsWindow::onPIDUpdateClicked()
{
    updatePID = true;
}

bool SettingsWindow::getUpdatePID()
{
    return updatePID;
}
