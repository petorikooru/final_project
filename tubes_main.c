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
            term_clean();
            print_info("Entering message debug mode...");
            sleep(1); fflush(stdout);
            debug_message();

            log_print_info("Exiting message debug mode...");
        } else if (strcmp(argv[1], "order") == 0) {
            term_clean();
            print_info("Entering order debug mode...");
            sleep(1); fflush(stdout);
            debug_order();

            log_print_info("Exiting message debug mode...");
        } else if (strcmp(argv[1], "manajemen") == 0) {
            term_clean();
            print_info("Entering manajemen debug mode...");
            sleep(1); fflush(stdout);
            debug_manajemen();

            log_print_info("Exiting manajemen debug mode...");
        } else if (strcmp(argv[1], "admin") == 0) {
            term_clean();
            print_info("Entering admin debug mode...");
            sleep(1); fflush(stdout);
            debug_admin();

            log_print_info("Exiting admin debug mode...");
        } else {
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
        display_utama();
    }
}

void debug_message(){
    typedef enum {
        M_ADMIN = 1,
        M_USER  = 2,
        M_EXIT  = 0,
    } menu_t;

    char username[MAX_STRLEN];
    int choice;

    while (true) {
        term_clean();

        bool status = draw_init(CENTER_CENTER, 1, 1, WIDTH, 8);
        if (status == false) return;

        draw_box(TITLE, YEL, "Message Debug");
        draw_line(LEFT, YEL, 1, YEL"Select the user type:");
        draw_line(LEFT, YEL, 0, "1: Broker");
        draw_line(LEFT, YEL, 0, "2: User");
        draw_line(LEFT, YEL, 1, RED"0: Exit");
        draw_decor(YEL);
        draw_input(YEL, 0, "Input:");
        draw_end();

        input_number(&choice);

        switch(choice) {
            case M_ADMIN:
                term_clean()

                draw_init(CENTER_CENTER, 1, 1, WIDTH, 3);
                draw_box(TITLE, YEL, "Message Debug");
                draw_input(YEL, 0, "User you want to chat: ");
                draw_end();

                input_string(username);
                display_pesan_start(username, BROKER);

                break;
            case M_USER:
                term_clean()

                draw_init(CENTER_CENTER, 1, 1, WIDTH, 3);
                draw_box(TITLE, YEL, "Message Debug");
                draw_input(YEL, 0, "Who are you: ");
                draw_end();

                input_string(username);
                display_pesan_start(username, USER);

                break;
            case M_EXIT:
                return;
            default:
                log_print_err("Invalid input! Press enter to continue...");
                getchar();
                break;
        }
    }
}

void debug_order(){
    display_stok_start();
}

void debug_manajemen(){
    display_user_start();
}

void debug_admin(){
    display_admin_start();
}

void display_utama(){
    typedef enum {
        M_USER  = 1,
        M_ADMIN = 2,
        M_EXIT  = 0,
    } menu_t;
    int choice;

    while (true) {
        term_clean();

        bool status = draw_init(CENTER_CENTER, 1, 1, WIDTH, 11);
        if (status == false) return;

        draw_box(TITLE, BLU, "D'Milsurp");
        draw_line(LEFT, BLU, 3, BLU"Selamat datang di "CYN"D'Milsurp"BLU", dimana anda dapat");
        draw_line(LEFT, BLU, 1, BLU"membeli barang permainan yang menyenangkan!");
        draw_line(LEFT, BLU, 1, MAG"Silahkan pilih menu dibawah!");
        draw_decor(BLU);
        draw_line(LEFT, BLU, 0, "1. Pergi ke menu USER");
        draw_line(LEFT, BLU, 0, "2. Pergi ke menu ADMIN");
        draw_line(LEFT, BLU, 1, RED"0. Keluar");
        draw_decor(BLU);
        draw_input(BLU, 0, "Input:");
        draw_end();

        input_number(&choice);

        switch(choice) {
            case M_USER:
                display_user_start();
                break;
            case M_ADMIN:
                log_print_todo("Admin login page");
                break;
            case M_EXIT:
                return;
            default:
                log_print_err("Invalid input! Press enter to continue...");
                getchar();
                break;
        }
    }
}
