#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "tubes_stok.h"
#include "tubes_input.h"
#include "tubes_handler.h"

static void tampilkan_menu(Item *items, int jmlh_brang);
static void liat_stok(Item *items, int jmlh_brang);
static void cari_item(Item *items, int jmlh_brang);
static void sort_nama(Item *items, int jmlh_brang);
static void sort_harga(Item *items, int jmlh_brang);
static void sort_jmlh(Item *items, int jmlh_brang);
static void load_stok(Item *items, int *jmlh_brang);

//prototipe unik
static void beli_barang(Item *items, int *jmlh_brang);
static void simpan_pesanan(Order *order);

bool display_stok_start(){
    Item items[MAX_ITEMS];
    int jmlh_brang = 0;
    load_stok(items, &jmlh_brang);

    tampilkan_menu(items, jmlh_brang);
}

static void load_stok(Item *items, int *Jmlh_brang) {
    term_clean();
    FILE *file = fopen(NAMA_FILE, "rb");
    if (file == NULL) {
        *Jmlh_brang = 0;
        return;
    }

    *Jmlh_brang = fread(items, sizeof(Item), MAX_ITEMS, file);
    fclose(file);
}

static void tampilkan_menu(Item *items, int jmlh_brang) {
    Item temp_items[MAX_ITEMS];
    memcpy(temp_items, items, sizeof(Item) * jmlh_brang);

    int pilihan;
    do {
        term_clean();
        printf("\n=== Selamat Datang ke D MILSURP ===\n");
        printf("1. Lihat Stok Barang\n");
        printf("2. Cari Barang\n");
        printf("3. Urutkan Berdasarkan Nama\n");
        printf("4. Urutkan Berdasarkan Harga\n");
        printf("5. Urutkan Berdasarkan Jumlah\n");
        printf("6. Beli Barang\n");
        printf("0. Keluar\n");
        printf("Pilihan: ");
        scanf("%d", &pilihan);

        switch (pilihan) {
            case 1:
                liat_stok(temp_items, jmlh_brang);
                break;
            case 2:
                cari_item(temp_items, jmlh_brang);
                break;
            case 3:
                sort_nama(temp_items, jmlh_brang);
                liat_stok(temp_items, jmlh_brang);
                break;
            case 4:
                sort_harga(temp_items, jmlh_brang);
                liat_stok(temp_items, jmlh_brang);
                break;
            case 5:
                sort_jmlh(temp_items, jmlh_brang);
                liat_stok(temp_items, jmlh_brang);
                break;
            case 6:
                liat_stok(temp_items, jmlh_brang);
                beli_barang(items, &jmlh_brang);
                break;
            case 0:
                printf("Terima kasih!\n");
                break;
            default:
                printf("Pilihan tidak valid!\n");
        }
    } while (pilihan != 0);
}

static void liat_stok(Item *items, int jmlh_brang) {
    term_clean();
    printf("\n=== Stok Sekarang ===\n");
    printf("ID       Nama                      Jumlah     Harga     \n");
    printf("--------------------------------------------------------\n");
    for (int i = 0; i < jmlh_brang; i++) {
        printf("%-4d  %-30s %-10d $%-10.2f\n",
               i + 1, items[i].nama, items[i].jmlh, items[i].harga);
    }
    if (jmlh_brang == 0) {
        printf("Stok Kosong.\n");
    }
}

static void cari_item(Item *items, int jmlh_brang) {
    term_clean();
    char nama[MAX_STRLEN];
    printf("Masukan Nama Barang Yang Dicari: ");
    input_string(nama);

    printf("\n=== Stok Sekarang ===\n");
    printf("ID       Nama                      Jumlah     Harga     \n");
    printf("--------------------------------------------------------\n");
    int found = 0;
    for (int i = 0; i < jmlh_brang; i++) {
        if (strstr(items[i].nama, nama) != NULL) {
            printf("%-4d  %-30s %-10d $%-10.2f\n",
                   i + 1, items[i].nama, items[i].jmlh, items[i].harga);
            found = 1;
        }
    }
    if (!found) {
        printf("Nama Barang Tidak Ditemukan '%s'.\n", nama);
    }
}

static void sort_nama(Item *items, int jmlh_brang) {
    term_clean();
    for (int i = 0; i < jmlh_brang - 1; i++) {
        for (int j = i + 1; j < jmlh_brang; j++) {
            if (strcmp(items[i].nama, items[j].nama) > 0) {
                Item temp = items[i];
                items[i] = items[j];
                items[j] = temp;
            }
        }
    }
}

static void sort_harga(Item *items, int jmlh_brang) {
    term_clean();
    for (int i = 0; i < jmlh_brang - 1; i++) {
        for (int j = i + 1; j < jmlh_brang; j++) {
            if (items[i].harga > items[j].harga) {
                Item temp = items[i];
                items[i] = items[j];
                items[j] = temp;
            }
        }
    }
}

static void sort_jmlh(Item *items, int jmlh_brang) {
    term_clean();
    for (int i = 0; i < jmlh_brang - 1; i++) {
        for (int j = i + 1; j < jmlh_brang; j++) {
            if (items[i].jmlh > items[j].jmlh) {
                Item temp = items[i];
                items[i] = items[j];
                items[j] = temp;
            }
        }
    }
}

static void beli_barang(Item *items, int *jmlh_brang) {
    Order order;
    order.item_count = 0;
    strcpy(order.status, "Menunggu");

    char lanjut;
    do {
        int index = 0;
        char nama_barang[MAX_NAME_LENGTH];
        getchar();
        while (index == 0) {
        printf("\nMasukkan nama barang yang ingin dibeli: ");
        input_string(nama_barang);

        for (int i = 0; i < *jmlh_brang; i++) {
            if (strcmp(items[i].nama, nama_barang) == 0) {
                index = i;
                break;
                }
            }

        if (index == 0) {
            printf("\nBarang tidak ditemukan!\n");
            continue;
            }
        }

        int jumlah;
        int i = 0;
        while (i == 0){
        printf("\nMasukkan jumlah yang ingin dibeli: ");
        scanf("%d", &jumlah);

        i = 1;
        if (jumlah > items[index].jmlh) {
            printf("Stok tidak mencukupi!\n");
            i = 0;
            break;
        }
        }
        getchar();

        strcpy(order.items[order.item_count].nama_barang, items[index].nama);
        order.items[order.item_count].jumlah_dibeli = jumlah;
        order.items[order.item_count].harga_satuan = items[index].harga;
        order.item_count++;

        items[index].jmlh -= jumlah;

        do{
        printf("\nTambahkan barang lain? (y/n): ");
        scanf(" %c", &lanjut);
        }while (lanjut != 'y' && lanjut !='n' && lanjut == 'Y' && lanjut == 'N');
    } while ((lanjut == 'y' || lanjut == 'Y') && order.item_count < MAX_ITEMS);

    printf("\n=== DETAIL PENGIRIMAN ===\n");
    printf("Alamat: ");
    input_string(order.alamat);
    printf("Nomor Telepon: ");
    scanf(" %d", order.telepon);

    printf("=== DETAIL PEMBAYARAN ===\n");
    printf("Nomor Rekening\t: 707563602600\nNama Bank\t: CIMB NIAGA\nAtas Nama\t: DAFFA RIZQY ANDIKA\n");

    simpan_pesanan(&order);

    printf("\n\e[1mTERIMAKASIH TELAH MEMBELI DARI D MILSURP\e[0m\nTekan enter untuk melanjutkan...");
    getchar();
    getchar();
}

static void simpan_pesanan(Order *order) {
    char filename[50];
    int file_count = 0;

    strcpy(filename, "shipping.bin");

    FILE *test;
    while ((test = fopen(filename, "rb")) != NULL) {
        fclose(test);
        file_count++;
        sprintf(filename, "shipping%d.bin", file_count);
    }

    FILE *file = fopen(filename, "wb");
    if (!file) {
        printf("Gagal menyimpan pesanan!\n");
        return;
    }

    if (fwrite(order, sizeof(Order), 1, file) != 1) {
        printf("Gagal menulis data pesanan!\n");
    }

    fclose(file);
    printf("Pesanan berhasil disimpan ke %s\n", filename);
}
