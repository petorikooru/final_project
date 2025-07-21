#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tubes_manajemen.h"
#include "tubes_interface.h"
#include "tubes_handler.h"
#include "tubes_input.h"

static char current_user[MAX_STRLEN];
static user_order_t users[MAX_USERS];
int userCount = 0;

int loginUser () {
    term_clean();
    char username[MAX_LENGTH];
    char password[MAX_LENGTH];
    int attempts = 0;

    printf("=== LOGIN USER ===\n");
    while(attempts < 3) {
        printf("Username: ");
        fgets(username, MAX_LENGTH, stdin);
        username[strcspn(username, "\n")] = '\0';
        printf("Password: ");
        fgets(password, MAX_LENGTH, stdin);
        password[strcspn(password, "\n")] = '\0';
        for(int i = 0; i < userCount; i++) {
            if(strcmp(username, users[i].username) == 0 && strcmp(password, users[i].password) == 0) {
                return i;
            }
        }

        printf("\nLogin gagal! Kesempatan tersisa: %d\n", 2-attempts);
        attempts++;
    }

    printf("\nAnda telah melebihi batas percobaan login.\n");
    system("pause");
    return -1;
}

void lihatProfile(int userIndex) {
    term_clean();
    printf("=== PROFIL USER ===\n");
    printf("Username: %s\n", users[userIndex].username);
    printf("--- RIWAYAT PESANAN ---\n");
    if(users[userIndex].orderCount == 0) {
        printf("Belum ada pesanan...\n");
    } else {
        for(int i = 0; i < users[userIndex].orderCount; i++) {
            printf("%d. %s - Status: %s\n", i+1, users[userIndex].orders[i], users[userIndex].orderStatus[i]);
        }
    }
    system("pause");
}

void updateStatusPesanan(int userIndex) {
    int orderNum;
    char newStatus[MAX_LENGTH];
    term_clean();
    if(users[userIndex].orderCount == 0) {
        printf("Belum ada pesanan untuk diupdate.\n");
        system("pause");
        return;
    }

    printf("=== UPDATE STATUS PESANAN ===\n");
    printf("Daftar pesanan Anda:\n");
    for(int i = 0; i < users[userIndex].orderCount; i++) {
        printf("%d. %s - Status: %s\n", i+1, users[userIndex].orders[i], users[userIndex].orderStatus[i]);
    }

    printf("Masukkan status pesanan baru: ");
    fgets(newStatus, MAX_LENGTH, stdin);
    newStatus[strcspn(newStatus, "\n")] = '\0';
    strcpy(users[userIndex].orderStatus[orderNum-1], newStatus);
    printf("Status pesanan berhasil diupdate!\n");
    system("pause");
}

void tambahPesanan(int userIndex) {
    term_clean();
    if(users[userIndex].orderCount >= MAX_ORDERS) {
        printf("Batas pesanan telah tercapai\n");
        system("pause");
        return;
    }

    printf("=== PESANAN BARU ===\n");
    printf("Keterangan pesanan: ");
    fgets(users[userIndex].orders[users[userIndex].orderCount], MAX_LENGTH, stdin);
    users[userIndex].orders[users[userIndex].orderCount][strcspn(users[userIndex].orders[users[userIndex].orderCount], "\n")] = '\0';
    strcpy(users[userIndex].orderStatus[users[userIndex].orderCount], "Pending");
    users[userIndex].orderCount++;
    printf("Pesanan berhasil ditambahkan!\n");
    system("pause");
}

void display_user_add(){

}

void display_user_status(){

}

void display_user_profile(){

}

void display_user_menu() {
    typedef enum {
        M_PROFIL    = 1,
        M_STATUS    = 2,
        M_TAMBAH    = 3,
        M_LOGOUT    = 0,
    } menu_t;
    int choice;

    while(1) {
        term_clean();

        bool status = draw_init(CENTER_CENTER, 1, 1, WIDTH, 11);
        if (status == false) return;

        draw_box(TITLE, BLU, "User Menu");
        draw_line(LEFT, BLU, 5, BLU"Selamat datang di "MAG"D'Milsurp"RST", "GRN"%s"RST, current_user);
        draw_line(LEFT, BLU, 1, BLU"Pilih menu dibawa untuk memulai memesan orderan!");
        draw_decor(BLU);
        draw_line(LEFT, BLU, 0, "1. Lihat Profil");
        draw_line(LEFT, BLU, 0, "2. Update Status Pesanan");
        draw_line(LEFT, BLU, 0, "3. Tambah Pesanan");
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
                display_user_add();
                break;
            case M_LOGOUT:
                memset(current_user, 0, MAX_STRLEN);
                return;
            default:
                log_print_err("Invalid input! Press enter to continue...");
                getchar();
                break;
        }
    }
}

void display_user_register(){
    typedef enum {
        C_SUCCESS,
        C_FAILED,
    } choice_t;
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

    draw_end();
}

void display_user_login(){
    typedef enum {
        C_SUCCESS,
        C_FAILED,
    } choice_t;
    char username[MAX_STRLEN];
    char password[MAX_STRLEN];

    term_clean();

    bool status = draw_init(CENTER_CENTER, 1, 1, WIDTH, 4);
    if (status == false) return;

    draw_box(TITLE, BLU, "Login User");
    draw_line(LEFT, BLU, 1, BLU"Username:");
    draw_line(LEFT, BLU, 1, BLU"Password:");

    draw_change_current_line(1);
    draw_input(BLU, 1, BLU"Username:");
    input_string(username);
    draw_input(BLU, 1, BLU"Password:");
    input_string(password);

    strncpy(current_user, username, MAX_STRLEN);
    display_user_menu();

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
                break;
            case M_REGISTER:
                display_user_register();
                break;
            case M_EXIT:
                return;
            default:
                log_print_err("Invalid input! Press enter to continue...");
                getchar();
                break;
        }
    }
}
