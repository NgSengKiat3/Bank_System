#ifndef FILE_IO_H
#define FILE_IO_H

void create_database_dir();
void log_transaction(const char *operation, long account_number, double amount);

#endif