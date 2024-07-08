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

    // Загрузка настроек
    _settingsFileName = QApplication::applicationDirPath() + QDir::separator() + "settings.ini";
    loadSettings();
}

SettingsWindow::~SettingsWindow()
{
    delete ui;
}

void SettingsWindow::onCameraResolutionComboboxCurrentIndexChanged(int index)
{
    QString currentResolution = ui->cbCameraResolution->currentText();
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
    this->close();
}

void SettingsWindow::onCancelButtonClicked()
{
    // Закрыть форму
    this->close();
}

void SettingsWindow::loadSettings()
{
    QSettings settings(_settingsFileName, QSettings::IniFormat);

    settings.beginGroup("/CAMERA SETTINGS");
    ui->cbCameraResolution->setCurrentIndex(settings.value("/ResulutionIndex", "0").toInt());
    ui->leW->setText(settings.value("/Width", "640").toString());
    ui->leH->setText(settings.value("/Height", "480").toString());
    ui->leFPS->setText(settings.value("/FPS", "30").toString());
    ui->cbCameraMonoID->setCurrentIndex(settings.value("/MonoID", "0").toInt());
    ui->cbCameraLeftID->setCurrentIndex(settings.value("/LeftID", "1").toInt());
    ui->cbCameraRightID->setCurrentIndex(settings.value("/RightID", "2").toInt());
    ui->leVideoTimerInterval->setText(settings.value("/TimerInterval", "50").toString());
    settings.endGroup();

    settings.beginGroup("/JOYSTICK");
    ui->cbJoystickID->setCurrentIndex(settings.value("/JoystickID", "0").toInt());
    ui->leJoystickTimerInterval->setText(settings.value("/TimerInterval", "100").toString());
    settings.endGroup();
}

void SettingsWindow::saveSettings()
{
    QSettings settings(_settingsFileName, QSettings::IniFormat);

    settings.beginGroup("/CAMERA SETTINGS");
    settings.setValue("/ResulutionIndex", ui->cbCameraResolution->currentIndex());
    settings.setValue("/Width", ui->leW->text());
    settings.setValue("/Height", ui->leH->text());
    settings.setValue("/FPS", ui->leFPS->text());
    settings.setValue("/MonoID", ui->cbCameraMonoID->currentIndex());
    settings.setValue("/LeftID", ui->cbCameraLeftID->currentIndex());
    settings.setValue("/RightID", ui->cbCameraRightID->currentIndex());
    settings.setValue("/TimerInterval", ui->leVideoTimerInterval->text());
    settings.endGroup();

    settings.beginGroup("/JOYSTICK");
    settings.setValue("/JoystickID", ui->cbJoystickID->currentIndex());
    settings.setValue("/TimerInterval", ui->leJoystickTimerInterval->text());
    settings.endGroup();
}