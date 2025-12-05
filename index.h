#ifndef INDEX_H
#define INDEX_H

// Structure for index entry
typedef struct
{
    long account_number;
    char name[100];
    char id[20];
} IndexEntry;

// Index file functions
void create_index_file();
void add_to_index(long acc_number, char *name, char *id);
int is_unique_in_index(long acc_number);
void remove_from_index(long acc_number);
void list_accounts_from_index();
int count_accounts_in_index();
int find_account_in_index(char *search_name, char *search_id, long *found_acc);

#endif