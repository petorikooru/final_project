#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <pthread.h>
#include <unistd.h>

#include "tubes_interface.h"
#include "tubes_main.h"
#include "tubes_handler.h"
#include "tubes_input.h"
#include "tubes_manajemen.h"
#include "tubes_message.h"
#include "tubes_stok.h"
#include "tubes_admin.h"

int main(int argc, char *argv[]){
    if (argc > 1) {
        if (strcmp(argv[1], "message") == 0) {
            print_info("Entering message debug mode...");
            sleep(1); fflush(stdout);
            debug_message();

            print_info("Exiting message debug mode...");
        } else if (strcmp(argv[1], "order") == 0) {
            print_info("Entering order debug mode...");
            sleep(1); fflush(stdout);
            debug_order();

            print_info("Exiting message debug mode...");
        } else if (strcmp(argv[1], "manajemen") == 0) {
            print_info("Entering manajemen debug mode...");
            sleep(1); fflush(stdout);
            debug_manajemen();

            print_info("Exiting manajemen debug mode...");
        } else if (strcmp(argv[1], "admin") == 0) {
            print_info("Entering admin debug mode...");
            sleep(1); fflush(stdout);
            debug_admin();

            print_info("Exiting admin debug mode...");
        }
        else {
            printf(
                "Usage: ./tubes.bin <debug_mode>\n"
                "+ message\t: Run message debug mode \n"
                "+ order\t\t: Run order debug mode \n"
                "+ manajemen\t: Run manajemen debug mode \n"
                "+ admin\t\t: Run admin debug mode \n"
                "No argument: Run normal mode \n"
            );
        }
    } else {
        print_info("Running the program normally...");
        sleep(1); fflush(stdout);

        display_utama();
    }
}

void debug_message(){
    char username[MAX_STRLEN];
    int choice;

    while (true) {
        term_clean();

        printf("Select the user type:\n");
        printf("1: Broker\n");
        printf("2: User\n");
        printf("Your choice: ");
        input_number(&choice);

        switch(choice) {
            case 1:
                printf("User you want to contact: ");
                input_string(username);
                display_pesan_start(username, BROKER);

                break;
            case 2:
                printf("Who are you: ");
                input_string(username);
                display_pesan_start(username, USER);

                break;
            default:
                return;
        }
    }
}

void debug_order(){
    display_stok_start();
}

void debug_manajemen(){
    display_manajemen_start();
}

void debug_admin(){
    display_admin_start();
}

void display_utama(){
    int choice;

    while (true) {
        term_clean();

        screen_draw_box_title(OFFSET_X, OFFSET_Y, WIDTH, HEIGHT, "D'Milsurp", BLU);
        input_number(&choice);

        switch(choice) {
            case 1:

                break;
            case 2:

                break;
            default:
                return;
        }
    }
}
