/* =========== Libraries ============= */
/*
    // <- already defined in another
          file
*/
#include <stdarg.h> // ::{va_list, va_start, vprintf, va_end}
#include <stdbool.h>
#include <stdlib.h> // ::{malloc, free}
#include <stdint.h> // ::{uint8_t}
#include <stdio.h>  // ::{printf, fflush}
#include <string.h> // ::{strlen, strcpy, strcat}
#include <unistd.h> // ::{usleep}
#include <sys/ioctl.h>

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

static uint8_t current_width;
static uint8_t current_height;
static uint8_t current_x;
static uint8_t current_y;
static uint8_t current_line;
static uint8_t check_width;
static bool    initialized;

void get_window_size(uint8_t *width, uint8_t *height){
    struct winsize ws;
	ioctl(STDIN_FILENO, TIOCGWINSZ, &ws);

	*width  = ws.ws_col / 2;
	*height = ws.ws_row / 2;
}

static void draw_check(){
    // if (current_line > current_height + current_y) {
    //     SET_OFFSET_ERR(1, 20);
    //     print_warn("Draw: Current line passes the limit of the height!");
    //     fprintf(stderr, "\t line: %i > height: %i", current_line, current_height);

    // }
    // if (check_width >= current_width + current_x) {
    //     SET_OFFSET_ERR(1, 22);
    //     print_warn("Draw: Current line passes the limit of the width!");
    //     fprintf(stderr, "\t checked: %i > width: %i", check_width, current_width);
    // }
}

bool draw_init( const offset_t offset, const uint8_t offset_x, const uint8_t offset_y,
                const uint8_t width, const uint8_t height){

    if (offset == RAW && (offset_x <= 0 || offset_y <= 0)) {
        SET_OFFSET_ERR(1, 18);
        print_err("Draw: Offset must be greater than 0!");
        return false;
    }
    current_width   = width;
    current_height  = height;

    uint8_t center_x, center_y;
    get_window_size(&center_x, &center_y);

    switch (offset) {
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
    current_width   = 0;
    current_height  = 0;
    current_x       = 0;
    current_y       = 0;
    current_line    = 0;
    check_width     = 0;
    initialized     = false;
}

void draw_box(const box_t type, const char* color, const char* format, ...){
    if (initialized == false){
        SET_OFFSET_ERR(1, 19);
        log_print_err("Draw: Please initialize \"draw_init()\" first");
        return;
    }

    char string[LEN_MAX];
    va_list args;

    va_start(args, format);
    vsprintf(string, format, args);
    va_end(args);

    const uint8_t middle = current_width / 2;
    const uint8_t title_width = strlen(string);
    const uint8_t title_offset = 2;
    const uint8_t title_placement = middle - (title_width / 2) - title_offset;

    SET_OFFSET(current_x, current_y);
    SET_COLOR(color);

    switch (type) {
        case TITLE:
            for (uint8_t i = 0; i < current_height; i++){
                SET_OFFSET(current_x, current_y + i);

                for (uint8_t j = 0; j < current_width; j++){
                    if (i == 0){
                        if (j == 0)
                            printf(SPRITE_TOP_LEFT);
                        else if (j == (current_width - title_width) - 4)
                                printf(SPRITE_TOP_RIGHT);
                        else if (j == title_placement)
                            printf("{ %s }", string);
                        else if(j < (current_width - title_width) - 4)
                            printf(SPRITE_BAR_HORIZONTAL);
                        else break;
                    } else if (i == current_height - 1){
                        if (j == 0)
                            printf(SPRITE_BOTTOM_LEFT);
                        else if (j == current_width - 1)
                            printf(SPRITE_BOTTOM_RIGHT);
                        else
                            printf(SPRITE_BAR_HORIZONTAL);
                    } else {
                        if (j == 0 || j == current_width - 1)
                            printf(SPRITE_BAR_VERTICAL);
                        else
                            printf(" ");
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
                    if (i == 0 || i == current_height - 1){
                        if (j == 0 || j == current_width - 1)
                            printf("+");
                        else
                            printf(SPRITE_BAR_HORIZONTAL);
                    } else {
                        if (j == 0 || j == current_width - 1)
                            printf("│");
                        else
                            printf(" ");
                    }
                }
                SET_CURSOR(CURSOR_DOWN);
            }
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

    char string[LEN_MAX];
    va_list args;

    va_start(args, format);
    vsprintf(string, format, args);
    va_end(args);

    SET_OFFSET(current_x + 2, current_line);
    // SET_COLOR(RST); SET_COLOR(color); printf(SPRITE_BAR_VERTICAL" ");
    if (count == 0)
        SET_COLOR(RST);
    else
        SET_COLOR(color);

    const uint8_t width_offset = current_width - 4;
    const uint8_t string_len = strlen(string) - (count * 4);
    check_width = string_len;

    if (align == LEFT) {

        printf("%s", string);
        for (uint8_t i = 0; i < width_offset - string_len; i++) printf(" ");

    } else if (align == CENTER) {
        const uint8_t middle = current_width / 2;
        const uint8_t string_len = strlen(string);
        const uint8_t string_offset = 0;
        const uint8_t string_placement = middle - (string_len / 2) - string_offset;
        char* string_padding = malloc(string_placement + 2);

        for (uint8_t i = 0; i < string_placement; i++){
            string_padding[i] = ' ';
        }
        string_padding[string_placement] = '\0';

        char *string_center = malloc(string_len + string_placement + 2);
        strcpy(string_center, string_padding);
        strcat(string_center, string);

        printf("%s", string_center);

        free(string_padding);
        free(string_center);
    } else {
        print_todo("Align Right");
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
    SET_OFFSET(string_len + current_x + 3, current_line);

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
    char string[LEN_MAX];
    va_list args;

    va_start(args, format);
    vsprintf(string, format, args);
    va_end(args);

    term_clean();
    bool status = draw_init(CENTER_CENTER, 1, 1, WIDTH, 5);
    if (status == false) return;

    draw_box(TITLE, RED, "Confirmation Dialog");
    draw_line(LEFT, RED, 1, RED"%s", string);
    draw_decor(RED);
    draw_input(RED, 1, YEL"(y/N):");
    draw_end();
}

void draw_dialog_continue(const char* format, ...){
    char string[LEN_MAX];
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
    draw_line(CENTER, YEL, 1, YEL_BG"Press enter to continue...");
    draw_end();

    while (getchar() != '\n');
    SET_CURSOR(CURSOR_SHOW);
}

void draw_dialog_err(const char* format, ...){
    char string[LEN_MAX];
    va_list args;

    va_start(args, format);
    vsprintf(string, format, args);
    va_end(args);

    term_clean();
    SET_CURSOR(CURSOR_HIDE);

    bool status = draw_init(CENTER_CENTER, 1, 1, WIDTH, 4);
    if (status == false) return;

    draw_box(TITLE, RED, "Error Dialog");
    draw_line(CENTER, RED, 1, RED"%s", string);
    draw_line(CENTER, RED, 1, RED_BG"Press enter to continue...");
    draw_end();

    while (getchar() != '\n');
    SET_CURSOR(CURSOR_SHOW);
}

void draw_dialog_info(const char* format, ...){
    char string[LEN_MAX];
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
    draw_line(CENTER, BLU, 1, BLU_BG"Press enter to continue...");
    draw_end();

    while (getchar() != '\n');
    SET_CURSOR(CURSOR_SHOW);
}

void draw_dialog_warn(const char* format, ...){
    char string[LEN_MAX];
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
    draw_line(CENTER, YEL, 1, YEL_BG"Press enter to continue...");
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
