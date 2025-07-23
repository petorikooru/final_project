#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <pthread.h>

#include "tubes_database.h"
#include "tubes_message.h"
#include "tubes_handler.h"
#include "tubes_interface.h"
#include "tubes_input.h"

static char         current_user[MAX_STRLEN];
static struct tm    *current_timestamp;
static user_t       current_type;
static pesan_t      pesan;
static FILE         *pesan_ptr;
static bool         end;

bool pesan_init(){
    pesan_ptr       = fopen(PESAN_FILE, "ab+");

    if (pesan_ptr == NULL) {
        pesan_ptr   = fopen(PESAN_FILE, "wb+");
    }
    if (pesan_ptr == NULL) {
        draw_dialog_err("Cannot create or open the file!");
        return false;
    }
    end = false;

    return true;
}

void pesan_print(){
    if (pesan_ptr == NULL) {
        draw_dialog_err("Do the \"pesan_init()\" first!");
        return;
    }

    bool pesan_exist = false;
    rewind(pesan_ptr);

    while(fread(&pesan, sizeof(pesan_t), 1, pesan_ptr) == 1) {
        if (strcmp(current_user, pesan.nama) == 0) {
            pesan_exist = true;
            if (current_type == BROKER){
                if (pesan.whois == BROKER)
                    printf(GRY"%s"BLU"You"RST": %s\n", pesan.timestamp, pesan.teks);
                else
                    printf(GRY"%s"MAG"%s"RST": %s\n", pesan.timestamp, pesan.nama, pesan.teks);;
            } else {
                if (pesan.whois == BROKER)
                    printf(GRY"%s"MAG"Broker"RST": %s\n", pesan.timestamp, pesan.teks);
                else
                    printf(GRY"%s"BLU"You"RST": %s\n", pesan.timestamp, pesan.teks);;
            }
        }
    }

    if (pesan_exist == false)
        printf("Mari memulai percakapan!");
}

void pesan_kirim(const char *teks){
    if (pesan_ptr == NULL) {
        draw_dialog_err("Do the \"pesan_init()\" first!");
        return;
    }

    time_t time_raw;
    time(&time_raw);
    current_timestamp = localtime(&time_raw);

    strncpy(pesan.teks, teks, MAX_STRLEN);
    strncpy(pesan.nama, current_user, MAX_STRLEN);
    strncpy(pesan.timestamp, asctime(current_timestamp), MAX_STRLEN);

    if (current_type == BROKER) {
        pesan.whois = BROKER;
    } else {
        pesan.whois = USER;
    }

    fwrite(&pesan, sizeof(pesan_t), 1, pesan_ptr);
    fflush(pesan_ptr);
}

bool pesan_clear(){
    if (pesan_ptr != NULL) {
        draw_dialog_err("Please end the pesan chain first!");
        return false;
    }

    FILE *write_ptr = fopen(PESAN_FILE_TEMP, "wb");
    FILE *read_ptr = fopen(PESAN_FILE, "rb");

    while (fread(&pesan, sizeof(pesan_t), 1, read_ptr) == 1) {
        if (strcmp(current_user, pesan.nama) == 0 && pesan.whois != current_type)
            fwrite(&pesan, sizeof(pesan_t), 1, write_ptr);
    }
    fflush(write_ptr);

    rename(PESAN_FILE_TEMP, PESAN_FILE);
    remove(PESAN_FILE_TEMP);

    fclose(write_ptr);
    fclose(read_ptr);

    return true;
}

bool pesan_purge(char *username){
    if (pesan_ptr != NULL) {
        draw_dialog_err("Please end the pesan chain first!");
        return false;
    }

    FILE *write_ptr = fopen(PESAN_FILE_TEMP, "wb");
    FILE *read_ptr = fopen(PESAN_FILE, "rb");

    while (fread(&pesan, sizeof(pesan_t), 1, read_ptr) == 1) {
        if (strcmp(username, pesan.nama) == 0)
            fwrite(&pesan, sizeof(pesan_t), 1, write_ptr);
    }
    fflush(write_ptr);

    rename(PESAN_FILE_TEMP, PESAN_FILE);
    remove(PESAN_FILE_TEMP);

    fclose(write_ptr);
    fclose(read_ptr);

    return true;
}

void pesan_end() {
    fclose(pesan_ptr);
    pesan_ptr = NULL;
    end = true;
}

// Multithreaded
void *pesan_update(){
    FILE *current_ptr;

    current_ptr = pesan_ptr;
    fseek(current_ptr, 0, SEEK_END);
    long size_previous = ftell(current_ptr);

    term_clean();
    pesan_print();

    printf(YEL"\n(Ketik \"/exit\" untuk keluar)\n"RST);
    printf(BLU"[Ketikan sesuatu disini!] : "RST);

    while (end != true) {
        rewind(current_ptr);
        fseek(current_ptr, 0, SEEK_END);
        long size_current = ftell(current_ptr);

        if (size_previous != size_current) {
            term_clean();
            pesan_print();
            printf(YEL"\n(Ketik \"/exit\" untuk keluar)\n"RST);
            printf(BLU"[Ketikan sesuatu disini!] : "RST);
        }

        usleep(5000); fflush(stdout);

        size_previous = size_current;
    }
    return NULL;
}

void display_pesan_start(char *nama, user_t tipe){
    strncpy(current_user, nama, MAX_STRLEN);
    current_type = tipe;

    int choice;
    if (current_type == BROKER) {
        typedef enum {
            CHAT    = 1,
            CLEAR   = 2,
            PURGE   = 3,
            EXIT    = 0,
        } display_t;

        while(1){
            term_clean();

            bool status = draw_init(CENTER_CENTER, 1, 1, WIDTH, 8);
            if (status == false) return;

            draw_box(TITLE, BLU, "Chat Menu");
            draw_line(LEFT, BLU, 0, "1. Mulai percakapan!");
            draw_line(LEFT, BLU, 0, "2. Hapus hanya pesan dari anda");
            draw_line(LEFT, BLU, 0, "3. Hapus seluruh percakapan");
            draw_line(LEFT, BLU, 1, RED"0. Keluar");
            draw_decor(BLU);
            draw_input(BLU, 0, "Input :");
            draw_end();

            input_number(&choice);

            switch (choice) {
                case CHAT:
                    display_pesan_message();
                    break;
                case CLEAR:
                    display_pesan_clear();
                    break;
                case PURGE:
                    display_pesan_purge();
                    break;
                case EXIT:
                    return;
                default:
                    draw_dialog_err("Input tidak valid!");
            }
        }
    } else {
        typedef enum {
            CHAT    = 1,
            CLEAR   = 2,
            EXIT    = 0,
        } display_t;

        while(1){
            term_clean();

            bool status = draw_init(CENTER_CENTER, 1, 1, WIDTH, 7);
            if (status == false) return;

            draw_box(TITLE, BLU, "Chat Menu");
            draw_line(LEFT, BLU, 0, "1. Mulai percakapan!");
            draw_line(LEFT, BLU, 0, "2. Hapus hanya pesan dari anda");
            draw_line(LEFT, BLU, 1, RED"0. Keluar");
            draw_decor(BLU);
            draw_input(BLU, 0, "Input :");
            draw_end();

            input_number(&choice);

            switch (choice) {
                case CHAT:
                    display_pesan_message();
                    break;
                case CLEAR:
                    display_pesan_clear();
                    break;
                case EXIT:
                    return;
                default:
                    draw_dialog_err("Input tidak valid!");
            }
        }
    }
}

void display_pesan_message() {
    pesan_init();

    char teks[256];
    pthread_t thread;

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

void display_pesan_clear(){
    term_clean();

    char choice[MAX_STRLEN];

    draw_dialog_confirmation("Apa anda yakin ingin membersihkan percakapan anda?");
    input_string(choice);

    if (strcmp(choice, "y") == 0 || strcmp(choice, "Y") == 0) {
        pesan_clear();

        bool status = draw_init(CENTER_CENTER, 1, 1, WIDTH, 5);
        if (status == false) return;

        draw_dialog_continue("Pesan berhasil dibersihkan!");
    } else {
        draw_dialog_continue("Aksi dibatalkan!");
    }
}

void display_pesan_purge(){
    term_clean();

    char choice[MAX_STRLEN];

    draw_dialog_confirmation("Apa anda yakin ingin MENGHAPUS SEMUA percakapan?");
    input_string(choice);

    if (strcmp(choice, "y") == 0 || strcmp(choice, "Y") == 0) {
        if (pesan_purge(current_user))
            draw_dialog_continue("Pesan berhasil dihapus!");
        else
            draw_dialog_continue("Pesan gagal dihapus!");
    } else {
        draw_dialog_continue("Aksi dibatalkan!");
        getchar();
    }
}
