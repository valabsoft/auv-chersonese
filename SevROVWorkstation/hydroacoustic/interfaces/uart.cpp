#include "uart.h"


void list_available_ports(){
    HDEVINFO hDevInfo;
    SP_DEVINFO_DATA DeviceInfoData;
    DWORD i;

    hDevInfo = SetupDiGetClassDevs(&GUID_DEVCLASS_PORTS, 0, 0, DIGCF_PRESENT);
    if (hDevInfo == INVALID_HANDLE_VALUE){
        fprintf(stderr, "Error check avaliable ports\n");
    }

    // Enumerate through all devices in Set.
    DeviceInfoData.cbSize = sizeof(SP_DEVINFO_DATA);
    for (i = 0; SetupDiEnumDeviceInfo(hDevInfo, i, &DeviceInfoData); i++) {
        DWORD DataT;
        LPTSTR buffer = NULL;
        DWORD buffersize = 0;

        // Call function with null to begin with, then use the returned buffer size
        // to allocate the buffer. Keep calling until success or an unknown failure.
        while (!SetupDiGetDeviceRegistryProperty(hDevInfo, &DeviceInfoData, SPDRP_FRIENDLYNAME, &DataT, (PBYTE)buffer, buffersize, &buffersize)) {
            if (GetLastError() == ERROR_INSUFFICIENT_BUFFER) {
                // Change the buffer size.
                if (buffer) LocalFree(buffer);
                // Allocate new buffer.
                buffer = (LPTSTR)LocalAlloc(LPTR, buffersize);
            } else {
                // Insert error handling here.
                fprintf(stderr, "Error with buffer\n");
                break;
            }
        }

        printf("Port: %s\n", buffer);

        if (buffer) LocalFree(buffer);
    }

    if (GetLastError() != NO_ERROR && GetLastError() != ERROR_NO_MORE_ITEMS) {
        // Insert error handling here.
        fprintf(stderr, "Error\n");
    }

    //  Cleanup
    SetupDiDestroyDeviceInfoList(hDevInfo);
}

HANDLE init_serial(char* portName){
    HANDLE h_serial = CreateFile(portName, GENERIC_READ|GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);
    if (h_serial == INVALID_HANDLE_VALUE){
        fprintf(stderr, "Error opening serial port\n");
        return NULL;
    }

    DCB dcb_serial_params = {0};
    dcb_serial_params.DCBlength = sizeof(dcb_serial_params);
    if (!GetCommState(h_serial, &dcb_serial_params)){
        fprintf(stderr, "Error getting serial port state\n");
        CloseHandle(h_serial);
        return NULL;
    }

    dcb_serial_params.BaudRate = CBR_9600;
    dcb_serial_params.ByteSize = 8;
    dcb_serial_params.StopBits = ONESTOPBIT;
    dcb_serial_params.Parity = NOPARITY;

    if (!SetCommState(h_serial, &dcb_serial_params)){
        fprintf(stderr, "Error setting serial port state\n");
        CloseHandle(h_serial);
        return NULL;
    }

    COMMTIMEOUTS timeouts = {0};
    timeouts.ReadIntervalTimeout = 50;
    timeouts.ReadTotalTimeoutConstant = 50;
    timeouts.ReadTotalTimeoutMultiplier = 10;
    timeouts.WriteTotalTimeoutConstant = 50;
    timeouts.WriteTotalTimeoutMultiplier = 10;

    if (!SetCommTimeouts(h_serial, &timeouts)) {
        fprintf(stderr, "Error setting timeouts\n");
        CloseHandle(h_serial);
        return NULL;
    }

    // Очистка буферов ввода-вывода
    if (!PurgeComm(h_serial, PURGE_RXCLEAR | PURGE_TXCLEAR)) {
        fprintf(stderr, "Error clearing serial port buffers\n");
        CloseHandle(h_serial);
        return NULL;
    }

    return h_serial;
}

DWORD uart_read(HANDLE h_serial, char* buffer, DWORD buffer_size) {
    static char internal_buffer[4096] = {0};  // Внутренний буфер для накопления данных
    static size_t buffer_position = 0;        // Текущая позиция в буфере

    OVERLAPPED osReader = { 0 };
    osReader.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

    if (osReader.hEvent == NULL) {
        fprintf(stderr, "Error creating overlapped event\n");
        return 0;
    }

    DWORD bytes_read = 0;
    if (!ReadFile(h_serial, internal_buffer + buffer_position, sizeof(internal_buffer) - buffer_position - 1, &bytes_read, &osReader)) {
        if (GetLastError() != ERROR_IO_PENDING) {
            fprintf(stderr, "Error reading from serial port\n");
            CloseHandle(osReader.hEvent);
            return 0;
        }
        WaitForSingleObject(osReader.hEvent, INFINITE);
        GetOverlappedResult(h_serial, &osReader, &bytes_read, FALSE);
    }

    if (bytes_read > 0) {
        buffer_position += bytes_read;
        internal_buffer[buffer_position] = '\0';

        char *last_line_end = NULL;  // Указатель на последнюю завершённую команду

        // Поиск последней завершённой строки
        char *line_end = internal_buffer;
        while ((line_end = strstr(line_end, "\r\n")) != NULL) {
            last_line_end = line_end;
            line_end += 2;  // Переход на символ после \r\n
        }

        if (last_line_end != NULL) {
            size_t line_length = last_line_end - internal_buffer;

            if (line_length < buffer_size - 1) {
                memcpy(buffer, internal_buffer, line_length);
                buffer[line_length] = '\0';

                buffer_position -= (last_line_end + 2 - internal_buffer);
                memmove(internal_buffer, last_line_end + 2, buffer_position);
                memset(internal_buffer + buffer_position, 0, sizeof(internal_buffer) - buffer_position);

                CloseHandle(osReader.hEvent);
                return line_length;
            } else {
                fprintf(stderr, "Error: buffer overflow\n");
                CloseHandle(osReader.hEvent);
                return 0;
            }
        }
    }

    CloseHandle(osReader.hEvent);
    return 0;
}


DWORD uart_send(HANDLE h_serial, char* message) {
    DWORD bytes_written = 0;
    OVERLAPPED osWriter = { 0 };
    osWriter.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

    if (osWriter.hEvent == NULL) {
        fprintf(stderr, "Error creating overlapped event\n");
        return 0;
    }

    char dup[1024] = {0};
    strcpy(dup, message);

    if (strstr(dup, "PUWV") == NULL)
        printf("Message not recognized [UART]\n");

    char *istr = strstr(dup, "0x");
    if (istr != NULL) {
        istr = strtok(istr, "*");
        char dst[64] = {0};
        char src[128] = {0};
        strcpy(src, istr);

        char charIn[3] = {0}, charOut[3] = {0};
        for (int i = 1; i < (int)(strlen(src) / 2); i++) {
            charIn[0] = src[2 * i];
            charIn[1] = src[2 * i + 1];
            int num = (int)strtol(charIn, NULL, 16);
            sprintf(charOut, "%c", num);
            dst[i - 1] = charOut[0];
        }
    }

    int message_length = strlen(message);
    char new_message[1024] = {0};

    // Проверка и добавление \r\n к сообщению
    if (message_length >= 2 && message[message_length - 2] == '\r' && message[message_length - 1] == '\n') {
        strcpy(new_message, message);
        printf("<< [Correct UART]: %s\n", new_message);
    } else {
        strcpy(new_message, message);
        strcat(new_message, "\r\n");
        printf("<< [UART - without /r/n]: %s", new_message);
    }

    if (!WriteFile(h_serial, new_message, strlen(new_message), &bytes_written, &osWriter)) {
        if (GetLastError() != ERROR_IO_PENDING) {
            fprintf(stderr, "Error writing to serial port\n");
            CloseHandle(osWriter.hEvent);
            return 0;
        }
    }

    WaitForSingleObject(osWriter.hEvent, INFINITE);
    GetOverlappedResult(h_serial, &osWriter, &bytes_written, FALSE);

    CloseHandle(osWriter.hEvent);
    return bytes_written;
}

void close_serial(HANDLE h_serial){
    CloseHandle(h_serial);
}
