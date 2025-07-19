#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <pthread.h>
#include <unistd.h>

#include "tubes_main.h"
#include "tubes_handler.h"
#include "tubes_input.h"
#include "tubes_manajemen.h"
#include "tubes_message.h"
#include "tubes_stok.h"

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
        } else {
            printf( "Usage: ./tubes.bin <debug_mode>\n"
                    "+ message\t: Run message debug mode \n"
                    "+ order\t\t: Run order debug mode \n"
                    "+ manajemen\t: Run manajemen debug mode \n"
                    "No argument: Run normal mode \n"
            );
        }
    } else {
        print_info("Running the program normally...");
        sleep(1); fflush(stdout);

        tampilan_utama();
    }
}

void debug_message(){
    int choice;

    while (true) {
        term_clean();

        printf("Select the user type:\n");
        printf("0: Broker\n");
        printf("1: User\n");
        printf("Your choice: ");
        scanf("%d", &choice); getchar();

        switch(choice) {
            case BROKER:
                debug_message_as_broker();
                break;
            case USER:
                debug_message_as_user();
                break;
            default:
                return;
        }
    }
}

void debug_message_as_broker(){
    char username[256];
    char teks[256];
    pthread_t thread;

    printf("User you want to contact: ");
    fgets(username, sizeof(username), stdin);

    pesan_init(username, BROKER);

    pthread_create(&thread, NULL, pesan_update, NULL);
    while (true) {
        input_buffer_save();
        input_buffer_copy(teks);

        if (strcmp(teks, "/exit") == 0) break;

        pesan_kirim(teks);
    }
    pesan_end();
    pthread_join(thread, NULL);
}

void debug_message_as_user(){
    char username[256];
    char teks[256];
    pthread_t thread;

    printf("Who are you?: ");
    fgets(username, sizeof(username), stdin);

    pesan_init(username, USER);

    pthread_create(&thread, NULL, pesan_update, NULL);
    while (true) {
        input_buffer_save();
        input_buffer_copy(teks);

        if (strcmp(teks, "/exit") == 0) break;

        pesan_kirim(teks);
    }
    pesan_end();
    pthread_join(thread, NULL);
}

void debug_order(){
    stok_init();
}

void debug_manajemen(){
    manajemen_init();
}

void tampilan_utama(){

}
