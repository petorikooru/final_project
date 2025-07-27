#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <time.h>
#include <ctype.h>

// Windows compability
#ifdef _WIN32
    #define term_clean() system("cls");
// Linux and Mac (and BSD ig...)
#else
    #define term_clean() system("clear");
#endif

#define MAX_ORDERS 50
#define MAX_STRLEN 256

#define DEFAULT_BROKER_NAME     "Admin"
#define DEFAULT_BROKER_PASSWORD "123"

#define NOMOR_REKENING      "707563602600"
#define NAMA_BANK           "CIMB NIAGA"
#define ATAS_NAMA           "DAFFA RIZQY ANDIKA"

#define FEEDBACK_FILE       "feedback.dat"
#define DATABASE_FILE       "database.dat"
#define STOCK_FILE          "stok.dat"
#define PESAN_FILE          "message.dat"

#define DATABASE_FILE_TEMP  "database.dat.temp"
#define STOCK_FILE_TEMP     "stok.dat.temp"
#define PESAN_FILE_TEMP     "message.dat.temp"

#define TOKEN_STRING        "Agnes Tachyon"

typedef enum {
    BROKER      = 0,
    USER        = 1,
    NONE        = 2,
} user_t;

typedef struct {
    int     orderCount;
    char    orders      [MAX_ORDERS][MAX_STRLEN];
    char    orderStatus [MAX_ORDERS][MAX_STRLEN];
    char    alamat      [MAX_ORDERS][MAX_STRLEN];
    char    telepon     [MAX_ORDERS][MAX_STRLEN];
} order_t;

typedef struct {
    char username   [MAX_STRLEN];
    char password   [MAX_STRLEN];
    bool banned;

    user_t type;
    order_t order;
} data_t;

typedef struct {
    char    username    [MAX_STRLEN];
    char    date        [MAX_STRLEN];
    char    text        [MAX_STRLEN];

    uint8_t rating;
} feedback_t;

typedef struct {
    char            nama[MAX_STRLEN];
    uint16_t        jumlah;
    uint32_t        harga;
} product_t;

typedef struct {
    char    nama[256];
    char    teks[256];
    char    timestamp[256];
    user_t  whois;
} pesan_t;

void display_utama();

void display_admin_start();
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

void display_user_start();
void display_user_login();
void display_user_register();
void display_user_init(data_t *user);
void display_user_menu();
void display_user_profile();
void display_user_status();
void display_user_stok();
void display_user_stok_cari();
void display_user_stok_beli();
void display_user_stok_view();
void display_user_stok_view_product(int choice);
void display_user_feedback();

void sort_nama(product_t *items, int jmlh_brang);
void sort_harga(product_t *items, int jmlh_brang);
void sort_jumlah(product_t *items, int jmlh_brang);

void display_pesan_start(char *nama, user_t tipe);
void display_pesan_message();
void display_pesan_clear();
void display_pesan_purge();

bool pesan_init();
void pesan_kirim(const char *teks);
void pesan_print();
void pesan_end();
bool pesan_clear();
bool pesan_purge(char *username);

void    input_string(char string[]);

bool    database_user_signup(const char *username, const char *password, data_t *database_user, user_t type);
user_t  database_user_login(const char *username, const char *password, data_t *database_user, user_t type);
void    database_user_init(const char *username, const char *password, data_t *const use, user_t type);
void    database_user_update(data_t *user);

/* GLOBAL VARIABLES */
char        current_user[MAX_STRLEN];
user_t      current_type;
pesan_t     pesan;
FILE        *pesan_ptr;
data_t      data;
struct tm   *current_timestamp;

int main(){
    display_utama();
}

void display_utama(){
    typedef enum {
        M_USER  = 1,
        M_ADMIN = 2,
        M_EXIT  = 0,
    } menu_t;
    int choice;

    while (true) {
        term_clean();

        printf("================== D'Milsurp ==================\n");
        printf("Selamat datang di D'Milsurp, dimana anda dapat \n");
        printf("membeli barang permainan yang menyenangkan!    \n");
        printf("Silahkan pilih menu dibawah!                   \n");
        printf("1. Pergi ke menu User                          \n");
        printf("2. Pergi ke menu Broker                        \n");
        printf("0. Keluar                                      \n");
        printf("Input : ");

        scanf("%i", &choice); getchar();

        switch(choice) {
            case M_USER:
                display_user_start();
                break;
            case M_ADMIN:
                display_admin_start();
                break;
            case M_EXIT:
                term_clean();
                printf("Selamat tinggal!");
                return;
            default:
                printf("Input invalid!");
                getchar();
                break;
        }
    }
}

void display_user_init(data_t *user){
    strncpy(current_user, user->username, MAX_STRLEN);
    data = *user;
}

void display_user_status() {
    term_clean();

    char order_name[MAX_STRLEN] = {0};
    char new_status[MAX_STRLEN] = {0};
    int order_num;

    FILE* database_file = fopen(DATABASE_FILE, "rb+");
    if (!database_file) {
        printf("Database tidak dapat diakses!");
        getchar();
        return;
    }

    data_t user;
    while(fread(&user, sizeof(data_t), 1, database_file) == 1) {
        if (strcmp(user.username, current_user) == 0) {
            if (user.order.orderCount == 0) {
                printf("Anda belum memiliki orderan!");
                getchar();
                fclose(database_file);
                return;
            }

            term_clean();
            printf("================== Daftar Order Anda ================== \n");
            printf("Pilih order yang ingin diupdate:                        \n");
            for (int i = 0; i < user.order.orderCount; i++) {
                printf("%d. %-12s : %s\n", i+1, user.order.orders[i], user.order.orderStatus[i]);
            }
            printf("Masukkan nomor order : ");
            scanf("%i", &order_num); getchar();
            order_num -= 1;

            if (order_num < 0 || order_num >= user.order.orderCount) {
                printf("Nomor order tidak valid!");
                getchar();
                fclose(database_file);
                return;
            }

            term_clean();
            printf("================== Update Status Order ================== \n");
            printf("Order           : %s\n", user.order.orders[order_num]);
            printf("Alamat          : %s\n", user.order.alamat[order_num]);
            printf("Telepon         : %s\n", user.order.telepon[order_num]);
            printf("Status saat ini : %s\n", user.order.orderStatus[order_num]);
            printf("Status baru : ");

            input_string(new_status);

            if (strlen(new_status) == 0) {
                printf("Status tidak boleh kosing!\n");
                getchar();
                break;
            }

            strncpy(user.order.orderStatus[order_num], new_status, MAX_STRLEN);
            fseek(database_file, -sizeof(data_t), SEEK_CUR);
            fwrite(&user, sizeof(data_t), 1, database_file);
            printf("Status berhasil terupdate!\n");
            getchar();
            break;
        }
    }
    fclose(database_file);
}

void display_user_profile(){
    term_clean();

    FILE* database_file = fopen(DATABASE_FILE, "rb");
    data_t* user = malloc(sizeof(data_t));
    if (!database_file){
        printf("File doesn't exist!");
        getchar();
    }

    while(fread(user, sizeof(data_t), 1, database_file) == 1){
        if (strcmp(user->username, current_user) == 0) {
            term_clean();
            printf("================== Profil User ================== \n");
            printf("Nama User       : %s\n", user->username);
            printf("Status          : %s\n", (user->banned) ? "Ban" : "Aman");
            printf("Jumlah terorder : %i\n", user->order.orderCount);
            printf("Orderan: \n");
            for (int i = 0; i < user->order.orderCount; i++){
                printf("%2i. %-12s : %s\n", i + 1, user->order.orders[i], user->order.orderStatus[i]);
            }
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
        printf("================== User Menu ==================     \n");
        printf("Selamat datang di D'Milsurp, %s\n", current_user);
        printf("Pilih opsi dibawah untuk memulai memesan orderan!   \n");
        printf("1. Lihat Profil                                     \n");
        printf("2. Update Status Pesanan                            \n");
        printf("3. Tambah Pesanan                                   \n");
        printf("4. Kirim Feedback Aplikasi                          \n");
        printf("5. Kirim Pesan kepada Broker                        \n");
        printf("0. Logout                                           \n");
        printf("Input : ");

        scanf("%i",&choice); getchar();

        switch(choice) {
            case M_PROFIL:
                display_user_profile();
                break;
            case M_STATUS:
                display_user_status();
                break;
            case M_TAMBAH:
                display_user_stok();
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
                printf("Input invalid!");
                getchar();
                break;
        }
    }
}

void display_user_register(){
    char username[MAX_STRLEN];
    char password[MAX_STRLEN];

    term_clean();
    printf("================== Registrasi User ================== \n");
    printf("Username : ");
    input_string(username);
    printf("Password : ");
    input_string(password);

    data_t* user = malloc(sizeof(data_t));
    bool type = database_user_signup(username, password, user, USER);

    switch(type){
        case true:
            printf("Registrasi berhasil!");
            getchar();

            database_user_init(username, password, user, USER);
            break;
        case false:
            printf("Gagal untuk membuat akun!");
            getchar();

            break;
    }
    free(user);
}

void display_user_login(){
    char username[MAX_STRLEN];
    char password[MAX_STRLEN];

    term_clean();
    printf("================== Login User ==================\n");
    printf("Username : ");
    input_string(username);
    printf("Password : ");
    input_string(password);

    data_t* user = malloc(sizeof(data_t));
    user_t type = database_user_login(username, password, user, USER);

    switch(type){
            case BROKER:
                printf("Mohon untuk pergi ke seleksi Admin!");
                break;
            case USER:
                if (user->banned){
                    printf("Broker telah meng-ban anda!");
                } else {
                    printf("Login berhasil!");
                    getchar();

                    display_user_init(user);
                    display_user_menu();

                    printf("Selamat berbelanja lagi!");
                }
                break;
            case NONE:
                break;
        }
    getchar();
    free(user);
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

        printf("================== User Page ================== \n");
        printf("Pilih opsi dibawa ya...                         \n");
        printf("1. Login User                                   \n");
        printf("2. Registrasi User                              \n");
        printf("0. Keluar                                       \n");
        printf("Input : ");

        scanf("%i",&choice); getchar();

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
                printf("Input invalid!");
                getchar();
                break;
        }
    }
}

void display_user_feedback(){
    char teks[MAX_STRLEN];
    int rating;

    term_clean();
    printf("================== Beri Feedback ==================     \n");
    printf("Berikan uneg-uneg kalian di aplikasi ini!               \n");
    printf("Rating (1-10) : ");
    scanf("%i", &rating); getchar();
    printf("Deskripsi     : ");
    input_string(teks);

    FILE *feedback_file = fopen(FEEDBACK_FILE, "ab");
    if (!feedback_file) feedback_file = fopen(FEEDBACK_FILE, "wb");

    feedback_t feedback = {0};
    if (strlen(teks) != 0) {
        // Get the current timestamp
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

    printf("Terima kasih telah memberikan feedback!");
    getchar();

    return;
}

void display_user_stok(){
    typedef enum {
        M_LIHAT     = 1,
        M_CARI      = 2,
        M_BELI      = 3,
        M_KELUAR    = 0,
    } menu_t;
    int choice;

    while(1) {
        term_clean();
        printf("================== Stok Menu ================== \n");
        printf("Pilih opsi yang anda inginkan!                  \n");
        printf("1. Lihat Stok Barang                            \n");
        printf("2. Cari Barang                                  \n");
        printf("3. Beli Barang                                  \n");
        printf("0. Keluar                                       \n");
        printf("Input : ");

        scanf("%i",&choice); getchar();

        switch(choice) {
            case M_LIHAT:
                display_user_stok_view();
                break;
            case M_CARI:
                display_user_stok_cari();
                break;
            case M_BELI:
                display_user_stok_beli();
                break;
            case M_KELUAR:
                return;
            default:
                printf("Input invalid!");
                getchar();
                break;
        }
    }
}

void display_user_stok_view(){
    typedef enum {
        M_NAMA      = 1,
        M_JUMLAH    = 2,
        M_HARGA     = 3,
        M_KELUAR    = 0,
    } menu_t;
    int choice;

    while(1) {
        term_clean()
        printf("================== Stok Menu ================== \n");
        printf("Pilih opsi untuk mengurutkan barang!            \n");
        printf("1. [A - Z] Urutkan berdasarkan nama             \n");
        printf("2. [1 2 3] Urutkan berdasarkan jumlah           \n");
        printf("3. [Rp...] Urutkan berdasarkan harga            \n");
        printf("0. Keluar\n");
        printf("Input : ");

        scanf("%i", &choice); getchar();

        switch(choice) {
            case M_NAMA:
            case M_JUMLAH:
            case M_HARGA:
                display_user_stok_view_product(choice);
                break;
            case M_KELUAR:
                return;
            default:
                printf("Input invalid!");
                getchar();
                break;
        }
    }
}

void display_user_stok_view_product(int choice){
    term_clean();

    FILE* database_file = fopen(STOCK_FILE, "rb");
    if (!database_file) {
        printf("Database stok tidak ditemukan!");
        getchar();
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
        printf("Sepertinya masih belum ada produk...");
        getchar();
        fclose(database_file);
        return;
    }

    // Untuk diproses ke sortir nanti
    product_t* produk_stok = malloc(jumlah_produk * sizeof(product_t));
    rewind(database_file);

    for (uint16_t i = 0; i < jumlah_produk; i++) {
        fread(&produk_stok[i], sizeof(product_t), 1, database_file);
    }
    fclose(database_file);

    // Sorting stuff
    if (choice == 1)
        sort_nama(produk_stok, jumlah_produk);
    else if (choice == 2)
        sort_jumlah(produk_stok, jumlah_produk);
    else
        sort_harga(produk_stok, jumlah_produk);

    printf("================== Lihat seluruh produk ================== \n");
    printf("No | Nama Produk            | Stok | Harga\n");

    for (uint8_t i = 0; i < jumlah_produk; i++) {
        printf("%2i | %-22s | %-4i | %-8i\n",
                i+1,
                produk_stok[i].nama,
                produk_stok[i].jumlah,
                produk_stok[i].harga);
    }
    getchar();

    free(produk_stok);
    return;
}

/* https://stackoverflow.com/a/25456826
 * A better alternative of regular strcmp since
 * it also consider the upper-lower case letter
 */
int cmpalph(const char *string_1, const char *string_2) {
    const char *cp1 = string_1;
    const char *cp2 = string_2;
    int sccmp = 1;

    for (; toupper(*cp1) == toupper(*cp2); cp1++, cp2++){
        if (*cp1 == '\0') sccmp = 0;
    }
    if (sccmp) 
        return ((toupper(*cp1) < toupper(*cp2)) ? -1 : +1);

    for (; *string_1 == *string_2; string_1++, string_2++){
        if (*string_1 == '\0') return 0;
    }

    return ((*string_1 < *string_2) ? +1 : -1);
}

void sort_nama(product_t *items, int jmlh_brang) {
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

void sort_harga(product_t *items, int jmlh_brang) {
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

void sort_jumlah(product_t *items, int jmlh_brang) {
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

void display_user_stok_cari(){
    term_clean();

    char nama[MAX_STRLEN];

    printf("================== Order Produk ================== \n");
    printf("Nama produk : ");

    input_string(nama);

    FILE* database_file = fopen(STOCK_FILE, "rb");
    if (!database_file) {
        printf("Database stok tidak ditemukan!");
        getchar();
        return;
    }
    product_t produk;
    bool found = false;

    while(fread(&produk, sizeof(product_t), 1, database_file) == 1){
        if (strcmp(produk.nama, nama) == 0) {
            found = 1;

            if (produk.jumlah == 0) {
                printf("Produk tidak tersedia di dalam stok!");
                getchar();
                break;
            }
            term_clean();

            printf("================== Order Produk ==================  \n");
            printf("Nama produk   : %s\n", produk.nama);
            printf("Harga produk  : %i\n", produk.harga);
            printf("Jumlah stok   : %i\n", produk.jumlah);
            getchar();
        }
    }

    if (found == false) {
        printf("Produk tidak ditemukan!");
        getchar();
    }

    fclose(database_file);
    return;
}

void display_user_stok_beli() {
    display_user_stok_view_product(1);
    term_clean();

    char nama[MAX_STRLEN] = {0};
    char certainty[MAX_STRLEN] = {0};
    bool found = false;

    printf("================== Order Produk ================== \n");
    printf("Nama produk : ");

    input_string(nama);

    FILE* database_file = fopen(STOCK_FILE, "rb+");
    if (!database_file) {
        printf("Database stok tidak ditemukan!");
        getchar();
        return;
    }

    product_t produk;
    bool order_success = false;

    while(fread(&produk, sizeof(product_t), 1, database_file) == 1) {
        if (strcmp(produk.nama, nama) == 0) {
            found = true;

            if (produk.jumlah <= 0) {
                printf("Produk tidak tersedia di dalam stok!");
                getchar();
                break;
            }

            term_clean();
            printf("================== Order Produk ==================  \n");
            printf("Nama produk   : %s\n", produk.nama);
            printf("Harga produk  : %d\n", produk.harga);
            printf("Jumlah stok   : %d\n", produk.jumlah);
            printf("Apakah anda yakin ingin membelinya?\n");
            printf("(Y/n) : ");

            input_string(certainty);

            if (strcasecmp(certainty, "N") == 0) {
                printf("Pembelian dibatalkan!");
                getchar();
                break;
            }

            int order_index = -1;
            for (int i = 0; i < MAX_ORDERS; i++) {
                if (strlen(data.order.orders[i]) == 0) {
                    order_index = i;
                    break;
                }
            }

            if (order_index == -1) {
                printf("Uh oh... Orderan sudah full!");
                getchar();
                break;
            }

            term_clean();
            printf("================== Data Pengiriman ==================\n");
            printf("Alamat anda   : ");
            input_string(data.order.alamat[order_index]);
            printf("Nomor telepon : ");
            input_string(data.order.telepon[order_index]);

            strncpy(data.order.orders[order_index], produk.nama, MAX_STRLEN);
            strncpy(data.order.orderStatus[order_index], "Pending", MAX_STRLEN);
            data.order.orderCount++;

            produk.jumlah--;
            fseek(database_file, -sizeof(product_t), SEEK_CUR);
            fwrite(&produk, sizeof(product_t), 1, database_file);

            database_user_update(&data);

            term_clean();
            printf("================== Pembayaran ==================    \n");
            printf("Transfer ke rekening ini ya!                        \n");
            printf("Konfirmasi Pembayaran melalui chat!                 \n");
            printf("Nomor Rekening : %s\n", NOMOR_REKENING);
            printf("Nama Bank      : %s\n", NAMA_BANK);
            printf("Atas Nama      : %s\n", ATAS_NAMA);

            getchar();
            break;
        }
    }

    if (!found) {
        printf("Produk tidak ditemukan!");
        getchar();
    }

    fclose(database_file);
}

void display_admin_start() {
    typedef enum {
        M_LOGIN       = 1,
        M_REGISTER    = 2,
        M_EXIT        = 0,
    } menu_t;
    int choice;

    while(1) {
        term_clean();
        printf("================== Broker Page ==================   \n");
        printf("Pilih opsi dibawa ya...                             \n");
        printf("1. Login Broker                                     \n");
        printf("2. Registrasi Broker                                \n");
        printf("0. Keluar                                           \n");
        printf("Input : ");

        scanf("%i", &choice); getchar();

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
                printf("Input invalid!");
                getchar();
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
        printf("================== Registrasi Broker ==================\n");
        printf("Input token broker : ");

        input_string(token);

        if (strcmp(token, TOKEN_STRING) != 0) {
            printf("Token salah!");
            getchar();
        } else break;

        if (chance == 0) {
            printf("Percobaan telah habis!");
            getchar();
            return;
        }

        printf("Anda memiliki %i percobaan lagi!", chance);
        getchar();
    }

    term_clean();
    printf("================== Registrasi Broker ==================   \n");
    printf("Username : ");
    input_string(username);
    printf("Password : ");
    input_string(password);

    data_t* user = malloc(sizeof(data_t));
    bool type = database_user_signup(username, password, user, BROKER);

    switch(type){
        case true:
            printf("Registrasi berhasil!");
            getchar();

            database_user_init(username, password, user, BROKER);
            break;
        case false:
            printf("Gagal untuk membuat akun!");
            getchar();

            break;
    }
    free(user);
}

void display_admin_login(){
    char username[MAX_STRLEN];
    char password[MAX_STRLEN];
    uint8_t chance = 3;
    bool failed = false;

    while (!failed) {
        term_clean();
        printf("================== Login Broker ==================   \n");
        printf("Username : ");
        input_string(username);
        printf("Password : ");
        input_string(password);

        data_t* user = malloc(sizeof(data_t));
        user_t type = database_user_login(username, password, user, BROKER);
        free(user);

        switch(type){
            case BROKER:
                printf("Login berhasil!");
                getchar();

                display_admin_menu();
                return;
            case USER:
                printf("Anda bukanlah Broker!");
                getchar();

                failed = true;
                return;
            case NONE:
                printf("Login gagal!");
                getchar();

                chance--;
                break;
        }
        if (chance == 0) {
            printf("Percobaan telah habis!");
            getchar();

            return;
        }
        printf("Anda memiliki %i percobaan lagi!", chance);
        getchar();
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

        printf("================== Broker Menu ==================   \n");
        printf("Selamat datang Broker, apa yang akan kamu lakukan?  \n");
        printf("Silahkan pilih menu dibawah!                        \n");
        printf("1. Pengaturan User                                  \n");
        printf("2. Pengaturan Stok                                  \n");
        printf("3. Pengaturan Order                                 \n");
        printf("4. Lihat Broker yang teregistrasi!                  \n");
        printf("5. Kirim pesan ke user                              \n");
        printf("6. Lihat feedback aplikasi                          \n");
        printf("0. Keluar                                           \n");
        printf("Input : ");

        scanf("%i", &choice); getchar();

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
                printf("Selamat tinggal, Broker!");
                getchar();
                return;
            default:
                printf("Input invalid!");
                getchar();
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
        printf("================== Pengaturan User ==================   \n");
        printf("Selamat datang Broker, apa yang akan kamu lakukan?      \n");
        printf("Silahkan pilih menu dibawah!                            \n");
        printf("1. Lihat semua User                                     \n");
        printf("2. Lihat secara detail user                             \n");
        printf("3. Hapus user                                           \n");
        printf("4. Ban user                                             \n");
        printf("5. Unban user                                           \n");
        printf("0. Keluar                                               \n");
        printf("Input : ");

        scanf("%i", &choice); getchar();

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
                printf("Input invalid!");
                getchar();
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
        printf("Tidak ada pengguna terdaftar!");
        getchar();

        fclose(database_file);
        return;
    }
    rewind(database_file);

    term_clean();
    printf("================== Daftar Pengguna ==================\n");
    printf("No | Nama Pengguna            | Status  | Orderan\n");

    int i = 0;
    while(fread(&user, sizeof(data_t), 1, database_file) == 1) {
        if (user.type != BROKER && strlen(user.username) > 0) {
            const char* status = user.banned ? "BANNED" : "Aman";
            printf("%-2d | %-24s | %-7s | %-3d\n",
                ++i,
                user.username,
                status,
                user.order.orderCount
            );
        }
    }
    fclose(database_file);
    getchar();
}

void display_admin_user_view(){
    char username[MAX_STRLEN];

    term_clean();
    printf("================== Lihat User ==================\n");
    printf("Pilih user : ");
    input_string(username);

    FILE* database_file = fopen(DATABASE_FILE, "rb");
    if (!database_file){
        printf("File doesn't exist!");
        getchar();

        return;
    }

    data_t* user = malloc(sizeof(data_t));
    bool found = false;

    while(fread(user, sizeof(data_t), 1, database_file) == 1){
        if (strcmp(user->username, username) == 0) {
            found = true;

            term_clean();
            printf("================== Profil User ================== \n");
            printf("Nama User       : %s\n", user->username);
            printf("Status          : %s\n", (user->banned) ? "Ban" : "Aman");
            printf("Jumlah terorder : %i\n", user->order.orderCount);
            printf("Orderan: \n");
            for (int i = 0; i < user->order.orderCount; i++){
                printf("%2i. %-12s : %s\n", i + 1, user->order.orders[i], user->order.orderStatus[i]);
            }
        }
    }
    if (found == false) {
        printf("User tidak ditemukan!");
    }
    getchar();

    free(user);
    fclose(database_file);

    return;
}

void display_admin_user_delete() {
    display_admin_user_viewall();

    char username[MAX_STRLEN] = {0};
    char certainty[MAX_STRLEN] = {0};
    bool found = false;
    bool is_deleted = false;

    term_clean();
    printf("================== Hapus User ==================\n");
    printf("Pilih user : ");

    input_string(username);

    FILE* database_file = fopen(DATABASE_FILE, "rb");
    if (!database_file) {
        printf("Database tidak ditemukan!");
        getchar();
        return;
    }

    FILE* database_new = fopen(DATABASE_FILE_TEMP, "wb");

    data_t* user = malloc(sizeof(data_t));
    while(fread(user, sizeof(data_t), 1, database_file) == 1) {
        if (strcmp(user->username, username) == 0) {
            found = true;

            if (user->order.orderCount != 0) {
                printf("User masih memiliki orderan!\n");
                getchar();

                fwrite(user, sizeof(data_t), 1, database_new);
                continue;
            }

            if (user->type == BROKER) {
                printf("Anda akan menghapus BROKER!\n");
            }

            printf("Apakah anda yakin? (y/N) ");

            input_string(certainty);

            if (strcasecmp(certainty, "Y") == 0) {
                is_deleted = true;
                continue;
            } else {
                printf("Aksi Dibatalkan!");
                getchar();

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
        printf("User tidak ditemukan!");
        getchar();

        remove(DATABASE_FILE_TEMP);
        return;
    }

    if (is_deleted) {
        pesan_purge(username);
        remove(DATABASE_FILE);
        rename(DATABASE_FILE_TEMP, DATABASE_FILE);

        printf("User berhasil dihapus!");
        getchar();
    } else {
        remove(DATABASE_FILE_TEMP);
    }
}

void display_admin_user_ban(){
    display_admin_user_viewall();

    term_clean();

    char username[MAX_STRLEN];
    char certainty[MAX_STRLEN];

    term_clean();
    printf("================== Ban User ==================\n");
    printf("Pilih user : ");

    input_string(username);

    FILE* database_file = fopen(DATABASE_FILE, "rb+");
    data_t* user = malloc(sizeof(data_t));
    bool found = false;

    if (!database_file){
        printf("File doesn't exist!");
        getchar();
    }

    while(fread(user, sizeof(data_t), 1, database_file) == 1){
        if (strcmp(user->username, username) == 0) {
            found = true;
            printf("Apakah anda yakin? (y/N) ");

            input_string(certainty);

            if (strcasecmp(certainty, "Y") == 0) {
                user->banned = true;

                fseek(database_file, -sizeof(data_t), SEEK_CUR);
                fwrite(user, sizeof(data_t), 1, database_file);

                printf("User berhasil diban!");
                getchar();
            } else {
                printf("Aksi dibatalkan!");
                getchar();
            }
        }
    }

    if (found == false) {
        printf("User tidak ditemukan!");
        getchar();
    }

    free(user);
    fclose(database_file);

    return;
}

void display_admin_user_unban(){
    display_admin_user_viewall();

    term_clean();

    char username[MAX_STRLEN];
    char certainty[MAX_STRLEN];

    term_clean();
    printf("================== Ban User ==================\n");
    printf("Pilih user : ");

    input_string(username);

    FILE* database_file = fopen(DATABASE_FILE, "rb+");
    data_t* user = malloc(sizeof(data_t));
    bool found = false;

    if (!database_file){
        printf("File tidak ada!");
        getchar();
    }

    while(fread(user, sizeof(data_t), 1, database_file) == 1){
        if (strcmp(user->username, username) == 0) {
            found = true;
            printf("Apakah anda yakin? (y/N) ");

            input_string(certainty);

            if (strcasecmp(certainty, "Y") == 0) {
                user->banned = false;

                fseek(database_file, -sizeof(data_t), SEEK_CUR);
                fwrite(user, sizeof(data_t), 1, database_file);

                printf("User berhasil unban!");
                getchar();
            } else {
                printf("Aksi dibatalkan!");
                getchar();
            }
        }
    }

    if (found == false) {
        printf("User tidak ditemukan!");
        getchar();
    }

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
        printf("================== Pengaturan Stok ==================   \n");
        printf("Proses orderan dan Tambah Stok!                         \n");
        printf("Silahkan pilih menu dibawah!                            \n");
        printf("1. Lihat barang yang terorder                           \n");
        printf("2. Proses orderan user                                  \n");
        printf("3. Hapus orderan user                                   \n");
        printf("0. Keluar                                               \n");
        printf("Input : ");

        scanf("%i", &choice); getchar();

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
                printf("Input invalid!");
                getchar();
                break;
        }
    }
}

void display_admin_order_view(){
    term_clean();

    FILE* database_file = fopen(DATABASE_FILE, "rb");
    if (!database_file){
        printf("File doesn't exist!");
        getchar();
        return;
    }

    // Nyari order yang ada
    data_t user;
    uint8_t order_count = 0;
    while(fread(&user, sizeof(data_t), 1, database_file) == 1) {
        order_count += user.order.orderCount;
    }

    if (order_count == 0) {
        printf("Sepertinya masih belum ada yang order...");
        getchar();

        fclose(database_file);
        return;
    }

    uint8_t height = order_count + 6;
    printf("================== Lihat seluruh orderan yang ada ==================\n");
    printf("No | Nama Kustomer  | Produk         | Status \n");

    uint8_t i = 0;
    rewind(database_file);
    while(fread(&user, sizeof(data_t), 1, database_file) == 1) {
        if (user.type == USER && user.order.orderCount > 0) {
            for (int j = 0; j < user.order.orderCount; j++) {
                if (strlen(user.order.orders[j]) > 0) {
                    printf("%2i | %-14s | %-14s | %-s\n",
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

    printf("================== Proses Orderan User ==================\n");
    printf("Pilih user : ");

    input_string(username);

    FILE* database_file = fopen(DATABASE_FILE, "rb+");
    if (!database_file) {
        printf("Database tidak ditemukan!");
        getchar();

        return;
    }

    data_t user;
    while(fread(&user, sizeof(data_t), 1, database_file) == 1) {
        if (strcmp(user.username, username) == 0) {
            found = true;

            if (user.order.orderCount == 0) {
                printf("User tidak memiliki orderan!");
                getchar();

                break;
            }

            printf("Pilih orderan:\n");
            for (int j = 0; j < user.order.orderCount; j++) {
                if (strlen(user.order.orders[j]) > 0) {
                    printf("%2i. %-14s : %-s\n",
                        j + 1,
                        user.order.orders[j],
                        user.order.orderStatus[j]
                    );
                }
            }
            printf("Input : ");
            scanf("%i",&order); getchar();

            order--;
            if (strlen(user.order.orders[order]) == 0){
                printf("Order tidak ditemukan!");
                getchar();
                break;
            }

            term_clean();
            printf("================== Proses Orderan User ==================\n");
            printf("Nama User        : %s\n", user.username);
            printf("Nama Orderan     : %s\n", user.order.orders[order]);
            printf("Alamat           : %s\n", user.order.alamat[order]);
            printf("Nomor Telepon    : %s\n", user.order.telepon[order]);
            printf("Status Orderan   : %s\n", user.order.orderStatus[order]);
            printf("Ganti status : ");

            input_string(new_status);

            if (strlen(new_status) == 0){
                printf("Status tidak boleh kosong!");
                getchar();
                break;
            }

            strncpy(user.order.orderStatus[order], new_status, MAX_STRLEN);

            fseek(database_file, -sizeof(data_t), SEEK_CUR);
            fwrite(&user, sizeof(data_t), 1, database_file);

            printf("Status order berhasil diupdate!");
            getchar();
            break;
        }
    }

    if (!found) {
        printf("User tidak ditemukan!");
        getchar();
    }

    fclose(database_file);
}

void display_admin_order_delete() {
    char username[MAX_STRLEN] = {0};
    char order_name[MAX_STRLEN] = {0};
    char certainty[MAX_STRLEN] = {0};
    bool user_found = false;
    int order_num;

    term_clean();
    printf("================== Hapus Order User ==================\n");
    printf("Username : ");

    input_string(username);

    FILE* database_file = fopen(DATABASE_FILE, "rb+");
    if (!database_file) {
        printf("Database tidak ditemukan!");
        getchar();
        return;
    }

    data_t user;
    while(fread(&user, sizeof(data_t), 1, database_file) == 1) {
        if (strcmp(user.username, username) == 0) {
            user_found = true;

            if (user.order.orderCount == 0) {
                printf("User tidak memiliki orderan!");
                getchar();
                break;
            }

            term_clean();
            printf("================== Daftar Order User ==================\n");
            printf("Orderan %s:\n", username);
            for (int i = 0; i < MAX_ORDERS; i++) {
                if (strlen(user.order.orders[i]) > 0) {
                    printf("%d. %s - %s\n",
                            i+1,
                            user.order.orders[i],
                            user.order.orderStatus[i]);
                }
            }
            printf("Pilih order (nomor) : ");
            scanf("%i", &order_num); getchar();
            order_num -= 1;

            if (order_num < 0 || order_num >= MAX_ORDERS ||
                strlen(user.order.orders[order_num]) == 0) {
                printf("Pilihan order tidak valid!");
                getchar();
                break;
            }

            printf("Hapus order '%s'? (y/N) ", user.order.orders[order_num]);
            input_string(certainty);

            if (strcasecmp(certainty, "Y") == 0) {
                memset(&user.order.orders[order_num], 0, MAX_STRLEN);
                memset(&user.order.orderStatus[order_num], 0, MAX_STRLEN);
                memset(&user.order.alamat[order_num], 0, MAX_STRLEN);
                memset(&user.order.telepon[order_num], 0, MAX_STRLEN);
                user.order.orderCount--;

                fseek(database_file, -sizeof(data_t), SEEK_CUR);
                fwrite(&user, sizeof(data_t), 1, database_file);

                printf("Order berhasil dihapus!");
                getchar();
            }
            else {
                printf("Penghapusan dibatalkan");
                getchar();
            }
            break;
        }
    }

    fclose(database_file);

    if (!user_found) {
        printf("User tidak ditemukan!");
        getchar();
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
        printf("================== Pengaturan Stok ================== \n");
        printf("Proses stok yang ada pada toko\n");
        printf("Silahkan pilih menu dibawah!\n");
        printf("1. Lihat stok yang ada\n");
        printf("2. Tambah stok baru\n");
        printf("3. Hapus stok\n");
        printf("0. Keluar\n");
        printf("Input : ");

        scanf("%i", &choice); getchar();

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
                printf("Input invalid!");
                getchar();
                break;
        }
    }
}

void display_admin_stock_view(){
    FILE* database_file = fopen(STOCK_FILE, "rb");
    if (!database_file){
        printf("File doesn't exist!");
        getchar();
        return;
    }

    term_clean();
    printf("================== Lihat seluruh produk ==================\n");
    printf("No | Nama Produk            | Stok | Harga\n");

    uint8_t i = 1;
    product_t produk;
    while(fread(&produk, sizeof(product_t), 1, database_file) == 1){
        if (strlen(produk.nama) != 0) {
            printf("%2i | %-22s | %-4i | %-8i\n",
                i++, produk.nama, produk.jumlah, produk.harga
            );
        }
    }
    fclose(database_file);
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

    printf("================== Tambah produk ==================\n");
    printf("Input nama produk : ");

    input_string(nama);

    if (strlen(nama) == 0){
        printf("Nama produk tidak boleh kosong!");
        getchar();
        return;
    }

    term_clean();
    printf("================== Tambah produk ==================\n");
    printf("Nama Produk: %s\n", nama);
    printf("Jumlah Produk : ");
    scanf("%i", &jumlah); getchar();
    printf("Harga Produk  : ");
    scanf("%i", &harga); getchar();

    printf("Apakah anda yakin? (y/N) ");
    input_string(certainty);

    if (strcasecmp(certainty, "Y") != 0) {
        printf("Aksi dibatalkan!");
        getchar();
        return;
    }

    FILE* database_file = fopen(STOCK_FILE, "rb+");
    if (!database_file) {
        database_file = fopen(STOCK_FILE, "wb+");
    }

    product_t produk;
    rewind(database_file);
    while(fread(&produk, sizeof(product_t), 1, database_file) == 1) {
        if (strcmp(produk.nama, nama) == 0) {
            found = true;
            produk.jumlah = jumlah;
            produk.harga = harga;

            fseek(database_file, -sizeof(product_t), SEEK_CUR);
            fwrite(&produk, sizeof(product_t), 1, database_file);

            break;
        }
    }

    if (!found) {
        memset(&produk, 0, sizeof(product_t));
        strncpy(produk.nama, nama, sizeof(produk.nama));
        produk.jumlah = jumlah;
        produk.harga = harga;

        fwrite(&produk, sizeof(product_t), 1, database_file);
    }

    fclose(database_file);

    printf("Produk berhasil terupdate!");
    getchar();
}

void display_admin_stock_delete(){
    char nama[MAX_STRLEN] = {0};
    char certainty[MAX_STRLEN] = {0};
    bool found = false;
    bool is_deleted = false;


    FILE* database_file = fopen(STOCK_FILE, "rb");
    if (!database_file) {
        printf("File tidak ada!");
        getchar();
        return;
    }

    term_clean();

    printf("================== Hapus produk ==================\n");
    printf("Input nama produk : ");

    input_string(nama);

    FILE* database_new = fopen(STOCK_FILE_TEMP, "wb");
    product_t produk;
    while(fread(&produk, sizeof(product_t), 1, database_file) == 1) {
        if (strcmp(produk.nama, nama) == 0) {
            found = true;

            printf("Apakah anda yakin? (y/N) ");
            input_string(certainty);

            if (strcasecmp(certainty, "Y") == 0) {
                is_deleted = true;
                continue;
            } else {
                printf("Aksi Dibatalkan!");
                getchar();

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
        printf("Produk tidak ditemukan!");
        getchar();

        remove(STOCK_FILE_TEMP);
        return;
    }

    if (is_deleted) {
        remove(STOCK_FILE);
        rename(STOCK_FILE_TEMP, STOCK_FILE);

        printf("Produk berhasil dihapus!");
        getchar();
    } else {
        remove(STOCK_FILE_TEMP);
    }
}

void display_admin_broker(){
    FILE* database_file = fopen(DATABASE_FILE, "rb");

    term_clean();
    printf("================== Lihat seluruh broker ==================\n");
    printf("No | Nama Broker\n");

    data_t user;
    uint8_t i = 0;

    while(fread(&user, sizeof(data_t), 1, database_file) == 1){
        if (user.type == USER) continue;
        printf("%2i | %-s\n",
            ++i, user.username
        );
    }
    fclose(database_file);
    getchar();

    return;
}

void display_admin_message(){
    term_clean();

    char username[MAX_STRLEN];
    printf("================== Kirim Pesan ==================\n");
    printf("Kirim pesan ke : ");

    input_string(username);

    FILE* database_file = fopen(DATABASE_FILE, "rb");
    data_t* user = malloc(sizeof(data_t));
    bool found = false;

    if (!database_file){
        printf("File doesn't exist!");
        getchar();
        return;
    }

    while(fread(user, sizeof(data_t), 1, database_file) == 1){
        if (strcmp(user->username, username) == 0) {
            display_pesan_start(username, BROKER);
            found = 1;
        }
    }

    if (found == false) {
        printf("User tidak ditemukan!");
        getchar();
    }

    free(user);
    fclose(database_file);

    return;
}

void display_admin_feedback() {
    FILE *feedback_file = fopen(FEEDBACK_FILE, "rb");
    if (!feedback_file) {
        printf("Belum ada feedback yang diterima!");
        getchar();
        return;
    }

    // Nyari jumlah feedback yang ada
    feedback_t feedback;
    int count = 0;
    while (fread(&feedback, sizeof(feedback_t), 1, feedback_file) == 1) {
        count++;
    }

    if (count == 0) {
        printf("Belum ada uneg-uneg yang disampaikan!");
        getchar();

        fclose(feedback_file);
        return;
    }

    rewind(feedback_file);
    term_clean();

    printf("================== Daftar Feedback ==================\n");
    printf("No | Tanggal     | Nama            | Rating | Feedback\n");

    int i = 0;
    while (fread(&feedback, sizeof(feedback_t), 1, feedback_file) == 1) {
        printf("%-2i | %-11s | %-15s |  %2d    | %-s\n",
            ++i,
            feedback.date,
            feedback.username,
            feedback.rating,
            feedback.text
        );
    }

    fclose(feedback_file);
    getchar();
}

bool pesan_init(){
    pesan_ptr       = fopen(PESAN_FILE, "ab+");

    if (pesan_ptr == NULL) {
        pesan_ptr   = fopen(PESAN_FILE, "wb+");
    }
    if (pesan_ptr == NULL) {
        printf("Cannot create or open the file!");
        return false;
    }

    return true;
}

void pesan_print(){
    bool pesan_exist = false;
    rewind(pesan_ptr);

    while(fread(&pesan, sizeof(pesan_t), 1, pesan_ptr) == 1) {
        if (strcmp(current_user, pesan.nama) == 0) {
            pesan_exist = true;
            if (current_type == BROKER){
                if (pesan.whois == BROKER)
                    printf("%s" 
                           "You: %s\n", pesan.timestamp, pesan.teks);
                else
                    printf("%s" 
                           "%s: %s\n", pesan.timestamp, pesan.nama, pesan.teks);;
            } else {
                if (pesan.whois == BROKER)
                    printf("%s" 
                           "Broker : %s\n", pesan.timestamp, pesan.teks);
                else
                    printf("%s" 
                           "You : %s\n", pesan.timestamp, pesan.teks);;
            }
        }
    }

    if (pesan_exist == false)
        printf("Mari memulai percakapan!");
}

void pesan_kirim(const char *teks){
    // Get the timestamp
    time_t time_raw;
    time(&time_raw);
    current_timestamp = localtime(&time_raw);

    // Copy it into te file
    strncpy(pesan.teks, teks, MAX_STRLEN);
    strncpy(pesan.nama, current_user, MAX_STRLEN);
    strncpy(pesan.timestamp, asctime(current_timestamp), MAX_STRLEN);

    if (current_type == BROKER) {
        pesan.whois = BROKER;
    } else {
        pesan.whois = USER;
    }

    fwrite(&pesan, sizeof(pesan_t), 1, pesan_ptr);
    fflush(pesan_ptr);
}

bool pesan_clear(){
    FILE *write_ptr = fopen(PESAN_FILE_TEMP, "wb");
    FILE *read_ptr = fopen(PESAN_FILE, "rb");

    while (fread(&pesan, sizeof(pesan_t), 1, read_ptr) == 1) {
        if (strcmp(current_user, pesan.nama) == 0 && pesan.whois != current_type)
            fwrite(&pesan, sizeof(pesan_t), 1, write_ptr);
    }
    fflush(write_ptr);

    rename(PESAN_FILE_TEMP, PESAN_FILE);
    remove(PESAN_FILE_TEMP);

    fclose(write_ptr);
    fclose(read_ptr);

    return true;
}

bool pesan_purge(char *username){
    FILE *write_ptr = fopen(PESAN_FILE_TEMP, "wb");
    FILE *read_ptr = fopen(PESAN_FILE, "rb");

    while (fread(&pesan, sizeof(pesan_t), 1, read_ptr) == 1) {
        if (strcmp(username, pesan.nama) == 0)
            fwrite(&pesan, sizeof(pesan_t), 1, write_ptr);
    }
    fflush(write_ptr);

    rename(PESAN_FILE_TEMP, PESAN_FILE);
    remove(PESAN_FILE_TEMP);

    fclose(write_ptr);
    fclose(read_ptr);

    return true;
}

void pesan_end() {
    fclose(pesan_ptr);
    pesan_ptr = NULL;
}

void display_pesan_start(char *nama, user_t tipe){
    int choice;

    strncpy(current_user, nama, MAX_STRLEN);
    current_type = tipe;

    if (current_type == BROKER) {
        typedef enum {
            CHAT    = 1,
            CLEAR   = 2,
            PURGE   = 3,
            EXIT    = 0,
        } display_t;

        while(1){
            term_clean();
            printf("================== Chat Menu ==================\n");
            printf("1. Mulai percakapan!\n");
            printf("2. Hapus hanya pesan dari anda\n");
            printf("3. Hapus seluruh percakapan\n");
            printf("0. Keluar\n");
            printf("Input : ");

            scanf("%i", &choice); getchar();

            switch (choice) {
                case CHAT:
                    display_pesan_message();
                    break;
                case CLEAR:
                    display_pesan_clear();
                    break;
                case PURGE:
                    display_pesan_purge();
                    break;
                case EXIT:
                    return;
                default:
                    printf("Input invalid!");
                    getchar();
            }
        }
    } 
    else {
        typedef enum {
            CHAT    = 1,
            CLEAR   = 2,
            EXIT    = 0,
        } display_t;

        while(1){
            term_clean();
            printf("================== Chat Menu ==================\n");
            printf("1. Mulai percakapan!\n");
            printf("2. Hapus hanya pesan dari anda\n");
            printf("0. Keluar\n");
            printf("Input : ");

            scanf("%i", &choice); getchar();

            switch (choice) {
                case CHAT:
                    display_pesan_message();
                    break;
                case CLEAR:
                    display_pesan_clear();
                    break;
                case EXIT:
                    return;
                default:
                    printf("Input invalid!");
                    getchar();
            }
        }
    }
}

void display_pesan_message() {
    char teks[256];

    pesan_init();
    while (true) {
        term_clean();
        pesan_print();

        printf("\n(Ketik \"/exit\" untuk keluar)\n");
        printf("[Ketikan sesuatu disini!] : ");

        input_string(teks);

        if (strcmp(teks, "/exit") != 0) 
            pesan_kirim(teks);
        else
            break;
    }
    pesan_end();
}

void display_pesan_clear(){
    term_clean();

    char choice[MAX_STRLEN];

    printf("Apa anda yakin ingin membersihkan percakapan anda? (y/N) ");
    input_string(choice);

    if (strcmp(choice, "y") == 0 || strcmp(choice, "Y") == 0) {
        pesan_clear();
        printf("Pesan berhasil dibersihkan!");
    } else {
        printf("Aksi dibatalkan!");
    }
    getchar();
}

void display_pesan_purge(){
    term_clean();

    char choice[MAX_STRLEN];

    printf("Apa anda yakin ingin MENGHAPUS SEMUA percakapan? (y/N) ");
    input_string(choice);

    if (strcmp(choice, "y") == 0 || strcmp(choice, "Y") == 0) {
        pesan_purge(current_user);
        printf("Pesan berhasil dihapus!");
    } else {
        printf("Aksi dibatalkan!");
    }
    getchar();
}


void input_string(char string[]) {
    fgets(string, MAX_STRLEN, stdin);
    string[strlen(string) - 1] = '\0';
}

void database_user_update(data_t *user){
    FILE    *database_file = fopen(DATABASE_FILE, "rb+");
    data_t  database_user;

    // Update to file
    while (fread(&database_user, sizeof(data_t), 1, database_file) == 1) {
        if (strcmp(database_user.username, user->username) == 0) {
            fseek(database_file, -sizeof(data_t), SEEK_CUR);
            fwrite(user, sizeof(data_t), 1, database_file);
            break;
        }
    }

    // Save the broker data
    fclose(database_file);
    return;
}

void database_create(FILE *database_file){
    database_file = fopen(DATABASE_FILE, "wb");
    data_t broker;

    // Write into the data_t struct
    memcpy(broker.username, DEFAULT_BROKER_NAME, MAX_STRLEN);
    memcpy(broker.password, DEFAULT_BROKER_PASSWORD, MAX_STRLEN);
    broker.banned = false;

    // Save the broker data
    fwrite(&broker, sizeof(data_t), 1, database_file);
    fclose(database_file);
}

void database_user_init(const char *username, const char *password, data_t *const user, const user_t type){

    // Create a new user
    strncpy(user->username, username, sizeof(user->username));
    strncpy(user->password, password, sizeof(user->password));
    user->banned = false;
    user->type  = type;

    // Save the user data
    FILE* database_file = fopen(DATABASE_FILE, "ab");
    fwrite(user, sizeof(data_t), 1, database_file);
    fclose(database_file);
}

user_t database_user_login(const char *username, const char *password, data_t *database_user, user_t type){
    FILE* database_file = fopen(DATABASE_FILE, "rb");
    if (!database_file) {
        return NONE;
    }

    bool found = false;
    user_t user_type;

    while(fread(database_user, sizeof(data_t), 1, database_file) == 1){
        if (strcmp(username, database_user->username) == 0){
            found = true;

            // Check the password + the type of the user
            if (strcmp(database_user->password, password) == 0){
                if (database_user->type == type)
                    user_type = type;
                else
                    user_type = database_user->type;
                break;
            } 

            else {
                printf("Wrong Password bleh :p\n");
                user_type = NONE;
                break;
            }
        }
    }

    if (!found){
        printf("User tidak ditemukan! :p\n");
        user_type = NONE;
    }

    fclose(database_file);
    return user_type;
}

bool database_user_signup( const char *username, const char *password,
                                   data_t *database_user, user_t type){
    // If the user input blank
    if (strlen(username) == 0){
        printf("Username cannot be blank!\n");
        return false;
    } else if (strlen(password) == 0){
        printf("Password cannot be blank!\n");
        return false;
    }

    // Check whether the data exist or not yet
    FILE* database_file = fopen(DATABASE_FILE, "rb+");
    if (!database_file){
        database_create(database_file);
        database_file = fopen(DATABASE_FILE, "rb+");
    }

    // Check whether the username is already registered
    bool found = false;
    while(fread(database_user, sizeof(data_t), 1, database_file) == 1){
        if (strcmp(username, database_user->username) == 0){
            found = true;
            break;
        }
    }
    fclose(database_file);

    if (found){
        printf("Username already exist!\n");
        return false;
    } else
        return true;
}
