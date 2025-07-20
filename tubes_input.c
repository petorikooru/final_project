#include <stdio.h>
#include <string.h>

#include "tubes_input.h"
#include "tubes_handler.h"

void input_string(char string[]) {
    fgets(string, MAX_STRLEN, stdin);
    string[strlen(string) - 1] = '\0';
}

void input_number(int *integer) {
    char input[MAX_STRLEN];
    input_string(input);

    sscanf(input, "%i", integer);
}

/* Buffer Functions */
static char buffer[256];

void input_buffer_load() {
    printf("%s", buffer);
    return;
}

void input_buffer_save() {
    int i = 0;
    int end = 0;
    char key;

    input_buffer_reset();

    while (end != 1) {
        key = getchar();
        switch (key) {
            // Enter keycode
            case 10:
                end = 1;
                break;

            // Backspace keycode
            case '\b':
            case 127:
                i--;
                if (i != 0) printf("\b");
                break;

            default:
                buffer[i] = (char)key;
                i++;
                break;
        }
    }
}

void input_buffer_reset() {
    memset(buffer, 0, sizeof(buffer));
}

void input_buffer_copy(char *string){
    strncpy(string, buffer, MAX_STRLEN);
}
