#ifndef SETTINGSWINDOW_H
#define SETTINGSWINDOW_H

#include <QDialog>
#include <QDir>
#include <QSettings>

namespace Ui {
class SettingsWindow;
}

class SettingsWindow : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsWindow(QWidget *parent = nullptr);
    ~SettingsWindow();

private slots:
    void onCameraResolutionComboboxCurrentIndexChanged(int index);
    void onOKButtonClicked();
    void onCancelButtonClicked();

private:
    Ui::SettingsWindow *ui;
    QString _settingsFileName;

    void loadSettings();
    void saveSettings();
};

#endif // SETTINGSWINDOW_H
