#ifndef SOCKET__H
#define SOCKET__H

#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#include <windows.h>
#include <string.h>


#define BUFFER_SIZE 1024
#define CONFIG_FILE "config.txt"


void confirm_connection(char ip[], int *port);

int load_config(char *ip, int *port);
void save_config(const char *ip, int port);

SOCKET init_socket(char *ip, int *port, struct sockaddr_in *serverAddr);

size_t read_udp(int clientSocket, struct sockaddr_in *serverAddr, char *buffer);
size_t send_udp(int clientSocket, struct sockaddr_in *serverAddr, const char *message);

#endif // SOCKET__H
