#include <stdint.h>
#include <stdbool.h>

#ifndef TUI_H
#define TUI_H

/* =============== TUI =============== */
/*
    You can set the default settings
    of your ui here
*/
#define WIDTH       57
#define HEIGHT      10
#define OFFSET_X    1
#define OFFSET_Y    1

/* ============= Colors ============== */
/*
    I love Eye Candy
*/
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
#define PNK             "\e[91m"
#define LME             "\e[92m"
#define RST             "\e[00m"

/* ============= Cursor ============== */
/*
    Cursor controls
*/
#define SET_COLOR(color)        printf("%s", (color))
#define SET_CURSOR(cursor)      printf("%s", (cursor))
#define SET_OFFSET(x, y)        printf("\e[%d;%dH", (y), (x))
#define SET_OFFSET_ERR(x,y)     fprintf(stderr, "\e[%d;%dH", (y), (x))

#define CURSOR_UP       "\e[A"
#define CURSOR_DOWN     "\n"
#define CURSOR_DOWN_ALT "\e[B"
#define CURSOR_LEFT     "\b"
#define CURSOR_RIGHT    " "
#define CURSOR_HIDE     "\e[?25l"
#define CURSOR_SHOW     "\e[?25h"
#define CURSOR_START    "\r"
#define CURSOR_HOME     "\e[H"

typedef enum {
    TITLE,
    PLAIN,
} box_t;

typedef enum {
    LEFT,
    CENTER,
    RIGHT,
} align_t;

/* ====================================== NEW IMPLEMENTATION ====================================== */

/*
 * Initialize terminal drawing stuff.
 *
 * offset_x : x cordinate position
 *
 * offset_y : y cordinate position
 *
 * width    : drawable width size
 *
 * height   : drawable height size
 */
bool draw_init( const uint8_t offset_x, const uint8_t offset_y,
                const uint8_t width, const uint8_t height);

/*
 * End the drawing session
 */
void draw_end();

/*
 * Draw a box
 *
 * type     : type of box it will generates (PLAIN, TITLE)
 *
 * color    : the color of the box
 *
 * format   : "%s" "%i" stuff, just insert the text here or something
 */
void draw_box(const box_t type, const char* color, const char* format, ...);

/*
 * Draw a line with text
 *
 * alig     : alignment type (LEFT, CENTER, RIGHT)
 *
 * color    : the color of the box
 *
 * count    : number of color uses in the string (RED, BLUE, etc...)
 *
 * format   : "%s" "%i" stuff, just insert the text here or something
 */
void draw_line(const align_t align, const char* color, const uint8_t count, const char* format, ...);

/*
 * Draw a line with text, but as a user input
 *
 * alig     : alignment type (LEFT, CENTER, RIGHT)
 *
 * color    : the color of the box
 *
 * count    : number of color uses in the string (RED, BLUE, etc...)
 *
 * format   : "%s" "%i" stuff, just insert the text here or something
 */
void draw_input(const char* color, const uint8_t count, const char* format, ...);

/*
 * Draw a line decoration
 *
 * color    : the color of the box
 */
void draw_decor(const char* color);

/*
 * Change the offset of current line
 *
 * newline   : how much do you want to change
 */
void draw_change_current_line(uint8_t newline);

/* ====================================== OLD IMPLEMENTATION ====================================== */
/*
    Draw a text in (x, y) coordinate as you wish

    offset_x : x cordinate position
    offset_y : y cordinate position
    color    : box's color
    format   : the text format ("%s", "%d", etc.)
    ...      : contents
*/
void screen_draw_raw(const uint8_t offset_x, const uint8_t offset_y,
                     const char* color, const char* format, ...);

/*
    Draw a plain box in your terminal screen

    offset_x : x cordinate position
    offset_y : y cordinate position
    size_x   : box's width
    size_y   : box's height
    color    : box's color
*/
void screen_draw_box(const uint8_t offset_x, const uint8_t offset_y,
                     const uint8_t size_x, const uint8_t size_y,
                     const char* color);

/*
    Draw a identifiable box in your terminal screen

    offset_x : x cordinate position
    offset_y : y cordinate position
    size_x   : box's width
    size_y   : box's height
    title    : box's title
    color    : box's color
*/
void screen_draw_box_title(const uint8_t offset_x, const uint8_t offset_y,
                           const uint8_t size_x, const uint8_t size_y,
                           const char* title, const char* color);

/*
    Draw a line segment of the box with contents

    offset_x : x cordinate position
    offset_y : y cordinate position
    size_x   : line's width
    string   : contents
    string_color    : contents's color
    border_color    : box's color
*/
void screen_draw_line(const uint8_t offset_x, const uint8_t offset_y,
                      const uint8_t size_x, const char* string,
                      const char* string_color, const char* border_color);

/*
    Draw a line segment of the box with contents, but as an input
    for the user

    offset_x : x cordinate position
    offset_y : y cordinate position
    size_x   : line's width
    string   : contents
    string_color    : contents's color
    border_color    : box's color
*/
void screen_draw_line_input(const uint8_t offset_x, const uint8_t offset_y,
                            const uint8_t size_x, const char* string,
                            const char* string_color, const char* border_color);

/*
    Draw a line segment of the box with contents centered

    offset_x : x cordinate position
    offset_y : y cordinate position
    size_x   : line's width
    string   : contents
    string_color    : contents's color
    border_color    : box's color
*/
void screen_draw_line_center(const uint8_t offset_x, const uint8_t offset_y,
                             const uint8_t size_x, const char *string,
                             const char *string_color, const char *border_color);

/*
    Draw the top/bottom side of the box

    offset_x : x cordinate position
    offset_y : y cordinate position
    size_x   : line's width
    border_color    : box's color
*/
void screen_draw_line_decor(const uint8_t offset_x, const uint8_t offset_y,
                            const uint8_t size_x, const char* border_color);

/*
    Draw the minified loading bar (just for decoration though)

    offset_x : x cordinate position
    offset_y : y cordinate position
*/
void screen_loading_mini(const uint8_t offset_x, const uint8_t offset_y, const uint16_t time);

/*
    Draw the regular loading bar (just for decoration though)

    offset_x : x cordinate position
    offset_y : y cordinate position
    width    : line's width
    time     : time in miliseconds
*/
void screen_loading_default(const uint8_t offset_x, const uint8_t offset_y,
                            const uint8_t width, const uint16_t time);

/*
    Draw the nuke loading bar (think fast!)

    offset_x : x cordinate position
    offset_y : y cordinate position
    width    : line's width
    time     : time in miliseconds
*/
void screen_loading_nuke(const uint8_t offset_x, const uint8_t offset_y,
                         const uint8_t widt, const uint16_t time);

#endif
