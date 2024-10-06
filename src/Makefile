TETRIS_DIR := brick_game/tetris
GUI_DIR := gui/cli
SOURCES := $(TETRIS_DIR)/*.c $(GUI_DIR)/*.c
TESTS_DIR := tests
BIN_DIR := bin
TESTS_FLAGS = -lcheck -lpthread -pthread -lm -lsubunit

CC := gcc
CCFLAGS := -I $(TETRIS_DIR) -I $(GUI_DIR) -Wall -Werror -Wextra -std=c11 -g
LDFLAGS := -lncurses -lc
MKDIR_P := mkdir -p


.PHONY: all clean install uninstall dvi dist test gcov_report

all: clean $(BIN_DIR)/tetris
# all: install

install:
	@$(MKDIR_P) $(BIN_DIR)
	$(CC) $(CCFLAGS) $(SOURCES) -o $(BIN_DIR)/tetris $(LDFLAGS)

uninstall:
	rm -rf $(BIN_DIR)

$(BIN_DIR)/tetris:
	@$(MKDIR_P) $(@D)
	$(CC) $(CCFLAGS) $(SOURCES) -o $@ $(LDFLAGS)
	./bin/tetris

clean:
	rm -rf $(BIN_DIR) high_score.txt $(TESTS_DIR)/tests gcov_report $(TESTS_DIR)/*.info $(TESTS_DIR)/*.gcno $(TESTS_DIR)/*.gcda tetris.tar.gz

dvi:
	open doc/tetris.png

dist:
	tar -czvf tetris.tar.gz . makefile

test:
	$(CC) $(CCFLAGS) $(TETRIS_DIR)/*.c $(TESTS_DIR)/*.c -o $(TESTS_DIR)/tests $(TESTS_FLAGS) $(LDFLAGS)
	./$(TESTS_DIR)/tests

gcov_report:
	$(CC) $(CCFLAGS) $(TETRIS_DIR)/*.c $(TESTS_DIR)/*.c -o $(TESTS_DIR)/tests $(LDFLAGS) $(TESTS_FLAGS) -fprofile-arcs -ftest-coverage
	./$(TESTS_DIR)/tests
	lcov -t "$(TESTS_DIR)/tests" -o $(TESTS_DIR)/tests.info -c -d .
	genhtml -o gcov_report $(TESTS_DIR)/*.info
	open gcov_report/index.html

leaks: test
ifeq ($(shell uname -s), Linux)
	valgrind --leak-check=yes ./$(TESTS_DIR)/tests
else
	leaks --atExit -- ./$(TESTS_DIR)/tests
endif