#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "tubes_stok.h"
#include "tubes_input.h"
#include "tubes_handler.h"
#include "tubes_interface.h"
#include "tubes_database.h"

static void tampilkan_menu(product_t *items, int jmlh_brang);
static void liat_stok(product_t *items, int jmlh_brang);
static void cari_item(product_t *items, int jmlh_brang);
static void sort_nama(product_t *items, int jmlh_brang);
static void sort_harga(product_t *items, int jmlh_brang);
static void sort_jmlh(product_t *items, int jmlh_brang);
static void load_stok(product_t *items, int *jmlh_brang);

//prototipe unik
static void beli_barang(product_t *items, int *jmlh_brang);
static void simpan_pesanan(product_t *order);

void display_stok_cari();

void display_stok_beli();

void display_stok_view();
void display_stok_view_name();
void display_stok_view_number();
void display_stok_view_price();

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
        draw_input(BLU, 0, "Input:");
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
                log_print_err("Invalid input! Press enter to continue...");
                getchar();
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
        draw_input(BLU, 0, "Input:");
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
                log_print_err("Invalid input! Press enter to continue...");
                getchar();
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
        jumlah_produk++;
    }

    if (jumlah_produk == 0) {
        draw_dialog_info("Sepertinya masih belum ada produk...");
        fclose(database_file);
        return;
    }

    // Untuk diproses ke sortir nanti
    product_t* produk_stok = malloc(jumlah_produk * sizeof(product_t));
    if (!produk_stok) {
        draw_dialog_err("Gagal mengalokasikan memori!");
        fclose(database_file);
        return;
    }
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

    bool status = draw_init(TOP_CENTER, 1, 1, WIDTH, 20);
    if (status == false) return;

    draw_box(TITLE_NOBOX, MAG, "Stok Sekarang");
    draw_line(LEFT, MAG, 0, "No | Nama Produk            | Stok | Harga");
    draw_decor(MAG);

    for (uint16_t i = 0; i < jumlah_produk; i++) {
        draw_line(LEFT, MAG, 0, "%-2d | %-22s | %-4d | %-6d",
                i+1,
                produk_stok[i].nama,
                produk_stok[i].jumlah,
                produk_stok[i].harga);
    }

    draw_line(CENTER, MAG, 1, MAG_BG" Press enter to continue... ");
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
        jumlah_produk++;
    }

    if (jumlah_produk == 0) {
        draw_dialog_info("Sepertinya masih belum ada produk...");
        fclose(database_file);
        return;
    }

    // Untuk diproses ke sortir nanti
    product_t* produk_stok = malloc(jumlah_produk * sizeof(product_t));
    if (!produk_stok) {
        draw_dialog_err("Gagal mengalokasikan memori!");
        fclose(database_file);
        return;
    }
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

    sort_jmlh(produk_stok, jumlah_produk);

    bool status = draw_init(TOP_CENTER, 1, 1, WIDTH, 20);
    if (status == false) return;

    draw_box(TITLE_NOBOX, MAG, "Stok Sekarang");
    draw_line(LEFT, MAG, 0, "No | Nama Produk            | Stok | Harga");
    draw_decor(MAG);

    for (uint16_t i = 0; i < jumlah_produk; i++) {
        draw_line(LEFT, MAG, 0, "%-2d | %-22s | %-4d | %-6d",
                i+1,
                produk_stok[i].nama,
                produk_stok[i].jumlah,
                produk_stok[i].harga);
    }

    draw_line(CENTER, MAG, 1, MAG_BG" Press enter to continue... ");
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
        jumlah_produk++;
    }

    if (jumlah_produk == 0) {
        draw_dialog_info("Sepertinya masih belum ada produk...");
        fclose(database_file);
        return;
    }

    // Untuk diproses ke sortir nanti
    product_t* produk_stok = malloc(jumlah_produk * sizeof(product_t));
    if (!produk_stok) {
        draw_dialog_err("Gagal mengalokasikan memori!");
        fclose(database_file);
        return;
    }
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

    bool status = draw_init(TOP_CENTER, 1, 1, WIDTH, 20);
    if (status == false) return;

    draw_box(TITLE_NOBOX, MAG, "Stok Sekarang");
    draw_line(LEFT, MAG, 0, "No | Nama Produk            | Stok | Harga");
    draw_decor(MAG);

    for (uint16_t i = 0; i < jumlah_produk; i++) {
        draw_line(LEFT, MAG, 0, "%-2d | %-22s | %-4d | %-6d",
                i+1,
                produk_stok[i].nama,
                produk_stok[i].jumlah,
                produk_stok[i].harga);
    }

    draw_line(CENTER, MAG, 1, MAG_BG" Press enter to continue... ");
    draw_end();
    getchar();

    free(produk_stok);

    return;
}

static void sort_nama(product_t *items, int jmlh_brang) {
    term_clean();
    for (int i = 0; i < jmlh_brang - 1; i++) {
        for (int j = i + 1; j < jmlh_brang; j++) {
            if (strcmp(items[i].nama, items[j].nama) > 0) {
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

static void sort_jmlh(product_t *items, int jmlh_brang) {
    term_clean();
    for (int i = 0; i < jmlh_brang - 1; i++) {
        for (int j = i + 1; j < jmlh_brang; j++) {
            if (items[i].jumlah > items[j].jumlah) {
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
    draw_input(MAG, 1, MAG"Nama produk:");
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
            draw_line(LEFT, MAG, 1, MAG"Nama produk\t: %s", produk.nama);
            draw_line(LEFT, MAG, 1, MAG"Harga produk\t: %i", produk.harga);
            draw_line(LEFT, MAG, 1, MAG"Jumlah stok\t: %i", produk.jumlah);
            draw_decor(MAG);
            draw_line(CENTER, MAG, 1, MAG_BG" Press enter to continue... ");
            getchar();
        }
    }

    if (found == false) draw_dialog_err("Produk tidak ditemukan!");

    fclose(database_file);

    return;
}

void display_stok_beli() {
    term_clean();

    char nama[MAX_STRLEN] = {0};
    char certainty[MAX_STRLEN] = {0};
    bool found = false;

    draw_init(CENTER_CENTER, 1, 1, WIDTH, 3);
    draw_box(TITLE, MAG, "Order Produk");
    draw_input(MAG, 1, MAG"Nama produk:");
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
            draw_line(LEFT, MAG, 1, MAG"Nama produk\t: %s", produk.nama);
            draw_line(LEFT, MAG, 1, MAG"Harga produk\t: %d", produk.harga);
            draw_line(LEFT, MAG, 1, MAG"Jumlah stok\t: %d", produk.jumlah);
            draw_decor(MAG);
            draw_line(LEFT, MAG, 1, MAG"Apakah anda yakin ingin membelinya?");
            draw_input(MAG, 1, "(Y/n)    ");
            draw_end();

            input_string(certainty);

            // Handle user confirmation
            if (strcasecmp(certainty, "n") == 0) {
                draw_dialog_info("Pembelian dibatalkan!");
                break;
            }

            term_clean();

            draw_init(CENTER_CENTER, 1, 1, WIDTH, 4);
            draw_box(TITLE, MAG, "Data Pengiriman");
            draw_line(LEFT, MAG, 1, MAG"Alamat anda  :");
            draw_line(LEFT, MAG, 1, MAG"Nomor telepon:");

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
            draw_input(MAG, 1, MAG"Alamat anda:");
            input_string(data.order.alamat[order_index]);
            draw_input(MAG, 1, MAG"Nomor telepon:");
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
            }
            break;
        }
    }

    if (!found) {
        draw_dialog_err("Produk tidak ditemukan!");
    }

    fclose(database_file);
}

// static void beli_barang(product_t *items, int *jmlh_brang) {
//     data_t user;
//     user.order.orderCount = 0;
//     strcpy(order.status, "Menunggu");

//     char lanjut;
//     do {
//         int index = 0;
//         char nama_barang[MAX_NAME_LENGTH];
//         getchar();
//         while (index == 0) {
//         printf("\nMasukkan nama barang yang ingin dibeli: ");
//         input_string(nama_barang);

//         for (int i = 0; i < *jmlh_brang; i++) {
//             if (strcmp(items[i].nama, nama_barang) == 0) {
//                 index = i;
//                 break;
//                 }
//             }

//         if (index == 0) {
//             printf("\nBarang tidak ditemukan!\n");
//             continue;
//             }
//         }

//         int jumlah;
//         int i = 0;
//         while (i == 0){
//         printf("\nMasukkan jumlah yang ingin dibeli: ");
//         scanf("%d", &jumlah);

//         i = 1;
//         if (jumlah > items[index].jmlh) {
//             printf("Stok tidak mencukupi!\n");
//             i = 0;
//             break;
//         }
//         }
//         getchar();

//         strcpy(order.items[order.item_count].nama_barang, items[index].nama);
//         order.items[order.item_count].jumlah_dibeli = jumlah;
//         order.items[order.item_count].harga_satuan = items[index].harga;
//         order.item_count++;

//         items[index].jmlh -= jumlah;

//         do{
//         printf("\nTambahkan barang lain? (y/n): ");
//         scanf(" %c", &lanjut);
//         }while (lanjut != 'y' && lanjut !='n' && lanjut == 'Y' && lanjut == 'N');
//     } while ((lanjut == 'y' || lanjut == 'Y') && order.item_count < MAX_ITEMS);

//     printf("\n=== DETAIL PENGIRIMAN ===\n");
//     printf("Alamat: ");
//     input_string(order.alamat);
//     printf("Nomor Telepon: ");
//     scanf(" %d", order.telepon);

//     printf("=== DETAIL PEMBAYARAN ===\n");
//     printf("Nomor Rekening\t: 707563602600\nNama Bank\t: CIMB NIAGA\nAtas Nama\t: DAFFA RIZQY ANDIKA\n");

//     simpan_pesanan(&order);

//     printf("\n\e[1mTERIMAKASIH TELAH MEMBELI DARI D MILSURP\e[0m\nTekan enter untuk melanjutkan...");
//     getchar();
//     getchar();
// }
