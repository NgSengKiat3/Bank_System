#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "index.h"

#define INDEX_FILE "database/index.txt"

void create_index_file()
{
    FILE *index = fopen(INDEX_FILE, "a");
    if (index != NULL)
    {
        fclose(index);
    }
}

void add_to_index(long acc_number, char *name, char *id)
{
    FILE *index = fopen(INDEX_FILE, "a");
    if (index == NULL)
    {
        printf("Warning: Could not open index file for writing!\n");
        return;
    }

    fprintf(index, "%ld|%s|%s\n", acc_number, name, id);
    fclose(index);
}

int is_unique_in_index(long acc_number)
{
    FILE *index = fopen(INDEX_FILE, "r");
    if (index == NULL)
    {
        return 1; // File doesn't exist, so it's unique
    }

    char line[200];
    while (fgets(line, sizeof(line), index))
    {
        long stored_num;
        sscanf(line, "%ld|", &stored_num);
        if (stored_num == acc_number)
        {
            fclose(index);
            return 0; // Not unique
        }
    }

    fclose(index);
    return 1; // Unique
}

void remove_from_index(long acc_number)
{
    FILE *index = fopen(INDEX_FILE, "r");
    if (index == NULL)
        return;

    FILE *temp = fopen("database/temp_index.txt", "w");
    if (temp == NULL)
    {
        fclose(index);
        return;
    }

    char line[200];
    int found = 0;

    while (fgets(line, sizeof(line), index))
    {
        long stored_num;
        sscanf(line, "%ld|", &stored_num);

        if (stored_num != acc_number)
        {
            fputs(line, temp); // Keep this line
        }
        else
        {
            found = 1; // Skip this line (delete it)
        }
    }

    fclose(index);
    fclose(temp);

    // Replace old index with new one
    if (found)
    {
        remove(INDEX_FILE);
        rename("database/temp_index.txt", INDEX_FILE);
    }
    else
    {
        remove("database/temp_index.txt");
    }
}

void list_accounts_from_index()
{
    FILE *index = fopen(INDEX_FILE, "r");
    if (index == NULL)
    {
        printf("No accounts found.\n");
        return;
    }

    printf("\n=== EXISTING ACCOUNTS (From Index File) ===\n");
    char line[200];
    int count = 0;

    while (fgets(line, sizeof(line), index))
    {
        long acc_num;
        char name[100], id[20];

        if (sscanf(line, "%ld|%99[^|]|%19[^\n]", &acc_num, name, id) == 3)
        {
            printf("%d. Account: %ld | Name: %s | ID: %s\n",
                   ++count, acc_num, name, id);
        }
    }

    fclose(index);

    if (count == 0)
    {
        printf("No accounts found.\n");
    }
    else
    {
        printf("Total accounts: %d\n", count);
    }
}

int count_accounts_in_index()
{
    FILE *index = fopen(INDEX_FILE, "r");
    if (index == NULL)
        return 0;

    int count = 0;
    char line[200];

    while (fgets(line, sizeof(line), index))
    {
        count++;
    }

    fclose(index);
    return count;
}

int find_account_in_index(char *search_name, char *search_id, long *found_acc)
{
    FILE *index = fopen(INDEX_FILE, "r");
    if (index == NULL)
        return 0;

    char line[200];

    while (fgets(line, sizeof(line), index))
    {
        long acc_num;
        char name[100], id[20];

        if (sscanf(line, "%ld|%99[^|]|%19[^\n]", &acc_num, name, id) == 3)
        {
            if (strcmp(name, search_name) == 0 && strcmp(id, search_id) == 0)
            {
                *found_acc = acc_num;
                fclose(index);
                return 1; // Found
            }
        }
    }

    fclose(index);
    return 0; // Not found
}