#pragma once
#ifndef _PUWV_PARSER_H
#define _PUWV_PARSER_H

/** @cond */
#include <stdbool.h>
#include <stdint.h>
/** @endcond */

#include "puwv_codes.h"
#include "../checksum/checksum.h"

#define uWAVE_UNDEFINED_VAL       (-1)
#define uWAVE_IF_DEFINED_VAL(v)   ((v) != uWAVE_UNDEFINED_VAL)

#define uWAVE_UNDEFINED_FLOAT_VAL (-32768)
#define uWAVE_IF_FLOAT_VALID(f)   ((f) != uWAVE_UNDEFINED_FLOAT_VAL)

#define uWAVE_OUT_BUFFER_SIZE     (128)
#define uWAVE_IN_BUFFER_SIZE      (128)
#define uWAVE_PKT_MAX_SIZE        (64)

#define uWAVE_LOC_TIMEOUT_MS      (3000)
#define uWAVE_REM_TIMEOUT_MS      (6000)
#define uWAVE_PKT_SEND_TIMEOUT_MS (uWAVE_REM_TIMEOUT_MS * 255)

#define PUWV_SIGNATURE            (0x50555756)

#define HEX_DIGIT2B(b)            ((b) >= 0x41 ? ((b) - 0x37) : ((b) - 0x30))
#define DIGIT_2HEX(h)             ((h) > 9     ? ((h) + 0x37) : ((h) + 0x30))

#define C2B(b)                    ((b - '0'))
#define CC2B(b1, b2)              ((10 * (b1 - '0') + (b2 - '0')))
#define CCC2B(b1, b2, b3)         ((100 * (b1 - '0') + 10 * (b2 - '0') + (b3 - '0')))

#define MAX_TOKENS                 20
#define BUFFER_SIZE                1024

// //---------------- IC_H2D_DINFO_GET
// /** @brief Обратный запрос информации об устройстве */
// static uint8_t reversed_dinfo_get;

// //---------------- IC_H2D_PT_SETTINGS_READ
// /** @brief Обратный запрос чтения настроек */
// static uint8_t reversed_setting_read;

// //---------------- IC_H2D_AQPNG_SETTINGS_READ
// /** @brief Обратный запрос AQPNG */
// static uint8_t reversed_aqpng;

//------------------------ ACK ----------------------------
/**
 * @struct ACK_parse
 * @brief Структура для разбора ACK-запросов
 */
typedef struct ACK_parse{
    char cmdID[1];           ///< ID команды
    unsigned int errCode; ///< Код ошибки
} puwv_ack_t;

/**
* @struct Setting_parse
* @brief Структура для настроек приложения
*/
typedef struct Setting_parse{
    unsigned int rxChID; ///< Идентификатор канала приема
    unsigned int txChID; ///< Идентификатор канала передачи
    double styPSU;       ///< Соленость в PSU
    uint8_t isCmdMode;   ///< Флаг режима команды
    uint8_t isACKOnTXFinished; ///< Флаг подтверждения при завершении передачи
    double gravityAcc;   ///< Гравитационное ускорение
} puwv_setting_t;

//---------------- IC_H2D_RC_REQUEST-----------------------
/**
 * @brief Структура данных для запроса удаленного управления из хоста в устройство
 */
typedef struct RC_REQUEST_parse{
    unsigned int rxChID; ///< Идентификатор канала приема
    unsigned int txChID; ///< Идентификатор канала передачи
    unsigned int rcCmdID; ///< Идентификатор команды удаленного управления
} puwv_rc_request_t;

//---------------- IC_D2H_RC_RESPONSE----------------------
/**
 * @struct RC_RESPONSE_parse
 * @brief Структура данных для IC_D2H_RC_RESPONSE
 */
typedef struct RC_RESPONSE_parse{
    unsigned txChID;   ///< Идентификатор канала передачи
    unsigned rcCmdID;  ///< Идентификатор команды удаленного управления
    double MSR;        ///< Среднеквадратичное отклонение
    double value;      ///< Значение
    double propTime;   ///< Время распространения сигнала
    double azimuth;    ///< Горизонтальный угол прихода сигнала (если доступен)
} puwv_rc_response_t;

/**
 * @brief Структура данных данных для IC_D2H_RC_TIMEOUT
 */
typedef struct RC_TIMEOUT_parse{
    unsigned int txChID; ///< ID канала передачи
    unsigned int rcCmdID; ///< ID команды удаленного управления
} puwv_rc_timeout_t;

//---------------- IC_D2H_RC_ASYNC_IN----------------------
/**
 * @struct ASYNC_IN_parse
 * @brief  Структура данных для асинхронного входа от IC D2H
 */
typedef struct ASYNC_IN_parse{
    unsigned int rcCmdID; ///< Идентификатор команды удаленного управления
    double MSR;           ///< MSR для устройства
    double azimuth;       ///< Горизонтальный угол прихода сигнала (если доступен)
} puwv_rc_async_in_t;

//---------------- IC_H2D_AMB_DTA_CFG----------------------
/**
 * @brief Частота обновления данных об окружающей среде устройства.
 * Структура содержит параметры периодичности и активности сбора данных
 * по различным показателям окружающей среды.
 */
typedef struct AMB_DTA_CFG_parse{
    uint8_t isSaveToFlash; ///< Флаг сохранения данных во флеш-память
    unsigned PeriodMs; ///< Период обновления данных в миллисекундах
    uint8_t isPressure; ///< Флаг сбора данных о давлении
    uint8_t isTemperature; ///< Флаг сбора данных о температуре
    uint8_t isDepth; ///< Флаг сбора данных о глубине
    uint8_t isVCC; ///< Флаг сбора данных о напряжении питания
} puwv_amb_dta_cfg_t;

//---------------- IC_H2D_AMB_DTA--------------------------
/**
 * @brief Структура данных окружающей среды
 * Данная структура содержит поля для хранения информации о текущем состоянии окружающей среды.
 */
typedef struct AMB_DTA_parse{
    double pressure_mBar; ///< Давление в миллибарах
    double temperature_C; ///< Температура в градусах Цельсия
    double Depth_m; ///< Глубина в метрах
    double VCC_V; ///< Напряжение питания в вольтах
} puwv_amb_dta_t;

//---------------- IC_D2H_DINFO----------------------------
typedef struct DINFO_parse{
    /* Например,
    PUWV! = IC_D2H_DINFO,
    3A001E000E51363437333330 = серийный номер,
    STRONG = наименование системы,
    256 = 0x0100 версия системы 01.00,
    uWAVE [JULY] = наименование подсистемы связи,
    257 = 0x0101 версия подсистемы связи 01.01,
    78.27 = скорость передачи данных по гидроакустическому каналу, бит/с,
    0 = идентификатор канала передачи,
    0 = идентификатор канала приема,
    28 = общее число доступных кодовых каналов,
    0.0 = соленость, PSU,
    1 = встроенный датчик давления/температуры присутствует и исправен,
    0 = командный режим по умолчанию отключен */
    char* serial_number;  ///< Серийный номер
    char* system_moniker; ///< Наименование системы
    char* system_version; ///< Версия системы
    char* core_moniker;   ///< Наименование подсистемы связи
    char* core_version;   ///< Версия подсистемы связи
    double acBaudrate;    ///< Скорость передачи данных по гидроакустическому каналу, бит/с
    unsigned rxChID;      ///< Идентификатор канала передачи
    unsigned txChID;      ///< Идентификатор канала приема
    int maxCh;            ///< Общее число доступных кодовых каналов
    double PSU;           ///< Соленость, PSU
    uint8_t isTempDepthAcces; ///< Показывает, присутствует ли и исправен ли встроенный датчик давления/температуры
    uint8_t isCmdMode;    ///< Показывает, включен ли командный режим по умолчанию
} puwv_dinfo_t;

//---------------- IC_D2H_PT_SETTINGS----------------------
/**
 * @brief PT_SETTINGS_parse структура
 * @details Структура для pt (Packet Transmission) настроек
 */
typedef struct PT_SETTINGS_parse{
    uint8_t isPTMode; /**< флаг, обозначающий, активирован ли PT (Packet Transmission) режим */
    int ptLocalAddress; /**< локальный адрес в PT (Packet Transmission) режиме */
} puwv_pt_settings_t;

//---------------- IC_H2D_PT_SETTINGS_WRITE----------------
typedef struct PT_H2H_SETTINGS_WRITE_parse{
    unsigned int txChId; //!< Идентификатор канала передачи
    unsigned int rxChId; //!< Идентификатор канала приема
    double styPSU; //!< Соленость в PSU
    unsigned int isCmdMode; //!< Флаг режима команды
    uint8_t isACK0nTXFinished; //!< Флаг подтверждения завершения передачи
    double gravityAcc; //!< Гравитационное ускорение
} puwv_h2d_pt_settings_write_t;

//---------------- IC_H2H_PT_SETTINGS_WRITE----------------
/**
 * @struct PT_H2D_SETTINGS_WRITE_parse
 * @brief Структура для настроек записи PT_H2D
 */
typedef struct PT_H2D_SETTINGS_WRITE_parse{
    uint8_t isSaveToFlash; /**< Флаг сохранения во флеш-память */
    uint8_t isPTMode; /**< Флаг режима PT */
    unsigned int ptLocalAddress; /**< Локальный адрес PT */
} puwv_h2h_pt_settings_write_t;

//---------------- IC_H2D_PT_SEND--------------------------
typedef struct PT_SEND_parse{
    unsigned int target_ptAddress; ///< Целевой адрес pt
    unsigned int maxTries; ///< Максимальное количество попыток
    char dataPacket[uWAVE_OUT_BUFFER_SIZE]; ///< Пакет данных
    char data[uWAVE_PKT_MAX_SIZE];
} puwv_pt_send_t;

//---------------- IC_D2H_PT_FAILED------------------------
typedef struct PT_FAILED_parse{
    unsigned int target_ptAddress; ///< Целевой адрес pt
    unsigned int maxTries; ///< Максимальное количество попыток
    char dataPacket[uWAVE_OUT_BUFFER_SIZE];//uWAVE_PKT_MAX_SIZE];
    char data[uWAVE_PKT_MAX_SIZE];          ///< Пакет данных
} puwv_pt_failed_t;

//---------------- IC_D2H_PT_DLVRD-------------------------
typedef struct PT_DLVRD_parse{
    unsigned int target_ptAddress;          ///< Целевой адрес pt
    unsigned int maxTries;                  ///< Максимальное количество попыток
    double azimuth;                         ///< Горизонтальный угол прихода сигнала (если доступен)
    char dataPacket[uWAVE_OUT_BUFFER_SIZE];
    char data[uWAVE_PKT_MAX_SIZE];          ///< Пакет данных
} puwv_pt_dlvrd_t;

//---------------- IC_D2H_PT_RCVD--------------------------
typedef struct PT_RCVD_parse{
    unsigned int sender_ptAddress;          ///< Адрес отправителя
    double azimuth;                         ///< Горизонтальный угол прихода сигнала (если доступен)
    char dataPacket[uWAVE_OUT_BUFFER_SIZE];
    char data[uWAVE_PKT_MAX_SIZE];          ///< Пакет данных
} puwv_pt_rcvd_t;

//---------------- IC_H2D_PT_ITG---------------------------
typedef struct PT_ITG_parse{
    unsigned int target_ptAddress;  ///<Целевой адрес
    unsigned int dataID;            ///<Идентификатор данных
} puwv_pt_itg_t;

//---------------- IC_D2H_PT_ITG_TMO-----------------------
typedef struct PT_ITG_TMO_parse{
    /** @brief Целевой адрес */
    unsigned int target_ptAddress;
    /** @brief Идентификатор данных */
    unsigned int dataID;
} puwv_pt_itg_tmo_t;

//---------------- IC_D2H_PT_ITG_RESP----------------------
typedef struct PT_ITG_RESP_parse{
    unsigned int target_ptAddress;  ///< Целевой адрес
    unsigned int dataID;            ///< Идентификатор данных
    double dataValue;               ///< Значение данных
    double pTime;                   ///< Время пропагации
    double azimuth;                 ///< Горизонтальный угол прихода сигнала (если доступен)
} puwv_pt_itg_resp_t;

//---------------- IC_H2D_INC_DTA_CFG----------------------
/**
*  @brief Структура конфигурации передачи данных от устройства
*/
typedef struct INC_DTA_CFG_parse{
    uint8_t isSaveToFlash; ///< Сохранение во флеш
    unsigned int periodMs; ///< Период в миллисекундах
} puwv_inc_dta_cfg_t;

//---------------- IC_D2H_INC_DTA--------------------------
typedef struct INC_DTA_parse {
    uint8_t reversed;   ///< Перевёрнутые данные
    double pitch;       ///< Крен
    double roll;        ///< Тангаж
} puwv_inc_dta_t;

//---------------- IC_HDH_AQPNG_SETTINGS-------------------
typedef struct AQPNG_SETTINGS_parse{
    uint8_t isSaveToFlash;          ///< Сохранить во flash.
    unsigned int AQPNG_Mode;        ///< Режим AQPNG.
    unsigned int PeriodMs;          ///< Период в миллисекундах.
    unsigned int RcTxID;            ///< ID канала передачи RC.
    unsigned int RcRxID;            ///< ID канала приема RC.
    unsigned short int DataID;      ///< ID данных.
    uint8_t isPT;                   ///< Пакетный режим.
    unsigned int PTTargetAddress;   ///< Целевой адрес PT.
} puwv_aqpng_settings_t;

//----------------Гиперструктура парсера-------------------
typedef struct puwv_struct{
    puwv_ack_t                   ack;                   ///<Данные ответа ack.
    puwv_amb_dta_cfg_t           amb_dta_cfg;           ///<Конфигурация данных окружающей среды amb_dta
    puwv_amb_dta_t               amb_dta;               ///<Данные окружающей среды amb_dta
    puwv_dinfo_t                 dinfo;                 ///<Информация об устройстве dinfo
    puwv_h2d_pt_settings_write_t h2d_set_write;         ///<Запись настроек h2d_pt_settings от хоста до устройства
    puwv_h2h_pt_settings_write_t h2h_set_write;         ///<Запись настроек h2h_pt_settings от устройства до хоста
    puwv_pt_dlvrd_t              dlvrd;                 ///<Доставленные пакеты dlvrd
    puwv_pt_failed_t             failed;                ///<Пакеты failed, которые не удалось отправить
    puwv_pt_itg_resp_t           itg_resp;              ///<Ответ на настройку интервала пакета itg_resp
    puwv_pt_itg_t                itg;                   ///<Настройки интервала пакета itg
    puwv_pt_itg_tmo_t            itg_tmo;               ///<Превышение пакета itg_tmo
    puwv_pt_rcvd_t               rcvd;                  ///<Полученные пакеты rcvd
    puwv_pt_send_t               pt_send;               ///<Отправка пакета pt_send
    puwv_pt_settings_t           pt_set;                ///<Настройки пакета pt_set
    puwv_rc_async_in_t           rc_async_in;           ///<Входные данные rc_async_in
    puwv_rc_request_t            rc_request;            ///<Запрос на удаленное управление rc_request
    puwv_rc_response_t           rc_resp;               ///<Ответ на удаленное управление rc_resp
    puwv_rc_timeout_t            rc_timeout;            ///<Тайм-аут удаленного управления rc_timeout
    uint8_t                      reversed_aqpng;        ///<Обратный запрос aqpng
    uint8_t                      reversed_dinfo_get;    ///<Обратный запрос информации об устройстве dinfo_get
    uint8_t                      reversed_setting_read; ///<Обратный запрос чтения настроек setting_read
    int                          last_com;              ///<Последняя команда
}puwv_t;

/**
 * @brief Переводит строку в строку HEX формата.
 * @param[out] dst Вывод(строка).
 * @param[in] src Ввод(строка).
 */
void str2hex(char *dst, char *src);

/**
 * @brief Переводит строку HEX формата в строку.
 * @param[out] dst Вывод(строка).
 * @param[in] src Ввод(строка).
 */
void hex2str(char dst[uWAVE_PKT_MAX_SIZE], char src[uWAVE_PKT_MAX_SIZE]);

/**
 * @brief Выполняет операции командного переключателя.
 * @param puwv Указатель на структуру puwv_t.
 * @param command Команда, которую необходимо выполнить.
 * @param out Указатель на вывод.
 */
void perform_command_switch(/*FILE *fptr,*/ puwv_t *puwv, int command, void* out);

/**
 * @brief Обрабатывает ответы от устройства.
 * @param puwv Указатель на структуру puwv_t.
 * @param out Указатель на вывод.
 */
void handle_ack(puwv_t *puwv, void* out);

/**
 * @brief Обрабатывает данные об окружающей среде.
 * @param puwv Указатель на структуру puwv_t.
 * @param out Указатель на вывод.
 */
void handle_amb_data(puwv_t *puwv, void *out);

/**
 * @brief Обрабатывает конфигурацию данных об окружающей среде.
 * @param puwv Указатель на структуру puwv_t.
 * @param out Указатель на вывод.
 */
void handle_amb_data_cfg(puwv_t *puwv, void *out);

/**
 * @brief Обрабатывает настройки пакетного режима.
 * @param puwv Указатель на структуру puwv_t.
 * @param out Указатель на вывод.
 */
void handle_d2h_pt_settings(puwv_t *puwv, void *out);

/**
 * @brief Обрабатывает информацию об устройстве.
 * @param puwv Указатель на структуру puwv_t.
 * @param out Указатель на вывод.
 */
void handle_dinfo(puwv_t *puwv, void *out);

/**
 * @brief Обрабатывает запрос информации об устройстве.
 * @param puwv Указатель на структуру puwv_t.
 * @param out Указатель на вывод.
 */
void handle_dinfo_get(puwv_t *puwv, void *out);

/**
 * @brief Обрабатывает запись настроек пакетного режима от хоста до устройства.
 * @param puwv Указатель на структуру puwv_t.
 * @param out Указатель на вывод.
 */
void handle_h2d_pt_settings_write(puwv_t *puwv, void *out);

/**
 * @brief Обрабатывает запись настроек пакетного режима от устройства до хоста.
 * @param puwv Указатель на структуру puwv_t.
 * @param out Указатель на вывод.
 */
void handle_h2h_pt_settings_write(puwv_t *puwv, void *out);

/**
 * @brief Обрабатыватывает доставленные пакеты.
 * @param puwv Указатель на структуру puwv_t.
 * @param out Указатель на вывод.
 */
void handle_pt_delivered(puwv_t *puwv, void *out);

/**
 * @brief Обрабатывает пакеты, которые не удалось отправить.
 * @param puwv Указатель на структуру puwv_t.
 * @param out Указатель на вывод.
 */
void handle_pt_failed(puwv_t *puwv, void *out);

/**
 * @brief Обрабатывает настройки пакетного интервала.
 * @param puwv Указатель на структуру puwv_t.
 * @param out Указатель на вывод.
 */
void handle_pt_itg(puwv_t *puwv, void *out);

/**
 * @brief Обрабатывает ответ настройки пакетного интервала.
 * @param puwv Указатель на структуру puwv_t.
 * @param out Указатель на вывод.
 */
void handle_pt_itg_resp(puwv_t *puwv, void *out);
/**
*  @brief Обрабатывает получение пакетов
*  @param puwv Указатель на структуру типа puwv_t
*  @param out Указатель на вывод
*/
void handle_pt_received(puwv_t *puwv, void *out);

/**
*  @brief Обрабатывает отправку пакетов
*  @param puwv Указатель на структуру типа puwv_t
*  @param out Указатель на вывод
*/
void handle_pt_send(puwv_t *puwv, void *out);

/**
*  @brief Обрабатывает превышение последовательных попыток передачи пакета
*  @param puwv Указатель на структуру типа puwv_t
*  @param out Указатель на вывод
*/
void handle_pt_tmo(puwv_t *puwv, void *out);

/**
*  @brief Обрабатывает асинхронное входное удаленное управление
*  @param puwv Указатель на структуру типа puwv_t
*  @param out Указатель на вывод
*/
void handle_rc_async_in(puwv_t *puwv, void *out);

/**
*  @brief Обрабатывает запрос удаленного управления
*  @param puwv Указатель на структуру типа puwv_t
*  @param out Указатель на вывод
*/
void handle_rc_request(puwv_t *puwv, void *out);

/**
*  @brief Обрабатывает ответ на запрос удаленного управления
*  @param puwv Указатель на структуру типа puwv_t
*  @param out Указатель на вывод
*/
void handle_rc_response(puwv_t *puwv, void *out);

/**
*  @brief Обрабатывает превышение таймаута удаленного управления
*  @param puwv Указатель на структуру типа puwv_t
*  @param out Указатель на вывод
*/
void handle_rc_timeout(puwv_t *puwv, void *out);

/**
*  @brief Обработка чтения настроек
*  @param puwv Указатель на структуру типа puwv_t
*  @param out Указатель на вывод
*/
void handle_setting_read(puwv_t *puwv, void *out);


/**
 * @brief Извлекает строку из входной строки.
 * @param input_line Входная строка для парсинга.
 * @return Извлеченная строка.
 */
char* puwv_line_parser(char *input_line);

/**
 * @brief Определяет токен из входной строки.
 * @param input_line Входная строка для парсинга.
 * @param tokens Массив токенов для извлечения данных.
 * @return Количество определенных токенов.
 */
int token_parser(char *input_line, char tokens[MAX_TOKENS][uWAVE_IN_BUFFER_SIZE]);

/**
 * @brief Определяет команду из входной строки.
 * @param input_line Входная строка для парсинга.
 * @param out Указатель на вывод.
 * @return Определенная команда.
 */
int command_definer(char *input_line, void *out);

/**
 * @brief Запрос информации об устройстве (PUWV?)
 * @return Указатель на строку с запросом(обязательно освободить память с free)
 */
size_t queryForDeviceInfo(char *sendline);

/**
 * @brief Запрос обновления настроек (PUWV1)
 * @param txChID ID канала передачи
 * @param rxChID ID канала приема
 * @param salinityPSU Соленость PSU
 * @param isCmdModeByDefault Режим команды по умолчанию
 * @param isACKOnTxFinished Подтверждение завершения передачи
 * @param gravityAcc Гравитационное ускорение
 * @return Указатель на строку с запросом
 */
size_t queryForSettingsUpdate(char *sendline, uint8_t txChID, uint8_t rxChID, float salinityPSU,
            bool isCmdModeByDefault, bool isACKOnTxFinished, float gravityAcc);

/**
 * @brief Запрос конфигурации данных об окружающей среде (PUWV6)
 * @param isSaveToFlash Сохранить в flash
 * @param periodMs Период в миллисекундах
 * @param isPressure Давление
 * @param isTemperature Температура
 * @param isDepth Глубина
 * @param isVCC Электропитание
 * @return Указатель на строку с запросом
 */
size_t queryForAmbientDataConfig(char *sendline, bool isSaveToFlash, int periodMs, bool isPressure,
                                bool isTemperature, bool isDepth, bool isVCC);

/**
 * @brief Отключение приёма данных об окружающей среде
 * @return Указатель на строку с запросом
*/
size_t queryForAbortAmbientData(char *sendline);

/**
 * @brief Командный запрос удаленного модема (PUWV2)
 * Ответ
 * @param txChID ID канала передачи
 * @param rxChID ID канала приема
 * @param cmdID ID команды (RC_PING, RC_DPT_GET,...,RC_USR_CMD_007)
 * @return Указатель на строку с запросом
 */
size_t queryRemoteModem(char *sendline, uint8_t txChID, uint8_t rxChID, char cmdID);

/**
 * @brief Запрос настроек режима пакета (PUWVD)
 * Ответ (PUWVE)
 * @return Указатель на строку с запросом
 */
size_t queryForPktModeSettings(char *sendline);

/**
 * @brief Запрос обновления настроек режима пакета (PUWVF)
 * @param isSaveToFlash Сохранить в flash
 * @param isPktMode Режим пакета
 * @param localAddress Локальный адрес
 * @return Указатель на строку с запросом
 */
size_t queryForPktModeSettingsUpdate(char *sendline, bool isSaveToFlash, bool isPktMode,
                                                    uint8_t localAddress);

/**
 * @brief Запрос на остановку отправки пакета (PUWVG)
 * @return Указатель на строку с запросом
 */
size_t queryForPktAbortSend(char *sendline);

/**
 * @brief Запрос на отправку пакета (PUWVG)
 * @param targetPktAddress Целевой адрес пакета
 * @param maxTries Максимальное количество попыток
 * @param data Данные
 * @return Указатель на строку с запросом
 */
size_t queryForPktSend(char *sendline, uint8_t targetPktAddress, uint8_t maxTries, char* data);

/**
 * @brief Запрос пакета данных датчиков с удалённого модема (PUWVK)
 * @param targetPktAddress Целевой адрес пакета
 * @param dataID ID данных (0-Глубина, 1-температура, 2-батарея)
 * @return Указатель на строку с запросом
 */
size_t queryForPktITG(char *sendline, uint8_t targetPktAddress, uint8_t dataID);

/**
 * @brief парсинг строки
 * @param log_fptr
 * @param puwv
 * @param test_str
 * @param mine_id
 * @return int
 */
int parse_line(/*FILE *log_fptr,*/ puwv_t *puwv, char *test_str, int *mine_id);

void parse_send_p_command(char* command_line, uint8_t* addr, uint8_t* num_tries, char* message);

void parse_ambient_data(char* command_line, bool* isSaveToFlash, int* period, 
                        bool* isPressure, bool* isTemperature, bool* isDepth, bool* isVCC);

void parse_itg_command(char* command_line, int* addr, int* dataID);

void puwv2str(puwv_t puwv, int command, char *out_buffer);

int puwv_parser(char *buffer, int *mine_id);

void print_values(puwv_t puwv, int command);

#endif // _PUWV_PARSER_H