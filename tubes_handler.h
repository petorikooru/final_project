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

#define print_err(text)     fprintf(stderr, RED "ERROR: " RST text "\n")
#define print_info(text)    fprintf(stderr, BLU "INFO: " RST text "\n")
#define print_warn(text)    fprintf(stderr, YEL "WARN: " RST text "\n")

#endif
