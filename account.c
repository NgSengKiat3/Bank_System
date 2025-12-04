#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <dirent.h>
#include "account.h"

// Create new account (existing code)
BankAccount create_new_account()
{
    BankAccount new_account;

    printf("\nCREATE NEW BANK ACCOUNT\n");

    printf("Enter full name: ");
    getchar(); // Clear input buffer
    fgets(new_account.name, MAX_NAME_LENGTH, stdin);
    new_account.name[strcspn(new_account.name, "\n")] = 0;

    printf("Enter identification number: ");
    fgets(new_account.id, MAX_ID_LENGTH, stdin);
    new_account.id[strcspn(new_account.id, "\n")] = 0;

    int valid_type = 0;
    while (!valid_type)
    {
        printf("Enter account type (Savings/Current): ");
        char type[10];
        scanf("%s", type);

        if (strcmp(type, "Savings") == 0 || strcmp(type, "savings") == 0)
        {
            strcpy(new_account.account_type, "Savings");
            valid_type = 1;
        }
        else if (strcmp(type, "Current") == 0 || strcmp(type, "current") == 0)
        {
            strcpy(new_account.account_type, "Current");
            valid_type = 1;
        }
        else
        {
            printf("Invalid account type! Please enter 'Savings' or 'Current'.\n");
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

    save_account_to_file(new_account);
    printf("\nAccount created successfully!\n");
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
    printf("Account saved to: %s\n", filename);
}

void print_account_info(BankAccount account)
{
    printf("\nAccount Information\n");
    printf("\n--------------------\n")
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
        if (strstr(entry->d_name, ".txt") != NULL &&
            strcmp(entry->d_name, "transaction.log") != 0)
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
