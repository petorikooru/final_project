#ifndef MANAJEMEN_H
#define MANAJEMEN_H

#define MAX_USERS 100
#define MAX_ORDERS 100
#define MAX_LENGTH 100

typedef struct {
    char    username[MAX_LENGTH];
    char    password[MAX_LENGTH];
    int     orderCount;
    char    orders[MAX_ORDERS][MAX_LENGTH];
    char    orderStatus[MAX_ORDERS][MAX_LENGTH];
} user_order_t;

void display_user_start();

#endif
