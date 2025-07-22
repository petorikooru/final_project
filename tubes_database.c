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

    FILE    *database_file = fopen(DATABASE_FILE, "rb+");
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

    database_file = fopen(DATABASE_FILE, "wb");

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
                        data_t *const user , const user_t type){
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
    user->type  = type;

    // Save the user data
    FILE* database_file = fopen(DATABASE_FILE, "ab");
    fwrite(user, sizeof(data_t), 1, database_file);
    fclose(database_file);
}

database_user_t database_user_login(const char *username, const char *password, data_t *database_user, user_t type){
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

    FILE* database_file = fopen(DATABASE_FILE, "rb");
    if (!database_file) {
        return D_NONE;
    }

    database_user_t user_type;
    bool found = false;

    while(fread(database_user, sizeof(data_t), 1, database_file) == 1){
        if (strcmp(username, database_user->username) == 0){
            found = true;

            if (strcmp(database_user->password, password) == 0){
                if (database_user->type == type)
                    user_type = (database_user_t)type;
                else {
                    user_type = (database_user_t)database_user->type;
                }
                break;
            } else {
                draw_dialog_err("Wrong Password bleh :p");
                user_type = D_NONE;
                break;
            }
        }
    }

    if (!found){
        draw_dialog_err("User tidak ditemukan! :p");
        user_type = D_NONE;
    }

    fclose(database_file);
    return user_type;
}

choice_t database_user_signup( const char *username, const char *password,
                                   data_t *database_user, user_t type){
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
        draw_dialog_err("Username cannot be blank!");
        return C_FAILED;
    } else if (strlen(password) == 0){
        draw_dialog_err("Password cannot be blank!");
        return C_FAILED;
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
        draw_dialog_err("Username already exist!");
        return C_FAILED;
    }

    return C_SUCCESS;
}
