CC = gcc
CFLAGS = -Wall -Wextra -g -I./filters -I./libs

FILTERS_DIR = filters
BIN = convolution.out

SRC = 1_task/seq_convo.c 2_task/mt_convo.c filters/filter.c src/main.c tests/utils/test_utils.c

all: $(BIN)

$(BIN): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(BIN) -lm

clean:
	rm -f $(BIN)

TEST_SRC = 1_task/seq_convo.c filters/filter.c tests/utils/test_utils.c tests/seq_test/seq_test.c tests/test.c tests/mt_test/mt_test.c 2_task/mt_convo.c
TEST_BIN = test.out

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
	         ./$(TEST_BIN) --mode=seq --filter=blur iamges/mypersonalphoto.bmp images/res1.bmp
	@echo "Valgrind report saved to valgrind_report.txt"

valgrind-src: $(BIN)
	valgrind --leak-check=full \
	         --show-leak-kinds=all \
	         --track-origins=yes \
	         --error-exitcode=1 \
	         --log-file=valgrind_report.txt \
	         ./$(BIN) --mode=seq --filter=blur images/mypersonalphoto.bmp images/res1.bmp
	@echo "Valgrind report saved to valgrind_report.txt"


helgrind-check: $(TEST_BIN)
	valgrind --tool=helgrind \
	         --log-file=helgrind.log \
	         --read-var-info=yes \
			 --gen-suppressions=all \
	         --history-level=full \
	         ./$(TEST_BIN)
	@echo "Helgrind report saved to helgrind.log"


format: 
	./scripts/format.sh

.PHONY: all format test valgrind-test valgrind-src clean run run-test helgrind-check

