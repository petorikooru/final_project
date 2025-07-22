/* =========== Libraries ============= */
/*
    // <- already defined in another
          file
*/
#include <stdarg.h> // ::{va_list, va_start, vprintf, va_end}
#include <stdbool.h>
#include <stdint.h> // ::{uint8_t}
#include <stdio.h>  // ::{printf, fflush}
#include <string.h> // ::{strlen, strcpy, strcat}
#include <unistd.h> // ::{usleep}

#ifdef _WIN32
    #include <windows.h>
#else
    #include <sys/ioctl.h>
#endif

#include "tubes_interface.h"
#include "tubes_handler.h"

/* =========== Loading Bar =========== */
/*
    Customize how long your loading
    bar will be!!!!
*/
#define MILISECONDS 1000
#define HOLD_DELAY  750

/* ============ New implementation ============ */
#define LEN_MAX 256

#define SPRITE_TOP_LEFT         "┌"
#define SPRITE_TOP_RIGHT        "┐"
#define SPRITE_BOTTOM_LEFT      "└"
#define SPRITE_BOTTOM_RIGHT     "┘"
#define SPRITE_BAR_VERTICAL     "│"
#define SPRITE_BAR_HORIZONTAL   "─"
#define SPRITE_T_LEFT           "├"
#define SPRITE_T_RIGHT          "┤"

static box_t    current_box_type;
static offset_t current_offset;
static uint8_t  current_width;
static uint8_t  current_height;
static uint8_t  current_x;
static uint8_t  current_y;
static uint8_t  current_line;
static char     current_color[MAX_STRLEN];
static char     current_string[MAX_STRLEN];
static uint8_t  check_width;
static uint8_t  input_x;
static uint8_t  input_y;

static bool    initialized;

void get_window_size(uint8_t *width, uint8_t *height){
#ifdef _WIN32
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    int col, row;

    if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi)) {
        col = csbi.srWindow.Right - csbi.srWindow.Left + 1;
        row = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
	*height = row / 2;
	*width = col / 2;
    } else {
        printf("Failed to get console screen buffer info.\n");
    }
#else
    struct winsize ws;
	ioctl(STDIN_FILENO, TIOCGWINSZ, &ws);

	*width  = ws.ws_col / 2;
	*height = ws.ws_row / 2;
#endif
}

static void draw_check(){
    if (current_box_type == TITLE) {
        if (current_line > current_height + current_y) {
            draw_dialog_warn("Draw: Current line passes the limit of the height!");
            draw_dialog_info("Printed Height: %i > Init Height: %i", current_line, current_height);

        }
        if (check_width >= current_width + current_x) {
            draw_dialog_warn("Draw: Current line passes the limit of the width!");
            draw_dialog_info("Printed Width: %i > Init Width: %i", check_width, current_width);
        }
    }
}


void draw_restore_input(){
    SET_OFFSET(input_x, input_y);
}

bool draw_init( const offset_t offset, const uint8_t offset_x, const uint8_t offset_y,
                const uint8_t width, const uint8_t height){

    if (offset == RAW && (offset_x <= 0 || offset_y <= 0)) {
        SET_OFFSET_ERR(1, 18);
        draw_dialog_err("Draw: Offset must be greater than 0!");
        return false;
    }
    current_width   = width;
    current_height  = height;
    current_offset  = offset;

    uint8_t center_x, center_y;
    get_window_size(&center_x, &center_y);

    switch (current_offset) {
        case TOP_LEFT:
            current_x       = 1;
            current_y       = 1;
            break;
        case TOP_CENTER:
            current_x       = center_x - (width / 2);
            current_y       = 1;
            break;
        case CENTER_LEFT:
            current_x       = 1;
            current_y       = center_y - (height / 2);
            break;
        case CENTER_CENTER:
            current_x       = center_x - (width / 2);
            current_y       = center_y - (height / 2);
            break;
        case RAW:
            current_x       = offset_x;
            current_y       = offset_y;
            break;
    }
    current_line    = current_y;
    check_width     = 0;
    initialized     = true;

    return true;
}

void draw_end(){
    if (current_box_type == TITLE_LATER){
        uint8_t center_x, center_y;
        get_window_size(&center_x, &center_y);

        if (current_offset == CENTER_CENTER) {
            current_height = current_line - center_y + 2;
        }
        draw_box(TITLE, current_color, current_string);
    }

    current_width   = 0;
    current_height  = 0;
    current_x       = 0;
    current_y       = 0;
    current_line    = 0;
    check_width     = 0;
    memset(current_color, 0, MAX_STRLEN);
    memset(current_string, 0, MAX_STRLEN);

    initialized     = false;
}

void draw_box(const box_t type, const char* color, const char* format, ...){
    if (initialized == false){
        SET_OFFSET_ERR(1, 19);
        log_print_err("Draw: Please initialize \"draw_init()\" first");
        return;
    }

    current_box_type = type;

    char string[LEN_MAX];
    va_list args;

    va_start(args, format);
    vsprintf(string, format, args);
    va_end(args);

    const uint8_t middle = current_width / 2;
    const uint8_t title_width = strlen(string);
    const uint8_t title_offset = 2;
    const uint8_t title_placement = middle - (title_width / 2) - title_offset;
    bool string_placed = false;

    SET_OFFSET(current_x, current_y);
    SET_COLOR(color);

    switch (type) {
        case TITLE:
            for (uint8_t i = 0; i < current_height; i++){
                SET_OFFSET(current_x, current_y + i);

                for (uint8_t j = 0; j < current_width; j++){
                    // Top bar
                    if (i == 0){
                        if (j == 0){
                            SET_OFFSET(j + current_x, i + current_y);
                            printf(SPRITE_TOP_LEFT);
                        }
                        else if (j == (current_width - title_width) - 4){
                            SET_OFFSET(j + current_x + title_width + 3, i + current_y);
                            printf(SPRITE_TOP_RIGHT);
                        }
                        else if (j == title_placement){
                            SET_OFFSET(j + current_x, i + current_y);
                            printf("{ %s }", string);
                            string_placed = true;
                        }
                        else if(j < (current_width - title_width) - 4){
                            if (string_placed == true)
                                SET_OFFSET(j + current_x + title_width + title_offset + 1, i + current_y);
                            else
                                SET_OFFSET(j + current_x, i + current_y);
                            printf(SPRITE_BAR_HORIZONTAL);
                        }
                        else break;
                    }
                    // Bottom bar
                    else if (i == current_height - 1){
                        if (j == 0){
                            SET_OFFSET(j + current_x, i + current_y);
                            printf(SPRITE_BOTTOM_LEFT);
                        }
                        else if (j == current_width - 1){
                            SET_OFFSET(j + current_x, i + current_y);
                            printf(SPRITE_BOTTOM_RIGHT);
                        }
                        else {
                            SET_OFFSET(j + current_x, i + current_y);
                            printf(SPRITE_BAR_HORIZONTAL);
                        }
                    }
                    // The middle
                    else {
                        if (j == 0 || j == current_width - 1){
                            SET_OFFSET(j + current_x, i + current_y);
                            printf(SPRITE_BAR_VERTICAL);
                        }
                    }
                }
                SET_CURSOR(CURSOR_DOWN);
            }
            break;

        case TITLE_NOBOX:
            SET_OFFSET(current_x, current_y);

            for (uint8_t j = 0; j < current_width; j++){
                if (j == 0)
                    printf(SPRITE_T_LEFT);
                else if (j == (current_width - title_width) - 4)
                    printf(SPRITE_T_RIGHT);
                else if (j == title_placement)
                    printf("{ %s }", string);
                else if(j < (current_width - title_width) - 4)
                    printf(SPRITE_BAR_HORIZONTAL);
            }
            break;

        case PLAIN:
            for (uint8_t i = 0; i < current_height; i++){
                SET_OFFSET(current_x, current_y + i);

                for (uint8_t j = 0; j < current_width; j++){
                    // Top Part
                    if (i == 0){
                        if (j == 0)
                            printf(SPRITE_TOP_LEFT);
                        else if (j == current_width - 1)
                            printf(SPRITE_TOP_RIGHT);
                        else
                            printf(SPRITE_BAR_HORIZONTAL);
                    }
                    // Bottom Part
                    else if (i == current_height - 1){
                        if (j == 0)
                            printf(SPRITE_BOTTOM_LEFT);
                        else if (j == current_width - 1)
                            printf(SPRITE_BOTTOM_RIGHT);
                        else
                            printf(SPRITE_BAR_HORIZONTAL);
                    }
                    // Others
                    else {
                        if (j == 0 || j == current_width - 1)
                            printf("│");
                        else
                            printf(" ");
                    }
                }
                SET_CURSOR(CURSOR_DOWN);
            }
            break;
        case TITLE_LATER:
            strncpy(current_color, color, MAX_STRLEN);
            strncpy(current_string, string, MAX_STRLEN);
            break;
    }

    SET_COLOR(RST);
    current_line++;
}

void draw_line(const align_t align, const char* color, const uint8_t count, const char* format, ...){
    if (initialized == false){
        log_print_err("Draw: Please initialize \"draw_init()\" first");
        return;
    }

    char string[MAX_STRLEN];
    va_list args;

    va_start(args, format);
    vsprintf(string, format, args);
    va_end(args);

    SET_OFFSET(current_x + 2, current_line);
    if (count == 0)
        SET_COLOR(RST WHT);
    else
        SET_COLOR(color);

    const uint8_t width_offset = current_width - 4;
    const uint8_t string_len = strlen(string) - (count * 4);
    check_width = string_len;

    if (align == LEFT) {
        printf("%s", string);

    } else if (align == CENTER) {
        const uint8_t padding = (current_width - string_len) / 2 - 2;

        for (uint8_t i = 0; i < padding; i++) {
            putchar(' ');
        }
        printf("%s", string);

    } else {
        const uint8_t padding = current_width - string_len;
        for (uint16_t i = 0; i < padding; i++) {
            putchar(' ');
        }
        printf("%s", string);
    }

    SET_OFFSET(string_len + current_x + 3, current_line);
    SET_COLOR(RST);
    draw_check();
    current_line++;
}

void draw_input(const char* color, const uint8_t count, const char* format, ...){
    if (initialized == false){
        log_print_err("Draw: Please initialize \"draw_init()\" first");
        return;
    }

    SET_OFFSET(current_x, current_line);

    char string[LEN_MAX];
    va_list args;

    va_start(args, format);
    vsprintf(string, format, args);
    va_end(args);

    SET_OFFSET(current_x + 2, current_line);
    SET_COLOR(color);

    const uint8_t width_offset = current_width - 4;
    const uint8_t string_len = strlen(string) - (count * 4);

    check_width = string_len;

    printf("%s", string);

    SET_COLOR(WHT);

    input_x = current_x + string_len + 3;
    input_y = current_line;
    SET_OFFSET(input_x, input_y);

    draw_check();
    SET_COLOR(RST);
    current_line++;
}

void draw_decor(const char* color){
    if (initialized == false){
        log_print_err("Draw: Please initialize \"draw_init()\" first");
        return;
    }

    SET_OFFSET(current_x, current_line);
    SET_COLOR(color);
    for (uint8_t i = 0; i < current_width; i++){
        if (i == 0 )
            printf(SPRITE_T_LEFT);
        else if (i == current_width - 1)
            printf(SPRITE_T_RIGHT);
        else
            printf(SPRITE_BAR_HORIZONTAL);
    }
    SET_COLOR(RST);
    current_line++;
}

void draw_change_current_line(uint8_t newline){
    if (initialized == false){
        log_print_err("Draw: Please initialize \"draw_init()\" first");
        return;
    }

    if (current_line - newline + current_y <= 0){
        log_print_err("Line cannot be less than 1!");
        return;
    }
    current_line = newline + current_y;
}

void draw_raw(  const uint8_t offset_x, const uint8_t offset_y,
                const char* color, const char* format, ...){

    va_list args;

    SET_OFFSET(offset_x, offset_y);
    SET_COLOR(color);

    va_start(args, format);
    vprintf(format, args);
    va_end(args);

    SET_COLOR(RST);

    SET_OFFSET(offset_x, offset_y);
}

void draw_dialog_confirmation(const char* format, ...){
    char string[MAX_STRLEN];
    va_list args;

    va_start(args, format);
    vsprintf(string, format, args);
    va_end(args);

    term_clean();
    bool status = draw_init(CENTER_CENTER, 1, 1, WIDTH, 4);
    if (status == false) return;

    draw_box(TITLE, YEL, "Confirmation Dialog");
    draw_line(LEFT, YEL, 1, YEL"%s", string);
    draw_input(RED, 1, YEL"(y/N):");
    draw_end();
}

void draw_dialog_continue(const char* format, ...){
    char string[MAX_STRLEN];
    va_list args;

    va_start(args, format);
    vsprintf(string, format, args);
    va_end(args);

    term_clean();
    SET_CURSOR(CURSOR_HIDE);

    bool status = draw_init(CENTER_CENTER, 1, 1, WIDTH, 4);
    if (status == false) return;

    draw_box(TITLE, YEL, "Confirmation Dialog");
    draw_line(CENTER, YEL, 1, YEL"%s", string);
    draw_line(CENTER, YEL, 2, YEL_BG BLK" Press enter to continue... ");
    draw_end();

    while (getchar() != '\n');
    SET_CURSOR(CURSOR_SHOW);
}

void draw_dialog_err(const char* format, ...){
    char string[MAX_STRLEN];
    va_list args;

    va_start(args, format);
    vsprintf(string, format, args);
    va_end(args);

    term_clean();
    SET_CURSOR(CURSOR_HIDE);
    draw_init(CENTER_CENTER, 1, 1, WIDTH, 4);

    draw_box(TITLE, RED, "Error Dialog");
    draw_line(CENTER, RED, 1, RED"%s", string);
    draw_line(CENTER, RED, 2, RED_BG WHT" Press enter to continue... ");
    draw_end();

    while (getchar() != '\n');
    SET_CURSOR(CURSOR_SHOW);
}

void draw_dialog_info(const char* format, ...){
    char string[MAX_STRLEN];
    va_list args;

    va_start(args, format);
    vsprintf(string, format, args);
    va_end(args);

    term_clean();
    SET_CURSOR(CURSOR_HIDE);

    bool status = draw_init(CENTER_CENTER, 1, 1, WIDTH, 4);
    if (status == false) return;

    draw_box(TITLE, BLU, "Info Dialog");
    draw_line(CENTER, BLU, 1, BLU"%s", string);
    draw_line(CENTER, BLU, 2, BLU_BG WHT" Press enter to continue... ");
    draw_end();

    while (getchar() != '\n');
    SET_CURSOR(CURSOR_SHOW);
}

void draw_dialog_warn(const char* format, ...){
    char string[MAX_STRLEN];
    va_list args;

    va_start(args, format);
    vsprintf(string, format, args);
    va_end(args);

    term_clean();
    SET_CURSOR(CURSOR_HIDE);

    bool status = draw_init(CENTER_CENTER, 1, 1, WIDTH, 4);
    if (status == false) return;

    draw_box(TITLE, YEL, "Warn Dialog");
    draw_line(CENTER, YEL, 1, YEL"%s", string);
    draw_line(CENTER, YEL, 2, YEL_BG BLK" Press enter to continue... ");
    draw_end();

    while (getchar() != '\n');
    SET_CURSOR(CURSOR_SHOW);
}

void screen_loading_mini(const uint8_t offset_x, const uint8_t offset_y, const uint16_t time){

    const uint8_t chunks    = time / 6;
    const char *shape[]     = {"—", "\\", "|", "/"};

    SET_CURSOR(CURSOR_HIDE);
    SET_OFFSET(offset_x, offset_y);

    printf("[ ]");
    for (uint8_t i=0; i <= chunks; i++){
        SET_OFFSET(offset_x + 1, offset_y);


        if      (i%4 == 1)  printf(YEL"%s"RST, shape[1]);
        else if (i%4 == 2)  printf(YEL"%s"RST, shape[2]);
        else if (i%4 == 3)  printf(YEL"%s"RST, shape[3]);
        else                printf(YEL"%s"RST, shape[0]);

        fflush(stdout);
        usleep(time * MILISECONDS);
    }

    SET_CURSOR(CURSOR_SHOW);
}

void screen_loading_default(const uint8_t offset_x, const uint8_t offset_y,
                            const uint8_t width, const uint16_t time){

    const uint8_t durations = 60; //In miliseconds
    const uint8_t size      = width - 6;
    const char *shape[]     = {" ", "-", "o", "c", "C"};

    //Display pre-filled bar
    SET_CURSOR(CURSOR_HIDE);

    printf("[");
    for (uint8_t i = 0; i < size; i++){
        if (i % 2 == 1) printf("%s", shape[0]);
        else            printf("%s", shape[2]);
    }
    printf("]");

    //I love eating candies
    for (uint8_t i = 1; i < size; i++){
        SET_OFFSET(offset_x + i + 2, offset_y);
        printf(YEL "%s" RST, shape[1]);

        if (i%2 >= 1)   printf(MAG"%s"RST, shape[4]);
        else            printf(MAG"%s"RST, shape[3]);

        fflush(stdout);
        usleep(durations * MILISECONDS);
    }

    SET_CURSOR(CURSOR_SHOW);

    fflush(stdout);
    usleep(HOLD_DELAY * MILISECONDS);
}

void screen_loading_nuke(const uint8_t offset_x, const uint8_t offset_y,
                         const uint8_t width, const uint16_t time){

    const uint8_t durations = 75; //In miliseconds
    const uint8_t size      = width - 6;
    const char *shape[]     = {".", "~", "/", "—", "\\", "|"};

    //Display pre-filled bar
    SET_CURSOR(CURSOR_HIDE);

    printf("[");
    for (uint8_t i = 0; i < size * 2; i++){
        if (i % 2 == 1) printf("%s", shape[0]);
    }
    printf("]");

    //Tactical Nuke Incoming!!!
    for (uint8_t i=0; i < size - 1; i++){
        SET_OFFSET(offset_x + i + 3, offset_y);
        printf(YEL "%s" RST, shape[1]);

        if      (i == size-1) printf(RED"*"RST);
        else if (i%5 == 1)  printf(RED"%s"RST, shape[2]);
        else if (i%5 == 2)  printf(RED"%s"RST, shape[3]);
        else if (i%5 == 3)  printf(RED"%s"RST, shape[4]);
        else                printf(RED"%s"RST, shape[5]);

        fflush(stdout);
        usleep(durations * MILISECONDS);
    }
    SET_CURSOR(CURSOR_SHOW);

    fflush(stdout);
    usleep(HOLD_DELAY * MILISECONDS);
}
