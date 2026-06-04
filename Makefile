CC := gcc
CFLAGS := -Wall -Werror -Wextra -std=c11
LDFLAGS := -lcheck -lsubunit -lm -lpthread -Wl,--wrap=calloc

LIB := s21_matrix.a
TESTBIN := test
REPORT_DIR := gcov_report
SRC_DIR := modules
HDR := s21_matrix.h
TESTSRC := test.c
LIB_SRC := $(wildcard $(SRC_DIR)/*.c) #поиск фалов расширения .c в директории ./modules
LIB_OBJ := $(LIB_SRC:.c=.o)
LIB_SRC_TEST := $(filter-out $(SRC_DIR)/s21_create_matrix.c, $(LIB_SRC))
GCOV_FLAGS := -fprofile-arcs -ftest-coverage

.PHONY: all clean test gcov_report format format_check

all: s21_matrix.a

s21_matrix.a: $(LIB_OBJ)
	ar rcs s21_matrix.a $(LIB_OBJ)

$(SRC_DIR)/%.o: $(SRC_DIR)/%.c $(HDR)
	$(CC) $(CFLAGS) -c $< -o $@

test: clean
	$(CC) $(CFLAGS) $(GCOV_FLAGS) \
	$(LIB_SRC_TEST) $(TESTSRC) \
	-o $(TESTBIN) $(LDFLAGS)
	./$(TESTBIN)

gcov_report: test
	mkdir -p $(REPORT_DIR)
	lcov --capture --directory . --output-file $(REPORT_DIR)/coverage.info
	genhtml $(REPORT_DIR)/coverage.info --output-directory $(REPORT_DIR)


format:
	clang-format -i *.c *.h modules/*.c


format_check:
	clang-format --dry-run --Werror $(HDR) $(TESTSRC) $(shell find $(SRC_DIR) -name "*.c")

clean:
	rm -rf $(SRC_DIR)/*.o s21_matrix.a $(TESTBIN) \
	       *.gcda *.gcno \
	       $(SRC_DIR)/*.gcda $(SRC_DIR)/*.gcno \
	       $(REPORT_DIR)

