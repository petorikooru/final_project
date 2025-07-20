#include <stdbool.h>
#include <stdint.h>

#include "tubes_handler.h"

#ifndef ORDER_H
#define ORDER_H

typedef enum {
    DEFAULT,
    NAMA_AZ,
    NAMA_ZA,
    JENIS_AZ,
    JENIS_ZA,
    HARGA_TERENDAH,
    HARGA_TERTINGGI,
    JUMLAH_TERENDAH,
    JUMLAH_TERTINGGI
} sorting_t;

/* Order Function */
void order_init();
void order_list(const sorting_t sortir);
void order_cari(const char *nama);
void order_beli(const char *nama);
void order_resuply(const char *nama, const uint32_t jumlah);
void order_hapus(const char *nama);
void order_end();

/* Stok Function */
void stok_list();

#endif
