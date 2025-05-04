# Компилятор и флаги
CC = gcc
CFLAGS = -Wall -Wextra -g -I./filters -I./libs

# Пути
SRC_DIR = 1_task
FILTERS_DIR = filters
BIN = $(SRC_DIR)/seq_filters_apply
IMG = 
# Исходники
SRC = $(SRC_DIR)/seq_filters_apply.c $(FILTERS_DIR)/filter.c

# Сборка
all: $(BIN)

$(BIN): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(BIN) -lm

# Очистка
clean:
	rm -f $(BIN)

# Пример запуска
run: all
	./$(BIN)
