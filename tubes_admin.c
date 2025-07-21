#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tubes_handler.h"
#include "tubes_interface.h"
#include "tubes_database.h"
#include "tubes_input.h"
#include "tubes_manajemen.h"
#include "tubes_message.h"
#include "tubes_stok.h"

#define MAX_USERS 100
#define MAX_NAME_LENGTH 50

void display_admin_menu();

void display_admin_user();
void display_admin_user_viewall();
void display_admin_user_view();
void display_admin_user_delete();
void display_admin_user_ban();
void display_admin_user_unban();

void display_admin_stock();
void display_admin_stock_view();
void display_admin_stock_add();
void display_admin_stock_delete();

void display_admin_order();
void display_admin_order_view();
void display_admin_order_process();

void display_admin_message();

void display_admin_start() {
    typedef enum {
        C_SUCCESS,
        C_FAILED,
    } choice_t;
    char username[MAX_STRLEN];
    char password[MAX_STRLEN];

    term_clean();

    bool status = draw_init(CENTER_CENTER, 1, 1, WIDTH, 4);
    if (status == false) return;

    draw_box(TITLE, MAG, "Login Admin");
    draw_line(LEFT, MAG, 1, MAG"Username:");
    draw_line(LEFT, MAG, 1, MAG"Password:");

    draw_change_current_line(1);
    draw_input(MAG, 1, MAG"Username: ");
    input_string(username);
    draw_input(MAG, 1, MAG"Password: ");
    input_string(password);

    draw_end();

    data_t* user = malloc(sizeof(data_t));
    database_user_t type = database_user_login(username, password, user);
    free(user);

    switch(type){
            case D_BROKER:
                draw_dialog_info("Login berhasil!");
                display_admin_menu();
                break;
            case D_USER:
                draw_dialog_err("Anda bukanlah Broker!");
                break;
            case D_NONE:
                draw_dialog_err("Login gagal!");
                break;
        }
}

void display_admin_menu() {
    typedef enum {
        M_USER      = 1,
        M_STOCK     = 2,
        M_ORDER     = 3,
        M_MESSAGE   = 4,
        M_EXIT      = 0,
    } menu_t;
    int choice;

    while (true) {
        term_clean();

        bool status = draw_init(CENTER_CENTER, 1, 1, WIDTH, 12);
        if (status == false) return;

        draw_box(TITLE, MAG, "Broker Menu");
        draw_line(LEFT, MAG, 3, MAG"Selamat datang Broker, apa yang akan kamu lakukan?");
        draw_line(LEFT, MAG, 1, MAG"Silahkan pilih menu dibawah!");
        draw_decor(MAG);
        draw_line(LEFT, MAG, 0, "1. Pengaturan User");
        draw_line(LEFT, MAG, 0, "2. Pengaturan Stok");
        draw_line(LEFT, MAG, 0, "3. Pengaturan Order");
        draw_line(LEFT, MAG, 0, "4. Kirim pesan ke user");

        draw_line(LEFT, MAG, 1, RED"0. Keluar");
        draw_decor(MAG);
        draw_input(MAG, 0, "Input:");
        draw_end();

        input_number(&choice);

        switch(choice) {
            case M_USER:
                display_admin_user();
                break;
            case M_STOCK:
                display_admin_stock();
                break;
            case M_ORDER:
                display_admin_order();
                break;
            case M_MESSAGE:
                display_admin_message();
                break;
            case M_EXIT:
                return;
            default:
                draw_dialog_err("Input tidak valid!");
                break;
        }
    }
}

void display_admin_user(){
    typedef enum {
        M_VIEWALL   = 1,
        M_VIEW      = 2,
        M_HAPUS     = 3,
        M_BAN       = 4,
        M_UNBAN     = 5,
        M_EXIT      = 0,
    } menu_t;
    int choice;

    while (true) {
        term_clean();

        bool status = draw_init(CENTER_CENTER, 1, 1, WIDTH, 13);
        if (status == false) return;

        draw_box(TITLE, MAG, "Broker Menu");
        draw_line(LEFT, MAG, 3, MAG"Selamat datang Broker, apa yang akan kamu lakukan?");
        draw_line(LEFT, MAG, 1, MAG"Silahkan pilih menu dibawah!");
        draw_decor(MAG);
        draw_line(LEFT, MAG, 0, "1. Lihat semua User");
        draw_line(LEFT, MAG, 0, "2. Lihat secara detail user");
        draw_line(LEFT, MAG, 0, "3. Hapus user");
        draw_line(LEFT, MAG, 0, "4. Ban user");
        draw_line(LEFT, MAG, 0, "5. Unban user");

        draw_line(LEFT, MAG, 1, RED"0. Keluar");
        draw_decor(MAG);
        draw_input(MAG, 0, "Input:");
        draw_end();

        input_number(&choice);

        switch(choice) {
            case M_VIEWALL:
                display_admin_user_viewall();
                break;
            case M_VIEW:
                display_admin_user_view();
                break;
            case M_HAPUS:
                display_admin_user_delete();
                break;
            case M_BAN:
                display_admin_user_ban();
                break;
            case M_UNBAN:
                display_admin_user_unban();
                break;
            case M_EXIT:
                return;
            default:
                draw_dialog_err("Input tidak valid!");
                break;
        }
    }
}

void display_admin_user_viewall(){
    term_clean();

    bool status = draw_init(TOP_CENTER, 1, 1, WIDTH, 20);
    if (status == false) return;

    FILE* database_file = fopen(DATABASE_FILE, "rb");
    data_t* user = malloc(sizeof(data_t));

    // Check whether the file exist or not
    if (!database_file){
        draw_dialog_err("File doesn't exist!");
    }

    draw_box(TITLE_NOBOX, MAG, "Lihat seluruh user");
    draw_line(LEFT, MAG, 0, "No | Nama Pengguna            | Status | Orderan");

    uint8_t i;
    while(fread(user, sizeof(data_t), 1, database_file) == 1){
        if (strcmp(user->username, "Admin") == 0) continue;
        draw_line(LEFT, MAG, 0, "%2i | %-24s | %-6s | %-3i",
            ++i, user->username, (user->banned) ? "Ban" : "Aman", user->order.orderCount
        );
    }

    draw_line(CENTER, MAG, 1, MAG_BG" Press enter to continue... ");
    draw_end();
    getchar();

    free(user);
    fclose(database_file);

    return;
}

void display_admin_user_view(){
    term_clean();

    char username[MAX_STRLEN];

    bool status = draw_init(CENTER_CENTER, 1, 1, WIDTH, 3);
    if (status == false) return;

    draw_box(TITLE, MAG, "Lihat User");
    draw_input(MAG, 1, MAG"Pilih user:");
    draw_end();

    input_string(username);

    FILE* database_file = fopen(DATABASE_FILE, "rb");
    data_t* user = malloc(sizeof(data_t));
    bool found = false;
    // Check whether the file exist or not
    if (!database_file){
        draw_dialog_err("File doesn't exist!");
    }
    while(fread(user, sizeof(data_t), 1, database_file) == 1){
        if (strcmp(user->username, username) == 0) {
            found = 1;

            term_clean();

            char username[MAX_STRLEN];

            bool status = draw_init(TOP_CENTER, 1, 1, WIDTH, 3);
            if (status == false) return;

            draw_box(TITLE_NOBOX, MAG, "Lihat User");
            draw_line(LEFT, MAG, 0, "Nama User\t: %s", user->username);
            draw_line(LEFT, MAG, 0, "Status\t\t: %s", (user->banned) ? "Ban" : "Aman");
            draw_line(LEFT, MAG, 0, "Jumlah terorder\t: %i", user->order.orderCount);
            draw_line(LEFT, MAG, 0, "Orderan\t:");
            for (int i = 0; i < user->order.orderCount; i++){
                draw_line(LEFT, MAG, 0, "%2i. %20s : %10s",
                    i + 1, user->order.orders[i], user->order.orderStatus[i]
                );
            }
            draw_line(CENTER, MAG, 1, MAG_BG" Press enter to continue... ");
            draw_end();
            getchar();
        }
    }

    if (found == false) draw_dialog_err("User tidak ditemukan!");

    free(user);
    fclose(database_file);

    return;
}

void display_admin_user_delete() {
    term_clean();

    char username[MAX_STRLEN] = {0};
    char certainty[MAX_STRLEN] = {0};
    bool found = false;
    bool is_deleted = false;
    const char* temp_file = "database.tb.temp";

    bool status = draw_init(CENTER_CENTER, 1, 1, WIDTH, 3);
    if (status == false) return;

    draw_box(TITLE, MAG, "Hapus User");
    draw_input(MAG, 1, MAG"Pilih user:");
    draw_end();

    input_string(username);

    FILE* database_file = fopen(DATABASE_FILE, "rb");
    if (!database_file) {
        draw_dialog_err("Database tidak ditemukan!");
        return;
    }

    FILE* database_new = fopen(temp_file, "wb");
    if (!database_new) {
        draw_dialog_err("Gagal membuat file temporary!");
        fclose(database_file);
        return;
    }

    data_t* user = malloc(sizeof(data_t));
    if (!user) {
        draw_dialog_err("Alokasi memori gagal!");
        fclose(database_file);
        fclose(database_new);
        return;
    }

    while(fread(user, sizeof(data_t), 1, database_file) == 1) {
        if (strcmp(user->username, username) == 0) {
            found = true;

            if (user->order.orderCount != 0) {
                draw_dialog_err("User masih memiliki orderan!");
                fwrite(user, sizeof(data_t), 1, database_new);
                continue;
            }

            draw_dialog_confirmation("Apakah anda yakin?");
            input_string(certainty);

            if (strcmp(certainty, "y") == 0 || strcmp(certainty, "Y") == 0) {
                is_deleted = true;
                continue;
            } else {
                draw_dialog_info("Aksi Dibatalkan!");
                fwrite(user, sizeof(data_t), 1, database_new);
                is_deleted = false;
                break;
            }
        }
        fwrite(user, sizeof(data_t), 1, database_new);
    }

    fclose(database_file);
    fclose(database_new);
    free(user);

    if (!found) {
        draw_dialog_err("User tidak ditemukan!");
        remove(temp_file);
        return;
    }

    if (is_deleted) {
        remove(DATABASE_FILE);
        rename(temp_file, DATABASE_FILE);
        draw_dialog_info("User berhasil dihapus!");
    } else {
        remove(temp_file);
    }
}

void display_admin_user_ban(){
    term_clean();

    char username[MAX_STRLEN];
    char certainty[MAX_STRLEN];

    bool status = draw_init(CENTER_CENTER, 1, 1, WIDTH, 3);
    if (status == false) return;

    draw_box(TITLE, MAG, "Ban User");
    draw_input(MAG, 1, MAG"Pilih user:");
    draw_end();

    input_string(username);

    FILE* database_file = fopen(DATABASE_FILE, "rb+");
    data_t* user = malloc(sizeof(data_t));
    bool found = false;

    if (!database_file){
        draw_dialog_err("File doesn't exist!");
    }

    while(fread(user, sizeof(data_t), 1, database_file) == 1){
        if (strcmp(user->username, username) == 0) {
            found = true;
            draw_dialog_confirmation("Apakah anda yakin?");
            input_string(certainty);

            if (strcmp(certainty, "y") == 0 || strcmp(certainty, "Y") == 0 ) {
                user->banned = true;

                fseek(database_file, -sizeof(data_t), SEEK_CUR);
                fwrite(user, sizeof(data_t), 1, database_file);

                draw_dialog_info("User berhasil diban!");
            } else {
                draw_dialog_err("Aksi Dibatalkan!");
            }
        }
    }

    if (found == false) draw_dialog_err("User tidak ditemukan!");

    free(user);
    fclose(database_file);

    return;
}

void display_admin_user_unban(){
    term_clean();

    char username[MAX_STRLEN];
    char certainty[MAX_STRLEN];

    bool status = draw_init(CENTER_CENTER, 1, 1, WIDTH, 3);
    if (status == false) return;

    draw_box(TITLE, MAG, "Unban User");
    draw_input(MAG, 1, MAG"Pilih user:");
    draw_end();

    input_string(username);

    FILE* database_file = fopen(DATABASE_FILE, "rb+");
    data_t* user = malloc(sizeof(data_t));
    bool found = false;

    if (!database_file){
        draw_dialog_err("File doesn't exist!");
    }
    while(fread(user, sizeof(data_t), 1, database_file) == 1){
        if (strcmp(user->username, username) == 0) {
            found = true;
            draw_dialog_confirmation("Apakah anda yakin?");
            input_string(certainty);

            if (strcmp(certainty, "y") == 0 || strcmp(certainty, "Y") == 0 ) {
                user->banned = false;

                fseek(database_file, -sizeof(data_t), SEEK_CUR);
                fwrite(user, sizeof(data_t), 1, database_file);

                draw_dialog_info("User berhasil diunban!");
            } else {
                draw_dialog_err("Aksi Dibatalkan!");
            }
        }
    }

    if (found == false) draw_dialog_err("User tidak ditemukan!");

    free(user);
    fclose(database_file);

    return;
}

void display_admin_order(){
    typedef enum {
        M_VIEW      = 1,
        M_PROCESS   = 2,
        M_EXIT      = 0,
    } menu_t;
    int choice;

    while (true) {
        term_clean();

        bool status = draw_init(CENTER_CENTER, 1, 1, WIDTH, 10);
        if (status == false) return;

        draw_box(TITLE, MAG, "Broker Menu");
        draw_line(LEFT, MAG, 3, MAG"Proses orderan dan Tambah Stok!");
        draw_line(LEFT, MAG, 1, MAG"Silahkan pilih menu dibawah!");
        draw_decor(MAG);
        draw_line(LEFT, MAG, 0, "1. Lihat barang yang terorder");
        draw_line(LEFT, MAG, 0, "2. Proses orderan user");
        draw_line(LEFT, MAG, 1, RED"0. Keluar");
        draw_decor(MAG);
        draw_input(MAG, 0, "Input:");
        draw_end();

        input_number(&choice);

        switch(choice) {
            case M_VIEW:
                display_admin_order_view();
                break;
            case M_PROCESS:
                display_admin_order_process();
                break;
            case M_EXIT:
                return;
            default:
                draw_dialog_err("Input tidak valid!");
                break;
        }
    }
}

void display_admin_order_view(){
    term_clean();

    bool status = draw_init(TOP_CENTER, 1, 1, WIDTH, 20);
    if (status == false) return;

    FILE* database_file = fopen(DATABASE_FILE, "rb");
    data_t* user = malloc(sizeof(data_t));

    // Check whether the file exist or not
    if (!database_file){
        draw_dialog_err("File doesn't exist!");
    }

    draw_box(TITLE_NOBOX, MAG, "Lihat seluruh orderan yang ada");
    draw_line(LEFT, MAG, 0, "No | Nama Kustomer  | Produk         | Status");

    uint8_t i;
    while(fread(user, sizeof(data_t), 1, database_file) == 1){
        if (strcmp(user->username, "Admin") == 0) continue;

        if (user->order.orderCount != 0) {
            for (int j = 0; j < user->order.orderCount; j++) {
                draw_line(LEFT, MAG, 0, "%2i | %-14s | %-14s | %-s",
                    ++i, user->username, user->order.orders[j],  user->order.orderStatus[j]
                );
            }
        }
    }

    draw_line(CENTER, MAG, 1, MAG_BG" Press enter to continue... ");
    draw_end();
    getchar();

    free(user);
    fclose(database_file);

    return;
}

void display_admin_order_process() {
    term_clean();

    char username[MAX_STRLEN]   = {0};
    char order[MAX_STRLEN]      = {0};
    char new_status[MAX_STRLEN] = {0};
    bool found = false;
    bool found_order = false;

    bool status = draw_init(CENTER_CENTER, 1, 1, WIDTH, 3);
    if (!status) return;

    draw_box(TITLE, MAG, "Proses Orderan User");
    draw_input(MAG, 1, MAG"Pilih user:");
    draw_end();

    input_string(username);

    FILE* database_file = fopen(DATABASE_FILE, "rb+");
    if (!database_file) {
        draw_dialog_err("Database tidak ditemukan!");
        return;
    }

    data_t user;
    bool update_success = false;

    while(fread(&user, sizeof(data_t), 1, database_file) == 1) {
        if (strcmp(user.username, username) == 0) {
            found = true;

            if (user.order.orderCount == 0) {
                draw_dialog_err("User tidak memiliki orderan!");
                break;
            }

            term_clean();
            status = draw_init(TOP_CENTER, 1, 1, WIDTH, 4);
            if (!status) {
                fclose(database_file);
                return;
            }

            draw_box(TITLE_NOBOX, MAG, "Proses Orderan User");
            draw_line(LEFT, MAG, 0, "Nama User\t: %s", user.username);
            draw_input(MAG, 0, "Pilih orderan:");
            draw_end();

            input_string(order);

            for (int j = 0; j < user.order.orderCount; j++) {
                if (strcmp(order, user.order.orders[j]) == 0) {
                    found_order = true;

                    term_clean();
                    status = draw_init(TOP_CENTER, 1, 1, WIDTH, 8);
                    if (!status) {
                        fclose(database_file);
                        return;
                    }

                    draw_box(TITLE_NOBOX, MAG, "Proses Orderan User");
                    draw_line(LEFT, MAG, 0, "Nama User\t: %s", user.username);
                    draw_line(LEFT, MAG, 0, "Nama Orderan\t: %s", user.order.orders[j]);
                    draw_line(LEFT, MAG, 0, "Alamat\t: %s", user.order.alamat[j]);
                    draw_line(LEFT, MAG, 0, "Nomor Telepon\t: %s", user.order.telepon[j]);
                    draw_line(LEFT, MAG, 0, "Status Orderan\t: %s", user.order.orderStatus[j]);
                    draw_input(MAG, 0, "Ganti status: ");
                    draw_end();

                    input_string(new_status);

                    strncpy(user.order.orderStatus[j], new_status, MAX_STRLEN);

                    fseek(database_file, -sizeof(data_t), SEEK_CUR);
                    fwrite(&user, sizeof(data_t), 1, database_file);

                    draw_dialog_info("Status order berhasil diupdate!");
                    break;
                }
            }

            if (!found_order) {
                draw_dialog_err("User tidak memiliki orderan tersebut!");
            }
            break;
        }
    }

    if (!found) {
        draw_dialog_err("User tidak ditemukan!");
    }

    fclose(database_file);
}

void display_admin_stock(){
    typedef enum {
        M_VIEW      = 1,
        M_ADD       = 2,
        M_DELETE    = 3,
        M_EXIT      = 0,
    } menu_t;
    int choice;

    while (true) {
        term_clean();

        bool status = draw_init(CENTER_CENTER, 1, 1, WIDTH, 11);
        if (status == false) return;

        draw_box(TITLE, MAG, "Stock Menu");
        draw_line(LEFT, MAG, 3, MAG"Proses stock yang ada pada toko");
        draw_line(LEFT, MAG, 1, MAG"Silahkan pilih menu dibawah!");
        draw_decor(MAG);
        draw_line(LEFT, MAG, 0, "1. Lihat stock yang ada");
        draw_line(LEFT, MAG, 0, "2. Tambah stock baru");
        draw_line(LEFT, MAG, 0, "3. Hapus stock");
        draw_line(LEFT, MAG, 1, RED"0. Keluar");
        draw_decor(MAG);
        draw_input(MAG, 0, "Input:");
        draw_end();

        input_number(&choice);

        switch(choice) {
            case M_VIEW:
                display_admin_stock_view();
                break;
            case M_ADD:
                display_admin_stock_add();
                break;
            case M_DELETE:
                display_admin_stock_delete();
                break;
            case M_EXIT:
                return;
            default:
                draw_dialog_err("Input tidak valid!");
                break;
        }
    }
}

void display_admin_stock_view(){
    term_clean();

    FILE* database_file = fopen(STOCK_FILE, "rb");
    product_t* produk = malloc(sizeof(product_t));

    // Check whether the file exist or not
    if (!database_file){
        draw_dialog_err("File doesn't exist!");
        return;
    }

    bool status = draw_init(TOP_CENTER, 1, 1, WIDTH, 20);
    if (status == false) return;

    draw_box(TITLE_NOBOX, MAG, "Lihat seluruh produk");
    draw_line(LEFT, MAG, 0, "No | Nama Produk            | Stok | Harga");

    uint8_t i;
    while(fread(produk, sizeof(product_t), 1, database_file) == 1){
        draw_line(LEFT, MAG, 0, "%2i | %-22s | %-4i | %-8i",
            ++i, produk->nama, produk->jumlah, produk->harga
        );
    }

    draw_line(CENTER, MAG, 1, MAG_BG" Press enter to continue... ");
    draw_end();
    getchar();

    free(produk);
    fclose(database_file);

    return;
}

void display_admin_stock_add() {
    char nama[MAX_STRLEN] = {0};
    char certainty[MAX_STRLEN] = {0};
    int jumlah = 0;
    int harga = 0;
    bool found = false;

    term_clean();
    bool status = draw_init(CENTER_CENTER, 1, 1, WIDTH, 3);
    if (!status) return;

    draw_box(TITLE, MAG, "Tambah produk");
    draw_input(MAG, 1, MAG"Input nama produk:");
    draw_end();
    input_string(nama);

    term_clean();
    status = draw_init(CENTER_CENTER, 1, 1, WIDTH, 5);
    if (!status) return;

    draw_box(TITLE, MAG, "Tambah produk");
    draw_line(LEFT, MAG, 0, "Nama Produk: %s", nama);
    draw_line(LEFT, MAG, 1, MAG"Jumlah Produk:");
    draw_line(LEFT, MAG, 1, MAG"Harga Produk:");

    draw_change_current_line(2);
    draw_input(MAG, 1, MAG"Jumlah Produk:");
    input_number(&jumlah);
    draw_input(MAG, 1, MAG"Harga Produk:");
    input_number(&harga);
    draw_end();

    draw_dialog_confirmation("Apakah anda yakin?");
    input_string(certainty);

    if (strcasecmp(certainty, "y") != 0) {
        draw_dialog_err("Aksi dibatalkan!");
        return;
    }

    FILE* database_file = fopen(STOCK_FILE, "rb+");
    if (!database_file) {
        database_file = fopen(STOCK_FILE, "wb+");
        if (!database_file) {
            draw_dialog_err("Gagal membuat file database!");
            return;
        }
    }

    product_t produk;
    bool update_success = false;

    rewind(database_file);
    while(fread(&produk, sizeof(product_t), 1, database_file) == 1) {
        if (strcmp(produk.nama, nama) == 0) {
            found = true;
            produk.jumlah = jumlah;
            produk.harga = harga;

            fseek(database_file, -sizeof(product_t), SEEK_CUR);
            if (fwrite(&produk, sizeof(product_t), 1, database_file) != 1) {
                draw_dialog_err("Gagal update produk!");
            } else {
                update_success = true;
            }
            break;
        }
    }

    if (!found) {
        memset(&produk, 0, sizeof(product_t));
        strncpy(produk.nama, nama, MAX_STRLEN);
        produk.jumlah = jumlah;
        produk.harga = harga;

        if (fwrite(&produk, sizeof(product_t), 1, database_file) != 1) {
            draw_dialog_err("Gagal menambah produk!");
        } else {
            update_success = true;
        }
    }

    if (update_success) {
        draw_dialog_info("Produk berhasil terupdate!");
    }

    fclose(database_file);
}

void display_admin_stock_delete(){
    char nama[MAX_STRLEN] = {0};
    char certainty[MAX_STRLEN] = {0};
    bool found = false;
    bool is_deleted = false;
    char temp_file[] = "database.tb.tmp";

    term_clean();
    bool status = draw_init(CENTER_CENTER, 1, 1, WIDTH, 3);
    if (status == false) return;

    draw_box(TITLE, MAG, "Hapus produk");
    draw_input(MAG, 1, MAG"Input nama produk:");
    draw_end();

    input_string(nama);

    product_t produk;

    FILE* database_file = fopen(STOCK_FILE, "rb");
    if (!database_file) {
        draw_dialog_err("File not found!");
        return;
    }

    FILE* database_new = fopen(temp_file, "wb");
    if (!database_new) {
        draw_dialog_err("Idk, failed to create temp file ig!");
        fclose(database_file);
        return;
    }

    while(fread(&produk, sizeof(product_t), 1, database_file) == 1) {
        if (strcmp(produk.nama, nama) == 0) {
            found = true;
            draw_dialog_confirmation("Apakah anda yakin?");
            input_string(certainty);

            if (strcmp(certainty, "y") == 0 || strcmp(certainty, "Y") == 0) {
                is_deleted = true;
                continue;
            } else {
                draw_dialog_info("Aksi Dibatalkan!");
                fwrite(&produk, sizeof(product_t), 1, database_new);
                is_deleted = false;
                break;
            }
        }
        fwrite(&produk, sizeof(product_t), 1, database_new);
    }

    fclose(database_file);
    fclose(database_new);

    if (!found) {
        draw_dialog_err("Produk tidak ditemukan!");
        remove(temp_file);
        return;
    }

    if (is_deleted) {
        remove(STOCK_FILE);
        rename(temp_file, STOCK_FILE);
        draw_dialog_info("Produk berhasil dihapus!");
    } else {
        remove(temp_file);
    }
}

void display_admin_message(){
    term_clean();

    char username[MAX_STRLEN];

    bool status = draw_init(CENTER_CENTER, 1, 1, WIDTH, 4);
    if (status == false) return;

    draw_box(TITLE, MAG, "Kirim Pesan");
    draw_input(MAG, 1, MAG"Kirim pesan ke:");
    draw_end();

    input_string(username);

    FILE* database_file = fopen(DATABASE_FILE, "rb");
    data_t* user = malloc(sizeof(data_t));
    bool found = false;

    if (!database_file){
        draw_dialog_err("File doesn't exist!");
    }
    while(fread(user, sizeof(data_t), 1, database_file) == 1){
        if (strcmp(user->username, username) == 0) {
            display_pesan_start(username, BROKER);
            found = 1;
        }
    }

    if (found == false) draw_dialog_err("User tidak ditemukan!");

    free(user);
    fclose(database_file);

    return;
}
