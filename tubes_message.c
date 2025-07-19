#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>

#include "tubes_message.h"
#include "tubes_handler.h"

static char     current_user[MAX_STRLEN];
static user_t   current_type;
static pesan_t  pesan;
static FILE     *pesan_ptr;
static bool     end = false;

bool pesan_init(const char* nama, user_t tipe){
    pesan_ptr       = fopen(PESAN_FILE, "ab+");

    if (pesan_ptr == NULL) {
        pesan_ptr   = fopen(PESAN_FILE, "wb+");
    }
    if (pesan_ptr == NULL) {
        print_err("Cannot create or open the file!");
        return false;
    }

    strncpy(current_user, nama, MAX_STRLEN - 1);
    current_type = tipe;
    end = false;

    return true;
}

void pesan_print(){
    if (pesan_ptr == NULL) {
        print_err("Do the \"pesan_init()\" first!");
        return;
    }

    bool pesan_exist = false;
    rewind(pesan_ptr);

    while(fread(&pesan, sizeof(pesan_t), 1, pesan_ptr) == 1) {
        if (strcmp(current_user, pesan.nama) == 0) {
            pesan_exist = true;
            if (pesan.whois == BROKER)
                printf(MAG_BG"Broker"RST": %s\n", pesan.teks);
            else
                printf(BLU_BG"You"RST": %s\n", pesan.teks);
        }
    }

    if (pesan_exist == false)
        printf("Mari memulai percakapan!");
}

void pesan_kirim(const char *teks){
    if (pesan_ptr == NULL) {
        print_err("Do the \"pesan_init()\" first!");
        return;
    }

    strncpy(pesan.teks, teks, MAX_STRLEN);
    strncpy(pesan.nama, current_user, MAX_STRLEN);

    if (current_type == BROKER) {
        pesan.whois = BROKER;
    } else {
        pesan.whois = USER;
    }

    fwrite(&pesan, sizeof(pesan_t), 1, pesan_ptr);
    fflush(pesan_ptr);
}

void pesan_clear(const char *nama, const user_t type){
    if (pesan_ptr != NULL) {
        print_err("Please end the pesan chain first!");
        return;
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
}

void pesan_purge(const char *nama){
    if (pesan_ptr != NULL) {
        print_err("Please end the pesan chain first!");
        return;
    }

    FILE *write_ptr = fopen(PESAN_FILE_TEMP, "wb");
    FILE *read_ptr = fopen(PESAN_FILE, "rb");

    while (fread(&pesan, sizeof(pesan_t), 1, read_ptr) == 1) {
        if (strcmp(current_user, pesan.nama) == 0)
            fwrite(&pesan, sizeof(pesan_t), 1, write_ptr);
    }
    fflush(write_ptr);

    rename(PESAN_FILE_TEMP, PESAN_FILE);
    remove(PESAN_FILE_TEMP);

    fclose(write_ptr);
    fclose(read_ptr);
}

void pesan_end() {
    fclose(pesan_ptr);
    end = true;
}

// Multithreaded
void *pesan_update(void *arg){
    FILE *current_ptr;

    current_ptr = pesan_ptr;
    fseek(current_ptr, 0, SEEK_END);
    long size_previous = ftell(current_ptr);

    term_clean();
    pesan_print();
    printf("\n[Say something!] : ");

    while (end != true) {
        rewind(current_ptr);
        fseek(current_ptr, 0, SEEK_END);
        long size_current = ftell(current_ptr);

        if (size_previous != size_current) {
            term_clean();
            pesan_print();
            printf("\n[Say something!] : ");
        }

        usleep(5000); fflush(stdout);

        size_previous = size_current;
    }
}
