# Compiler and flags
CC = clang
CFLAGS = -g

# Targets
TARGET = tubes.bin
SRC = tubes_main.c tubes_message.c tubes_stok.c tubes_manajemen.c tubes_admin.c
SRC += tubes_interface.c tubes_input.c tubes_database.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f $(TARGET)

.PHONY: all clean
