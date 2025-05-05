# Компилятор и флаги
CC = gcc
CFLAGS = -Wall -Wextra -g -I./filters -I./libs

# Пути
FILTERS_DIR = filters
BIN = govnishko
# Исходники
SRC = 1_task/seq_convo.c 2_task/mt_by_row.c filters/filter.c main.c

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
