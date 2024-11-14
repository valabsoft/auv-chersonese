#include "checksum.h"

unsigned int sum_checksum(char *str) {
   unsigned int sum = 0;
   while (*str) {
      sum += *str;
      str++;
   } 
   return sum;
}

unsigned char xor_checksum(char* str) {
   unsigned char sum = 0;
   while(*str){
      sum ^= *str;
      str++;
   }
   return sum;
}

void print_in_hex(char* str) {
   while(*str)
      printf("%02x\t", *str++);
   printf("\n");
}