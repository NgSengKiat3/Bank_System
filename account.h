#ifndef ACCOUNT_H
#define ACCOUNT_H

#define MAX_NAME_LENGTH 100
#define MAX_ID_LENGTH 20
#define MAX_PIN_LENGTH 5
#define MAX_FILENAME_LENGTH 50

typedef struct
{
    char name[MAX_NAME_LENGTH];
    char id[MAX_ID_LENGTH];
    char account_type[10];
    char pin[MAX_PIN_LENGTH];
    long account_number;
    double balance;
} BankAccount;

// Account management
BankAccount create_new_account();
int delete_account();
BankAccount load_account(long account_number);
int save_account(BankAccount account);
int authenticate_account(long account_number, char *pin);
long generate_account_number();
int is_account_number_unique(long acc_number);
void print_account_info(BankAccount account);
int get_last_four_id(char *id, char *last_four);
int account_exists(long account_number);

// Account operation
int verify_pin(long acc_number, char *input_pin);
BankAccount load_account(long acc_number);
void update_account_balance(long acc_number, double new_balance);
int account_exists(long acc_number);
int get_account_count();
void list_all_accounts();

#endif