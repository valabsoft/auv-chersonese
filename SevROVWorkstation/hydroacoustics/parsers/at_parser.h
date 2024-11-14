#pragma once
#ifndef AT_H
#define AT_H

/** @cond */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
/** @endcond */

#define MAX_COMMANDS 20
#define MAX_CHARACTERS 1024
#define NUM_COMMANDS 12

#define ACK_MASK            (1 << 0)

#define AT_COMMAND          0
#define AT_PAYLOAD_LEN      1
#define AT_SOURCE           2
#define AT_DESTINATION      3
#define AT_ACK_FLAG         4
#define AT_DURATION         5//in microseconds
#define AT_RSSI             6//in db (recieved signal strength indicator)
#define AT_INTEGRITY        7//signal distortion (the higher - the greater)
#define AT_VELOCITY         8//meter/sec
#define AT_PAYLOAD          9

#define S2C_PKT_MAX_SIZE 2048

#define fchunk(i) atof(chunks[i])
#define ichunk(i) atoi(chunks[i])
//#define NUM_COMMANDS (sizeof(str_list)/sizeof(struct at_commands))

typedef enum at_comms{RECV,
                    RECVIM,
                    DELIVERED,
                    DELIVEREDIM,
                    USBLLONG,
                    USBLANGLES,
                    OK,
                    BUSY,
                    FAILED,
                    FAILEDIM,
                    EWRONGFORMAT,
                    USBLPHYP,
                    USBLPHYD,
                    UNKNOWN,
}at_enum;

typedef struct at_commands
{
    char *str;
    at_enum en;
}at_struct;

/** @brief Функция выбора номера команды в перечислении*/
at_enum at_chooser(char chunks[MAX_COMMANDS][MAX_CHARACTERS]);
/** @brief Функция обработки команд **/
int at_handler(char input_line[MAX_CHARACTERS], int id, void *out);
//generator
int gen_message(char* generated_message, char* message, int address, int flags, int (*prot_payload)(char*, char*, int));
void at_extended(char* generated_message, bool flag);
void at_positioning(char* generated_message, bool flag);
size_t send_at(char *sendline, int addr, char* mes, int ack);
int polling(char* generated_message);

#endif // AT_H