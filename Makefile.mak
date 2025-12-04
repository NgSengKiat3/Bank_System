CC = gcc
CFLAGS = -Wall -Wextra -std=c99
TARGET = banking_system
SOURCES = main.c account.c transaction.c file_io.c
HEADERS = account.h transaction.h file_io.h

$(TARGET): $(SOURCES) $(HEADERS)
	$(CC) $(CFLAGS) -o $(TARGET) $(SOURCES)

clean:
	rm -f $(TARGET)
	rm -rf database

run: $(TARGET)
	./$(TARGET)

.PHONY: clean run