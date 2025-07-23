#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tubes_handler.h"
#include "tubes_interface.h"
#include "tubes_database.h"
#include "tubes_input.h"
#include "tubes_message.h"
#include "tubes_stok.h"
#include "tubes_admin.h"

#define MAX_USERS 100
#define MAX_NAME_LENGTH 50

void display_admin_login();
void display_admin_register();
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
void display_admin_order_delete();

void display_admin_broker();
void display_admin_message();
void display_admin_feedback();

void display_admin_start() {
    typedef enum {
        M_LOGIN       = 1,
        M_REGISTER    = 2,
        M_EXIT        = 0,
    } menu_t;
    int choice;

    while(1) {
        term_clean();

        bool status = draw_init(CENTER_CENTER, 1, 1, WIDTH, 9);
        if (status == false) return;

        draw_box(TITLE, BLU, "Broker Page");
        draw_line(LEFT, BLU, 1, BLU"Pilih opsi dibawa ya...");
        draw_decor(BLU);
        draw_line(LEFT, BLU, 0, "1. Login Broker");
        draw_line(LEFT, BLU, 0, "2. Registrasi Broker");
        draw_line(LEFT, BLU, 1, RED"0. Keluar");
        draw_decor(BLU);
        draw_input(BLU, 0, "Input :");
        draw_end();

        input_number(&choice);

        switch(choice) {
            case M_LOGIN:
                display_admin_login();
                return;
            case M_REGISTER:
                display_admin_register();
                break;
            case M_EXIT:
                return;
            default:
                draw_dialog_err("Input invalid!");
                break;
        }
    }
}

void display_admin_register(){
    char username[MAX_STRLEN];
    char password[MAX_STRLEN];
    char token[MAX_STRLEN];
    uint8_t chance = 3;

    while (true) {
        chance--;
        term_clean();
        draw_init(CENTER_CENTER, 1, 1, WIDTH, 3);
        draw_box(TITLE, MAG, "Registrasi Broker");
        draw_input(BLU, 1, MAG"Input token broker :");
        input_string(token);

        if (strcmp(token, TOKEN_STRING) != 0) {
            draw_dialog_err("Token salah!");
        } else break;

        if (chance == 0) {
            draw_dialog_err("Percobaan telah habis!");
            return;
        }

        draw_dialog_warn("Anda memiliki %i percobaan lagi!", chance);
    }
    term_clean();

    draw_init(CENTER_CENTER, 1, 1, WIDTH, 4);

    draw_box(TITLE, MAG, "Registrasi Broker");
    draw_line(LEFT, MAG, 1, MAG"Username :");
    draw_line(LEFT, MAG, 1, MAG"Password :");

    draw_change_current_line(1);
    draw_input(MAG, 1, MAG"Username :");
    input_string(username);
    draw_input(MAG, 1, MAG"Password :");
    input_string(password);

    data_t* user = malloc(sizeof(data_t));
    choice_t type = database_user_signup(username, password, user, BROKER);

    switch(type){
        case C_SUCCESS:
            draw_dialog_continue("Registrasi berhasil!");
            database_user_init(username, password, user, BROKER);
            break;
        case C_FAILED:
            draw_dialog_err("Gagal untuk membuat akun!");
            break;
    }

    free(user);
    draw_end();
}

void display_admin_login(){
    typedef enum {
        C_SUCCESS,
        C_FAILED,
    } choice_t;
    char username[MAX_STRLEN];
    char password[MAX_STRLEN];
    uint8_t chance = 3;
    bool failed = false;

    while (!failed) {
        term_clean();

        draw_init(CENTER_CENTER, 1, 1, WIDTH, 4);
        draw_box(TITLE, MAG, "Login Admin");
        draw_line(LEFT, MAG, 1, MAG"Username :");
        draw_line(LEFT, MAG, 1, MAG"Password :");

        draw_change_current_line(1);
        draw_input(MAG, 1, MAG"Username :");
        input_string(username);
        draw_input(MAG, 1, MAG"Password :");
        input_string(password);

        draw_end();

        data_t* user = malloc(sizeof(data_t));
        database_user_t type = database_user_login(username, password, user, BROKER);
        free(user);

        switch(type){
            case D_BROKER:
                draw_dialog_info("Login berhasil!");
                display_admin_menu();
                return;
            case D_USER:
                draw_dialog_err("Anda bukanlah Broker!");
                failed = true;
                return;
            case D_NONE:
                chance--;
                draw_dialog_err("Login gagal!");
                break;
        }
        if (chance == 0) {
            draw_dialog_err("Percobaan telah habis!");
            return;
        }
        draw_dialog_warn("Anda memiliki %i percobaan lagi!", chance);
    }
}

void display_admin_menu() {
    typedef enum {
        M_USER      = 1,
        M_STOCK     = 2,
        M_ORDER     = 3,
        M_BROKER    = 4,
        M_MESSAGE   = 5,
        M_FEEDBACK  = 6,
        M_EXIT      = 0,
    } menu_t;
    int choice;

    while (true) {
        term_clean();

        bool status = draw_init(CENTER_CENTER, 1, 1, WIDTH, 14);
        if (status == false) return;

        draw_box(TITLE, MAG, "Broker Menu");
        draw_line(LEFT, MAG, 3, MAG"Selamat datang Broker, apa yang akan kamu lakukan?");
        draw_line(LEFT, MAG, 1, MAG"Silahkan pilih menu dibawah!");
        draw_decor(MAG);
        draw_line(LEFT, MAG, 0, "1. Pengaturan User");
        draw_line(LEFT, MAG, 0, "2. Pengaturan Stok");
        draw_line(LEFT, MAG, 0, "3. Pengaturan Order");
        draw_line(LEFT, MAG, 0, "4. Lihat Broker yang teregistrasi!");
        draw_line(LEFT, MAG, 0, "5. Kirim pesan ke user");
        draw_line(LEFT, MAG, 0, "6. Lihat feedback aplikasi");
        draw_line(LEFT, MAG, 1, RED"0. Keluar");
        draw_decor(MAG);
        draw_input(MAG, 0, "Input :");
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
            case M_BROKER:
                display_admin_broker();
                break;
            case M_MESSAGE:
                display_admin_message();
                break;
            case M_FEEDBACK:
                display_admin_feedback();
                break;
            case M_EXIT:
                draw_dialog_info("Selamat tinggal, Broker!");
                return;
            default:
                draw_dialog_err("Input invalid!");
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

        draw_box(TITLE, MAG, "Pengaturan User");
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
        draw_input(MAG, 0, "Input :");
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
                draw_dialog_err("Input invalid!");
                break;
        }
    }
}

void display_admin_user_viewall() {
    FILE* database_file = fopen(DATABASE_FILE, "rb");

    // Nyari user yang ada
    data_t user;
    uint16_t user_count = 0;
    while(fread(&user, sizeof(data_t), 1, database_file) == 1) {
        if (user.type != BROKER && strlen(user.username) > 0) {
            user_count++;
        }
    }

    if (user_count == 0) {
        draw_dialog_info("Tidak ada pengguna terdaftar!");
        fclose(database_file);
        return;
    }

    rewind(database_file);
    term_clean();

    uint8_t height = user_count + 6;
    draw_init(CENTER_CENTER, 1, 1, WIDTH, height);
    draw_box(TITLE, MAG, "Daftar Pengguna");
    draw_line(LEFT, MAG, 0, "No | Nama Pengguna            | Status  | Orderan");
    draw_decor(MAG);

    uint16_t display_count = 0;
    while(fread(&user, sizeof(data_t), 1, database_file) == 1) {
        if (user.type != BROKER && strlen(user.username) > 0) {
            const char* status = user.banned ? "BANNED" : "Aman";
            draw_line(LEFT, MAG, 0, "%-2d | %-24s | %-7s | %-3d",
                ++display_count,
                user.username,
                status,
                user.order.orderCount
            );
        }
    }

    SET_CURSOR(CURSOR_HIDE);
    draw_decor(MAG);
    draw_line(CENTER, MAG, 2, MAG_BG BLK" Press enter to continue... ");
    draw_end();

    SET_CURSOR(CURSOR_SHOW);

    fclose(database_file);
    getchar();
}

void display_admin_user_view(){
    term_clean();

    char username[MAX_STRLEN];

    bool status = draw_init(CENTER_CENTER, 1, 1, WIDTH, 3);
    if (status == false) return;

    draw_box(TITLE, MAG, "Lihat User");
    draw_input(MAG, 1, MAG"Pilih user :");
    draw_end();

    input_string(username);

    term_clean();

    FILE* database_file = fopen(DATABASE_FILE, "rb");
    data_t* user = malloc(sizeof(data_t));
    bool found = false;

    if (!database_file){
        draw_dialog_err("File doesn't exist!");
    }

    while(fread(user, sizeof(data_t), 1, database_file) == 1){
        if (strcmp(user->username, username) == 0) {
            found = true;

            term_clean();
            draw_init(CENTER_CENTER, 1, 1, WIDTH, user->order.orderCount + 9);
            draw_box(TITLE, MAG, "Profil User");
            draw_line(LEFT, MAG, 0, "Nama User      : %s", user->username);
            draw_line(LEFT, MAG, 0, "Status         : %s", (user->banned) ? "Ban" : "Aman");
            draw_line(LEFT, MAG, 0, "Jumlah terorder: %i", user->order.orderCount);
            draw_decor(MAG);
            draw_line(CENTER, MAG, 2, MAG_BG BLK"Orderan:");
            for (int i = 0; i < user->order.orderCount; i++){
                draw_line(LEFT, MAG, 0, "%2i. %-10s : %s",
                    i + 1, user->order.orders[i], user->order.orderStatus[i]
                );
            }
            draw_decor(MAG);
            draw_line(CENTER, MAG, 2, MAG_BG BLK" Press enter to continue... ");
            draw_end();

            getchar();
        }
    }

    if (found == false) {
        draw_dialog_err("User tidak ditemukan!");
    }

    free(user);
    fclose(database_file);

    return;
}

void display_admin_user_delete() {
    display_admin_user_viewall();

    term_clean();

    char username[MAX_STRLEN] = {0};
    char certainty[MAX_STRLEN] = {0};
    bool found = false;
    bool is_deleted = false;
    const char* temp_file = "database.tb.temp";

    bool status = draw_init(CENTER_CENTER, 1, 1, WIDTH, 3);
    if (status == false) return;

    draw_box(TITLE, MAG, "Hapus User");
    draw_input(MAG, 1, MAG"Pilih user :");
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

            if (user->type == BROKER) {
                draw_dialog_info("Anda akan menghapus BROKER!");
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
        pesan_purge(username);
        remove(DATABASE_FILE);
        rename(temp_file, DATABASE_FILE);
        draw_dialog_info("User berhasil dihapus!");
    } else {
        remove(temp_file);
    }
}

void display_admin_user_ban(){
    display_admin_user_viewall();

    term_clean();

    char username[MAX_STRLEN];
    char certainty[MAX_STRLEN];

    bool status = draw_init(CENTER_CENTER, 1, 1, WIDTH, 3);
    if (status == false) return;

    draw_box(TITLE, MAG, "Ban User");
    draw_input(MAG, 1, MAG"Pilih user :");
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
    display_admin_user_viewall();
    term_clean();

    char username[MAX_STRLEN];
    char certainty[MAX_STRLEN];

    bool status = draw_init(CENTER_CENTER, 1, 1, WIDTH, 3);
    if (status == false) return;

    draw_box(TITLE, MAG, "Unban User");
    draw_input(MAG, 1, MAG"Pilih user :");
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
        M_DELETE    = 3,
        M_EXIT      = 0,
    } menu_t;
    int choice;

    while (true) {
        term_clean();

        bool status = draw_init(CENTER_CENTER, 1, 1, WIDTH, 11);
        if (status == false) return;

        draw_box(TITLE, MAG, "Pengaturan Stok");
        draw_line(LEFT, MAG, 3, MAG"Proses orderan dan Tambah Stok!");
        draw_line(LEFT, MAG, 1, MAG"Silahkan pilih menu dibawah!");
        draw_decor(MAG);
        draw_line(LEFT, MAG, 0, "1. Lihat barang yang terorder");
        draw_line(LEFT, MAG, 0, "2. Proses orderan user");
        draw_line(LEFT, MAG, 0, "3. Hapus orderan user");
        draw_line(LEFT, MAG, 1, RED"0. Keluar");
        draw_decor(MAG);
        draw_input(MAG, 0, "Input :");
        draw_end();

        input_number(&choice);

        switch(choice) {
            case M_VIEW:
                display_admin_order_view();
                break;
            case M_PROCESS:
                display_admin_order_process();
                break;
            case M_DELETE:
                display_admin_order_delete();
                break;
            case M_EXIT:
                return;
            default:
                draw_dialog_err("Input invalid!");
                break;
        }
    }
}

void display_admin_order_view(){
    term_clean();

    FILE* database_file = fopen(DATABASE_FILE, "rb");
    if (!database_file){
        draw_dialog_err("File doesn't exist!");
        return;
    }

    // Nyari order yang ada
    data_t user;
    uint8_t order_count = 0;
    while(fread(&user, sizeof(data_t), 1, database_file) == 1) {
        order_count += user.order.orderCount;
    }

    if (order_count == 0) {
        draw_dialog_info("Sepertinya masih belum ada yang order...");
        fclose(database_file);
        return;
    }

    uint8_t height = order_count + 6;
    draw_init(CENTER_CENTER, 1, 1, WIDTH, height);
    draw_box(TITLE, MAG, "Lihat seluruh orderan yang ada");
    draw_line(LEFT, MAG, 0, "No | Nama Kustomer  | Produk         | Status");
    draw_decor(MAG);

    uint8_t i = 0;
    rewind(database_file);
    while(fread(&user, sizeof(data_t), 1, database_file) == 1) {
        if (user.type == USER && user.order.orderCount > 0) {
            for (int j = 0; j < user.order.orderCount; j++) {
                if (strlen(user.order.orders[j]) > 0) {
                    draw_line(LEFT, MAG, 0, "%2i | %-14s | %-14s | %-s",
                        ++i,
                        user.username,
                        user.order.orders[j],
                        user.order.orderStatus[j]
                    );
                }
            }
        }
    }

    fclose(database_file);

    draw_decor(MAG);
    draw_line(CENTER, MAG, 2, MAG_BG BLK" Press enter to continue... ");
    draw_end();
    getchar();

    return;
}

void display_admin_order_process(){
    display_admin_user_viewall();

    term_clean();

    char username[MAX_STRLEN]   = {0};
    char new_status[MAX_STRLEN] = {0};
    bool found = false;
    int order;

    draw_init(CENTER_CENTER, 1, 1, WIDTH, 3);

    draw_box(TITLE, MAG, "Proses Orderan User");
    draw_input(MAG, 1, MAG"Pilih user :");
    draw_end();

    input_string(username);

    FILE* database_file = fopen(DATABASE_FILE, "rb+");
    if (!database_file) {
        draw_dialog_err("Database tidak ditemukan!");
        return;
    }

    data_t user;
    while(fread(&user, sizeof(data_t), 1, database_file) == 1) {
        if (strcmp(user.username, username) == 0) {
            found = true;

            if (user.order.orderCount == 0) {
                draw_dialog_err("User tidak memiliki orderan!");
                break;
            }

            uint8_t height = user.order.orderCount + 7;

            term_clean();
            draw_init(CENTER_CENTER, 1, 1, WIDTH, height);
            draw_box(TITLE, MAG, "Proses Orderan User");
            draw_line(CENTER, MAG, 2, BLU"Nama User : "WHT"%s", user.username);
            draw_decor(MAG);
            draw_line(LEFT, MAG, 1, MAG"Pilih orderan:");
            for (int j = 0; j < user.order.orderCount; j++) {
                if (strlen(user.order.orders[j]) > 0) {
                    draw_line(LEFT, MAG, 0, "%2i. %-14s : %-s",
                        j + 1,
                        user.order.orders[j],
                        user.order.orderStatus[j]
                    );
                }
            }
            draw_decor(MAG);
            draw_input(MAG, 1, MAG"Input :");
            draw_end();

            input_number(&order);
            order--;
            if (strlen(user.order.orders[order]) == 0){
                draw_dialog_err("Order tidak ditemukan!");
                break;
            }

            term_clean();
            draw_init(CENTER_CENTER, 1, 1, WIDTH, 10);
            draw_box(TITLE, MAG, "Proses Orderan User");
            draw_line(CENTER, MAG, 2, BLU"Nama User : "WHT"%s", user.username);
            draw_line(CENTER, MAG, 2, BLU"Nama Orderan : "WHT"%s", user.order.orders[order]);
            draw_decor(MAG);
            draw_line(LEFT, MAG, 0, "Alamat           : %s", user.order.alamat[order]);
            draw_line(LEFT, MAG, 0, "Nomor Telepon    : %s", user.order.telepon[order]);
            draw_line(LEFT, MAG, 1, YEL"Status Orderan   "WHT": %s", user.order.orderStatus[order]);
            draw_decor(MAG);
            draw_input(MAG, 0, "Ganti status :");
            draw_end();

            input_string(new_status);

            if (strlen(new_status) == 0){
                draw_dialog_err("Status tidak boleh kosong!");
                break;
            }

            strncpy(user.order.orderStatus[order], new_status, MAX_STRLEN);

            fseek(database_file, -sizeof(data_t), SEEK_CUR);
            fwrite(&user, sizeof(data_t), 1, database_file);

            draw_dialog_info("Status order berhasil diupdate!");
            break;
        }
    }

    if (!found) {
        draw_dialog_err("User tidak ditemukan!");
    }

    fclose(database_file);
}

void display_admin_order_delete() {
    term_clean();

    char username[MAX_STRLEN] = {0};
    char order_name[MAX_STRLEN] = {0};
    char certainty[MAX_STRLEN] = {0};
    bool user_found = false;

    draw_init(CENTER_CENTER, 1, 1, WIDTH, 3);
    draw_box(TITLE, MAG, "Hapus Order User");
    draw_input(MAG, 1, MAG"Username :");
    draw_end();

    input_string(username);

    FILE* database_file = fopen(DATABASE_FILE, "rb+");
    if (!database_file) {
        draw_dialog_err("Database tidak ditemukan!");
        return;
    }

    data_t user;
    while(fread(&user, sizeof(data_t), 1, database_file) == 1) {
        if (strcmp(user.username, username) == 0) {
            user_found = true;

            if (user.order.orderCount == 0) {
                draw_dialog_err("User tidak memiliki orderan!");
                break;
            }

            term_clean();
            draw_init(CENTER_CENTER, 1, 1, WIDTH, user.order.orderCount + 5);
            draw_box(TITLE, MAG, "Daftar Order User");
            draw_line(LEFT, MAG, 0, "Orderan %s:", username);

            for (int i = 0; i < MAX_ORDERS; i++) {
                if (strlen(user.order.orders[i]) > 0) {
                    draw_line(LEFT, MAG, 0, "%d. %s - %s",
                            i+1,
                            user.order.orders[i],
                            user.order.orderStatus[i]);
                }
            }
            draw_decor(MAG);
            draw_input(MAG, 1, MAG"Pilih order (nomor) :");
            draw_end();

            input_string(order_name);
            int order_num = atoi(order_name) - 1;

            if (order_num < 0 || order_num >= MAX_ORDERS ||
                strlen(user.order.orders[order_num]) == 0) {
                draw_dialog_err("Pilihan order tidak valid!");
                break;
            }

            draw_dialog_confirmation("Hapus order '%s'?",
                                    user.order.orders[order_num]);
            input_string(certainty);

            if (strcasecmp(certainty, "y") == 0) {
                memset(&user.order.orders[order_num], 0, MAX_STRLEN);
                memset(&user.order.orderStatus[order_num], 0, MAX_STRLEN);
                memset(&user.order.alamat[order_num], 0, MAX_STRLEN);
                memset(&user.order.telepon[order_num], 0, MAX_STRLEN);
                user.order.orderCount--;

                fseek(database_file, -sizeof(data_t), SEEK_CUR);
                if (fwrite(&user, sizeof(data_t), 1, database_file) == 1) {
                    draw_dialog_info("Order berhasil dihapus!");
                } else {
                    draw_dialog_err("Gagal menyimpan perubahan!");
                }
            } else {
                draw_dialog_info("Penghapusan dibatalkan");
            }
            break;
        }
    }

    fclose(database_file);

    if (!user_found) {
        draw_dialog_err("User tidak ditemukan!");
    }
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

        draw_box(TITLE, MAG, "Pengaturan Stok");
        draw_line(LEFT, MAG, 3, MAG"Proses stok yang ada pada toko");
        draw_line(LEFT, MAG, 1, MAG"Silahkan pilih menu dibawah!");
        draw_decor(MAG);
        draw_line(LEFT, MAG, 0, "1. Lihat stok yang ada");
        draw_line(LEFT, MAG, 0, "2. Tambah stok baru");
        draw_line(LEFT, MAG, 0, "3. Hapus stok");
        draw_line(LEFT, MAG, 1, RED"0. Keluar");
        draw_decor(MAG);
        draw_input(MAG, 0, "Input :");
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
                draw_dialog_err("Input invalid!");
                break;
        }
    }
}

void display_admin_stock_view(){
    FILE* database_file = fopen(STOCK_FILE, "rb");
    if (!database_file){
        draw_dialog_err("File doesn't exist!");
        return;
    }

    // Nyari produk yang ada
    product_t produk;
    uint8_t produk_count = 0;
    while(fread(&produk, sizeof(product_t), 1, database_file) == 1) {
        if (strlen(produk.nama) != 0) {
            produk_count++;
        }
    }

    rewind(database_file);

    uint8_t height = produk_count + 6;

    term_clean();
    draw_init(CENTER_CENTER, 1, 1, WIDTH, height);
    draw_box(TITLE, MAG, "Lihat seluruh produk");
    draw_line(LEFT, MAG, 0, "No | Nama Produk            | Stok | Harga");
    draw_decor(MAG);

    uint8_t i = 1;
    while(fread(&produk, sizeof(product_t), 1, database_file) == 1){
        if (strlen(produk.nama) != 0) {
            draw_line(LEFT, MAG, 0, "%2i | %-22s | %-4i | %-8i",
                i++, produk.nama, produk.jumlah, produk.harga
            );
        }
    }
    fclose(database_file);

    draw_decor(MAG);
    draw_line(CENTER, MAG, 2, MAG_BG BLK" Press enter to continue... ");
    draw_end();
    getchar();

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
    draw_input(MAG, 1, MAG"Input nama produk :");
    draw_end();
    input_string(nama);

    if (strlen(nama) == 0){
        draw_dialog_err("Nama produk tidak boleh kosong!");
        return;
    }

    term_clean();
    status = draw_init(CENTER_CENTER, 1, 1, WIDTH, 5);
    if (!status) return;

    draw_box(TITLE, MAG, "Tambah produk");
    draw_line(CENTER, MAG, 2, CYN"Nama Produk: "WHT"%s", nama);
    draw_line(LEFT, MAG, 1, MAG"Jumlah Produk :");
    draw_line(LEFT, MAG, 1, MAG"Harga Produk  :");

    draw_change_current_line(2);
    draw_input(MAG, 1, MAG"Jumlah Produk :");
    input_number(&jumlah);
    draw_input(MAG, 1, MAG"Harga Produk  :");
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
        strncpy(produk.nama, nama, sizeof(produk.nama));
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
    draw_input(MAG, 1, MAG"Input nama produk :");
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
        draw_dialog_err("Idk, failed to create temp file ig...");
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

void display_admin_broker(){
    FILE* database_file = fopen(DATABASE_FILE, "rb");

    // Nyari user yang ada
    data_t user;
    uint16_t user_count = 0;
    while(fread(&user, sizeof(data_t), 1, database_file) == 1) {
        if (user.type == BROKER) {
            user_count++;
        }
    }

    rewind(database_file);

    uint8_t height = user_count + 6;
    term_clean();
    draw_init(CENTER_CENTER, 1, 1, WIDTH, height);
    draw_box(TITLE, MAG, "Lihat seluruh broker");
    draw_line(LEFT, MAG, 0, "No | Nama Broker");
    draw_decor(MAG);

    uint8_t i = 0;
    while(fread(&user, sizeof(data_t), 1, database_file) == 1){
        if (user.type == USER) continue;
        draw_line(LEFT, MAG, 1, WHT"%2i | "MAG"%-s",
            ++i, user.username
        );
    }

    draw_decor(MAG);
    draw_line(CENTER, MAG, 2, MAG_BG BLK" Press enter to continue... ");
    draw_end();
    getchar();

    fclose(database_file);

    return;
}

void display_admin_message(){
    term_clean();

    char username[MAX_STRLEN];

    bool status = draw_init(CENTER_CENTER, 1, 1, WIDTH, 3);
    if (status == false) return;

    draw_box(TITLE, MAG, "Kirim Pesan");
    draw_input(MAG, 1, MAG"Kirim pesan ke :");
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

void display_admin_feedback() {
    FILE *feedback_file = fopen(FEEDBACK_FILE, "rb");
    if (!feedback_file) {
        draw_dialog_err("Belum ada feedback yang diterima!");
        return;
    }

    // Nyari jumlah feedback yang ada
    feedback_t feedback;
    int count = 0;
    while (fread(&feedback, sizeof(feedback_t), 1, feedback_file) == 1) {
        count++;
    }

    if (count == 0) {
        draw_dialog_err("Belum ada uneg-uneg yang disampaikan!");
        fclose(feedback_file);
        return;
    }

    rewind(feedback_file);
    term_clean();

    int height = count + 4;
    draw_init(CENTER_CENTER, 1, 1, 65, height);
    draw_box(TITLE_NOBOX, BLU, "Daftar Feedback");
    draw_line(LEFT, BLU, 0, "No | Tanggal     | Nama            | Rating | Feedback");
    draw_decor(BLU);

    int i = 0;
    while (fread(&feedback, sizeof(feedback_t), 1, feedback_file) == 1) {
        draw_line(LEFT, BLU, 0, "%-2i | %-11s | %-15s |  %2d    | %-s",
            ++i,
            feedback.date,
            feedback.username,
            feedback.rating,
            feedback.text
        );
    }

    draw_decor(BLU);
    draw_line(CENTER, BLU, 2, YEL_BG BLK" Press enter to continue... ");
    draw_end();

    fclose(feedback_file);
    getchar();
}
