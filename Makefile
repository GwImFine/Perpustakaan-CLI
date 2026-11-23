CC      = gcc
CFLAGS  = -std=c99 -Wall -Wextra -pedantic -O2
TARGET  = perpustakaan
SRCS    = $(wildcard src/*.c)
OBJS    = $(SRCS:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

src/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

debug: CFLAGS = -std=c99 -Wall -Wextra -pedantic -g -fsanitize=address,undefined
debug: clean $(TARGET)

clean:
	rm -f src/*.o $(TARGET)

# Menghapus data operasional hasil program (dataset asli Books.txt TIDAK disentuh)
reset-data:
	rm -f data/katalog.txt data/user.txt data/peminjaman.txt data/*.tmp

.PHONY: all debug clean reset-data