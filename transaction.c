#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include "account.h"
#include "file_io.h"

void deposit_money()
{
    printf("\nDEPOSIT MONEY\n");

    printf("\nAccount Holder: %s\n", account.name);
    printf("Account Type: %s\n", account.account_type);
    printf("Current Balance: RM %.2lf\n", account.balance);

    printf("Enter deposit amount (RM0 - RM50,000): RM ");

    // Authentication
    long acc_number;
    char pin[5];
    double amount;

    printf("Enter account number: ");
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

    printf("Enter 4-digit PIN: ");
    scanf("%4s", pin);

    if (!verify_pin(acc_number, pin))
    {
        printf("Invalid PIN! Access denied.\n");
        return;
    }

    printf("Enter deposit amount (RM0 - RM50,000): RM ");
    if (scanf("%lf", &amount) != 1)
    {
        printf("Invalid amount!\n");
        while (getchar() != '\n')
            ;
        return;
    }

    // Validate amount
    if (amount <= 0)
    {
        printf("Deposit amount must be greater than RM0!\n");
        return;
    }

    if (amount > 50000)
    {
        printf("Deposit amount cannot exceed RM50000 per transaction!\n");
        return;
    }

    // Load current account status
    BankAccount account = load_account(acc_number);
    // Update balance
    double new_balance = account.balance + amount;
    // Save back to file
    update_account_balance(acc_number, new_balance);
    // Log transaction
    log_transaction("DEPOSIT", acc_number, amount);

    printf("\nDeposit successful!\n");
    printf("Previous balance: RM %.2lf\n", account.balance);
    printf("Deposit amount: RM %.2lf\n", amount);
    printf("New balance: RM %.2lf\n", new_balance);
}

void withdraw_money()
{
    printf("\nWITHDRAW MONEY\n");

    long acc_number;
    char pin[5];
    double amount;

    printf("Enter account number: ");
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

    printf("Enter 4-digit PIN: ");
    scanf("%4s", pin);

    if (!verify_pin(acc_number, pin))
    {
        printf("Invalid PIN! Access denied.\n");
        return;
    }

    BankAccount account = load_account(acc_number);
    printf("Current balance: RM %.2lf\n", account.balance);

    printf("Enter withdrawal amount: RM ");
    if (scanf("%lf", &amount) != 1)
    {
        printf("Invalid amount!\n");
        while (getchar() != '\n')
            ;
        return;
    }

    if (amount <= 0)
    {
        printf("Withdrawal amount must be greater than RM0!\n");
        return;
    }

    if (amount > account.balance)
    {
        printf("Insufficient funds! Available balance: RM %.2lf\n", account.balance);
        return;
    }

    double new_balance = account.balance - amount;
    update_account_balance(acc_number, new_balance);

    log_transaction("WITHDRAWAL", acc_number, amount);

    printf("\nWithdrawal successful!\n");
    printf("Previous balance: RM %.2lf\n", account.balance);
    printf("Withdrawal amount: RM %.2lf\n", amount);
    printf("New balance: RM %.2lf\n", new_balance);
}

void transfer_money()
{
    printf("\nTRANSFER MONEY\n");

    long from_acc, to_acc;
    char pin[5];
    double amount;

    // Sender account
    printf("Enter YOUR account number (sender): ");
    if (scanf("%ld", &from_acc) != 1)
    {
        printf("Invalid account number!\n");
        while (getchar() != '\n')
            ;
        return;
    }

    if (!account_exists(from_acc))
    {
        printf("Sender account not found!\n");
        return;
    }

    printf("Enter your 4-digit PIN: ");
    scanf("%4s", pin);

    if (!verify_pin(from_acc, pin))
    {
        printf("Invalid PIN! Access denied.\n");
        return;
    }

    // Recipient account
    printf("Enter RECIPIENT account number: ");
    if (scanf("%ld", &to_acc) != 1)
    {
        printf("Invalid account number!\n");
        while (getchar() != '\n')
            ;
        return;
    }

    if (!account_exists(to_acc))
    {
        printf("Recipient account not found!\n");
        return;
    }

    if (from_acc == to_acc)
    {
        printf("Cannot transfer to the same account!\n");
        return;
    }

    // Get transfer amount
    printf("Enter transfer amount: RM ");
    if (scanf("%lf", &amount) != 1)
    {
        printf("Invalid amount!\n");
        while (getchar() != '\n')
            ;
        return;
    }

    if (amount <= 0)
    {
        printf("Transfer amount must be greater than RM0!\n");
        return;
    }

    // Load both accounts
    BankAccount sender = load_account(from_acc);
    BankAccount recipient = load_account(to_acc);

    // Calculate fees
    double fee = 0.0;
    double total_deduction = amount;

    if (strcmp(sender.account_type, "Savings") == 0 &&
        strcmp(recipient.account_type, "Current") == 0)
    {
        fee = amount * 0.02; // 2%
        // 100% + 2%(fee)
        total_deduction = amount + fee;
        printf("Transfer fee (2%%): RM %.2lf\n", fee);
    }
    else if (strcmp(sender.account_type, "Current") == 0 &&
             strcmp(recipient.account_type, "Savings") == 0)
    {
        fee = amount * 0.03; // 3% fee
        total_deduction = amount + fee;
        printf("Transfer fee (3%%): RM %.2lf\n", fee);
    }

    // Check sufficient funds
    if (total_deduction > sender.balance)
    {
        printf("Insufficient funds! Available: RM %.2lf, Required: RM %.2lf\n",
               sender.balance, total_deduction);
        return;
    }

    // Update balances
    double new_sender_balance = sender.balance - total_deduction;
    double new_recipient_balance = recipient.balance + amount;

    update_account_balance(from_acc, new_sender_balance);
    update_account_balance(to_acc, new_recipient_balance);

    // Log transactions
    log_transaction("REMITTANCE_OUT", from_acc, -total_deduction);
    log_transaction("REMITTANCE_IN", to_acc, amount);

    printf("\nTransfer successful!\n");
    printf("Transferred: RM %.2lf\n", amount);
    if (fee > 0)
    {
        printf("Fee charged: RM %.2lf\n", fee);
    }
    printf("Your new balance: RM %.2lf\n", new_sender_balance);
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