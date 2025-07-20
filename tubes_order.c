#include <stdio.h>
#include <stdlib.h>

#include "tubes_order.h"
#include "tubes_stok.h"
#include "tubes_handler.h"

#define ORDER_FILE      "order.tb"
#define ORDER_FILE_TEMP "order.tb.tmp"

static char             current_user[MAX_STRLEN];
static sorting_t        current_sort;
static FILE             *order_ptr;
static bool             end = false;

bool display_stok_start() {
    order_ptr       = fopen(ORDER_FILE, "ab+");

    if (order_ptr == NULL) {
        order_ptr   = fopen(ORDER_FILE, "wb+");
    }
    if (order_ptr == NULL) {
        print_err("Cannot create or open the file!");
        return false;
    }

    end = false;

    return true;
}

void order_list(const sorting_t sortir) {
    int total = sizeof(ordered_item);

    switch (sortir){
        case NAMA_AZ:
            for (int i = 0; i < jumlah_item - 1; i++) {
                for (int j = i + 1; j < jumlah_item; j++) {
                    if (strcmp(items[i].nama, items[j].nama) > 0) {
                        Item temp = items[i];
                        items[i] = items[j];
                        items[j] = temp;
                    }
                }
            }
            break;

        case NAMA_ZA:

        case JENIS_AZ:

        case JENIS_ZA:

        case JUMLAH:

        default:

    }
}

void order_beli(const char *nama) {

}

void order_hapus(const char *nama) {

}

void order_end() {
    free(ordered_item);
}
