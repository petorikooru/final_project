/* =========== Libraries ============= */
/*
    // <- already defined in another
          file
*/
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "tubes_interface.h"
#include "tubes_handler.h"
#include "tubes_database.h"

/* ============ Functions ============ */
void database_update(data_t *user){
    /*
        Update database file

        user  : Update the database of this user

        Return : None
    */

    FILE    *database_file = fopen(FILENAME, "rb+");
    data_t  *database_user = malloc(sizeof(data_t));

    // Update to file
    while (fread(database_user, sizeof(data_t), 1, database_file) == 1) {
        if (strcmp(database_user->username, user->username) == 0) {
            fseek(database_file, -sizeof(data_t), SEEK_CUR);
            fwrite(user, sizeof(data_t), 1, database_file);
            break;
        }
    }
    fclose(database_file);
    free(database_user);
    return;
}

static void database_create(FILE *database_file){
    /*
        Create a new database file (*.dat)

        database_file : the database file you want to create

        Return : None
    */

    database_file = fopen(FILENAME, "wb");

    // Default admin configuration
    const char username[] = "Admin";
    const char password[] = "123";
    data_t *admin = malloc(sizeof(data_t));

    // Account creation
    memcpy(admin->username, username, sizeof(username));
    memcpy(admin->password, password, sizeof(password));
    admin->banned = false;

    fwrite(admin, sizeof(data_t), 1, database_file);
    fclose(database_file);
    free(admin);
}

void database_user_init(const char *username, const char *password,
                               data_t *const user){
    /*
        Initialize new user

        username        : user's username
        password        : user's password
        user            : user that you want to initialize
        user_balances   : user's balances

        Return : None
    */

    strncpy(user->username, username, sizeof(user->username));
    strncpy(user->password, password, sizeof(user->password));

    user->banned = false;
    user->type  = USER;

    // Save the user data
    FILE* database_file = fopen(FILENAME, "ab");
    fwrite(user, sizeof(data_t), 1, database_file);
    fclose(database_file);
}

database_user_t database_user_login(const char *username, const char *password, data_t *database_user){
    /*
        Check whether the user is an admin, user, or none of them

        Offset in here is used to handle error message

        offset_x        : x cordinate position
        offset_y        : y cordinate position
        username        : user's username
        password        : user's password
        database_user   : The Users that are in the database

        Return : user type
    */

    FILE* database_file = fopen(FILENAME, "rb");
    if (!database_file) {
        return D_NONE;
    }

    database_user_t user_type;
    bool found = false;

    while(fread(database_user, sizeof(data_t), 1, database_file) == 1){
        if (strcmp(username, database_user->username) == 0){
            found = true;

            if (strcmp(database_user->password, password) == 0){
                if (strcmp(username, "Admin") == 0)
                    user_type = D_BROKER;
                else
                    user_type = D_USER;
                break;
            } else {
                draw_dialog_continue("Wrong Password bleh :p");
                user_type = D_NONE;
                break;
            }
        }
    }

    if (!found){
        draw_dialog_continue("User tidak ditemukan! :p");
        user_type = D_NONE;
    }

    fclose(database_file);
    return user_type;
}

Status database_user_signup( const char *username, const char *password,
                                   data_t *database_user){
    /*
        Check whether the user input valid inputs or whether
        the inputted username already exist in the database

        offset_x        : x cordinate position
        offset_y        : y cordinate position
        username        : user's username
        password        : user's password
        database_user   : The Users that are in the database

        Return : user type
    */

    // If the user input blank
    if (strlen(username) == 0){
        draw_dialog_continue("Username cannot be blank!");
        return FAILED;
    } else if (strlen(password) == 0){
        draw_dialog_continue("Password cannot be blank!");
        return FAILED;
    }

    // Check whether the data exist or not yet
    FILE* database_file = fopen(FILENAME, "rb+");

    if (!database_file){
        database_create(database_file);
        database_file = fopen(FILENAME, "rb+");
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
        draw_dialog_continue("Username already exist!");
        return FAILED;
    }

    return SUCCESS;
}
