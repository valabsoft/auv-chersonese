#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QRandomGenerator>
#include <QSettings>

#include "sevrovconnector.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void loadSettings();
    void saveSettings();

private slots:
    void on_pushButtonConnect_clicked();

public slots:
    void OnSocketProcessConnectDatagram(QString ip, int port);
    void OnSocketProcessControlDatagram();
    void OnSocketConnect();
    void OnSocketDisconnect();

private:
    Ui::MainWindow *ui;
    SevROVConnector rovConnector;
    QString settingsFileName;
};
#endif // MAINWINDOW_H
