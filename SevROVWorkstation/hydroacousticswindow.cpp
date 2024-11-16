#include "hydroacousticswindow.h"
#include "ui_hydroacousticswindow.h"
#include <QKeyEvent>
#include <QScrollBar>
#include <QDateTime>
#include <QMap>


AcousticWindow::AcousticWindow(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::AcousticWindow)
{
    ui->setupUi(this);

    commandNumScroller();   // Подгрузка доступных запросов для uWave
    updatePortList();       // Обновление списка доступных COM-портов при запуске программы

    setupWindowGeometry();  // Позиционирование окна
    setupControlsStyle();   // Установка стилей компонентов


    connect(ui->sendButton, &QPushButton::clicked, this, &AcousticWindow::onSendButtonClicked);
    connect(ui->pbserialConnect, &QPushButton::clicked, this, &AcousticWindow::onSerialConnectClicked);
    connect(ui->pbsocketConnect, &QPushButton::clicked, this, &AcousticWindow::onSocketConnectClicked);
}

AcousticWindow::~AcousticWindow()
{
    if(readerThread){
        readerThread->quit();
        readerThread->wait();
    }
    if(writerThread){
        writerThread->quit();
        writerThread->wait();
    }
    if (h_serial){
        close_serial(h_serial);
    }
    delete ui;
}


QMap<QString, CommandType> commandMap = {
    {"SEND", SEND},
    {"TEST", TEST},
    {"PING", PING},
    {"DEVINFO", DEVINFO},
    {"PT_SETTINGS", PT_SETTINGS},
    {"AMBIENT", AMBIENT},
    {"ITG_REQ", ITG_REQ},
    {"PITCH_ROLL", PITCH_ROLL}
};

/** @brief Функция-конвертер C-представления строк в QString C++
 *
 * Необходима для корректного взаимодействия с C-функциями
 *
 * @param wcharStr - строка в C-формате wchar_t
 * @return - строка в формате QString
*/
QString charToString(const char* cString) {
    return QString::fromUtf8(cString);
}

/** @brief Функция-конвертер из типа строки wchar в std::string
 *
 * Необходима для корректного взаимодействия с C-функциями
 *
 * @param wcharStr - строка в C-формате wchar_t
 * @return - строка в формате string
*/
std::string wcharToString(const wchar_t* wcharStr){
    std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
    return converter.to_bytes(wcharStr);
}

/** @brief Функция-конвертер из типа строки QString в char*
 *
 * Необходима для корректного взаимодействия с C-функциями
 *
 * @param qString - строка в формате QString
 * @return - строка в формате char*
*/
char* qStringToChar(const QString & qString){
    return qString.toUtf8().data();
}

void AcousticWindow::commandNumScroller() {
    ui->requestTyprComboBox->clear();

    ui->requestTyprComboBox->addItems(commandMap.keys());
}

void AcousticWindow::onSendButtonClicked()
{
    QString requestType = ui->requestTyprComboBox->currentText();
    CommandType command = commandMap.value(requestType, SEND); // Получение значения ENUM в зависимости от выбранной команды (SEND по умолчанию)
    QString data = ui->inputField->text();
    int dest_addr = ui->dstAddressField->text().toInt();

    char sendBuffer[500] = {0};

    switch(command){
    case SEND:
        queryForPktSend(sendBuffer, dest_addr, 2, data.toUtf8().data());
        break;
    case TEST:
        queryForPktSend(sendBuffer, dest_addr, 2, qStringToChar("hydro_test"));
        break;
    case PING:
        queryRemoteModem(sendBuffer, dest_addr, 0, RC_PING);
        break;
    case DEVINFO:
        queryForDeviceInfo(sendBuffer);
        break;
    case PT_SETTINGS:
        queryForPktModeSettings(sendBuffer);
        break;
    case AMBIENT:
        //queryForAmbientDataConfig(sendBuffer, isSaveToFlash, period, isPressure,Temperature,isDepth,isVCC);
        break;
    case ITG_REQ:
        queryForPktITG(sendBuffer, dest_addr, 0);
        break;
    default:
        updateOutput("Unknown requset type");
        break;
    }

    ui->inputField->clear();    // Очистка текстового поля после ввода значений
    ui->inputField->setFocus(); // Удержание фокуса на текстовом поле после отправки

    updateOutput(" >> " + charToString(sendBuffer)); // Вывод сообщения, отправленного по COM-порту

    if(writerThread && h_serial != NULL)
    {
        writerThread->writeData(sendBuffer); // Отправка по последовательному порту
    }
    else
    {
        updateOutput("Serial threads or connection error");
    }
}

void AcousticWindow::updateOutput(const QString &text)
{
    // Формирование метки времени для ввода и вывода
    QString timestamp = QDateTime::currentDateTime().toString(" hh::mm::ss ");

    // Получение существующего layout'а
    QVBoxLayout *layout = qobject_cast<QVBoxLayout*>(ui->scrollAreaWidgetContents_3->layout());

    // Если layout не найден, создание нового (один раз)
    if (!layout) {
        layout = new QVBoxLayout(ui->scrollAreaWidgetContents_3);
        ui->scrollAreaWidgetContents_3->setLayout(layout);
    }

    // Получение скроллбара из outputArea и прокрутка его вниз
    QScrollBar *scrollBar = ui->outputArea->verticalScrollBar();
    if (scrollBar) {
        scrollBar->setValue(scrollBar->maximum());
    }

    // Создание нового QLabel'а для вывода текста
    QLabel *newLabel = new QLabel(timestamp + text, ui->scrollAreaWidgetContents_3);
    newLabel->setWordWrap(true);  //  Перенос строк, если текст длинный
    layout->addWidget(newLabel);
}

/** @brief
 * Функция обработки нажатия клавиши ENTER для отправки
 *
*/
void AcousticWindow::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return)
    {
        onSendButtonClicked();  // Отправка данных по нажатию Enter
    }
    QWidget::keyPressEvent(event);
}

void AcousticWindow::onSerialConnectClicked()
{
    double distance = 1.0;
    double pressure = 2.0;
    double telemetry = 3.0;

    emit onTelemetry(distance, pressure, telemetry);

    QString portName = ui->serialPortListDropDown->currentText(); // Получение текущего доступного COM-порта из выпадающего списка

    if(h_serial != NULL)
    { 
        // Остановка потоков чтения и записи через последовательный порт
        if (readerThread && readerThread->isRunning())
        {
            readerThread->quit(); // Завершение потока записи
            // Принудительное завершение потока чтения, если он не был закрыт штантно по истечению 3-х секунд
            if(!readerThread->wait(3000))
            {
                readerThread->terminate();
                readerThread->wait();
            }
            delete readerThread;
            readerThread = nullptr;
        }

        if (writerThread && writerThread->isRunning())
        {
            writerThread->quit(); // Завершение потока записи
            // Принудительное завершение потока записи, если он не был закрыт штантно по истечению 3-х секунд
            if(!writerThread->wait(3000))
            {
                writerThread->terminate();
                writerThread->wait();
            }
            delete writerThread;
            writerThread = nullptr;
        }

        close_serial(h_serial); // Закрытие соединения с последовательным портом
        h_serial = NULL;
        updateOutput("--- Serial disconnected from " + portName + " ---"); // Вывод в текстовое поле

        // Изменение параметров кнопки при отключении от последовательного порта
        ui->pbserialConnect->setText("CONNECT");
        ui->pbserialConnect->setStyleSheet("background-color: rgb(0,127,0);");
    }
    else
    {
        h_serial = init_serial(portName.toUtf8().data()); // Инициализация серийного порта

        if(h_serial != NULL)
        {
            updateOutput("--- Serial connected to " + portName + " ---"); // Вывод в текстовое поле

            ui->pbserialConnect->setText("DISCONNECT");
            ui->pbserialConnect->setStyleSheet("background-color: rgb(127,0,0);");

            // Инициализация потоков чтения и записи с последовательного порта
            readerThread = new SerialOutput(h_serial,this);
            connect(readerThread, &SerialOutput::dataReceived, this, &AcousticWindow::updateOutput);
            readerThread->start();

            writerThread = new SerialInput(h_serial, this);
            writerThread->start();
        }
        else
        {
            qDebug() << "Serial " << portName << " connection error";
            updateOutput("--- Serial " + portName + " connection error ---");
        }
    }
}

/** @brief
 * Функция для подключения модемов через ETHERNET (на будущее)
 *
*/
void AcousticWindow::onSocketConnectClicked(){
    //updateOutput(" << Socket connected"); // Заглушка
}

/** @brief
 * Функция для поиска и формирования списка доступных COM-портов
 *
 * @return portList - вектор строк доступных COM-портов
*/
std::vector<std::string> listAvailablePorts()
{
    std::vector<std::string> portList;
    HDEVINFO hDevInfo;
    SP_DEVINFO_DATA DeviceInfoData;
    DWORD i;

    hDevInfo = SetupDiGetClassDevs(&GUID_DEVCLASS_PORTS, 0, 0, DIGCF_PRESENT);
    if (hDevInfo == INVALID_HANDLE_VALUE)
    {
        qDebug() << "Error check avaliable ports\n";
    }

    DeviceInfoData.cbSize = sizeof(SP_DEVINFO_DATA);
    for (i = 0; SetupDiEnumDeviceInfo(hDevInfo, i, &DeviceInfoData); i++)
    {
        DWORD DataT;
        LPTSTR buffer = NULL;
        DWORD buffersize = 0;

        while (!SetupDiGetDeviceRegistryProperty(hDevInfo, &DeviceInfoData, SPDRP_FRIENDLYNAME, &DataT, (PBYTE)buffer, buffersize, &buffersize))
        {
            if (GetLastError() == ERROR_INSUFFICIENT_BUFFER)
            {
                if (buffer) LocalFree(buffer); // Change the buffer size
                buffer = (LPTSTR)LocalAlloc(LPTR, buffersize); // Allocate new buffer.
            }
            else
            {
                fprintf(stderr, "Error with buffer\n");
                break;
            }
        }

        if (buffer)
        {
            //portList.emplace_back(wcharToString(buffer));
            std::string fullPortName = wcharToString(buffer);
            LocalFree(buffer);

            size_t pos = fullPortName.find("COM");
            if(pos != std::string::npos && pos + 3 < fullPortName.size())
            {
                if (isdigit(fullPortName[pos + 3]))
                {
                    std::string portName = fullPortName.substr(pos,4);
                    if (isdigit(fullPortName[pos+4]))
                    {
                        portName += fullPortName[pos + 4];
                    }
                    portList.push_back(portName); // Добавление номера COM-порта в список
                }
            }
        }
    }

    if (GetLastError() != NO_ERROR && GetLastError() != ERROR_NO_MORE_ITEMS) {
        qDebug() << "Updating serial port list error\n";
    }

    SetupDiDestroyDeviceInfoList(hDevInfo); //  Cleanup
    return portList;                        // Возвращение итогового перечня доступных COM-портов
}

void AcousticWindow::updatePortList () {
    std::vector<std::string> ports = listAvailablePorts();

    ui->serialPortListDropDown->clear();
    for (const auto& port : ports){
        ui->serialPortListDropDown->addItem(QString::fromStdString(port));
    }

    if(ports.empty()){
        qDebug() << "No available COM ports found";
        ui->serialPortListDropDown->addItem("No ports available");
    }
}

/**
 * @brief Функция-обработчик пришедших команд от uWave
 *
 *  Переводит пришедшие от uWave'а запросы в человекочитабельный вид.
 *
 * @param puwv - структура для хранения параметров
 * @param command - ID пришедшей команды
 * @param out_buffer - Выходная строка с обработанным результатом
 */
void puwv2Qstr(puwv_t puwv, int command, QString& out_buffer) {
    QString result;
    QString data;
    char cdata[64] = {0};

    switch (command) {
    case ACK:
        result += QStringLiteral("\n\t\t\tAcknowledge from remote device\n");
        result += QString(" Command ID: %1\n Error code: %2\n")
                      .arg(charToString(puwv.ack.cmdID))
                      .arg(puwv.ack.errCode);
        break;

    case RC_RESPONSE: {
        QString code = (puwv.rc_resp.rcCmdID == 1) ? "PONG" : "UNRECOGNIZED";
        result += QStringLiteral("\n\t\t\tRemote command response\n");
        result += QString(" Tx channel ID: %1\n Remote command ID: %2 (%3)\n Propagation time: %4\n MSR: %5\n Requested value: %6\n")
                      .arg(puwv.rc_resp.txChID)
                      .arg(puwv.rc_resp.rcCmdID)
                      .arg(code)
                      .arg(puwv.rc_resp.propTime)
                      .arg(puwv.rc_resp.MSR)
                      .arg(puwv.rc_resp.value);
        break;
    }

    case RC_TIMEOUT:
        result += QStringLiteral("\n\t\t\tThe remote subscriber did not respond for the request\n");
        result += QString(" Tx channel ID: %1\n Remote command ID: %2\n")
                      .arg(puwv.rc_timeout.txChID)
                      .arg(puwv.rc_timeout.rcCmdID);
        break;

    case RC_ASYNC_IN:
        result += QStringLiteral("\n\t\t\tIncoming code message from a remote subscriber\n");
        result += QString(" Command ID: %1\n MSR: %2\n Azimuth (supported in USBL modems): %3\n")
                      .arg(puwv.rc_async_in.rcCmdID)
                      .arg(puwv.rc_async_in.MSR)
                      .arg(puwv.rc_async_in.azimuth);
        break;

    case DINFO:
        qDebug() << "AAAAA" << charToString(puwv.dinfo.serial_number) << " " << charToString(puwv.dinfo.system_moniker);
        result += QStringLiteral("\n\t\t\tResponse to device information request:\n");
        result += QString(" Serial num: %1\n Moniker: %2\n Version: %3\n Core moniker: %4\n Core version: %5\n Baudrate: %6\n")
                      .arg(charToString(puwv.dinfo.serial_number))
                      .arg(charToString(puwv.dinfo.system_moniker))
                      .arg(charToString(puwv.dinfo.system_version))
                      .arg(charToString(puwv.dinfo.core_moniker))
                      .arg(charToString(puwv.dinfo.core_version))
                      .arg(puwv.dinfo.acBaudrate);
        result += QString(" Rx channel ID: %1\n Tx channel ID: %2\n Max channels: %3\n PSU: %4\n Temperature/Depth sensor: %5\n Command mode: %6\n")
                      .arg(puwv.dinfo.rxChID)
                      .arg(puwv.dinfo.txChID)
                      .arg(puwv.dinfo.maxCh)
                      .arg(puwv.dinfo.PSU)
                      .arg(puwv.dinfo.isTempDepthAcces)
                      .arg(puwv.dinfo.isCmdMode);
        break;

    case D2H_PT_SETTINGS:
        result += QStringLiteral("\n\t\t\tPacket mode settings\n");
        result += QString(" PT mode status: %1\n Local address in PT mode: %2\n")
                      .arg(puwv.pt_set.isPTMode)
                      .arg(puwv.pt_set.ptLocalAddress);
        break;

    case PT_FAILED: {
        memset(cdata, 0, sizeof(cdata));
        hex2str(cdata, puwv.failed.dataPacket);
        data = charToString(cdata);

        result += QStringLiteral("\n\t\t\tThe transmission of the data packet was not successful\n");
        result += QString(" Target address: %1\n Max tries: %2\n Data: %3\n")
                      .arg(puwv.failed.target_ptAddress)
                      .arg(puwv.failed.maxTries)
                      .arg(data);
        break;
    }
    case PT_DELIVERED: {
        memset(cdata, 0, sizeof(cdata));
        hex2str(cdata, puwv.dlvrd.dataPacket);
        data = charToString(cdata);
        result += QStringLiteral("\n\t\t\tPacket was delivered\n");
        result += QString(" Target address: %1\n Max tries: %2\n Azimuth: %3\n Data: %4\n")
                      .arg(puwv.dlvrd.target_ptAddress)
                      .arg(puwv.dlvrd.maxTries)
                      .arg(puwv.dlvrd.azimuth)
                      .arg(data);
        break;
    }

    case PT_RECIEVED: {
        memset(cdata, 0, sizeof(cdata));
        hex2str(cdata, puwv.rcvd.dataPacket);
        data = charToString(cdata);
        result += QStringLiteral("\n\t\t\tPacket was received\n");
        result += QString(" Sender address: %1\n Data: %2\n")
                      .arg(puwv.rcvd.sender_ptAddress)
                      .arg(data);
        out_buffer = QString("%1;%2").arg(puwv.rcvd.sender_ptAddress).arg(data);
        return;
    }

    case PT_ITG:
        result += QStringLiteral("\n\t\t\tTimeout for a response to a request with logical addressing\n");
        result += QString(" Target address: %1\n Data ID: %2\n")
                      .arg(puwv.itg_tmo.target_ptAddress)
                      .arg(puwv.itg_tmo.dataID);
        break;

    case PT_ITG_RESP:
        result += QStringLiteral("\n\t\t\tRemote subscriber response with logical addressing\n");
        result += QString(" Target address: %1\n Data ID: %2\n Requested value: %3\n Propagation time: %4\n Azimuth: %5\n")
                      .arg(puwv.itg_resp.target_ptAddress)
                      .arg(puwv.itg_resp.dataID)
                      .arg(puwv.itg_resp.dataValue)
                      .arg(puwv.itg_resp.pTime)
                      .arg(puwv.itg_resp.azimuth);
        break;

    default:
        result += QStringLiteral("Unknown command\n");
        break;
    }

    out_buffer = result;
}

void SerialOutput::run()
{
    int puwv_command;
    int mine_id;
    puwv_t puwv;
    QString parse_descript;

    while(!isInterruptionRequested()) {
        char buffer[1024];
        DWORD bytes_read = uart_read(h_serial, buffer, sizeof(buffer));
        if (bytes_read > 0) {
            emit dataReceived(" >> " + QString::fromLocal8Bit(buffer, bytes_read));
            puwv_command = puwv_parser(buffer, &mine_id, &puwv);
            puwv2Qstr(puwv, puwv_command, parse_descript);
            if (parse_descript.length() > 0){
                emit dataReceived(parse_descript);
                parse_descript.clear();
            }
            memset(buffer, 0, sizeof(buffer));
        }
        QThread::msleep(10);
    }
}

void SerialInput::run() {
    while(!isInterruptionRequested()){
        if (dataToWrite != nullptr && strlen(dataToWrite)>0){
            uart_send(h_serial, dataToWrite);
            dataToWrite = nullptr;
        }
    }
}

void SerialInput::writeData(char *data)
{
    dataToWrite = data;
    if(!isRunning()){
        start();
    }

}

void AcousticWindow::setupControlsStyle()
{
    QString QComboBoxStyle = "QComboBox {"
                             "border-style: solid;"
                             "border-width: 1px;"
                             "border-color: silver;"
                             "color: silver;"
                             "}"
                             "QListView { color: silver; }";

    QString QLabelTextStyle = "QLabel {"
                              "color: silver;"
                              "}";

    QString QLineEditStyle = "QLineEdit {"
                             "color: silver;"
                             "border: 1px solid silver;"
                             "}";

    QString QGroupBoxStyle = "QGroupBox {"
                             "color: silver;"
                             "}";

    QString QScroolAreaStyle = "QScroolArea {"
                               "color: silver;"
                               "}"
                               "QWidget{ background-color: black; color: silver; }"
                               "QScrollBar{ background-color: none }";

    ui->serialPortListDropDown->setStyleSheet(QComboBoxStyle);
    ui->requestTyprComboBox->setStyleSheet(QComboBoxStyle);

    ui->lbPort->setStyleSheet(QLabelTextStyle);
    ui->lbBaudRate->setStyleSheet(QLabelTextStyle);
    ui->lbSocketIP->setStyleSheet(QLabelTextStyle);
    ui->lbSocketPort->setStyleSheet(QLabelTextStyle);

    ui->serialBaudrate->setStyleSheet(QLineEditStyle);
    ui->socketIPField->setStyleSheet(QLineEditStyle);
    ui->socketPortField->setStyleSheet(QLineEditStyle);
    ui->dstAddressField->setStyleSheet(QLineEditStyle);
    ui->inputField->setStyleSheet(QLineEditStyle);

    ui->gbSerialConfig->setStyleSheet(QGroupBoxStyle);
    ui->gbSocketConfig->setStyleSheet(QGroupBoxStyle);

    ui->outputArea->setStyleSheet(QScroolAreaStyle);

    ui->frame_6->setStyleSheet("QFrame { border: 1px solid silver; }");
    ui->lbBorder->setStyleSheet("QLabel {"
                                "border-style: solid;"
                                "border-width: 1px;"
                                "border-color: #F0BE50; "
                                "}");
}

void AcousticWindow::setupWindowGeometry()
{
    // Установка размера главного окна// Установка размера главного окна
    int windowWidth = 640; //_appSet.CAMERA_WIDTH + _appSet.CONTROL_PANEL_WIDTH + _appSet.CAMERA_VIEW_BORDER_WIDTH * 4;
    int windowHeight = 500; //_appSet.CAMERA_HEIGHT + _appSet.CAMERA_VIEW_BORDER_WIDTH * 2;

    // Фиксируем размер окна и убираем иконку ресайза
    setFixedSize(QSize(windowWidth, windowHeight));

    // Центрируем окно в пределах экрана
    moveWindowToCenter();

    QRect mainWindowRect = this->geometry();
    ui->lbBorder->setGeometry(
        10,
        10,
        mainWindowRect.width() - 20,
        mainWindowRect.height() - 20);
}

void AcousticWindow::moveWindowToCenter()
{
    auto primaryScreen = QGuiApplication::primaryScreen(); // Главный экран
    QRect primaryScreenRect = primaryScreen->availableGeometry(); // Размер главного экрана
    QPoint primaryScreenRectCenter = primaryScreenRect.center();
    primaryScreenRectCenter.setX(primaryScreenRectCenter.x() - (this->width()/2));
    primaryScreenRectCenter.setY(primaryScreenRectCenter.y() - (this->height()/2));
    move(primaryScreenRectCenter);
}
