#include <stdlib.h>
#include "tubes_interface.h"

#ifndef HANDER_H
#define HANDLER_H

#define MAX_STRLEN 256

#ifdef _WIN32
    #define term_clean() system("cls");
#else
    #define term_clean() system("clear");
#endif

#define print_err(text)     fprintf(stderr, RED "ERROR: " RST text)
#define print_info(text)    fprintf(stderr, BLU "INFO: " RST text)
#define print_warn(text)    fprintf(stderr, YEL "WARN: " RST text)
#define print_todo(text)    fprintf(stderr, MAG "TODO: " RST text); exit(0);

#define log_print_def()         SET_OFFSET_ERR(1, 18); fprintf(stderr, YEL "<-- Debug Message -->" RST)
#define log_print_err(text)     SET_OFFSET_ERR(1, 19); print_err(text)
#define log_print_info(text)    SET_OFFSET_ERR(1, 19); print_info(text)
#define log_print_warn(text)    SET_OFFSET_ERR(1, 19); print_warn(text)
#define log_print_todo(text)    SET_OFFSET_ERR(1, 19); print_todo(text)

#endif
