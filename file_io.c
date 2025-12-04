#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#ifdef _WIN32
#include <direct.h>
#define mkdir _mkdir
#else
#include <sys/stat.h>
#endif
#include "file_io.h"

void create_database_dir()
{
#ifdef _WIN32
    if (_access("database", 0) == -1)
    {
        _mkdir("database");
        printf("Created database directory\n");
    }
#else
    struct stat st = {0};
    if (stat("database", &st) == -1)
    {
        mkdir("database", 0700);
        printf("Created database directory\n");
    }
#endif
}

void log_transaction(const char *operation, long account_number, double amount)
{
    FILE *log_file = fopen("database/transaction.log", "a");
    if (log_file == NULL)
    {
        printf("Warning: Could not open transaction log file!\n");
        return;
    }

    time_t now = time(NULL);
    char timestamp[20];
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", localtime(&now));

    fprintf(log_file, "[%s] %s - Account: %ld - Amount: RM%.2lf\n",
            timestamp, operation, account_number, amount);

    fclose(log_file);
}