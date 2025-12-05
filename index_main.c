#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "index.h"

void display_menu()
{
    printf("\n=== INDEX FILE MANAGER ===\n");
    printf("1. List all accounts in index\n");
    printf("2. Count accounts in index\n");
    printf("3. Check if account number is unique\n");
    printf("4. Find account by name and ID\n");
    printf("5. Test index file operations\n");
    printf("0. Exit\n");
    printf("==========================\n");
}

int main()
{
    int choice;

    printf("Index File Manager - Testing Index File Operations\n");

    do
    {
        display_menu();
        printf("Enter choice: ");
        scanf("%d", &choice);

        switch (choice)
        {
        case 1:
            list_accounts_from_index();
            break;

        case 2:
        {
            int count = count_accounts_in_index();
            printf("Total accounts in index: %d\n", count);
            break;
        }

        case 3:
        {
            long acc_num;
            printf("Enter account number to check: ");
            scanf("%ld", &acc_num);

            if (is_unique_in_index(acc_num))
            {
                printf("Account number %ld is UNIQUE\n", acc_num);
            }
            else
            {
                printf("Account number %ld already EXISTS in index\n", acc_num);
            }
            break;
        }

        case 4:
        {
            char name[100], id[20];
            long found_acc;

            printf("Enter name to search: ");
            getchar();
            fgets(name, sizeof(name), stdin);
            name[strcspn(name, "\n")] = 0;

            printf("Enter ID to search: ");
            fgets(id, sizeof(id), stdin);
            id[strcspn(id, "\n")] = 0;

            if (find_account_in_index(name, id, &found_acc))
            {
                printf("Account found! Account number: %ld\n", found_acc);
            }
            else
            {
                printf("Account not found in index.\n");
            }
            break;
        }

        case 5:
            printf("\nTesting index operations...\n");
            create_index_file();
            printf("1. Index file created/verified\n");

            // Add a test entry
            add_to_index(9999999, "Test User", "1234567");
            printf("2. Added test account to index\n");

            // Check uniqueness
            if (is_unique_in_index(9999999))
            {
                printf("3. Uniqueness check FAILED (should not be unique)\n");
            }
            else
            {
                printf("3. Uniqueness check PASSED (correctly found duplicate)\n");
            }

            // List accounts
            printf("4. Listing all accounts:\n");
            list_accounts_from_index();

            // Remove test entry
            remove_from_index(9999999);
            printf("5. Removed test account from index\n");
            break;

        case 0:
            printf("Exiting Index Manager...\n");
            break;

        default:
            printf("Invalid choice!\n");
        }

    } while (choice != 0);

    return 0;
}