#include "tubes_interface.h"
#include "tubes_handler.h"

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
    D_BROKER,
    D_USER,
    D_NONE,
} database_user_t;

typedef enum {
    BROKER,
    USER,
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

#define DATABASE_FILE    "database.tb"

typedef enum {
    C_SUCCESS,
    C_FAILED,
} choice_t;

choice_t database_user_signup( const char *username, const char *password,
                                   data_t *database_user);
database_user_t database_user_login(const char *username, const char *password, data_t *database_user);
void database_update(data_t *user);
void database_user_init(const char *username, const char *password,
                               data_t *const user);

#endif
