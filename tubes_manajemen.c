#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tubes_manajemen.h"
#include "tubes_handler.h"
#include "tubes_input.h"

User  users[MAX_USERS];
int userCount = 0;

int registerUser () {
    term_clean();
    if(userCount >= MAX_USERS) {
        printf("Batas user telah tercapai!\n");
        system("pause");
        return -1;
    }

    printf("=== REGISTRASI USER BARU ===\n");
    printf("Masukkan username: ");
    fgets(users[userCount].username, MAX_LENGTH, stdin);
    users[userCount].username[strcspn(users[userCount].username, "\n")] = '\0';
    printf("Masukkan password: ");
    fgets(users[userCount].password, MAX_LENGTH, stdin);
    users[userCount].password[strcspn(users[userCount].password, "\n")] = '\0';

    users[userCount].orderCount = 0;
    userCount++;
    printf("\nRegistrasi berhasil!\n");
    system("pause");
    return 1;
}

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

void manajemen_init() {
    int choice;
    int loggedInUser  = -1;

    while(1) {
        term_clean();
        if(loggedInUser  == -1) {
            printf("=== MENU UTAMA USER ===\n");
            printf("1. Registrasi\n");
            printf("2. Login\n");
            printf("3. Keluar\n");
            printf("Pilihan: ");
            scanf("%d", &choice);
            getchar();

            switch(choice) {
                case 1:
                    registerUser ();
                    break;
                case 2:
                    loggedInUser  = loginUser ();
                    break;
                case 3:
                    printf("Terimakasih telah menggunakan D'Milsurp!");
                    exit(0);
                default:
                    printf("Pilihan tidak valid\n");
                    system("pause");
            }
        } else {
            printf("SELAMAT DATANG DI D'MILSURP, %s!\n", users[loggedInUser ].username);
            printf("1. Lihat Profil\n");
            printf("2. Update Status Pesanan\n");
            printf("3. Tambah Pesanan\n");
            printf("4. Logout\n");
            printf("Pilih opsi: ");
            scanf("%d", &choice);
            getchar();

            switch(choice) {
                case 1:
                    lihatProfile(loggedInUser );
                    break;
                case 2:
                    updateStatusPesanan(loggedInUser );
                    break;
                case 3:
                    tambahPesanan(loggedInUser );
                    break;
                case 4:
                    loggedInUser  = -1;
                    printf("Anda telah logout, terimakasih telah menggunakan D'Milsurp!\n");
                    system("pause");
                    break;
                default:
                    printf("Pilihan tidak valid! coba lagi.\n");
                    system("pause");
            }
        }
    }

    return;
}
