#include "common_parser.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int line_parser(char *input_line, char* output_lines){
    char *line = {0};
    char *dup = strdup(input_line);
    int i=0;
    while((line=strtok(dup,"\n\r"))!=NULL){
        //while((line=strsep(&dup,"\n\r"))!=NULL){
        if(strcmp(line,"") == 0)
            continue;
        strcpy((char *)(output_lines + i*N),line);
        i++;
    }
    free(dup);
    return i;
}

int dotcomma_parser(char *input_line, char *chunks){
    int i = 0;
    char *dup = strdup(input_line);
    char *chunk = strtok(dup, ";");
    while(chunk != NULL){
        if(strcmp(chunk,"") == 0)
            continue;
        strcpy((char *)(chunks + i*M),chunk);
        i++;
        chunk = strtok(NULL, ";");
    }
    free(dup);
    return i;
}

int dot_parser(char *input_line, char *chunks){
    int i = 0;
    char *dup = strdup(input_line);
    char *chunk = strtok(dup, ".");
    while(chunk != NULL){
        if(strcmp(chunk,"") == 0)
            continue;
        strcpy((char *)(chunks + i*M),chunk);
        i++;
        chunk = strtok(NULL, ".");
    }
    free(dup);
    return i;
}

int comma_parser(char *input_line, char *chunks) {
    int i = 0;
    char *dup = strdup(input_line);
    char *chunk = strtok(dup, ",");
    while(chunk != NULL) {
        if(strcmp(chunk,"") == 0)
            continue;
        strcpy((char *)(chunks + i * M), chunk);
        i++;
        chunk = strtok(NULL, ",");
    }
    free(dup);
    return i;
}

int grgrparenthesis_parser(char *input_line, char *chunks) {
    int i = 0;
    char *dup = strdup(input_line);
    char *chunk = strtok(dup, ">>");
    while(chunk != NULL) {
        if(strcmp(chunk,"") == 0)
            continue;
        strcpy((char *)(chunks + i * M), chunk);
        i++;
        chunk = strtok(NULL, ">>");
    }
    free(dup);
    return i;
}

int grparenthesis_parser(char *input_line, char *chunks) {
    int i = 0;
    char *dup = strdup(input_line);
    char *chunk = strtok(dup, ">");
    while(chunk != NULL) {
        if(strcmp(chunk,"") == 0)
            continue;
        strcpy((char *)(chunks + i * M), chunk);
        i++;
        chunk = strtok(NULL, ">");
    }
    free(dup);
    return i;
}
