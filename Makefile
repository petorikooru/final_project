# Compiler and flags
CC = clang
CFLAGS = -g

# Targets
TARGET = tubes.bin
SRC = tubes_main.c tubes_message.c tubes_stok.c tubes_user.c tubes_admin.c
SRC += tubes_interface.c tubes_input.c tubes_database.c

PKG_CONFIG = pkg-config
LIBNAME = gstreamer-1.0 # Main audio handler
LDLIBS = $(shell $(PKG_CONFIG) --libs $(LIBNAME))
CFLAGS += $(shell $(PKG_CONFIG) --cflags $(LIBNAME))

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $@ $^ $(LDLIBS)

clean:
	rm -f $(TARGET)

.PHONY: all clean
