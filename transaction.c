#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include "account.h"
#include "file_io.h"
#include "transaction.h"

void deposit_money()
{
    printf("\nDEPOSIT MONEY\n");

    // Authentication
    long acc_number;
    char pin[5];
    // Buffer for string input
    char amount_str[100];
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
    while (getchar() != '\n')
        ;

    if (!verify_pin(acc_number, pin))
    {
        printf("Invalid PIN! Access denied.\n");
        return;
    }
    // Load current account status
    BankAccount account = load_account(acc_number);

    printf("\nAccount Holder: %s\n", account.name);
    printf("Account Type: %s\n", account.account_type);
    printf("Current Balance: RM %.2lf\n", account.balance);

    // Function to validate money amount (allows 2 decimal places)
    // Returns: 1 if valid, 0 if invalid format, -1 if too many decimals
    int validate_money_amount(char *input)
    {
        int length = strlen(input);
        int decimal_point_found = 0;
        int digits_after_decimal = 0;

        // Check first character
        if (!isdigit(input[0]) && input[0] != '.')
        {
            return 0; // Invalid: must start with digit or decimal point
        }

        // Check each character
        for (int i = 0; i < length; i++)
        {
            if (input[i] == '.')
            {
                if (decimal_point_found)
                {
                    return 0; // Invalid: multiple decimal points
                }
                decimal_point_found = 1;
            }
            else if (!isdigit(input[i]))
            {
                return 0; // Invalid: non-digit character
            }
            else if (decimal_point_found)
            {
                digits_after_decimal++;
                if (digits_after_decimal > 2)
                {
                    return -1; // Too many decimal places
                }
            }
        }

        // Check if decimal point is at the end without digits
        if (decimal_point_found && digits_after_decimal == 0)
        {
            return 0; // Invalid: decimal point with no digits after
        }

        // Check if amount is 0
        double value = atof(input);
        if (value <= 0)
        {
            return 0; // Invalid: amount must be positive
        }

        return 1; // Valid
    }

    int valid_input = 0;
    while (!valid_input)
    {
        printf("Enter deposit amount (RM0.01 - RM50,000): RM ");

        fgets(amount_str, sizeof(amount_str), stdin);
        amount_str[strcspn(amount_str, "\n")] = 0;

        // empty input validation
        if (strlen(amount_str) == 0)
        {
            printf("Error: No amount entered. Please enter your deposit amount.\n");
            continue;
        }

        int validation_result = validate_money_amount(amount_str);

        if (validation_result == 1)
        {
            amount = atof(amount_str);
            valid_input = 1;
        }
        else if (validation_result == 0)
        {
            printf("Invalid amount format. Please try again.\n");
        }
        else if (validation_result == -1)
        {
            printf("Amount can only have 2 decimal places");
        }

        // Validate amount
        if (amount <= 0)
        {
            printf("Deposit amount must be greater than RM0\n");
            return;
        }

        if (amount > 50000)
        {
            printf("Deposit amount cannot exceed RM50000 per transaction\n");
            return;
        }

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

void remittance()
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

    if (strcmp(sender.account_type, "Saving") == 0 &&
        strcmp(recipient.account_type, "Current") == 0)
    {
        fee = amount * 0.02; // 2%
        // 100% + 2%(fee)
        total_deduction = amount + fee;
        printf("Transfer fee (2%%): RM %.2lf\n", fee);
    }
    else if (strcmp(sender.account_type, "Current") == 0 &&
             strcmp(recipient.account_type, "Saving") == 0)
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