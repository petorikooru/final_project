#include <stdlib.h>

#define CURSOR_HIDE "\e[?25l"
#define LINE_UP     "\e[A"
#define LINE_ERASE  "\e[0K"

#define BLK             "\e[30m"
#define BLK_BG          "\e[40m"
#define RED             "\e[31m"
#define RED_BG          "\e[41m"
#define GRN             "\e[32m"
#define GRN_BG          "\e[42m"
#define YEL             "\e[33m"
#define YEL_BG          "\e[43m"
#define BLU             "\e[34m"
#define BLU_BG          "\e[44m"
#define MAG             "\e[35m"
#define MAG_BG          "\e[45m"
#define CYN             "\e[36m"
#define CYN_BG          "\e[46m"
#define WHT             "\e[37m"
#define WHT_BG          "\e[47m"
#define GRY             "\e[90m"
#define GRY_BG          "\e[100m"
#define PNK             "\e[91m"
#define PNK_BG          "\e[101m"
#define LME             "\e[92m"
#define LME_BG          "\e[102m"
#define RST           "\e[0m"

#define MAX_STRLEN 256

#ifdef _WIN32
    #define term_clean() system("cls");
#else
    #define term_clean() system("clear");
#endif

#define print_err(text)     fprintf(stderr, RED "ERROR: " RST text "\n")
#define print_info(text)    fprintf(stderr, BLU "INFO: " RST text "\n")
