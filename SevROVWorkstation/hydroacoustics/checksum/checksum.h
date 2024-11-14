#ifndef __CHECKSUM_H
#define __CHECKSUM_H

/** @cond */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/** @endcond */

unsigned int sum_checksum(char *str);
unsigned char xor_checksum(char* str);
void print_in_hex(char* str);

#endif //__CHECKSUM_H