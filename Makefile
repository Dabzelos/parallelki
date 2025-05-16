# Компилятор и флаги
CC = gcc
CFLAGS = -Wall -Wextra -g -I./filters -I./libs

# Пути
FILTERS_DIR = filters
BIN = shi.out
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


# Для тестов
TEST_SRC = 1_task/seq_convo.c filters/filter.c tests/utils/test_utils.c tests/seq_test/seq_test.c tests/test.c tests/mt_test/mt_test.c 2_task/mt_by_row.c
TEST_BIN = seq_convo_test.out

test: $(TEST_BIN)

$(TEST_BIN): $(TEST_SRC)
	$(CC) $(CFLAGS) $(TEST_SRC) -o $(TEST_BIN) -lm

run-test: test
	./$(TEST_BIN)


valgrind-test: $(TEST_BIN)
	valgrind --leak-check=full \
	         --show-leak-kinds=all \
	         --track-origins=yes \
	         --error-exitcode=1 \
	         --log-file=valgrind_report.txt \
	         ./$(TEST_BIN)
	@echo "Valgrind report saved to valgrind_report.txt"




.PHONY: all test valgrind-test clean run run-test 