#ifndef UART_H
#define UART_H

#include <stdio.h>
#include <windows.h>
#include <initguid.h>
#include <setupapi.h>

DEFINE_GUID(GUID_DEVCLASS_PORTS, 0x4d36e978, 0xe325, 0x11ce, 0xbf, 0xc1, 0x08, 0x00, 0x2b, 0xe1, 0x03, 0x18);


HANDLE init_serial(char* portName);
DWORD uart_read(HANDLE h_serial, char *rcv_buffer, DWORD rcv_buffer_size);
DWORD uart_send(HANDLE h_serial, char *send_buffer);
void close_serial(HANDLE h_serial);

void list_available_ports();

#endif // UART_H
