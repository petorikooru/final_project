#include <stdio.h>
#include <stdbool.h>
#include <time.h>

#include "tubes_database.h"

#ifndef MESSAGE_H
#define MESSAGE_H

#define PESAN_FILE      "message.tb"
#define PESAN_FILE_TEMP "message.tb.tmp"

typedef struct {
    char    nama[256];
    char    teks[256];
    char    timestamp[256];
    user_t  whois;
} pesan_t;

bool pesan_init();
void pesan_kirim(const char *teks);
void pesan_print();
void pesan_end();

bool pesan_clear(const char *nama, const user_t type);
bool pesan_purge(const char *nama);
void *pesan_update(void *arg);

// Use this instead to start the conversation
void display_pesan_start(char *nama, user_t tipe);
void display_pesan_message();
void display_pesan_clear();
void display_pesan_purge();

/* Example Usage:
 * #include <pthread.h>
 * #include "tubes_message.h"
 *
 * {
 *      pthread_t thread;
 *      pesan_init(username, user_t);                           <-- Dont forget to do this first
 *      pthread_create(&thread, NULL, pesan_update, NULL);
 *      while (true) {
 *          input_buffer_save();
 *          input_buffer_copy(teks);
 *
 *          if (strcmp(teks, "/exit") == 0) break;
 *          else pesan_kirim(teks);
 *      }
 *      pesan_end();                                            <-- Dont forget to end it
 *      pthread_join(thread, NULL);
 * }
 */

#endif
