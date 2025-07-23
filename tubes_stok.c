#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

#include "tubes_stok.h"
#include "tubes_input.h"
#include "tubes_handler.h"
#include "tubes_interface.h"
#include "tubes_database.h"

#define NOMOR_REKENING "707563602600"
#define NAMA_BANK "CIMB NIAGA"
#define ATAS_NAMA "DAFFA RIZQY ANDIKA"

void display_stok_cari();

void display_stok_beli();

void display_stok_view();
void display_stok_view_name();
void display_stok_view_number();
void display_stok_view_price();

static void sort_nama(product_t *items, int jmlh_brang);
static void sort_harga(product_t *items, int jmlh_brang);
static void sort_jumlah(product_t *items, int jmlh_brang);

static char current_user[MAX_STRLEN];
static data_t data;

void display_stok_init(data_t *user){
    strncpy(current_user, user->username, MAX_STRLEN);
    data = *user;
}

void display_stok_start(data_t *user){
    typedef enum {
        M_LIHAT     = 1,
        M_CARI      = 2,
        M_BELI      = 3,
        M_KELUAR    = 0,
    } menu_t;
    int choice;

    display_stok_init(user);

    while(1) {
        term_clean();

        bool status = draw_init(CENTER_CENTER, 1, 1, WIDTH, 10);
        if (status == false) return;

        draw_box(TITLE, BLU, "Stok Menu");
        draw_line(LEFT, BLU, 1, BLU"Pilih opsi yang anda inginkan!");
        draw_decor(BLU);
        draw_line(LEFT, BLU, 0, "1. Lihat Stok Barang");
        draw_line(LEFT, BLU, 0, "2. Cari Barang");
        draw_line(LEFT, BLU, 0, "3. Beli Barang");
        draw_line(LEFT, BLU, 1, RED"0. Keluar");
        draw_decor(BLU);
        draw_input(BLU, 0, "Input :");
        draw_end();

        input_number(&choice);

        switch(choice) {
            case M_LIHAT:
                display_stok_view();
                break;
            case M_CARI:
                display_stok_cari();
                break;
            case M_BELI:
                display_stok_beli();
                break;
            case M_KELUAR:
                return;
            default:
                draw_dialog_err("Input invalid!");
                break;
        }
    }
}

void display_stok_view(){
    typedef enum {
        M_NAMA      = 1,
        M_JUMLAH    = 2,
        M_HARGA     = 3,
        M_KELUAR    = 0,
    } menu_t;
    int choice;

    while(1) {
        term_clean();

        bool status = draw_init(CENTER_CENTER, 1, 1, WIDTH, 10);
        if (status == false) return;

        draw_box(TITLE, BLU, "Stok Menu");
        draw_line(LEFT, BLU, 1, BLU"Pilih opsi untuk mengurutkan barang!");
        draw_decor(BLU);
        draw_line(LEFT, BLU, 0, "1. [A - Z] Urutkan berdasarkan nama");
        draw_line(LEFT, BLU, 0, "2. [1 2 3] Urutkan berdasarkan jumlah");
        draw_line(LEFT, BLU, 0, "3. [Rp...] Urutkan berdasarkan harga");
        draw_line(LEFT, BLU, 1, RED"0. Keluar");
        draw_decor(BLU);
        draw_input(BLU, 0, "Input :");
        draw_end();

        input_number(&choice);

        switch(choice) {
            case M_NAMA:
                display_stok_view_name();
                break;
            case M_JUMLAH:
                display_stok_view_number();
                break;
            case M_HARGA:
                display_stok_view_price();
                break;
            case M_KELUAR:
                return;
            default:
                draw_dialog_err("Input invalid!");
                break;
        }
    }
}

void display_stok_view_name(){
    term_clean();

    FILE* database_file = fopen(STOCK_FILE, "rb");
    if (!database_file) {
        draw_dialog_err("Database stok tidak ditemukan!");
        return;
    }

    // Mencari jumlah produk yang ada
    product_t produk;
    uint16_t jumlah_produk = 0;
    while(fread(&produk, sizeof(product_t), 1, database_file) == 1) {
        if (strlen(produk.nama) != 0)
            jumlah_produk++;
    }

    if (jumlah_produk == 0) {
        draw_dialog_info("Sepertinya masih belum ada produk...");
        fclose(database_file);
        return;
    }

    // Untuk diproses ke sortir nanti
    product_t* produk_stok = malloc(jumlah_produk * sizeof(product_t));
    rewind(database_file);

    for (uint16_t i = 0; i < jumlah_produk; i++) {
        if (fread(&produk_stok[i], sizeof(product_t), 1, database_file) != 1) {
            draw_dialog_err("Gagal membaca data produk!");
            free(produk_stok);
            fclose(database_file);
            return;
        }
    }
    fclose(database_file);

    sort_nama(produk_stok, jumlah_produk);

    uint8_t height = jumlah_produk + 6;

    term_clean();
    draw_init(CENTER_CENTER, 1, 1, WIDTH, height);
    draw_box(TITLE, MAG, "Lihat seluruh produk");
    draw_line(LEFT, MAG, 0, "No | Nama Produk            | Stok | Harga");
    draw_decor(MAG);

    for (uint8_t i = 0; i < jumlah_produk; i++) {
        draw_line(LEFT, MAG, 0, "%2i | %-22s | %-4i | %-8i",
                i+1,
                produk_stok[i].nama,
                produk_stok[i].jumlah,
                produk_stok[i].harga);
    }

    draw_decor(MAG);
    draw_line(CENTER, MAG, 2, MAG_BG BLK" Press enter to continue... ");
    draw_end();
    getchar();

    free(produk_stok);

    return;
}

void display_stok_view_number(){
    term_clean();

    FILE* database_file = fopen(STOCK_FILE, "rb");
    if (!database_file) {
        draw_dialog_err("Database stok tidak ditemukan!");
        return;
    }

    // Mencari jumlah produk yang ada
    product_t produk;
    uint16_t jumlah_produk = 0;
    while(fread(&produk, sizeof(product_t), 1, database_file) == 1) {
        if (strlen(produk.nama) != 0)
            jumlah_produk++;
    }

    if (jumlah_produk == 0) {
        draw_dialog_info("Sepertinya masih belum ada produk...");
        fclose(database_file);
        return;
    }

    // Untuk diproses ke sortir nanti
    product_t* produk_stok = malloc(jumlah_produk * sizeof(product_t));
    rewind(database_file);

    for (uint16_t i = 0; i < jumlah_produk; i++) {
        if (fread(&produk_stok[i], sizeof(product_t), 1, database_file) != 1) {
            draw_dialog_err("Gagal membaca data produk!");
            free(produk_stok);
            fclose(database_file);
            return;
        }
    }
    fclose(database_file);

    sort_jumlah(produk_stok, jumlah_produk);

    uint8_t height = jumlah_produk + 6;

    term_clean();
    draw_init(CENTER_CENTER, 1, 1, WIDTH, height);
    draw_box(TITLE, MAG, "Lihat seluruh produk");
    draw_line(LEFT, MAG, 0, "No | Nama Produk            | Stok | Harga");
    draw_decor(MAG);

    for (uint8_t i = 0; i < jumlah_produk; i++) {
        draw_line(LEFT, MAG, 0, "%2i | %-22s | %-4i | %-8i",
                i+1,
                produk_stok[i].nama,
                produk_stok[i].jumlah,
                produk_stok[i].harga);
    }

    draw_decor(MAG);
    draw_line(CENTER, MAG, 2, MAG_BG BLK" Press enter to continue... ");
    draw_end();
    getchar();

    free(produk_stok);

    return;
}

void display_stok_view_price(){
    term_clean();

    FILE* database_file = fopen(STOCK_FILE, "rb");
    if (!database_file) {
        draw_dialog_err("Database stok tidak ditemukan!");
        return;
    }

    // Mencari jumlah produk yang ada
    product_t produk;
    uint16_t jumlah_produk = 0;
    while(fread(&produk, sizeof(product_t), 1, database_file) == 1) {
        if (strlen(produk.nama) != 0)
            jumlah_produk++;
    }

    if (jumlah_produk == 0) {
        draw_dialog_info("Sepertinya masih belum ada produk...");
        fclose(database_file);
        return;
    }

    // Untuk diproses ke sortir nanti
    product_t* produk_stok = malloc(jumlah_produk * sizeof(product_t));
    rewind(database_file);

    for (uint16_t i = 0; i < jumlah_produk; i++) {
        if (fread(&produk_stok[i], sizeof(product_t), 1, database_file) != 1) {
            draw_dialog_err("Gagal membaca data produk!");
            free(produk_stok);
            fclose(database_file);
            return;
        }
    }
    fclose(database_file);

    sort_harga(produk_stok, jumlah_produk);

    uint8_t height = jumlah_produk + 6;

    term_clean();
    draw_init(CENTER_CENTER, 1, 1, WIDTH, height);
    draw_box(TITLE, MAG, "Lihat seluruh produk");
    draw_line(LEFT, MAG, 0, "No | Nama Produk            | Stok | Harga");
    draw_decor(MAG);

    for (uint8_t i = 0; i < jumlah_produk; i++) {
        draw_line(LEFT, MAG, 0, "%2i | %-22s | %-4i | %-8i",
                i+1,
                produk_stok[i].nama,
                produk_stok[i].jumlah,
                produk_stok[i].harga);
    }

    draw_decor(MAG);
    draw_line(CENTER, MAG, 2, MAG_BG BLK" Press enter to continue... ");
    draw_end();
    getchar();

    free(produk_stok);

    return;
}

// https://stackoverflow.com/a/25456826
int cmpalph(const char *string_1, const char *string_2) {
    const char *cp1 = string_1, *cp2 = string_2;
    int sccmp = 1;

    for (; toupper(*cp1) == toupper(*cp2); cp1++, cp2++)
        if (*cp1 == '\0')
            sccmp = 0;
    if (sccmp) return ((toupper(*cp1) < toupper(*cp2)) ? -1 : +1);

    for (; *string_1 == *string_2; string_1++, string_2++)
        if (*string_1 == '\0')
            return 0;
    return ((*string_1 < *string_2) ? +1 : -1);
}

static void sort_nama(product_t *items, int jmlh_brang) {
    term_clean()
    for (int i = 0; i < jmlh_brang - 1; i++) {
        for (int j = i + 1; j < jmlh_brang; j++) {
            if (cmpalph(items[i].nama, items[j].nama) > 0) {
                product_t temp = items[i];
                items[i] = items[j];
                items[j] = temp;
            }
        }
    }
}

static void sort_harga(product_t *items, int jmlh_brang) {
    term_clean();
    for (int i = 0; i < jmlh_brang - 1; i++) {
        for (int j = i + 1; j < jmlh_brang; j++) {
            if (items[i].harga > items[j].harga) {
                product_t temp = items[i];
                items[i] = items[j];
                items[j] = temp;
            }
        }
    }
}

static void sort_jumlah(product_t *items, int jmlh_brang) {
    term_clean();
    for (int i = 0; i < jmlh_brang - 1; i++) {
        for (int j = i + 1; j < jmlh_brang; j++) {
            if (items[i].jumlah < items[j].jumlah) {
                product_t temp = items[i];
                items[i] = items[j];
                items[j] = temp;
            }
        }
    }
}

void display_stok_cari(){
    term_clean();

    char nama[MAX_STRLEN];

    draw_init(CENTER_CENTER, 1, 1, WIDTH, 3);
    draw_box(TITLE, MAG, "Order Produk");
    draw_input(MAG, 1, MAG"Nama produk :");
    draw_end();

    input_string(nama);

    FILE* database_file = fopen(STOCK_FILE, "rb");
    if (!database_file) {
        draw_dialog_err("Database stok tidak ditemukan!");
        return;
    }
    product_t produk;
    bool found = false;

    while(fread(&produk, sizeof(product_t), 1, database_file) == 1){
        if (strcmp(produk.nama, nama) == 0) {
            found = 1;

            if (produk.jumlah == 0) {
                draw_dialog_err("Produk tidak tersedia di dalam stok!");
                break;
            }
            term_clean();

            draw_init(CENTER_CENTER, 1, 1, WIDTH, 7);
            draw_box(TITLE, MAG, "Order Produk");
            draw_line(LEFT, MAG, 1, MAG"Nama produk   : "WHT"%s", produk.nama);
            draw_line(LEFT, MAG, 1, MAG"Harga produk  : "WHT"%i", produk.harga);
            draw_line(LEFT, MAG, 1, MAG"Jumlah stok   : "WHT"%i", produk.jumlah);
            draw_decor(MAG);
            draw_line(CENTER, MAG, 2, MAG_BG BLK" Press enter to continue... ");
            getchar();
        }
    }

    if (found == false) draw_dialog_err("Produk tidak ditemukan!");

    fclose(database_file);

    return;
}

void display_stok_beli() {
    display_stok_view_name();
    term_clean();

    char nama[MAX_STRLEN] = {0};
    char certainty[MAX_STRLEN] = {0};
    bool found = false;

    draw_init(CENTER_CENTER, 1, 1, WIDTH, 3);
    draw_box(TITLE, MAG, "Order Produk");
    draw_input(MAG, 1, MAG"Nama produk :");
    draw_end();

    input_string(nama);

    FILE* database_file = fopen(STOCK_FILE, "rb+");
    if (!database_file) {
        draw_dialog_err("Database stok tidak ditemukan!");
        return;
    }

    product_t produk;
    bool order_success = false;

    while(fread(&produk, sizeof(product_t), 1, database_file) == 1) {
        if (strcmp(produk.nama, nama) == 0) {
            found = true;

            if (produk.jumlah <= 0) {
                draw_dialog_err("Produk tidak tersedia di dalam stok!");
                break;
            }

            term_clean();

            draw_init(CENTER_CENTER, 1, 1, WIDTH, 8);
            draw_box(TITLE, MAG, "Order Produk");
            draw_line(LEFT, MAG, 1, MAG"Nama produk   : "WHT"%s", produk.nama);
            draw_line(LEFT, MAG, 1, MAG"Harga produk  : "WHT"%d", produk.harga);
            draw_line(LEFT, MAG, 1, MAG"Jumlah stok   : "WHT"%d", produk.jumlah);
            draw_decor(MAG);
            draw_line(LEFT, MAG, 0, "Apakah anda yakin ingin membelinya?");
            draw_input(MAG, 1, YEL"(Y/n) :");
            draw_end();

            input_string(certainty);

            if (strcasecmp(certainty, "n") == 0) {
                draw_dialog_info("Pembelian dibatalkan!");
                break;
            }

            term_clean();

            draw_init(CENTER_CENTER, 1, 1, WIDTH, 4);
            draw_box(TITLE, MAG, "Data Pengiriman");
            draw_line(LEFT, MAG, 1, MAG"Alamat anda   :");
            draw_line(LEFT, MAG, 1, MAG"Nomor telepon :");

            int order_index = -1;
            for (int i = 0; i < MAX_ORDERS; i++) {
                if (strlen(data.order.orders[i]) == 0) {
                    order_index = i;
                    break;
                }
            }

            if (order_index == -1) {
                draw_dialog_err("Uh oh... Orderan sudah full!");
                break;
            }

            draw_change_current_line(1);
            draw_input(MAG, 1, MAG"Alamat anda   :");
            input_string(data.order.alamat[order_index]);
            draw_input(MAG, 1, MAG"Nomor telepon :");
            input_string(data.order.telepon[order_index]);
            draw_end();

            strncpy(data.order.orders[order_index], produk.nama, MAX_STRLEN);
            strncpy(data.order.orderStatus[order_index], "Pending", MAX_STRLEN);
            data.order.orderCount++;

            produk.jumlah--;
            fseek(database_file, -sizeof(product_t), SEEK_CUR);
            if (fwrite(&produk, sizeof(product_t), 1, database_file) != 1) {
                draw_dialog_err("Gagal mengupdate stok!");
            } else {
                order_success = true;
            }

            if (order_success) {
                database_update(&data);
                draw_dialog_info("Produk berhasil diorder!");

                term_clean();
                draw_init(CENTER_CENTER, 1, 1, WIDTH, 10);
                draw_box(TITLE, GRN, "Pembayaran");
                draw_line(LEFT, GRN, 1, GRN"Transfer ke rekening ini ya!");
                draw_line(LEFT, GRN, 1, BLU"Konfirmasi Pembayaran melalui chat!");
                draw_decor(GRN);
                draw_line(LEFT, GRN, 2, GRN"Nomor Rekening : "WHT"%s", NOMOR_REKENING);
                draw_line(LEFT, GRN, 2, GRN"Nama Bank      : "WHT"%s", NAMA_BANK);
                draw_line(LEFT, GRN, 2, GRN"Atas Nama      : "WHT"%s", ATAS_NAMA);
                draw_decor(GRN);

                SET_CURSOR(CURSOR_HIDE);
                draw_line(CENTER, GRN, 2, GRN_BG BLK" Press enter to continue... ");
                draw_end();

                while (getchar() != '\n');
                SET_CURSOR(CURSOR_SHOW);
            }
            break;
        }
    }

    if (!found) {
        draw_dialog_err("Produk tidak ditemukan!");
    }

    fclose(database_file);
}
