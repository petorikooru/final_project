#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_USERS 100
#define MAX_NAME_LENGTH 50

typedef struct {
    int user_id;
    char username[MAX_NAME_LENGTH];
    char email[MAX_NAME_LENGTH];
    char registration_date[20];
    char last_login[20];
    int is_banned;
} user_admin_t;

static user_admin_t users[MAX_USERS];
int total_users = 0;

void show_admin_menu();
void view_all_users();
void view_user_by_id();
void delete_user();
void ban_user();
void unban_user();
void load_user_data();
void save_user_data();

int display_admin_start() {
    int choice;

    printf("=== ADMIN CONTROL PANEL ===\n");
    printf("Loading user data...\n");

    load_user_data();

    while (1) {
        show_admin_menu();
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                view_all_users();
                break;
            case 2:
                view_user_by_id();
                break;
            case 3:
                delete_user();
                break;
            case 4:
                ban_user();
                break;
            case 5:
                unban_user();
                break;
            case 6:
                save_user_data();
                printf("Data saved. Exiting...\n");
                return 0;
            default:
                printf("Invalid choice! Try again.\n");
        }

        printf("\nPress Enter to continue...");
        getchar();
        getchar();
    }

    return 0;
}

void show_admin_menu() {
    printf("\n================================\n");
    printf("       ADMIN PANEL MENU\n");
    printf("================================\n");
    printf("1. View All Users\n");
    printf("2. View Specific User\n");
    printf("3. Delete User\n");
    printf("4. Ban User\n");
    printf("5. Unban User\n");
    printf("6. Save & Exit\n");
    printf("================================\n");
}

void view_all_users() {
    printf("\n=== ALL REGISTERED USERS ===\n");

    if (total_users == 0) {
        printf("No users found in the system.\n");
        return;
    }

    printf("%-5s %-20s %-25s %-12s %-15s %-8s\n",
           "ID", "Username", "Email", "Registered", "Last Login", "Status");
    printf("--------------------------------------------------------------------------------\n");

    for (int i = 0; i < total_users; i++) {
        printf("%-5d %-20s %-25s %-12s %-15s %-8s\n",
               users[i].user_id,
               users[i].username,
               users[i].email,
               users[i].registration_date,
               users[i].last_login,
               users[i].is_banned ? "BANNED" : "ACTIVE");
    }

    printf("\nTotal Users: %d\n", total_users);
}

void view_user_by_id() {
    int search_id;
    int found = 0;

    printf("\nEnter User ID to view: ");
    scanf("%d", &search_id);

    for (int i = 0; i < total_users; i++) {
        if (users[i].user_id == search_id) {
            printf("\n=== USER DETAILS ===\n");
            printf("User ID: %d\n", users[i].user_id);
            printf("Username: %s\n", users[i].username);
            printf("Email: %s\n", users[i].email);
            printf("Registration Date: %s\n", users[i].registration_date);
            printf("Last Login: %s\n", users[i].last_login);
            printf("Account Status: %s\n", users[i].is_banned ? "BANNED" : "ACTIVE");
            found = 1;
            break;
        }
    }

    if (!found) {
        printf("User with ID %d not found!\n", search_id);
    }
}

void delete_user() {
    int delete_id;
    char confirm;
    int found = 0;

    printf("\nEnter User ID to delete: ");
    scanf("%d", &delete_id);

    for (int i = 0; i < total_users; i++) {
        if (users[i].user_id == delete_id) {
            printf("User found: %s (%s)\n", users[i].username, users[i].email);
            printf("Are you sure you want to delete this user? (y/n): ");
            scanf(" %c", &confirm);

            if (confirm == 'y' || confirm == 'Y') {
                for (int j = i; j < total_users - 1; j++) {
                    users[j] = users[j + 1];
                }
                total_users--;
                printf("User deleted successfully!\n");

                // TODO (RADIT): Update user data file immediately
                // save_user_data();
            } else {
                printf("Deletion cancelled.\n");
            }
            found = 1;
            break;
        }
    }

    if (!found) {
        printf("User with ID %d not found!\n", delete_id);
    }
}

void ban_user() {
    int ban_id;
    int found = 0;

    printf("\nEnter User ID to ban: ");
    scanf("%d", &ban_id);

    for (int i = 0; i < total_users; i++) {
        if (users[i].user_id == ban_id) {
            if (users[i].is_banned == 1) {
                printf("User '%s' is already banned!\n", users[i].username);
            } else {
                users[i].is_banned = 1;
                printf("User '%s' has been banned successfully!\n", users[i].username);

                // TODO (RADIT): Update user status in file
                // save_user_data();
            }
            found = 1;
            break;
        }
    }

    if (!found) {
        printf("User with ID %d not found!\n", ban_id);
    }
}

void unban_user() {
    int unban_id;
    int found = 0;

    printf("\nEnter User ID to unban: ");
    scanf("%d", &unban_id);

    for (int i = 0; i < total_users; i++) {
        if (users[i].user_id == unban_id) {
            if (users[i].is_banned == 0) {
                printf("User '%s' is not banned!\n", users[i].username);
            } else {
                users[i].is_banned = 0;
                printf("User '%s' has been unbanned successfully!\n", users[i].username);

                // TODO (RADIT): Update user status in file
                // save_user_data();
            }
            found = 1;
            break;
        }
    }

    if (!found) {
        printf("User with ID %d not found!\n", unban_id);
    }
}

void load_user_data() {
    // TODO (RADIT): FILE HANDLING - Load user data from file
    // Example implementation:
    // FILE *file = fopen("users.dat", "rb");
    // if (file != NULL) {
    //     fread(&total_users, sizeof(int), 1, file);
    //     fread(users, sizeof(User), total_users, file);
    //     fclose(file);
    //     printf("Loaded %d users from file.\n", total_users);
    // } else {
    //     printf("No existing user data file found.\n");
    // }

    strcpy(users[0].username, "john_doe");
    strcpy(users[0].email, "john@example.com");
    strcpy(users[0].registration_date, "2024-01-15");
    strcpy(users[0].last_login, "2024-01-20");
    users[0].user_id = 1001;
    users[0].is_banned = 0;

    strcpy(users[1].username, "jane_smith");
    strcpy(users[1].email, "jane@example.com");
    strcpy(users[1].registration_date, "2024-01-18");
    strcpy(users[1].last_login, "2024-01-19");
    users[1].user_id = 1002;
    users[1].is_banned = 1;

    strcpy(users[2].username, "mike_wilson");
    strcpy(users[2].email, "mike@example.com");
    strcpy(users[2].registration_date, "2024-01-20");
    strcpy(users[2].last_login, "2024-01-21");
    users[2].user_id = 1003;
    users[2].is_banned = 0;

    total_users = 3;
    printf("Sample user data loaded for testing.\n");
}

void save_user_data() {
    // TODO (RADIT): FILE HANDLING - Save user data to file
    // Example implementation:
    // FILE *file = fopen("users.dat", "wb");
    // if (file != NULL) {
    //     fwrite(&total_users, sizeof(int), 1, file);
    //     fwrite(users, sizeof(User), total_users, file);
    //     fclose(file);
    //     printf("User data saved to file successfully.\n");
    // } else {
    //     printf("Error: Could not save user data to file.\n");
    // }

    printf("User data would be saved to file here.\n");
    printf("Current users in memory: %d\n", total_users);
}
