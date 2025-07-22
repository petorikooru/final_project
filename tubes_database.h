#include "tubes_interface.h"
#include "tubes_handler.h"
#include <stdint.h>

#ifndef DATABASE_H
#define DATABASE_H

/* =========== User Object =========== */
/*
    Index :
        1 : SLOT
        2 : RANDOM NUMBER GUESSER
        3 : COIN TOSS
        4 : TOTAL / RESERVED
*/

#define MAX_ORDERS 50

typedef enum {
    D_BROKER    = 0,
    D_USER      = 1,
    D_NONE      = 2,
} database_user_t;

typedef enum {
    BROKER      = 0,
    USER        = 1,
} user_t;

typedef struct {
    int     orderCount;
    char    orders      [MAX_ORDERS][MAX_STRLEN];
    char    orderStatus [MAX_ORDERS][MAX_STRLEN];
    char    alamat      [MAX_ORDERS][MAX_STRLEN];
    char    telepon     [MAX_ORDERS][MAX_STRLEN];
} order_t;

typedef struct {
    char username   [MAX_STRLEN];
    char password   [MAX_STRLEN];
    bool banned;
    user_t type;
    order_t order;
} data_t;

typedef struct {
    char    username    [MAX_STRLEN];
    char    date        [MAX_STRLEN];
    char    text        [MAX_STRLEN];
    uint8_t rating;
} feedback_t;


#define DATABASE_FILE   "database.tb"
#define FEEDBACK_FILE   "feedback.tb"

typedef enum {
    C_SUCCESS,
    C_FAILED,
} choice_t;

choice_t database_user_signup( const char *username, const char *password,
                                   data_t *database_user, user_t type);

database_user_t database_user_login(const char *username, const char *password,
                                    data_t *database_user, user_t type);

void database_update(data_t *user);

void database_user_init(const char *username, const char *password,
                        data_t *const use, user_t type);

#endif
