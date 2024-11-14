#include "puwv_parser.h"

static char err_descript[][500] = {
    "Запрос принят",
    "Ошибка синтаксиса",
    "Запрос не поддерживается",
    "Передатчик занят",
    "Указанный параметр вне допустимого диапазона",
    "Неверный запрос",
    "Неизвестный идентификатор",
    "Запрошенный параметр недоступен в данный момент",
    "Приемник занят (ожидается ответ удаленной системы)",
    "Буфер передатчика полон",
    "Ошибка контрольной суммы",
    "Акустический передатчик закончил передачу сообщения",
    "Устройство переходит в режим STAND-BY",
    "Устройство вышло из режима STAND-BY",
    "Напряжение питание слишком высоко (более 13 вольт) и усилитель мощности не будет использоваться во избежание его выхода из строя",
};

static char rc_descript[][500] = {
    "Ping",
    "Pong",
    "Запрос глубины удаленного абонента",
    "Запрос температуры удаленного абонента",
    "Запрос напряжения питания удаленного абонента",
    "Удаленная система ответила - запрос не поддерживается",
    "Удаленная система ответила - запрос принят",
    "Пользовательская команда",
    "Пользовательская команда",
    "Пользовательская команда",
    "Пользовательская команда",
    "Пользовательская команда",
    "Пользовательская команда",
    "Пользовательская команда",
    "Пользовательская команда",
    "Пользовательская команда",
    "Входящее сообщение в режиме прозрачного канал"
};

static char cmd_names[][30] = {
    "ACK",
    "SET_WRITE",
    "RC_REQUEST",
    "RC_RESPONSE",
    "RC_TIMEOUT",
    "RC_ASYNC_IN",
    "AMB_DATA_CFG",
    "AMB_DATA",
    "SETTING_READ",
    "D2H_PT_SETTINGS",
    "H2D_PT_SETTINGS_WRITE",
    "H2H_PT_SETTINGS_WRITE",
    "PT_SEND",
    "PT_FAILED",
    "PT_DELIVERED",
    "PT_RECIEVED",
    "PT_ITG",
    "PT_TMO",
    "PT_ITG_RESP",
    "DINFO_GET",
    "DINFO",
};

static char cmd_descript[][100] = {
    "Реакция на поступивший запрос",
    "Запись новых настроек",
    "Удалённая команда",
    "Ответ УК",
    "Таймаут УК",
    "Вход. УК запрос",
    "Настройка параметров среды и питания",
    "Параметры среды и питания",
    "SETTING_READ",
    "Ответ: настройки пакетного режима",
    "Настройка пакетного режима",
    "Настройка пакетного режима",
    "Послать пакет данных",
    "Пакет не доставлен",
    "Пакет доставлен",
    "Получен пакет",
    "Запрос удалённого абонента",
    "Превышение интервала ожидания на запрос",
    "Ответ на запрос удалённого абонента",
    "Запрос инфо об устройстве",
    "Инфо об устройстве",
};


void parse_send_p_command(char* command_line, uint8_t* addr, uint8_t* num_tries, char* message) {
    char command_copy[BUFFER_SIZE];
    strcpy(command_copy, command_line);
    char tokens[4][128];
    int t = 0;

    char* token = strtok(command_copy, ",");

    while (token != NULL && t<5) {
        strcpy(tokens[t], token);
        t++;
        token = strtok(NULL, ",");
    }

    *addr        = atoi(tokens[2]);
    *num_tries   = atoi(tokens[3]);
    strcpy(message, tokens[4]);
}

void parse_ambient_data(char* command_line, bool* isSaveToFlash, int* period,
                        bool* isPressure, bool* isTemperature, bool* isDepth, bool* isVCC){

    char command_copy[BUFFER_SIZE];
    strcpy(command_copy, command_line);
    char tokens[7][128];
    int t = 0;

    char* token = strtok(command_copy, ",");

    while (token != NULL && t < 7) {
        strcpy(tokens[t], token);
        t++;
        token = strtok(NULL, ",");
    }

    *isSaveToFlash        = atoi(tokens[1]);
    *period               = atoi(tokens[2]);
    *isPressure           = atoi(tokens[3]);
    *isTemperature        = atoi(tokens[4]);
    *isDepth              = atoi(tokens[5]);
    *isVCC                = atoi(tokens[6]);
}

void puwv2str(puwv_t puwv, int command, char *out_buffer) {
    char data[64];
    if (command == ACK) {
        printf("\n\t\t\tAcknowledge from remote device\n");
        printf(" Command ID: %s\n Error code: %u\n", puwv.ack.cmdID, puwv.ack.errCode);
    } else if (command == RC_RESPONSE) {
        printf("\n\t\t\tRemote command response\n");
        char code[20];
        if (puwv.rc_resp.rcCmdID == 1) {
            strcpy(code, "PONG");
        } else {
            strcpy(code, "UNRECOGNIZED");
        }
        printf(" Tx channel ID: %u\n Remote command ID: %u (%s)\n Propagation time: %f\n MSR: %f\n Requested value: %f\n",
               puwv.rc_resp.txChID, puwv.rc_resp.rcCmdID, code, puwv.rc_resp.propTime, puwv.rc_resp.MSR, puwv.rc_resp.value);
    } else if (command == RC_TIMEOUT) {
        printf("\n\t\t\tThe remote subscriber did not respond for the request\n");
        printf(" Tx channel ID: %u\n Remote command ID: %u\n", puwv.rc_timeout.txChID, puwv.rc_timeout.rcCmdID);
    } else if (command == RC_ASYNC_IN) {
        printf("\n\t\t\tIncoming code message from a remote subscriber\n");
        printf(" Command ID: %u\n MSR: %f\n Azimuth (supported in USBL modems): %f\n",
               puwv.rc_async_in.rcCmdID, puwv.rc_async_in.MSR, puwv.rc_async_in.azimuth);
    } else if (command == DINFO) {
        printf("\n\t\t\tResponse to device information request: \n");
        printf(" Serial num: %s\n Moniker: %s\n Version: %s\n Core moniker: %s\n Core version: %s\n Baudrate: %f\n",
               puwv.dinfo.serial_number, puwv.dinfo.system_moniker, puwv.dinfo.system_version, puwv.dinfo.core_moniker,
               puwv.dinfo.core_version, puwv.dinfo.acBaudrate);
        printf(" Rx channel ID: %u\n Tx channel ID: %u\n Max channels: %i\n PSU: %f\n Temperature/Depth sensor: %u\n Command mode: %u\n",
               puwv.dinfo.rxChID, puwv.dinfo.txChID, puwv.dinfo.maxCh, puwv.dinfo.PSU, puwv.dinfo.isTempDepthAcces, puwv.dinfo.isCmdMode);
    } else if (command == D2H_PT_SETTINGS) {
        printf("\n\t\t\tPacket mode settings\n");
        printf(" PT mode status: %i\n Local address in PT mode: %i\n", puwv.pt_set.isPTMode, puwv.pt_set.ptLocalAddress);
    } else if (command == PT_FAILED) {
        printf("\n\t\t\tThe transmission of the data packet was not successful\n");
        memset(data, 0, sizeof(data));
        hex2str(data, puwv.failed.dataPacket);
        printf(" Target address: %u\n Max tries: %u\n Data: %s\n",
               puwv.failed.target_ptAddress, puwv.failed.maxTries, data);
    } else if (command == PT_DELIVERED) {
        printf("\n\t\t\tPacket was delivered\n");
        memset(data, 0, sizeof(data));
        hex2str(data, puwv.dlvrd.dataPacket);
        printf(" Target address: %u\n Max tries: %u\n Azimuth: %f\n Data: %s\n",
               puwv.dlvrd.target_ptAddress, puwv.dlvrd.maxTries, puwv.dlvrd.azimuth, data);
    } else if (command == PT_RECIEVED) {
        printf("\n\t\t\tPacket was received\n");
        memset(data, 0, sizeof(data));
        hex2str(data, puwv.rcvd.dataPacket);
        printf(" Sender address: %u\n Data: %s\n",
               puwv.rcvd.sender_ptAddress, data);
        sprintf(out_buffer, "%i;%s", puwv.rcvd.sender_ptAddress, data);

    } else if (command == PT_ITG) {
        printf("\n\t\t\tTimeout for a response to a request with logical addressing\n");
        printf(" Target address: %u\n Data ID: %u\n", puwv.itg_tmo.target_ptAddress, puwv.itg_tmo.dataID);
    } else if (command == PT_ITG_RESP) {
        printf("\n\t\t\tRemote subscriber response with logical addressing\n");
        printf(" Target address: %u\n Data ID: %u\n Requested value: %f\n Propagation time: %f\n Azimuth: %f\n",
               puwv.itg_resp.target_ptAddress, puwv.itg_resp.dataID, puwv.itg_resp.dataValue, puwv.itg_resp.pTime, puwv.itg_resp.azimuth);
    }
}

int puwv_parser(char *buffer, int *mine_id){
    puwv_t puwv;
    int command = parse_line(&puwv, buffer, mine_id);
    puwv2str(puwv, command, buffer);
    return command;
}

void print_values(puwv_t puwv, int command) {
    char data[64];
    if (command == ACK) {
        printf("\nAcknowledge from remote device\n");
        printf(" Command ID: %s\n Error code: %u\n", puwv.ack.cmdID, puwv.ack.errCode);
    } else if (command == RC_RESPONSE) {
        printf("\nRemote command response\n");
        char code[20];
        if (puwv.rc_resp.rcCmdID == 1) {
            strcpy(code, "PONG");
        } else {
            strcpy(code, "UNRECOGNIZED");
        }
        printf(" Tx channel ID: %u\n Remote command ID: %u (%s)\n Propagation time: %f\n MSR: %f\n Requested value: %f\n",
               puwv.rc_resp.txChID, puwv.rc_resp.rcCmdID, code, puwv.rc_resp.propTime, puwv.rc_resp.MSR, puwv.rc_resp.value);
    } else if (command == RC_TIMEOUT) {
        printf("\nThe remote subscriber did not respond for the request\n");
        printf(" Tx channel ID: %u\n Remote command ID: %u\n", puwv.rc_timeout.txChID, puwv.rc_timeout.rcCmdID);
    } else if (command == RC_ASYNC_IN) {
        printf("\nIncoming code message from a remote subscriber\n");
        printf(" Command ID: %u\n MSR: %f\n Azimuth (supported in USBL modems): %f\n",
               puwv.rc_async_in.rcCmdID, puwv.rc_async_in.MSR, puwv.rc_async_in.azimuth);
    } else if (command == DINFO) {
        printf("\nResponse to device information request: \n");
        printf(" Serial num: %s\n Moniker: %s\n Version: %s\n Core moniker: %s\n Core version: %s\n Baudrate: %f\n",
               puwv.dinfo.serial_number, puwv.dinfo.system_moniker, puwv.dinfo.system_version, puwv.dinfo.core_moniker,
               puwv.dinfo.core_version, puwv.dinfo.acBaudrate);
        printf(" Rx channel ID: %u\n Tx channel ID: %u\n Max channels: %i\n PSU: %d\n Temperature/Depth sensor: %u\n Command mode: %u\n",
               puwv.dinfo.rxChID, puwv.dinfo.txChID, puwv.dinfo.maxCh, puwv.dinfo,
               puwv.dinfo.PSU, puwv.dinfo.isTempDepthAcces, puwv.dinfo.isCmdMode);
    } else if (command == D2H_PT_SETTINGS) {
        printf("\nPacket mode settings\n");
        printf(" PT mode status: %i\n Local address in PT mode: %i\n", puwv.pt_set.isPTMode, puwv.pt_set.ptLocalAddress);
    } else if (command == PT_FAILED) {
        printf("\nThe transmission of the data packet was not successful\n");
        memset(data, 0, sizeof(data));
        hex2str(data, puwv.failed.dataPacket);
        printf(" Target address: %u\n Max tries: %u\n Data: %s\n",
               puwv.failed.target_ptAddress, puwv.failed.maxTries, data);
    } else if (command == PT_DELIVERED) {
        printf("\nPacket was delivered\n");
        memset(data, 0, sizeof(data));
        hex2str(data, puwv.dlvrd.dataPacket);
        printf(" Target address: %u\n Max tries: %u\n Azimuth: %f\n Data: %s\n",
               puwv.dlvrd.target_ptAddress, puwv.dlvrd.maxTries, puwv.dlvrd.azimuth, data);
    } else if (command == PT_RECIEVED) {
        printf("\nPacket was received\n");
        memset(data, 0, sizeof(data));
        hex2str(data, puwv.rcvd.dataPacket);
        printf(" Sender address: %u\n Azimuth: %f\n Data: %s\n",
               puwv.rcvd.sender_ptAddress, puwv.rcvd.azimuth, data);
    } else if (command == PT_ITG) {
        printf("\nTimeout for a response to a request with logical addressing\n");
        printf(" Target address: %u\n Data ID: %u\n", puwv.itg_tmo.target_ptAddress, puwv.itg_tmo.dataID);
    } else if (command == PT_ITG_RESP) {
        printf("\nRemote subscriber response with logical addressing\n");
        printf(" Target address: %u\n Data ID: %u\n Requested value: %f\n Propagation time: %f\n Azimuth: %f\n",
               puwv.itg_resp.target_ptAddress, puwv.itg_resp.dataID, puwv.itg_resp.dataValue, puwv.itg_resp.pTime, puwv.itg_resp.azimuth);
    } else {
        printf("\nMessage unrecognized\n\n");
    }
}

void Str_WriteHexByte(uint8_t* buffer, uint8_t* srcIdx, uint8_t c)
{
    buffer[*srcIdx] = DIGIT_2HEX(c / 16);
    (*srcIdx)++;
    buffer[*srcIdx] = DIGIT_2HEX(c % 16);
    (*srcIdx)++;
}

void hex2str(char dst[uWAVE_PKT_MAX_SIZE], char src[uWAVE_PKT_MAX_SIZE]){
    char charIn[3] = {0}; char charOut[3] = {0};
    for(int i = 1; i < (int)(strlen(src)/2); i++){
        charIn[0] = src[2*i]; charIn[1] = src[2*i+1];
        int num = (int)strtol(charIn, NULL, 16);
        sprintf(charOut,"%c",num);
        dst[i-1] = charOut[0];
    }
}

void str2hex(char *dst, char *src){
    char character[3] = {0};
    for(int i = 0; i<(int)strlen(src);i++){
        sprintf(character,"%X",src[i]);
        strcat(dst,character);
    }
}

// Парсер строки, сверка чексумм
/*
char* puwv_line_parser(char *input_line){
    char *dup = strdup(input_line);
    char *message_start = strchr(dup, NMEA_SNT_STR);
    char *message_end = strchr(dup, NMEA_CHK_SEP);

    if (message_start != NULL && message_end != NULL) {
        char *hash = strtok(dup, "*");
        hash = strtok(NULL, "*");
        (void)hash;
        *message_end = '\0';
        //printf("Checksum from mess: \t%s", hash);
        //printf("XOR checksum is \t%02X\n", xor_checksum(message_start+1));
        return message_start+1;
    } else {
        printf("WRONG MESSAGE FORMAT!\n");
        return NULL;
    }
    free(dup);
}
*/

char* puwv_line_parser(char *input_line){

    char *dup = strdup(input_line);
    char *message_start = strchr(dup, NMEA_SNT_STR);
    char *message_end = strchr(dup, NMEA_CHK_SEP);

    if (message_start != NULL && message_end != NULL) {
        char *hash = strtok(dup, "*");
        hash = strtok(NULL, "*");
        //free(dup);
        if (hash != NULL) {
            *message_end = '\0';
            return message_start+1; // Всё, что после "$" и до "*"
        }
    } else {
        printf("WRONG MESSAGE FORMAT!\n");
    }
    free(dup); // Освобождаем память, выделенную для копии строки в случае ошибки
    return NULL;
}

char *stok(const char *src, size_t len, const char token) {
    static size_t idx = 0;
    char buf[len];
    size_t size;
    for (size = 0; idx < len; idx++) {
        if(src[idx] == token){
            if(size != 0)
                buf[size+1] = '\0';
            idx++; break;
        }
        buf[size++] = src[idx];
    }
    if (size == 0) return NULL;

    char *res = malloc(size + 1);
    strcpy(res, buf);
    return res;
}

int token_parser(char *input_line, char tokens[MAX_TOKENS][uWAVE_IN_BUFFER_SIZE]) {
    char *dup = strdup(input_line);
    char *result;
    int t = 0;


    result = strtok(dup, ","); // На Windows
    while (result != NULL && t < MAX_TOKENS) {
        strcpy(tokens[t], result);
        t++;
        result = strtok(NULL, ",");
    }

    free(dup);
    return t;
}

int command_definer(char *input_line, void *out){
    puwv_ack_t                      ack;
    puwv_amb_dta_cfg_t              amb_dta_cfg;
    puwv_amb_dta_t                  amb_dta;
    puwv_dinfo_t                    dinfo;
    puwv_h2d_pt_settings_write_t    h2d_set_write;
    puwv_h2h_pt_settings_write_t    h2h_set_write;
    puwv_pt_dlvrd_t                 dlvrd;
    puwv_pt_failed_t                failed;
    puwv_pt_itg_resp_t              itg_resp;
    puwv_pt_itg_t                   itg;
    puwv_pt_itg_tmo_t               itg_tmo;
    puwv_pt_rcvd_t                  rcvd;
    puwv_pt_send_t                  pt_send;
    puwv_pt_settings_t              pt_set;
    puwv_rc_async_in_t              rc_async_in;
    puwv_rc_request_t               rc_request;
    puwv_rc_response_t              rc_resp;
    puwv_rc_timeout_t               rc_timeout;
    uint8_t                         reversed_aqpng, reversed_dinfo_get, reversed_setting_read;
    char symbol;
    char tokens[MAX_TOKENS][uWAVE_IN_BUFFER_SIZE];
    int num_tokens = token_parser(input_line, tokens);


    (void)ack; (void)amb_dta_cfg;
    (void)amb_dta; (void)dinfo;
    (void)h2d_set_write; (void)h2h_set_write;
    (void)dlvrd; (void)failed;
    (void)itg_resp; (void)itg;
    (void)itg_tmo; (void)rcvd;
    (void)pt_send; (void)pt_set;
    (void)rc_async_in; (void)rc_request;
    (void)rc_resp; (void)rc_timeout;
    (void)reversed_aqpng; (void)reversed_dinfo_get;
    (void)reversed_setting_read;

    // Выделение символа после PUWV
    if (num_tokens > 0) {
        char *first_token = tokens[0];

        if (strstr(first_token, "PUWV") != NULL) {
            symbol = first_token[strlen(first_token) - 1];
        } else {
            printf("ОШИБКА:НЕ ТОТ ФОРМАТ КОМАНДЫ!\n");
            return -2;
        }
    } else{
        printf("ОШИБКА:НЕ НАЙДЕНЫ ДАННЫЕ\n");
        return -1;
    }

    // Парсер параметров
    if (symbol == IC_D2H_ACK){
        strcpy(ack.cmdID, tokens[1]);
        ack.errCode                     = atoi(tokens[2]);
        memcpy(out, &ack, sizeof(puwv_ack_t));
        return ACK;

    } else if (symbol == IC_H2D_SETTINGS_WRITE){
        h2d_set_write.rxChId            = atoi(tokens[1]);
        h2d_set_write.txChId            = atoi(tokens[2]);
        h2d_set_write.styPSU            = atof(tokens[3]);
        h2d_set_write.isCmdMode         = atoi(tokens[4]);
        h2d_set_write.isACK0nTXFinished = atoi(tokens[5]);
        h2d_set_write.gravityAcc        = atof(tokens[6]);
        memcpy(out, &h2d_set_write, sizeof(puwv_h2d_pt_settings_write_t));
        return H2D_PT_SETTINGS_WRITE;

    } else  if (symbol == IC_H2D_RC_REQUEST){
        rc_request.txChID               = atoi(tokens[1]);
        rc_request.rxChID               = atoi(tokens[2]);
        rc_request.rcCmdID              = atoi(tokens[3]);
        memcpy(out, &rc_request, sizeof(puwv_rc_request_t));
        return RC_REQUEST;

    } else if (symbol == IC_D2H_RC_RESPONSE){
        rc_resp.txChID                  = atoi(tokens[1]);
        rc_resp.rcCmdID                 = atoi(tokens[2]);
        rc_resp.propTime                = atof(tokens[3]);
        rc_resp.MSR                     = atof(tokens[4]);
        rc_resp.value                   = atof(tokens[5]);
        rc_resp.azimuth                 = atof(tokens[6]);
        memcpy(out, &rc_resp, sizeof(puwv_rc_response_t));
        return RC_RESPONSE;

    } else if (symbol == IC_D2H_RC_TIMEOUT){
        rc_timeout.txChID               = atoi(tokens[1]);
        rc_timeout.rcCmdID              = atoi(tokens[2]);
        memcpy(out, &rc_resp, sizeof(puwv_rc_timeout_t));
        return RC_TIMEOUT;
    }
    if (symbol == IC_D2H_RC_ASYNC_IN){
        rc_async_in.rcCmdID             = atoi(tokens[1]);
        rc_async_in.MSR                 = atof(tokens[2]);
        rc_async_in.azimuth             = atof(tokens[3]);
        memcpy(out, &rc_resp, sizeof(puwv_rc_async_in_t));
        return RC_ASYNC_IN;

    } else if (symbol == IC_H2D_AMB_DTA_CFG){
        amb_dta_cfg.isSaveToFlash       = atoi(tokens[1]);
        amb_dta_cfg.PeriodMs            = atoi(tokens[2]);
        amb_dta_cfg.isPressure          = atoi(tokens[3]);
        amb_dta_cfg.isTemperature       = atoi(tokens[4]);
        amb_dta_cfg.isDepth             = atoi(tokens[5]);
        amb_dta_cfg.isVCC               = atoi(tokens[6]);
        memcpy(out, &amb_dta_cfg, sizeof(puwv_amb_dta_cfg_t));
        return AMB_DATA_CFG;

    } else if (symbol == IC_D2H_AMB_DTA){
        amb_dta.pressure_mBar           = atof(tokens[1]);
        amb_dta.temperature_C           = atof(tokens[2]);
        amb_dta.Depth_m                 = atof(tokens[3]);
        amb_dta.VCC_V                   = atof(tokens[4]);
        memcpy(out, &amb_dta, sizeof(puwv_amb_dta_t));
        return AMB_DATA;

    } else if (symbol == IC_H2D_PT_SETTINGS_READ){
        reversed_setting_read           = atoi(tokens[1]);
        memcpy(out, &reversed_setting_read, sizeof(uint8_t));
        return SETTING_READ;

    } else if (symbol == IC_D2H_PT_SETTINGS){
        pt_set.isPTMode                 = atoi(tokens[1]);
        pt_set.ptLocalAddress           = atoi(tokens[2]);
        memcpy(out, &pt_set, sizeof(puwv_pt_settings_t));
        return D2H_PT_SETTINGS;

    } else if (symbol == IC_H2H_PT_SETTINGS_WRITE){
        h2h_set_write.isSaveToFlash     = atoi(tokens[1]);
        h2h_set_write.isPTMode          = atoi(tokens[2]);
        h2h_set_write.ptLocalAddress    = atoi(tokens[3]);
        memcpy(out, &h2h_set_write, sizeof(puwv_h2h_pt_settings_write_t));
        return H2H_PT_SETTINGS_WRITE;

    } else if (symbol == IC_H2D_PT_SEND){
        pt_send.target_ptAddress        = atoi(tokens[1]);
        pt_send.maxTries                = atoi(tokens[2]);
        strcpy(pt_send.dataPacket,             tokens[3]);
        memcpy(out, &pt_send, sizeof(puwv_pt_send_t));
        return PT_SEND;

    } else if (symbol == IC_D2H_PT_FAILED){
        failed.target_ptAddress         = atoi(tokens[1]);
        failed.maxTries                 = atoi(tokens[2]);
        strcpy(failed.dataPacket,              tokens[3]);
        memcpy(out, &failed, sizeof(puwv_pt_failed_t));
        return PT_FAILED;

    } else if (symbol == IC_D2H_PT_DLVRD){
        dlvrd.target_ptAddress          = atoi(tokens[1]);
        dlvrd.maxTries                  = atoi(tokens[2]);
        dlvrd.azimuth                   = atof(tokens[3]);
        if (strstr(tokens[4], "0x") != NULL){
            strcpy(dlvrd.dataPacket,           tokens[4]);
        } else {
            strcpy(dlvrd.dataPacket,           tokens[3]);
        }
        memcpy(out, &dlvrd, sizeof(puwv_pt_dlvrd_t));
        return PT_DELIVERED;

    } else if (symbol == IC_D2H_PT_RCVD){
        rcvd.sender_ptAddress           = atoi(tokens[1]);
        strcpy(rcvd.dataPacket,                tokens[2]);
        memcpy(out, &rcvd, sizeof(puwv_pt_rcvd_t));
        return PT_RECIEVED;

    } else if (symbol == IC_H2D_PT_ITG){
        itg.target_ptAddress            = atoi(tokens[1]);
        itg.dataID                      = atoi(tokens[2]);
        memcpy(out, &itg, sizeof(puwv_pt_itg_t));
        return PT_ITG;

    } else if (symbol == IC_D2H_PT_TMO){
        itg_tmo.target_ptAddress        = atoi(tokens[1]);
        itg_tmo.dataID                  = atoi(tokens[2]);
        memcpy(out, &itg_tmo, sizeof(puwv_pt_itg_tmo_t));
        return PT_TMO;

    } else if (symbol == IC_D2H_PT_ITG_RESP){
        itg_resp.target_ptAddress       = atoi(tokens[1]);
        itg_resp.dataID                 = atoi(tokens[2]);
        itg_resp.dataValue              = atof(tokens[3]);
        itg_resp.pTime                  = atof(tokens[4]);
        itg_resp.azimuth                = atof(tokens[5]);
        memcpy(out, &itg_resp, sizeof(puwv_pt_itg_resp_t));
        return PT_ITG_RESP;

    } else if (symbol == IC_H2D_DINFO_GET){
        reversed_dinfo_get              = atoi(tokens[1]);
        memcpy(out, &reversed_dinfo_get, sizeof(uint8_t));
        return DINFO_GET;

    } else if (symbol == IC_D2H_DINFO){
        dinfo.serial_number             = strdup(tokens[1]);
        dinfo.system_moniker            = strdup(tokens[2]);
        dinfo.system_version            = strdup(tokens[3]);
        dinfo.core_moniker              = strdup(tokens[4]);
        dinfo.core_version              = strdup(tokens[5]);
        dinfo.acBaudrate                = atof(tokens[6]);
        dinfo.rxChID                    = atoi(tokens[7]);
        dinfo.txChID                    = atoi(tokens[8]);
        dinfo.maxCh                     = atoi(tokens[9]);
        dinfo.PSU                       = atof(tokens[10]);
        dinfo.isTempDepthAcces          = atoi(tokens[11]);
        dinfo.isCmdMode                 = atoi(tokens[12]);

        memcpy(out, &dinfo, sizeof(puwv_dinfo_t));
        return DINFO;
    }
    //Можно добавить, новые команды
    return 0;
}

void cmd_sw_log(FILE* fptr, int command){
    char cmd_log[200];
    sprintf(cmd_log,"%-20s\t%s\n",cmd_names[command],cmd_descript[command]);
    //logger(fptr, cmd_log);
}

void ack_log(FILE* fptr, puwv_ack_t ack){
    char ack_log[1024];
    sprintf(ack_log, "%-20c\t%s\n", ack.cmdID, err_descript[ack.errCode]);
    //logger(fptr, ack_log);
}

void perform_command_switch(/*FILE *fptr,*/ puwv_t *puwv, int command, void* out) {
    puwv->last_com = command;
    //cmd_sw_log(fptr, command);
    if (command == ACK)                  {handle_ack(puwv, out); /*ack_log(fptr,puwv->ack);*/}
    if (command == AMB_DATA)             {handle_amb_data(puwv, out);}
    if (command == AMB_DATA_CFG)         {handle_amb_data_cfg(puwv, out);}
    if (command == D2H_PT_SETTINGS)      {handle_d2h_pt_settings(puwv, out);}
    if (command == DINFO)                {handle_dinfo(puwv, out);}
    if (command == DINFO_GET)            {handle_dinfo_get(puwv, out);}
    if (command == H2D_PT_SETTINGS_WRITE){handle_h2d_pt_settings_write(puwv, out);}
    if (command == H2H_PT_SETTINGS_WRITE){handle_h2h_pt_settings_write(puwv, out);}
    if (command == PT_DELIVERED)         {handle_pt_delivered(puwv, out);}
    if (command == PT_FAILED)            {handle_pt_failed(puwv, out);}
    if (command == PT_ITG)               {handle_pt_itg(puwv, out);}
    if (command == PT_ITG_RESP)          {handle_pt_itg_resp(puwv, out);}
    if (command == PT_RECIEVED)          {handle_pt_received(puwv, out); }
    if (command == PT_SEND)              {handle_pt_send(puwv, out);}
    if (command == PT_TMO)               {handle_pt_tmo(puwv, out);}
    if (command == RC_ASYNC_IN)          {handle_rc_async_in(puwv, out);}
    if (command == RC_REQUEST)           {handle_rc_request(puwv, out);}
    if (command == RC_RESPONSE)          {handle_rc_response(puwv, out);}
    if (command == RC_TIMEOUT)           {handle_rc_timeout(puwv, out);}
    if (command == SETTING_READ)         {handle_setting_read(puwv, out);}
    if (command < 0)                     {printf("ERR\n"); return;}
}

#define memcpy_puwv(puwv_param,out) memcpy(&(puwv_param), out, sizeof(puwv_param)+1);
//--------------parser------------------
void handle_ack(puwv_t *puwv, void* out){
    puwv->ack = *(puwv_ack_t *)out;
    return;
}

void handle_amb_data(puwv_t *puwv, void *out){
    puwv->amb_dta = *(puwv_amb_dta_t *)out;
    return;
}

void handle_amb_data_cfg(puwv_t *puwv, void *out){
    puwv->amb_dta_cfg = *(puwv_amb_dta_cfg_t *)out;
    return;
}

void handle_d2h_pt_settings(puwv_t *puwv, void *out){
    puwv->pt_set = *(puwv_pt_settings_t *)out;
    return;
}

void handle_dinfo(puwv_t *puwv, void *out){
    puwv->dinfo = *(puwv_dinfo_t *)out;
    return;
}

void handle_dinfo_get(puwv_t *puwv, void *out){
    puwv->reversed_dinfo_get = *(uint8_t*)out;
    return;
}

void handle_h2d_pt_settings_write(puwv_t *puwv, void *out){
    puwv->h2d_set_write = *(puwv_h2d_pt_settings_write_t *)out;
    return;
}

void handle_h2h_pt_settings_write(puwv_t *puwv, void *out){
    puwv->h2h_set_write = *(puwv_h2h_pt_settings_write_t *)out;
    return;
}

void handle_pt_itg(puwv_t *puwv, void *out){
    puwv->itg = *(puwv_pt_itg_t *)out;
    return;
}

void handle_pt_itg_resp(puwv_t *puwv, void *out){
    puwv->itg_resp = *(puwv_pt_itg_resp_t *)out;
    return;
}

void handle_pt_tmo(puwv_t *puwv, void *out){
    puwv->itg_tmo = *(puwv_pt_itg_tmo_t *)out;
    return;
}

void handle_rc_async_in(puwv_t *puwv, void *out){
    puwv->rc_async_in = *(puwv_rc_async_in_t *)out;
    printf("%s\n",                          rc_descript[puwv->rc_async_in.rcCmdID]);
    return;
}

void handle_rc_request(puwv_t *puwv, void *out){
    puwv->rc_request = *(puwv_rc_request_t *)out;
    return;
}

void handle_rc_response(puwv_t *puwv, void *out){
    puwv->rc_resp = *(puwv_rc_response_t *)out;
    // printf("УК ответ от %i: %s\nMSR=%lf\tЗначение=%lf\tВремя передачи=%lf\tАзимут=%lf\n",
    // puwv->rc_resp.txChID, rc_descript[puwv->rc_resp.rcCmdID], puwv->rc_resp.MSR,
    //                                 puwv->rc_resp.value, puwv->rc_resp.propTime,
    //                                                     puwv->rc_resp.azimuth);
    return;
}

void handle_rc_timeout(puwv_t *puwv, void *out){
    puwv->rc_timeout = *(puwv_rc_timeout_t *)out;
    return;
}

void handle_setting_read(puwv_t *puwv, void *out){
    puwv->reversed_setting_read = *(uint8_t*)out;
    return;
}

void handle_pt_received(puwv_t *puwv, void *out){
    puwv->rcvd = *(puwv_pt_rcvd_t *)out;
    hex2str(puwv->rcvd.data,puwv->rcvd.dataPacket);
    return;
}

void handle_pt_send(puwv_t *puwv, void *out){
    puwv->pt_send = *(puwv_pt_send_t *)out;
    hex2str(puwv->pt_send.data,puwv->pt_send.dataPacket);
    return;
}

void handle_pt_delivered(puwv_t *puwv, void *out){
    puwv->dlvrd = *(puwv_pt_dlvrd_t *)out;
    hex2str(puwv->dlvrd.data,puwv->dlvrd.dataPacket);
    return;
}

void handle_pt_failed(puwv_t *puwv, void *out){
    puwv->failed = *(puwv_pt_failed_t *)out;
    hex2str(puwv->failed.data,puwv->failed.dataPacket);
    return;
}

//--------------generator----------------
size_t queryForDeviceInfo(char *sendline){
    sprintf(sendline, "$PUWV?,0*27");
    return 1;
}

size_t queryForSettingsUpdate(char *sendline, uint8_t txChID, uint8_t rxChID, float salinityPSU,
                              bool isCmdModeByDefault, bool isACKOnTxFinished, float gravityAcc){
    char payload[uWAVE_OUT_BUFFER_SIZE-20] = {0};
    sprintf(payload,"PUWV1,%i,%i,%0.2f,%i,%i,%0.2f", txChID, rxChID,
            salinityPSU, isCmdModeByDefault, isACKOnTxFinished, gravityAcc);
    sprintf(sendline,"$%s*%02X\r\n",payload,xor_checksum(payload));
    return 1;
}

size_t queryForAmbientDataConfig(char *sendline, bool isSaveToFlash, int periodMs, bool isPressure,
                                 bool isTemperature, bool isDepth, bool isVCC){
    char payload[uWAVE_OUT_BUFFER_SIZE-20] = {0};
    sprintf(payload,"PUWV6,%d,%d,%d,%d,%d,%d", isSaveToFlash, periodMs,
            isPressure, isTemperature, isDepth, isVCC);
    sprintf(sendline,"$%s*%02X\r\n",payload,xor_checksum(payload));
    return 1;
}

size_t queryRemoteModem(char *sendline, uint8_t txChID, uint8_t rxChID, char cmdID){
    char payload[uWAVE_OUT_BUFFER_SIZE-20] = {0};
    sprintf(payload,"PUWV2,%d,%d,%d", txChID, rxChID, cmdID);
    sprintf(sendline,"$%s*%02X",payload,xor_checksum(payload));
    return 1;
}

size_t queryForPktModeSettings(char *sendline){
    sprintf(sendline, "$PUWVD,0*5C");
    return 1;
}

size_t queryForPktModeSettingsUpdate(char *sendline, bool isSaveToFlash, bool isPktMode,
                                     uint8_t localAddress){
    char payload[uWAVE_OUT_BUFFER_SIZE-20] = {0};
    sprintf(payload,"PUWVF,%d,%d,%d", isSaveToFlash, isPktMode, localAddress);
    sprintf(sendline,"$%s*%2X\r\n",payload,xor_checksum(payload));
    return 1;
}

size_t queryForPktAbortSend(char *sendline){
    char payload[uWAVE_OUT_BUFFER_SIZE-20] = {0};
    sprintf(payload,"PUWVG,,,");
    sprintf(sendline,"$%s*%2X\r\n",payload,xor_checksum(payload));
    return 1;
}

size_t queryForPktSend(char *sendline, uint8_t targetPktAddress, uint8_t maxTries, char* data){
    if(strlen(data)>=uWAVE_PKT_MAX_SIZE-1){
        printf("Wrong payload size (must be less)\n");
        return 0;
    }
    char payload[500] = {0};
    char packet_payload[uWAVE_OUT_BUFFER_SIZE] = {0}; //uWAVE_PKT_MAX_SIZE
    char HEX[uWAVE_OUT_BUFFER_SIZE-2] = {0}; //uWAVE_PKT_MAX_SIZE
    str2hex(HEX, data);
    sprintf(packet_payload,"0x%s",HEX);
    sprintf(payload,"PUWVG,%d,%d,%s", targetPktAddress, maxTries, packet_payload);
    sprintf(sendline,"$%s*%2X",payload,xor_checksum(payload));
    return 1;
}

size_t queryForPktITG(char *sendline, uint8_t targetPktAddress, uint8_t dataID){
    char payload[uWAVE_OUT_BUFFER_SIZE-20] = {0};
    sprintf(payload,"PUWVK,%d,%d", targetPktAddress, dataID);
    sprintf(sendline,"$%s*%2X\r\n",payload,xor_checksum(payload));
    return 1;
}

size_t queryForAbortAmbientData(char *sendline){
    sprintf(sendline, "$PUWV6,0,0,0,0,0,0*32\r\n");
    return 1;
}

int parse_line(/*FILE *log_fptr,*/ puwv_t *puwv, char *input_str, int *mine_id) {
    //todo сделать меньше аргументов ф-ции
    char* line_res = puwv_line_parser(input_str);

    if(line_res != NULL){
        void* out = malloc(1024);
        int command = command_definer(line_res, out);
        perform_command_switch(/*log_fptr,*/ puwv, command, out);
        free(out);
        if(command == D2H_PT_SETTINGS){
            *mine_id = puwv->pt_set.ptLocalAddress;
            //printf("-----------------\n");
            //printf("Адрес устройства\t%i\n",*mine_id);
            //printf("-----------------\n");
        }
        puwv->last_com = command;
        return command;
    }
    puwv->last_com = 0;
    return 0;
}
