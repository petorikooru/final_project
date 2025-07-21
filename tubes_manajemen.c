#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tubes_database.h"
#include "tubes_message.h"
#include "tubes_manajemen.h"
#include "tubes_interface.h"
#include "tubes_handler.h"
#include "tubes_input.h"
#include "tubes_stok.h"

static char current_user[MAX_STRLEN];
static data_t data;

void display_user_init(data_t *user){
    strncpy(current_user, user->username, MAX_STRLEN);
    data = *user;
}

void display_user_status(){
    term_clean();

    char new_status[MAX_STRLEN] = {0};
    char order[MAX_STRLEN] = {0};
    bool found_order = false;

    bool status = draw_init(CENTER_CENTER, 1, 1, WIDTH, 3);
    if (!status) return;

    FILE* database_file = fopen(DATABASE_FILE, "rb+");
    if (!database_file) {
        draw_dialog_err("Database tidak ditemukan!");
        return;
    }

    data_t user;
    bool update_success = false;

    while(fread(&user, sizeof(data_t), 1, database_file) == 1) {
        if (strcmp(user.username, current_user) == 0) {
            if (user.order.orderCount == 0) {
                draw_dialog_err("User tidak memiliki orderan!");
                break;
            }

            term_clean();
            status = draw_init(TOP_CENTER, 1, 1, WIDTH, 3);
            if (!status) {
                fclose(database_file);
                return;
            }

            draw_box(TITLE_NOBOX, MAG, "Proses Orderan User");
            draw_input(MAG, 0, "Pilih orderan:");
            draw_end();

            input_string(order);

            for (int j = 0; j < user.order.orderCount; j++) {
                if (strcmp(order, user.order.orders[j]) == 0) {
                    found_order = true;

                    term_clean();
                    status = draw_init(TOP_CENTER, 1, 1, WIDTH, 7);
                    if (!status) {
                        fclose(database_file);
                        return;
                    }

                    draw_box(TITLE_NOBOX, MAG, "Proses Orderan User");
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
                draw_dialog_err("Kamu tidak memiliki orderan tersebut!");
            }
            break;
        }
    }
    fclose(database_file);
}

void display_user_profile(){
    term_clean();

    bool status = draw_init(CENTER_CENTER, 1, 1, WIDTH, 3);
    if (status == false) return;

    FILE* database_file = fopen(DATABASE_FILE, "rb");
    data_t* user = malloc(sizeof(data_t));
    bool found = false;

    if (!database_file){
        draw_dialog_err("File doesn't exist!");
    }

    while(fread(user, sizeof(data_t), 1, database_file) == 1){
        if (strcmp(user->username, current_user) == 0) {

            term_clean();

            char username[MAX_STRLEN];

            bool status = draw_init(TOP_CENTER, 1, 1, WIDTH, 3);
            if (status == false) return;

            draw_box(TITLE_NOBOX, MAG, "Profil User");
            draw_line(LEFT, MAG, 0, "Nama User\t: %s", user->username);
            draw_line(LEFT, MAG, 0, "Status\t\t: %s", (user->banned) ? "Ban" : "Aman");
            draw_line(LEFT, MAG, 0, "Jumlah terorder: %i", user->order.orderCount);
            draw_line(LEFT, MAG, 0, "Orderan\t:");
            for (int i = 0; i < user->order.orderCount; i++){
                draw_line(LEFT, MAG, 0, "%2i. %s : %s",
                    i + 1, user->order.orders[i], user->order.orderStatus[i]
                );
            }
            draw_line(CENTER, MAG, 1, MAG_BG" Press enter to continue... ");
            draw_end();
            getchar();
        }
    }

    free(user);
    fclose(database_file);

    return;
}

void display_user_menu() {
    typedef enum {
        M_PROFIL    = 1,
        M_STATUS    = 2,
        M_TAMBAH    = 3,
        M_PESAN     = 4,
        M_LOGOUT    = 0,
    } menu_t;
    int choice;

    while(1) {
        term_clean();

        bool status = draw_init(CENTER_CENTER, 1, 1, WIDTH, 12);
        if (status == false) return;

        draw_box(TITLE, BLU, "User Menu");
        draw_line(LEFT, BLU, 5, BLU"Selamat datang di "MAG"D'Milsurp"RST", "GRN"%s"RST, current_user);
        draw_line(LEFT, BLU, 1, BLU"Pilih opsi dibawah untuk memulai memesan orderan!");
        draw_decor(BLU);
        draw_line(LEFT, BLU, 0, "1. Lihat Profil");
        draw_line(LEFT, BLU, 0, "2. Update Status Pesanan");
        draw_line(LEFT, BLU, 0, "3. Tambah Pesanan");
        draw_line(LEFT, BLU, 0, "4. Kirim Pesan kepada Broker");
        draw_line(LEFT, BLU, 1, RED"0. Logout");
        draw_decor(BLU);
        draw_input(BLU, 0, "Input:");
        draw_end();

        input_number(&choice);

        switch(choice) {
            case M_PROFIL:
                display_user_profile();
                break;
            case M_STATUS:
                display_user_status();
                break;
            case M_TAMBAH:
                display_stok_start(&data);;
                break;
            case M_PESAN:
                display_pesan_start(current_user, USER);
                break;
            case M_LOGOUT:
                memset(current_user, 0, MAX_STRLEN);
                return;
            default:
                draw_dialog_err("Input tidak valid!");
                break;
        }
    }
}

void display_user_register(){
    char username[MAX_STRLEN];
    char password[MAX_STRLEN];

    term_clean();

    bool status = draw_init(CENTER_CENTER, 1, 1, WIDTH, 4);
    if (status == false) return;

    draw_box(TITLE, BLU, "Registrasi User");
    draw_line(LEFT, BLU, 1, BLU"Username:");
    draw_line(LEFT, BLU, 1, BLU"Password:");

    draw_change_current_line(1);
    draw_input(BLU, 1, BLU"Username:");
    input_string(username);
    draw_input(BLU, 1, BLU"Password:");
    input_string(password);

    data_t* user = malloc(sizeof(data_t));
    choice_t type = database_user_signup(username, password, user);

    switch(type){
        case C_SUCCESS:
            draw_dialog_continue("Registrasi berhasil!");
            database_user_init(username, password, user);
            break;
        case C_FAILED:
            draw_dialog_err("Gagal untuk membuat akun!");
            break;
    }

    free(user);
    draw_end();
}

void display_user_login(){
    char username[MAX_STRLEN];
    char password[MAX_STRLEN];

    term_clean();

    bool status = draw_init(CENTER_CENTER, 1, 1, WIDTH, 4);
    if (status == false) return;

    draw_box(TITLE, BLU, "Login User");
    draw_line(LEFT, BLU, 1, BLU"Username:");
    draw_line(LEFT, BLU, 1, BLU"Password:");

    draw_change_current_line(1);
    draw_input(BLU, 1, BLU"Username: ");
    input_string(username);
    draw_input(BLU, 1, BLU"Password: ");
    input_string(password);

    data_t* user = malloc(sizeof(data_t));
    database_user_t type = database_user_login(username, password, user);

    switch(type){
            case D_BROKER:
                draw_dialog_err("Mohon untuk pergi ke seleksi Admin!");
                break;
            case D_USER:
                if (user->banned){
                    draw_dialog_err("Broker telah meng-ban anda!");
                } else {
                    draw_dialog_continue("Login berhasil!");
                    display_user_init(user);
                    display_user_menu();
                }
                break;
            case D_NONE:
                break;
        }
    free(user);
    draw_end();
}

void display_user_start() {
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

        draw_box(TITLE, BLU, "User Page");
        draw_line(LEFT, BLU, 1, BLU"Pilih opsi dibawa ya...");
        draw_decor(BLU);
        draw_line(LEFT, BLU, 0, "1. Login User");
        draw_line(LEFT, BLU, 0, "2. Registrasi User");
        draw_line(LEFT, BLU, 1, RED"0. Keluar");
        draw_decor(BLU);
        draw_input(BLU, 0, "Input:");
        draw_end();

        input_number(&choice);

        switch(choice) {
            case M_LOGIN:
                display_user_login();
                return;
            case M_REGISTER:
                display_user_register();
                break;
            case M_EXIT:
                return;
            default:
                draw_dialog_err("Invalid input! Press enter to continue...");
                break;
        }
    }
}
