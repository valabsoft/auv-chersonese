#ifndef COMMON_PARSER_H
#define COMMON_PARSER_H

#define M 1024
#define N 4096

int line_parser(char *input_line, char*output_lines);
int dotcomma_parser(char *input_line, char *chunks);
int dot_parser(char *input_line, char *chunks);
int comma_parser(char *input_line, char *chunks);
int grgrparenthesis_parser(char *input_line, char *chunks);
int grparenthesis_parser(char *input_line, char *chunks);

#endif // COMMON_PARSER_H
