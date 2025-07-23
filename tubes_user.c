#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tubes_database.h"
#include "tubes_message.h"
#include "tubes_user.h"
#include "tubes_interface.h"
#include "tubes_handler.h"
#include "tubes_input.h"
#include "tubes_stok.h"

static char current_user[MAX_STRLEN];
static data_t data;

void display_user_feedback();

void display_user_init(data_t *user){
    strncpy(current_user, user->username, MAX_STRLEN);
    data = *user;
}

void display_user_status() {
    term_clean();

    char order_name[MAX_STRLEN] = {0};
    char new_status[MAX_STRLEN] = {0};
    bool order_found = false;
    bool update_success = false;

    FILE* database_file = fopen(DATABASE_FILE, "rb+");
    if (!database_file) {
        draw_dialog_err("Database tidak dapat diakses!");
        return;
    }

    data_t user;
    while(fread(&user, sizeof(data_t), 1, database_file) == 1) {
        if (strcmp(user.username, current_user) == 0) {
            if (user.order.orderCount == 0) {
                draw_dialog_err("Anda belum memiliki orderan!");
                fclose(database_file);
                return;
            }

            term_clean();
            draw_init(CENTER_CENTER, 1, 1, WIDTH, user.order.orderCount + 6);
            draw_box(TITLE, BLU, "Daftar Order Anda");
            draw_line(LEFT, BLU, 0, "Pilih order yang ingin diupdate:");
            draw_decor(BLU);
            for (int i = 0; i < user.order.orderCount; i++) {
                draw_line(LEFT, BLU, 0, "%d. %-12s : %s",
                        i+1,
                        user.order.orders[i],
                        user.order.orderStatus[i]);
            }
            draw_decor(BLU);
            draw_input(BLU, 0, "Masukkan nomor order :");
            draw_end();

            draw_restore_input();

            input_string(order_name);
            int order_num = atoi(order_name) - 1;

            if (order_num < 0 || order_num >= user.order.orderCount) {
                draw_dialog_err("Nomor order tidak valid!");
                fclose(database_file);
                return;
            }

            term_clean();
            draw_init(CENTER_CENTER, 1, 1, WIDTH, 8);
            draw_box(TITLE, BLU, "Update Status Order");
            draw_line(LEFT, BLU, 2, BLU"Order          : "WHT"%s", user.order.orders[order_num]);
            draw_line(LEFT, BLU, 2, CYN"Alamat         : "WHT"%s", user.order.alamat[order_num]);
            draw_line(LEFT, BLU, 2, YEL"Telepon        : "WHT"%s", user.order.telepon[order_num]);
            draw_line(LEFT, BLU, 2, GRN"Status saat ini: "WHT"%s", user.order.orderStatus[order_num]);
            draw_decor(BLU);
            draw_input(BLU, 0, "Status baru :");
            draw_end();

            input_string(new_status);

            strncpy(user.order.orderStatus[order_num], new_status, MAX_STRLEN);

            fseek(database_file, -sizeof(data_t), SEEK_CUR);
            if (fwrite(&user, sizeof(data_t), 1, database_file) == 1) {
                update_success = true;
                draw_dialog_info("Status order berhasil diperbarui!");
            } else {
                draw_dialog_err("Gagal menyimpan perubahan!");
            }

            break;
        }
    }

    fclose(database_file);

    if (!update_success && !order_found) {
        draw_dialog_err("Tidak dapat menemukan data user!");
    }
}

void display_user_profile(){
    term_clean();

    FILE* database_file = fopen(DATABASE_FILE, "rb");
    data_t* user = malloc(sizeof(data_t));

    if (!database_file){
        draw_dialog_err("File doesn't exist!");
    }

    while(fread(user, sizeof(data_t), 1, database_file) == 1){
        if (strcmp(user->username, current_user) == 0) {

            term_clean();
            draw_init(CENTER_CENTER, 1, 1, WIDTH, user->order.orderCount + 9);
            draw_box(TITLE, BLU, "Profil User");
            draw_line(LEFT, BLU, 0, "Nama User      : %s", user->username);
            draw_line(LEFT, BLU, 0, "Status         : %s", (user->banned) ? "Ban" : "Aman");
            draw_line(LEFT, BLU, 0, "Jumlah terorder: %i", user->order.orderCount);
            draw_decor(BLU);
            draw_line(CENTER, BLU, 2, BLU_BG BLK" Orderan: ");
            for (int i = 0; i < user->order.orderCount; i++){
                draw_line(LEFT, BLU, 0, "%2i. %-12s : %s",
                    i + 1, user->order.orders[i], user->order.orderStatus[i]
                );
            }
            draw_decor(BLU);
            draw_line(CENTER, BLU, 2, BLU_BG BLK" Press enter to continue... ");
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
        M_FEEDBACK  = 4,
        M_PESAN     = 5,
        M_LOGOUT    = 0,
    } menu_t;
    int choice;

    while(1) {
        term_clean();

        bool status = draw_init(CENTER_CENTER, 1, 1, WIDTH, 13);
        if (status == false) return;

        draw_box(TITLE, BLU, "User Menu");
        draw_line(LEFT, BLU, 5, BLU"Selamat datang di "BLU"D'Milsurp"RST", "GRN"%s"RST, current_user);
        draw_line(LEFT, BLU, 1, BLU"Pilih opsi dibawah untuk memulai memesan orderan!");
        draw_decor(BLU);
        draw_line(LEFT, BLU, 0, "1. Lihat Profil");
        draw_line(LEFT, BLU, 0, "2. Update Status Pesanan");
        draw_line(LEFT, BLU, 0, "3. Tambah Pesanan");
        draw_line(LEFT, BLU, 0, "4. Kirim Feedback Aplikasi");
        draw_line(LEFT, BLU, 0, "5. Kirim Pesan kepada Broker");
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
            case M_FEEDBACK:
                display_user_feedback();
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
    draw_line(LEFT, BLU, 1, BLU"Username :");
    draw_line(LEFT, BLU, 1, BLU"Password :");

    draw_change_current_line(1);
    draw_input(BLU, 1, BLU"Username :");
    input_string(username);
    draw_input(BLU, 1, BLU"Password :");
    input_string(password);

    data_t* user = malloc(sizeof(data_t));
    choice_t type = database_user_signup(username, password, user, USER);

    switch(type){
        case C_SUCCESS:
            draw_dialog_continue("Registrasi berhasil!");
            database_user_init(username, password, user, USER);
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
    draw_line(LEFT, BLU, 1, BLU"Username :");
    draw_line(LEFT, BLU, 1, BLU"Password :");

    draw_change_current_line(1);
    draw_input(BLU, 1, BLU"Username :");
    input_string(username);
    draw_input(BLU, 1, BLU"Password :");
    input_string(password);

    data_t* user = malloc(sizeof(data_t));
    database_user_t type = database_user_login(username, password, user, USER);

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
                    draw_dialog_info("Selamat berbelanja lagi!");
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
        draw_input(BLU, 0, "Input :");
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

void display_user_feedback(){
    char teks[MAX_STRLEN];
    int rating;

    term_clean();
    draw_init(CENTER_CENTER, 1, 1, WIDTH, 5);
    draw_box(TITLE, BLU, "Beri Feedback");
    draw_line(LEFT, BLU, 0, WHT"Berikan uneg-uneg kalian di aplikasi ini!");
    draw_line(LEFT, BLU, 1, GRN"Rating (1-10) :");
    draw_line(LEFT, BLU, 1, GRN"Deskripsi     :");

    draw_change_current_line(2);
    draw_input(BLU, 1, GRN"Rating (0-10) :");
    input_number(&rating);
    draw_input(BLU, 1, GRN"Deskripsi     :");
    input_string(teks);
    draw_end();

    FILE *feedback_file = fopen(FEEDBACK_FILE, "ab");
    if (!feedback_file) feedback_file = fopen(FEEDBACK_FILE, "wb");

    feedback_t feedback = {0};
    if (strlen(teks) != 0) {

        time_t now = time(NULL);
        struct tm *local = localtime(&now);

        strftime(feedback.date, MAX_STRLEN, "%d/%m/%Y", local);

        strncpy(feedback.username,  current_user, MAX_STRLEN - 1);
        strncpy(feedback.text,      teks, MAX_STRLEN - 1);
        strftime(feedback.date,     MAX_STRLEN, "%d/%m/%Y", local);
        feedback.rating = rating;

        fwrite(&feedback, sizeof(feedback_t), 1, feedback_file);
        fflush(feedback_file);
    }
    fclose(feedback_file);

    draw_dialog_info("Terim akasih telah memberikan feedback!");

    return;
}
