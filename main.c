#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "account.h"
#include "transaction.h"
#include "file_io.h"

void display_menu();
int handle_menu_choice(char *input);
void show_session_info();
int get_account_count();
void clear_input_buffer();

int main()
{
    char input[20];
    int choice;

    printf("=== BANKING SYSTEM APPLICATION ===\n");
    create_database_dir();
    show_session_info();

    do
    {
        display_menu();
        printf("Enter your choice: ");
        scanf("%s", input);
        clear_input_buffer();

        choice = handle_menu_choice(input);

    } while (choice != 0);

    printf("Thank you for using the Banking System!\n");
    return 0;
}

void display_menu()
{
    printf("\n");
    printf("=== BANKING SYSTEM MENU ===\n");
    printf("1. Create New Account (or 'create')\n");
    printf("2. Delete Account (or 'delete')\n");
    printf("3. Deposit (or 'deposit')\n");
    printf("4. Withdrawal (or 'withdrawal')\n");
    printf("5. Remittance (or 'remittance')\n");
    printf("0. Exit (or 'exit')\n");
    printf("============================\n");
}

int handle_menu_choice(char *input)
{
    if (strcmp(input, "1") == 0 || strcmp(input, "create") == 0)
    {
        create_new_account();
        return 1;
    }
    else if (strcmp(input, "2") == 0 || strcmp(input, "delete") == 0)
    {
        delete_account();
        return 2;
    }
    else if (strcmp(input, "3") == 0 || strcmp(input, "deposit") == 0)
    {
        deposit_money();
        return 3;
    }
    else if (strcmp(input, "4") == 0 || strcmp(input, "withdrawal") == 0)
    {
        withdraw_money();
        return 4;
    }
    else if (strcmp(input, "5") == 0 || strcmp(input, "remittance") == 0)
    {
        remittance();
        return 5;
    }
    else if (strcmp(input, "0") == 0 || strcmp(input, "exit") == 0)
    {
        return 0;
    }
    else
    {
        printf("Invalid option! Please try again.\n");
        return -1;
    }
}

void show_session_info()
{
    time_t now = time(NULL);
    printf("Session started: %s", ctime(&now));
    printf("Loaded accounts: %d\n", get_account_count());
}

int get_account_count()
{
    DIR *dir;
    struct dirent *entry;
    int count = 0;

    dir = opendir("database");
    if (dir == NULL)
        return 0;

    while ((entry = readdir(dir)) != NULL)
    {
        if (strstr(entry->d_name, "database.txt") != NULL &&
            strcmp(entry->d_name, "transaction.log") != 0 &&
            strcmp(entry->d_name, "index.txt") != 0)
        {
            count++;
        }
    }
    closedir(dir);
    return count;
}

void clear_input_buffer()
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF)
        ;
}