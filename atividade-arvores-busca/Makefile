CC      = gcc
CFLAGS  = -Wall -Wextra -O2 -std=c11
LDFLAGS =

SRC_DIR = src
BIN     = experimento

SRCS = $(SRC_DIR)/main.c \
       $(SRC_DIR)/arvore.c \
       $(SRC_DIR)/buscas.c

OBJS = $(SRCS:.c=.o)

.PHONY: all clean run

all: $(BIN)

$(BIN): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

$(SRC_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

run: all
	mkdir -p resultados && ./$(BIN)

clean:
	rm -f $(OBJS) $(BIN)
