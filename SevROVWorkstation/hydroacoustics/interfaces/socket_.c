#include "socket_.h"

SOCKET init_socket(char *ip, int *port, struct sockaddr_in *serverAddr) {
    WSADATA wsaData;

    // Инициализация Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        fprintf(stderr, "Winsock initializing error\n");
        return INVALID_SOCKET;
    }

    // Загрузка конфигурации
    if (!load_config(ip, port)) {
        printf("Configuration not found or invalid.\nEnter the IP address of server: ");
        fgets(ip, 20, stdin);
        size_t len = strlen(ip);
        if (len > 0 && ip[len - 1] == '\n') {
            ip[len - 1] = '\0';
        }
        printf("Enter the port number: ");
        scanf("%d", port);
        getchar();
        save_config(ip, *port);
    } else {
        confirm_connection(ip, port);
    }

    // Проверка IP-адреса
    if (strlen(ip) >= 20) {
        fprintf(stderr, "Invalid IP address.\n");
        WSACleanup();
        return INVALID_SOCKET;
    }

    // Создание сокета
    SOCKET clientSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (clientSocket == INVALID_SOCKET) {
        perror("Socket creation error.");
        WSACleanup();
        return INVALID_SOCKET;
    }

    // Настройка адреса сервера
    memset(serverAddr, 0, sizeof(*serverAddr));
    serverAddr->sin_family = AF_INET;
    serverAddr->sin_port = htons(*port);
    serverAddr->sin_addr.s_addr = inet_addr(ip);

    // Подключение сокета
    if (connect(clientSocket, (struct sockaddr *)serverAddr, sizeof(*serverAddr)) < 0) {
        perror("Socked connection error.");
        closesocket(clientSocket);
        WSACleanup();
        return INVALID_SOCKET;
    }

    printf("Connected to %s:%i\n", ip, *port);
    return clientSocket;
}

void confirm_connection(char ip[], int *port) {
    char choice;
    printf("Do you want to use the existing configuration (IP: %s, Port: %d)? (Y/N): ", ip, *port);
    scanf(" %c", &choice);
    getchar();

    if (toupper(choice) == 'N') {
        printf("Enter the IP address of the server: ");
        fgets(ip, 20, stdin);

        size_t len = strlen(ip);
        if (len > 0 && ip[len - 1] == '\n') {
            ip[len - 1] = '\0';
        }

        printf("Enter the port number: ");
        scanf("%d", port);
        getchar();
        save_config(ip, *port);
    }
}

size_t send_udp(int clientSocket, struct sockaddr_in *serverAddr, const char *message) {
    size_t res = sendto(clientSocket, message, strlen(message), 0, (struct sockaddr *)serverAddr, sizeof(*serverAddr));
    return res;
}

size_t read_udp(int clientSocket, struct sockaddr_in *serverAddr, char *buffer) {
    int addr_size = sizeof(*serverAddr);
    size_t res = recvfrom(clientSocket, buffer, BUFFER_SIZE, 0, (struct sockaddr *)serverAddr, &addr_size);
    return res;
}

int load_config(char *ip, int *port) {
    FILE *file = fopen(CONFIG_FILE, "r");
    if (!file) {
        return 0; // Файл не существует или не удалось открыть
    }

    if (fscanf(file, "%s %d", ip, port) != 2) {
        fclose(file);
        return 0; // Ошибка чтения данных из файла
    }

    fclose(file);
    return 1; // Конфигурационные данные успешно загружены
}

void save_config(const char *ip, int port) {
    FILE *file = fopen(CONFIG_FILE, "w");
    if (!file) {
        fprintf(stderr, "Error when opening a file for writing\n");
        exit(1);
    }

    fprintf(file, "%s %d\n", ip, port);
    fclose(file);
}