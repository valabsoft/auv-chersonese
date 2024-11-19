#ifndef HYDROACOUSTICSWINDOW_H
#define HYDROACOUSTICSWINDOW_H

#include <QDialog>
#include <QScreen>
#include <QThread>
//#include <QTcpSocket>
#include <QQueue>

extern "C" {
    #include "hydroacoustics/interfaces/uart.h"
    #include "hydroacoustics/parsers/puwv_parser.h"
}

#include "applicationsettings.h"

namespace Ui {
class AcousticWindow;
}


/**
 * @brief Перечень доступных команд для модема
 */
enum CommandType {
    SEND,
    TEST,
    PING,
    DEVINFO,
    PT_SETTINGS,
    AMBIENT,
    ITG_REQ,
    PITCH_ROLL
};

extern QMap<QString, CommandType> commandMap;

/** @brief Класс для чтения данных из последовательного порта
 *
 * Нужен для обработки потока чтения данных из последовательного порта
*/
class SerialOutput : public QThread{
    Q_OBJECT

public:
    /**
     * @brief Конструктор класса по умолчанию
     *
     * @param serialHandle  - Дескриптор последовательного порта
     * @param parent        - Указатель на родительский объект окна
     */
    SerialOutput(HANDLE serialHandle, QObject *parent = nullptr)
        : QThread(parent), h_serial(serialHandle) {}

protected:
    /**
     * @brief Функция запуска потока чтения данных
     *
     * Реализует логику работы потока для получения данных с последовательного порта.
     */
    void run() override;

signals:
    /**
     * @brief Сигнал, извещающий о получении данных
     *
     * @param data - Принятые данные в формате строки
     */
    void dataReceived(const QString &data);

    void onTelemetry(const double &distance, const double &pressure, const double &temperature);

private:
    HANDLE h_serial; /**< Дескпритор последовательного порта */
};

/** @brief Класс для записи данных в последовательный порт
 *
*/
class SerialInput : public QThread{
    Q_OBJECT
public:
    /**
     * @brief Конструктор класса по умолчанию
     *
     * @param serialHandle  - Дескриптор последовательного порта
     * @param parent        - Указатель на родительский объект окна
     */
    SerialInput(HANDLE serialHandle, QObject *parent = nullptr)
        : QThread(parent), h_serial(serialHandle) {}

    /**
     * @brief Функция записи данных в последовательный порт
     *
     * @param data - Данные на отправку в формате строки
     */
    void writeData(char *data);

protected:
    /**
     * @brief Функция запуска потока записи данных
     *
     * Не используется напрямую, поскольку запись производится синхронно
     */
    void run() override;

private:
    HANDLE h_serial;                /**< Дескпритор последовательного порта */
    char* dataToWrite = nullptr;    /**< Данные для записи */
    QQueue<QString> writeQueue;     /**< Очередь данных для записи */
};

/**
 * @brief Класс для управления окном интерфейса гидроакустики
 */
class AcousticWindow : public QDialog
{
    Q_OBJECT

public:
    /**
     * @brief Конструктор класса AcousticWindow
     *
     * @param parent - указатель на родительский объект
     */
    explicit AcousticWindow(QWidget *parent = nullptr);

    /** @brief Деструктор класса AcousticWindow
     *
    */
    ~AcousticWindow ();

    void setupControlsStyle();
    void setupWindowGeometry();
    void moveWindowToCenter();

public slots:
    /**
     * @brief Функция-обработчик нажатия кнопки для отправки данных
     *
     * Получает данные, введённые в текстовое поле, и отправляет их в COM-порт
     */
    void onSendButtonClicked();

    /**
     * @brief Функция-обработчик нажатия кнопки для подключения к последовательному порту
     *
     * Получает текущее имя COM-порта в из выпадающего списка и производит к нему подключение с активацией потоков чтения и записи.
     * Повторное нажатие производит остановку потоков и отключение от порта.
     */
    void onSerialConnectClicked();

    /**
     * @brief Функция-обработчик нажатия кнопки для подключения к сокету (Может быть опционально добавлена в будущем)
     *
     * Получает IP-адрес и порт из соответствующих полей окна и производит к подключение по сокету с активацией потоков чтения и записи.
     * Повторное нажатие производит остановку потоков и отключение.
     */
    void onSocketConnectClicked();

    /**
     * @brief Функция обновления вывода в прокручиваемом текстовом поле
     *
     * @param text - текст для отображения
     */
    void updateOutput(const QString &text);

protected:
    /**
     * @brief Функция-обработчик нажатия клавиш
     *
     * Срабатывает при нажатии клавиши ENTER в окне интерфейса для отправки сообщения. ВЫполняет функцию, аналогичную кнопке "Send".
     *
     * @param event - Событие нажатия клавиши
     */
    void keyPressEvent(QKeyEvent *event) override;
signals:
    void onTelemetry(const double &distance, const double &pressure, const double &temperature);
private:
    Ui::AcousticWindow *ui;

    HANDLE h_serial = NULL;     /**< Дескпритор последовательного порта */
    SerialOutput *readerThread; /**< Дескприптор потока чтения последовательного порта */
    SerialInput *writerThread;  /**< Дескприптор потока записи в последовательный порт */

    //bool isConnected = false;

    /**
     * @brief Функция для обновления списка доступных COM-портов
     */
    void updatePortList();

    void commandNumScroller();
};


#endif // HYDROACOUSTICSWINDOW_H
