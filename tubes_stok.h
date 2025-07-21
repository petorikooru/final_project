#include <stdint.h>

#include "tubes_handler.h"

#ifndef STOK_H
#define STOK_H

#define MAX_ITEMS 500
#define MAX_NAME_LENGTH 50
#define MAX_ADDRESS_LENGTH 100
#define MAX_PHONE_LENGTH 15
#define STOCK_FILE "stok.tb"

typedef struct {
    char            nama[MAX_NAME_LENGTH];
    uint16_t        jumlah;
    uint32_t        harga;
} product_t;

void display_stok_start();

#endif
