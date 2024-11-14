#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

#include "interfaces/socket_.h"
#include <windows.h>

#include "interfaces/uart.h"
#include "parsers/puwv_parser.h"


struct sockaddr_in serverAddr;
volatile int shouldExit = 0;

typedef struct PassThroughParams {
    SOCKET clientSocket;
    HANDLE h_serial;
} pass_through_params_t;

enum Commands {
    SEND_TEST = 1,
    SEND__CUSTOM_PACKET,
    SEND_ABORT,
    GET_DEV_INFO,
    GET_PACKET_MODE_SETTINGS,
    SEND_PING,
    AMBIENT_CONFIGURATION,
    ABORT_AMBIENT
};


void print_instructions(){
    printf("\n------------------------------Main commands---------------------------------\n");
    printf("1                                   - Send broadcast test message\n");
    printf("2,<address>,<tries_num>,<message>   - Send custom message by uWave\n");
    printf("3                                   - Abort sending message by uWave\n");
    printf("4                                   - Get information about uWave modem\n");
    printf("5                                   - Get packet mode settings\n");
    printf("6,address                           - Send PING-message\n");
    printf("7,<isSaveToFlash>,<period(ms)>,<isPressure>,<isTemperature>,<isDepth>,<isVCC> — Setting for outputting data on environmental parameters\n\n");
    printf("8                                   - Abort getting ambient data\n");

    printf("\n------------------------------USBL-------------------------------------------\n");
    printf("9,<target_address>,<dataID>         - ITG request\n");
    printf("10                                  - Abort ITG\n");
    printf("11,<isSaveToFlash>,<period(ms)>     - Pitch and roll data config\n");

    printf("\nInput message format: <interface [s/e]>,<command_number>,<other_params>\n");
    printf("For example: s,6,255 - send PING\n");
    printf("\n Type [q] to exit\n\n");
}

void print_timestamp() {
    time_t now;
    time(&now);
    struct tm* local = localtime(&now);
    printf("[%02d:%02d:%02d] ", local->tm_hour, local->tm_min, local->tm_sec);
}

void usbl_3d_pos(double azimuth, double depth, double propagation_time){
    double rs, rh, X, Y, Z;
    double c = 1500;
    rs = c * propagation_time;
    rh = sqrt(rs*rs - depth*depth);
    X = rh * sin(azimuth);
    Y = rh * cos(azimuth);
    Z = depth;

    printf("Input data (local): Azimuth - %d Depth - %d Prop time - %d\n", azimuth, depth, propagation_time);
    printf("3D coordinates: X - %d Y - %d Z - %d\n", X, Y, Z);
}

// Multiple input (Socket + Serial)
DWORD WINAPI InputThread(LPVOID lpParam) {
    pass_through_params_t* params = (pass_through_params_t*)lpParam;
    SOCKET clientSocket = params->clientSocket;
    HANDLE h_serial     = params->h_serial;

    char send_buffer[BUFFER_SIZE];
    uint8_t addr, num_tries;
    char message[BUFFER_SIZE];
    char interface_choice;
    size_t len;

    bool isSaveToFlash;
    bool isPressure;
    bool isTemperature;
    bool isDepth;
    bool isVCC;
    int period;
    int data_id;

    enum Commands command_num;

    while (!shouldExit) {
        // Получение пользовательского ввода
        if (fgets(send_buffer, BUFFER_SIZE, stdin) != NULL) {
            len = strlen(send_buffer);
            if (len > 0 && send_buffer[len - 1] == '\n') {
                send_buffer[len - 1] = '\0';
            }

            if (strcmp(send_buffer, "q") == 0) {
                printf("Program terminated at the user's request\n");
                shouldExit = 1;
                break;
            }
            if (strcmp(send_buffer, "help") == 0) {
                print_instructions();
                continue;
            }

            interface_choice = send_buffer[0]; // Анализ первого символа строки
            command_num = strtol(&send_buffer[2], NULL, 10); // Анализ номера команды

            print_timestamp();

            // Обработка команд для серийного интерфейса
            if (interface_choice == 's'){
                if (command_num > 0){
                    switch (command_num)
                    {
                    case 1: // send_test
                        memset(send_buffer, 0, BUFFER_SIZE);
                        queryForPktSend(send_buffer, 255, 3, "HYDRO_TEST");
                        uart_send(h_serial, send_buffer);
                        break;
                    case 2: // send custom message
                        parse_send_p_command(send_buffer, &addr, &num_tries, message);
                        queryForPktSend(send_buffer, addr, num_tries, message);
                        uart_send(h_serial, send_buffer);
                        break;
                    case 3: // abort send
                        break;
                    case 4:
                        memset(send_buffer, 0, BUFFER_SIZE);
                        queryForDeviceInfo(send_buffer);
                        uart_send(h_serial, send_buffer);
                        break;
                    case 5:
                        memset(send_buffer, 0, BUFFER_SIZE);
                        queryForPktModeSettings(send_buffer);
                        uart_send(h_serial, send_buffer);
                        break;
                    case 6:
                        memset(send_buffer, 0, BUFFER_SIZE);
                        queryRemoteModem(send_buffer, 0, 0, RC_PING);
                        uart_send(h_serial, send_buffer);
                        break;
                    case 7:
                        parse_ambient_data(send_buffer, &isSaveToFlash, &period, &isPressure,
                                           &isTemperature, &isDepth, &isVCC);
                        queryForAmbientDataConfig(send_buffer, isSaveToFlash, period, isPressure,
                                                  isTemperature, isDepth, isVCC);
                        uart_send(h_serial, send_buffer);
                        break;
                    case 8:
                        memset(send_buffer, 0, BUFFER_SIZE);
                        queryForAbortAmbientData(send_buffer);
                        uart_send(h_serial, send_buffer);
                        break;
                    case 9: // ITG_request
                        memset(send_buffer, 0, BUFFER_SIZE);
                        queryForPktITG(send_buffer, 1, 0);
                        uart_send(h_serial, send_buffer);
                        break;
                    case 10:

                        break;
                    default:
                        printf("Command not recognized!\n");
                        break;
                    }
                }
            } else if (interface_choice == 'e') {
                // Обработка команд для Ethernet-интерфейса
                switch (command_num)
                {
                case 1: // send_test
                    memset(send_buffer, 0, BUFFER_SIZE);
                    send_udp(clientSocket, &serverAddr, send_buffer);
                    break;
                default:
                    printf("Command not recognized!\n");
                    break;
                }
            } else {
                printf("Invalid interface choice\n");
            }
        }
        Sleep(100);  // Ожидание между циклами для экономии ресурсов процессора
    }
    return 0;
}

DWORD WINAPI SocketOutputThread(LPVOID lpParam) {
    SOCKET clientSocket = *((SOCKET*)lpParam);

    char receive_buffer[BUFFER_SIZE];
    puwv_t recv;

    while (1) {
        read_udp(clientSocket, &serverAddr, receive_buffer);
        printf("The request has been received: %s\n", receive_buffer);

        char* res = puwv_line_parser(receive_buffer);
        void* out = malloc(BUFFER_SIZE);

        if (res != NULL) {
            int command = command_definer(res, out);
            perform_command_switch(&recv, command, out);
            print_values(recv, command);
            memset(receive_buffer, 0, BUFFER_SIZE);
            free(out);
        }
        memset(receive_buffer, 0, BUFFER_SIZE);
        if (shouldExit == 1){
            break;
        }
    }

    return 0;
}

DWORD WINAPI SerialReadThread(LPVOID lpParam){
    HANDLE h_serial = *((HANDLE*)lpParam);

    char rcv_buffer[BUFFER_SIZE];
    int puwv_command;
    int mine_id;

    while(1) {
        DWORD bytesRead = uart_read(h_serial, rcv_buffer, sizeof(rcv_buffer));
        if (bytesRead > 0){
            rcv_buffer[bytesRead] = '\0';

            printf("----------------------------------------------------------------------------------\n");
            print_timestamp();
            printf(">> UART: %s\n", rcv_buffer);
            printf("----------------------------------------------------------------------------------\n");

            puwv_command = puwv_parser(rcv_buffer, &mine_id);

        }
        if (shouldExit == 1){
            break;
        }
    }

    return 0;
}

DWORD WINAPI PassThroughThread(LPVOID lpParam) {
    pass_through_params_t* params = (pass_through_params_t*)lpParam;

    char buffer[BUFFER_SIZE];
    int bytesRead;
    DWORD bytesWritten;

    while (1) {
        // Чтение данных из сокета и запись в COM-порт
        bytesRead = recv(params->clientSocket, buffer, sizeof(buffer), 0);
        if (bytesRead > 0) {
            if (!WriteFile(params->h_serial, buffer, bytesRead, &bytesWritten, NULL)) {
                fprintf(stderr, "Error writing to serial port\n");
                break;
            }
        } else if (bytesRead == 0) {
            // Соединение закрыто
            break;
        } else {
            fprintf(stderr, "Error reading from socket\n");
            break;
        }

        // Чтение данных из COM-порта и запись в сокет
        bytesRead = uart_read(params->h_serial, buffer, sizeof(buffer));
        if (bytesRead > 0) {
            if (sendto(params->clientSocket, buffer, bytesRead, 0, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
                fprintf(stderr, "Error sending data to socket\n");
                break;
            }
        } else {
            fprintf(stderr, "Error reading from serial port\n");
            break;
        }

        if (shouldExit == 1) {
            break;
        }
    }

    return 0;
}


int main(int argc, char *argv[]) {
    // SERIAL INIT
    char serial_port_name[5];

    list_available_ports();

    printf("Enter the COM port to use (e.g., COM3): ");
    if(fgets(serial_port_name, sizeof(serial_port_name), stdin) != NULL){
        size_t len = strlen(serial_port_name);
        if (len > 0 && serial_port_name[len - 1] == '\n') {
            serial_port_name[len - 1] = '\0';
        }
    }

    getchar();

    HANDLE h_serial = init_serial(serial_port_name);

    // SOCKET INIT
    char ip[20]; int port;
    SOCKET clientSocket = init_socket(ip, &port, &serverAddr);


    if (clientSocket == INVALID_SOCKET) {
        fprintf(stderr, "Socked was not created\n");
    }
    if (h_serial == NULL){
        fprintf(stderr, "Failed to initialize serial port\n");
        return 1;
    } else {
        printf("Port %s is initialized\n", serial_port_name);
        char temp[1024] = {0};
        queryForSettingsUpdate(temp,0,0,35.0,1,1,9.84);
        uart_send(h_serial, temp);

        memset(temp, 0, sizeof(temp));
        queryForPktModeSettings(temp);
        uart_send(h_serial, temp);
    }

    // Initialized packet for Raspberry
    if (argc > 1 && argv[1] == "1"){
        if (clientSocket && atoi(argv[1]) > 0) {
            unsigned char initialPacket[2] = {0xAA, 0xFF};
            sendto(clientSocket, (const char *)initialPacket, sizeof(initialPacket), 0, (struct sockaddr *)&serverAddr, sizeof(serverAddr));
            printf("Initializing packet was sent: 0xAA 0xFF\n");
        } else {
            perror("Connection error");
            closesocket(clientSocket);
            exit(4);
        }
    }

    pass_through_params_t pass_params = {clientSocket, h_serial};

    HANDLE inputThread = CreateThread(NULL, 0, InputThread, &pass_params, 0, NULL);
    HANDLE serialOutThread = CreateThread(NULL, 0, SerialReadThread, &h_serial, 0, NULL);
    HANDLE socketOutputThread = CreateThread(NULL, 0, SocketOutputThread, &clientSocket, 0, NULL);
    HANDLE passThroughThread = CreateThread(NULL, 0, PassThroughThread, &pass_params, 0, NULL);

    if (socketOutputThread == NULL){
        fprintf(stderr, "Error creating socket threads.\n");
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }

    if (serialOutThread == NULL) {
        fprintf(stderr, "Error creating serial threads.\n");
        close_serial(h_serial);
        WSACleanup();
        return 1;
    }

    if (inputThread == NULL || passThroughThread == NULL) {
        fprintf(stderr, "Error creating pass and input threads.\n");
        close_serial(h_serial);
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }

    print_instructions();

    WaitForSingleObject(inputThread, INFINITE);
    WaitForSingleObject(serialOutThread, INFINITE);
    WaitForSingleObject(socketOutputThread, INFINITE);
    WaitForSingleObject(passThroughThread, INFINITE);

    CloseHandle(inputThread);
    CloseHandle(serialOutThread);
    CloseHandle(socketOutputThread);
    CloseHandle(passThroughThread);

    close_serial(h_serial);
    closesocket(clientSocket);
    WSACleanup();

    return 0;
}
