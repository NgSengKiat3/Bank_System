#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <dirent.h>
#include "account.h"
#include "file_io.h"

void save_account_to_file(BankAccount account)
{
    char filename[50];
    sprintf(filename, "database/%ld.txt", account.account_number);

    FILE *file = fopen(filename, "w");
    if (file == NULL)
    {
        printf("Error: Could not create account file!\n");
        return;
    }

    fprintf(file, "Name: %s\n", account.name);
    fprintf(file, "ID: %s\n", account.id);
    fprintf(file, "Account Type: %s\n", account.account_type);
    fprintf(file, "PIN: %s\n", account.pin);
    fprintf(file, "Account Number: %ld\n", account.account_number);
    fprintf(file, "Balance: %.2lf\n", account.balance);

    fclose(file);
}

// Create new account (existing code)
BankAccount create_new_account()
{
    BankAccount new_account;

    printf("\nCREATE NEW BANK ACCOUNT\n");

    printf("Enter full name: ");
    fgets(new_account.name, MAX_NAME_LENGTH, stdin);
    new_account.name[strcspn(new_account.name, "\n")] = 0;

    int valid_id = 0;
    while (!valid_id)
    {
        printf("Enter identification number (8 digits): ");
        fgets(new_account.id, MAX_ID_LENGTH, stdin);
        new_account.id[strcspn(new_account.id, "\n")] = 0;
        valid_id = validate_id_number(new_account.id);
    }

    int valid_type = 0;
    while (!valid_type)
    {
        printf("Enter account type (Saving/Current): ");
        char type[10];
        scanf("%s", type);

        if (strcmp(type, "Saving") == 0 || strcmp(type, "saving") == 0)
        {
            strcpy(new_account.account_type, "Saving");
            valid_type = 1;
        }
        else if (strcmp(type, "Current") == 0 || strcmp(type, "current") == 0)
        {
            strcpy(new_account.account_type, "Current");
            valid_type = 1;
        }
        else
        {
            printf("Invalid account type! Please enter 'Saving' or 'Current'.\n");
        }
    }

    int valid_pin = 0;
    while (!valid_pin)
    {
        printf("Enter 4-digit PIN: ");
        scanf("%s", new_account.pin);

        if (strlen(new_account.pin) == 4)
        {
            valid_pin = 1;
            for (int i = 0; i < 4; i++)
            {
                if (!isdigit(new_account.pin[i]))
                {
                    printf("PIN must contain only digits! Please try again.\n");
                    valid_pin = 0;
                    break;
                }
            }
        }
        else
        {
            printf("PIN must be exactly 4 digits! Please try again.\n");
        }
    }

    new_account.account_number = generate_account_number();
    new_account.balance = 0.0;

    // Display new account info
    save_account_to_file(new_account);
    printf("\nAccount created successfully!\n");
    printf("-------------------------------\n");
    printf("Please remember your account number, ID and PIN\n");
    printf("Account Number: %ld\n", new_account.account_number);
    printf("Name: %s\n", new_account.name);
    printf("Account Type: %s\n", new_account.account_type);
    printf("Initial Balance: RM %.2lf\n", new_account.balance);
    printf("-------------------------------\n");
    printf("Note: Account number is needed for all future transactions.");
    print_account_info(new_account);

    return new_account;
}

long generate_account_number()
{
    long acc_number;
    int unique = 0;

    srand(time(NULL));

    while (!unique)
    {
        acc_number = 1000000 + (rand() % 900000000);
        unique = is_account_number_unique(acc_number);
    }

    return acc_number;
}

int is_account_number_unique(long acc_number)
{
    char filename[50];
    sprintf(filename, "database/%ld.txt", acc_number);
    FILE *file = fopen(filename, "r");
    if (file != NULL)
    {
        fclose(file);
        return 0; // Not unique
    }
    return 1; // Unique
}

void print_account_info(BankAccount account)
{
    printf("\nAccount Information\n");
    printf("\n--------------------\n");
    printf("Account Number: %ld\n", account.account_number);
    printf("Name: %s\n", account.name);
    printf("ID: %s\n", account.id);
    printf("Account Type: %s\n", account.account_type);
    printf("Balance: RM %.2lf\n", account.balance);
}

// NEW FUNCTIONS

int verify_pin(long acc_number, char *input_pin)
{
    // Open account file
    char filename[50];
    sprintf(filename, "database/%ld.txt", acc_number);

    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {
        return 0; // Account not found
    }

    // Search for PIN line
    char line[100];
    char stored_pin[5];

    while (fgets(line, sizeof(line), file))
    {
        if (strstr(line, "PIN:") != NULL)
        {
            sscanf(line, "PIN: %4s", stored_pin);
            break;
        }
    }
    fclose(file);

    // Compare pin
    return (strcmp(input_pin, stored_pin) == 0);
}

BankAccount load_account(long acc_number)
{
    BankAccount account;
    char filename[50];
    sprintf(filename, "database/%ld.txt", acc_number);

    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {
        account.account_number = -1; // Mark as invalid
        return account;
    }

    char line[100];
    while (fgets(line, sizeof(line), file))
    {
        if (strstr(line, "Name:") != NULL)
        {
            sscanf(line, "Name: %99[^\n]", account.name);
        }
        else if (strstr(line, "ID:") != NULL)
        {
            sscanf(line, "ID: %19[^\n]", account.id);
        }
        else if (strstr(line, "Account Type:") != NULL)
        {
            sscanf(line, "Account Type: %9[^\n]", account.account_type);
        }
        else if (strstr(line, "PIN:") != NULL)
        {
            sscanf(line, "PIN: %4[^\n]", account.pin);
        }
        else if (strstr(line, "Account Number:") != NULL)
        {
            sscanf(line, "Account Number: %ld", &account.account_number);
        }
        else if (strstr(line, "Balance:") != NULL)
        {
            sscanf(line, "Balance: %lf", &account.balance);
        }
    }
    fclose(file);

    return account;
}

void update_account_balance(long acc_number, double new_balance)
{
    BankAccount account = load_account(acc_number);
    if (account.account_number == -1)
    {
        printf("Error: Account not found!\n");
        return;
    }

    account.balance = new_balance;
    save_account_to_file(account);
}

int account_exists(long acc_number)
{
    char filename[50];
    sprintf(filename, "database/%ld.txt", acc_number);
    FILE *file = fopen(filename, "r");
    if (file != NULL)
    {
        fclose(file);
        return 1;
    }
    return 0;
}

int get_account_count()
{
    DIR *dir;
    struct dirent *entry;
    int count = 0;

    dir = opendir("database");
    if (dir == NULL)
    {
        return 0;
    }

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

void list_all_accounts()
{
    DIR *dir;
    struct dirent *entry;

    printf("\n EXISTING ACCOUNTS \n");

    dir = opendir("database");
    if (dir == NULL)
    {
        printf("No accounts found.\n");
        return;
    }

    int count = 0;
    while ((entry = readdir(dir)) != NULL)
    {
        if (strstr(entry->d_name, ".txt") != NULL &&
            strcmp(entry->d_name, "transaction.log") != 0)
        {
            // Extract account number from filename
            long acc_number = atol(entry->d_name);
            BankAccount acc = load_account(acc_number);
            if (acc.account_number != -1)
            {
                printf("%d. %ld - %s (%s)\n", ++count, acc.account_number, acc.name, acc.account_type);
            }
        }
    }

    closedir(dir);

    if (count == 0)
    {
        printf("No accounts found.\n");
    }
}

int validate_id_number(char *id)
{
    if (strlen(id) == 0)
    {
        printf("Empty ID number\n");
        return 0;
    }

    if (strlen(id) != 8)
    {
        printf("ID should be 8 digits\n");
        return 0;
    }

    for (size_t i = 0; i < strlen(id); i++)
    {
        if (!isdigit(id[i]))
        {
            printf("ID number only accept digit numbers\n");
            printf("You entered: '%s'\n", id);
            return 0;
        }
    }

    return 1;
}

void delete_account()
{
    printf("\nDELETE ACCOUNT\n");
    // Show account
    list_all_accounts();

    long acc_number;
    char pin[5], last_four_id[5];

    printf("\nEnter account number to delete: ");
    if (scanf("%ld", &acc_number) != 1)
    {
        printf("Invalid account number!\n");
        while (getchar() != '\n')
            ;
        return;
    }

    if (!account_exists(acc_number))
    {
        printf("Account not found!\n");
        return;
    }

    BankAccount account = load_account(acc_number);

    // Triple verification
    printf("Enter last 4 characters of ID: ");
    scanf("%4s", last_four_id);

    printf("Enter 4-digit PIN: ");
    scanf("%4s", pin);

    // Get last 4 of stored ID
    char stored_last_four[5];
    int id_len = strlen(account.id);
    if (id_len >= 4)
    {
        strncpy(stored_last_four, account.id + (id_len - 4), 4);
        stored_last_four[4] = '\0';
    }
    else
    {
        strcpy(stored_last_four, account.id);
    }

    if (!verify_pin(acc_number, pin) || strcmp(last_four_id, stored_last_four) != 0)
    {
        printf("Verification failed! Incorrect PIN or ID.\n");
        return;
    }

    // Final confirmation
    char confirm;
    printf("\nWARNING: This will permanently delete account %ld\n", acc_number);
    printf("Are you sure? (y/n): ");
    scanf(" %c", &confirm);

    if (confirm != 'y' && confirm != 'Y')
    {
        printf("Deletion cancelled.\n");
        return;
    }

    // Delete account file
    char filename[50];
    sprintf(filename, "database/%ld.txt", acc_number);

    if (remove(filename) == 0)
    {
        log_transaction("DELETE_ACCOUNT", acc_number, 0);
        printf("Account deleted successfully!\n");
    }
    else
    {
        printf("Error deleting account file!\n");
    }
}