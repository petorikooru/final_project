#include <stdio.h>
#include <stdbool.h>

#ifndef MESSAGE_H
#define MESSAGE_H

#define PESAN_FILE      "message.tb"
#define PESAN_FILE_TEMP "message.tb.tmp"

typedef enum {
    BROKER  = 0,
    USER    = 1,
} user_t;

typedef struct {
    char username[64];
    char password[64];
} data_t;

typedef struct {
    char    nama[256];
    char    teks[256];
    user_t  whois;
} pesan_t;

bool pesan_init(const char* nama, user_t tipe);
void pesan_kirim(const char *teks);
void pesan_print();
void pesan_end();

void pesan_clear(const char *nama, const user_t type);
void pesan_purge(const char *nama);
void *pesan_update(void *arg);

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
