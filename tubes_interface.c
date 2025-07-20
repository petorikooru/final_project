/* =========== Libraries ============= */
/*
    // <- already defined in another
          file
*/
#include <stdarg.h> // ::{va_list, va_start, vprintf, va_end}
#include <stdlib.h> // ::{malloc, free}
#include <stdint.h> // ::{uint8_t}
#include <stdio.h>  // ::{printf, fflush}
#include <string.h> // ::{strlen, strcpy, strcat}
#include <unistd.h> // ::{usleep}

#include "tubes_interface.h"

/* =========== Loading Bar =========== */
/*
    Customize how long your loading
    bar will be!!!!
*/
#define MILISECONDS 1000
#define HOLD_DELAY  750

/* ============ Functions ============ */

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
