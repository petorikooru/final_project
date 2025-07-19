#include <stdint.h>

#include "tubes_handler.h"

#ifndef STOK_H
#define STOK_H

#define MAX_ITEMS 500
#define MAX_NAME_LENGTH 50
#define MAX_ADDRESS_LENGTH 100
#define MAX_PHONE_LENGTH 15
#define NAMA_FILE "stok.tb"

typedef struct {
    char    nama[MAX_NAME_LENGTH];
    int     jmlh;
    float   harga;
} Item;

typedef struct {
    char    nama_barang[MAX_NAME_LENGTH];
    int     jumlah_dibeli;
    float   harga_satuan;
} OrderedItem;

typedef struct {
    OrderedItem items[MAX_ITEMS];
    int         item_count;
    char        alamat[MAX_ADDRESS_LENGTH];
    int         telepon[MAX_PHONE_LENGTH];
    char        status[20];
} Order;

void stok_init();

#endif
