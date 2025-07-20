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

static uint8_t current_width;
static uint8_t current_height;
static uint8_t current_x;
static uint8_t current_y;
static uint8_t current_line;
static uint8_t check_width;
static bool    initialized;

bool draw_init( const uint8_t offset_x, const uint8_t offset_y,
                const uint8_t width, const uint8_t height){

    if (offset_x <= 0 || offset_y <= 0) {
        SET_OFFSET_ERR(1, 19);
        print_err("Draw: Offset must be greater than 0!");
        return false;
    }
    current_width   = width;
    current_height  = height;
    current_x       = offset_x;
    current_y       = offset_y;
    current_line    = offset_y;
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

static void draw_check(){
    if (current_line > current_height) {
        SET_OFFSET_ERR(1, 20);
        print_warn("Draw: Current line passes the limit of the height!");
        fprintf(stderr, "\t line: %i > height: %i", current_line, current_height);

    }
    if (check_width >= current_width) {
        SET_OFFSET_ERR(1, 22);
        print_warn("Draw: Current line passes the limit of the width!");
        fprintf(stderr, "\t checked: %i > width: %i", check_width, current_width);
    }
}

void draw_box(const box_t type, const char* color, const char* format, ...){
    if (initialized == false){
        SET_OFFSET_ERR(1, 19);
        log_print_err("Draw: Please initialize \"draw_init()\" first");
        return;
    }

    char string[LEN_MAX];
    va_list args;

    SET_OFFSET(current_x, current_y);
    SET_COLOR(color);

    if (type == TITLE) {
        va_start(args, format);
        vsprintf(string, format, args);
        va_end(args);

        const uint8_t middle = current_width / 2;
        const uint8_t title_width = strlen(string);
        const uint8_t title_offset = 2;
        const uint8_t title_placement = middle - (title_width / 2) - title_offset;

        for (uint8_t i = 0; i < current_height; i++){
            SET_OFFSET(current_x, current_y + i);

            for (uint8_t j = 0; j < current_width; j++){
                if (i == 0){
                    if (j == 0 || j == (current_width - title_width) - 4)
                        printf("+");
                    else if (j == title_placement)
                        printf("{ %s }", string);
                    else if(j < (current_width - title_width) - 4)
                        printf("-");
                    else break;
                } else if (i == current_height - 1){
                    if (j == 0 || j == current_width - 1)
                        printf("+");
                    else
                        printf("-");
                } else {
                    if (j == 0 || j == current_width - 1)
                        printf("|");
                    else
                        printf(" ");
                }
            }
            SET_CURSOR(CURSOR_DOWN);
        }
    } else {
        for (uint8_t i = 0; i < current_height; i++){
            SET_OFFSET(current_x, current_y + i);

            for (uint8_t j = 0; j < current_width; j++){
                if (i == 0 || i == current_height - 1){
                    if (j == 0 || j == current_width - 1)
                        printf("+");
                    else
                        printf("-");
                } else {
                    if (j == 0 || j == current_width - 1)
                        printf("|");
                    else
                        printf(" ");
                }
            }
            SET_CURSOR(CURSOR_DOWN);
        }
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

    SET_OFFSET(current_x, current_line);
    SET_COLOR(RST); SET_COLOR(color); printf("| ");
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

    } else if (align == RIGHT) {
        const uint8_t middle = current_width / 2;
        const uint8_t string_len = strlen(string);
        const uint8_t string_offset = 2;
        const uint8_t string_placement = middle - (string_len / 2) - string_offset;
        char* string_padding = malloc(string_placement + 1);

        for (uint8_t i = 0; i < string_placement; i++){
            string_padding[i] = ' ';
        }
        string_padding[string_placement] = '\0';

        char *string_center = malloc(string_len + string_placement + 1);
        strcpy(string_center, string_padding);
        strcat(string_center, string);

        SET_COLOR(RST); SET_COLOR(color); printf("| ");

        printf("%s", string_center);
        for (uint8_t i = 0; i < width_offset - string_len; i++) printf(" ");

        SET_COLOR(RST); SET_COLOR(color); printf(" |");

        free(string_padding);
        free(string_center);
    } else {
        print_todo("Align Right");
    }

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

    SET_OFFSET(current_x, current_line);
    SET_COLOR(color);

    const uint8_t width_offset = current_width - 4;
    const uint8_t string_len = strlen(string) - (count * 4);
    check_width = string_len;

    SET_COLOR(RST); SET_COLOR(color); printf("| ");

    printf("%s", string);
    for (uint8_t i = 0; i < width_offset - string_len; i++) printf(" ");

    SET_COLOR(RST); SET_COLOR(color); printf(" |");

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
        if (i == 0 || i == current_width - 1)
            printf("+");
        else
            printf("-");
    }
    SET_COLOR(RST);
    current_line++;
}

void draw_change_current_line(uint8_t newline){
    if (initialized == false){
        log_print_err("Draw: Please initialize \"draw_init()\" first");
        return;
    }

    if (current_line - newline <= 0){
        log_print_err("Line cannot be less than 1!");
        return;
    }
    current_line = newline;
}

/* ============ Old implementation ============ */

void screen_draw_raw(const uint8_t offset_x, const uint8_t offset_y,
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

void screen_draw_box(const uint8_t offset_x, const uint8_t offset_y,
                     const uint8_t size_x, const uint8_t size_y,
                     const char* color){

    SET_COLOR(color);

    for (uint8_t i = 0; i < size_y; i++){
        SET_OFFSET(offset_x, offset_y + i);

        for (uint8_t j = 0; j < size_x; j++){
            if (i == 0 || i == size_y - 1){
                if (j == 0 || j == size_x - 1)
                    printf("+");
                else
                    printf("-");
            } else {
                if (j == 0 || j == size_x - 1)
                    printf("|");
                else
                    printf(" ");
            }
        }
        SET_CURSOR(CURSOR_DOWN);
    }
    SET_COLOR(RST);
}

void screen_draw_box_title(const uint8_t offset_x, const uint8_t offset_y,
                           const uint8_t size_x, const uint8_t size_y,
                           const char* title, const char* color){

    SET_COLOR(color);

    const uint8_t middle = size_x / 2;
    const uint8_t title_width = strlen(title);
    const uint8_t title_offset = 2;
    const uint8_t title_placement = middle - (title_width / 2) - title_offset;

    for (uint8_t i = 0; i < size_y; i++){
        SET_OFFSET(offset_x, offset_y + i);

        for (uint8_t j = 0; j < size_x; j++){
            if (i == 0){
                if (j == 0 || j == (size_x - title_width) - 4)
                    printf("+");
                else if (j == title_placement)
                    printf("{ %s }", title);
                else if(j < (size_x - title_width) - 4)
                    printf("-");
                else break;
            } else if (i == size_y - 1){
                if (j == 0 || j == size_x - 1)
                    printf("+");
                else
                    printf("-");
            } else {
                if (j == 0 || j == size_x - 1)
                    printf("|");
                else
                    printf(" ");
            }
        }
        SET_CURSOR(CURSOR_DOWN);
    }
    SET_COLOR(RST);
}

void screen_draw_line(const uint8_t offset_x, const uint8_t offset_y,
                      const uint8_t size_x, const char* string,
                      const char* string_color, const char* border_color){

    SET_OFFSET(offset_x, offset_y);

    const uint8_t size_x_offset = size_x - 4;
    const uint8_t string_len = strlen(string);

    SET_COLOR(RST); SET_COLOR(border_color); printf("| ");
    SET_COLOR(string_color);

    printf("%s", string);
    for (uint8_t i = 0; i < size_x_offset - string_len; i++)
        printf(" ");

    SET_COLOR(RST); SET_COLOR(border_color); printf(" |");
}

void screen_draw_line_input(const uint8_t offset_x, const uint8_t offset_y,
                            const uint8_t size_x, const char* string,
                            const char* string_color, const char* border_color){

    SET_OFFSET(offset_x, offset_y);

    const uint8_t size_x_offset = size_x - 4;
    const uint8_t string_len = strlen(string);

    screen_draw_line(offset_x, offset_y, size_x, string, string_color, border_color);

    SET_COLOR(WHT);
    SET_OFFSET(string_len + offset_x + 2, offset_y);
}

void screen_draw_line_center(const uint8_t offset_x, const uint8_t offset_y,
                             const uint8_t size_x, const char *string,
                             const char *string_color, const char *border_color){

    const uint8_t middle = size_x / 2;
    const uint8_t string_len = strlen(string);
    const uint8_t string_offset = 2;
    const uint8_t string_placement = middle - (string_len / 2) - string_offset;
    char* string_padding = malloc(string_placement+1);

    for (uint8_t i = 0; i < string_placement; i++){
        string_padding[i] = ' ';
    }
    string_padding[string_placement] = '\0';

    char *string_center = malloc(string_len + string_placement + 1);
    strcpy(string_center, string_padding);
    strcat(string_center, string);

    screen_draw_line(offset_x, offset_y, size_x, string_center, string_color, border_color);

    free(string_padding);
    free(string_center);
}

void screen_draw_line_decor(const uint8_t offset_x, const uint8_t offset_y,
                            const uint8_t size_x, const char* border_color){


    SET_OFFSET(offset_x, offset_y);
    SET_COLOR(border_color);
    for (uint8_t i = 0; i < size_x; i++){
        if (i == 0 || i == size_x - 1)
            printf("+");
        else
            printf("-");
    }
    SET_COLOR(RST);
    SET_CURSOR(CURSOR_DOWN);
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
    screen_draw_line_input(offset_x, offset_y, width, "", GRN, GRN);

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

    screen_draw_line(offset_x, offset_y, width, "", GRN_BG, GRN);
    screen_draw_line_center(offset_x, offset_y, width, "Done! (≧ ∇ ≦)", GRN_BG, GRN);
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
    screen_draw_line_input(offset_x, offset_y, width, "", RED, RED);

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
    screen_draw_line_center(offset_x, offset_y, width, "Nuked! (≧ ∇ ≦)", RED_BG, RED);
    SET_CURSOR(CURSOR_SHOW);

    fflush(stdout);
    usleep(HOLD_DELAY * MILISECONDS);
}
