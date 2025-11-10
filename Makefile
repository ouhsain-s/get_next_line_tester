# get_next_line_tester/Makefile
CC := cc
CFLAGS := -Wall -Wextra -Werror

BUFFER ?= 21
ARGS ?=

# tester sources (inside this folder)
TST_MANDATORY_SRC := tst_mandatory.c
TST_BONUS_SRC := tst_bonus.c
TST_BONUS_YOURFILES_SRC := tst_bonus_with_yourfiles.c

MANDATORY_TARGET := tst_mandatory
BONUS_TARGET := tst_bonus
BONUS_YOURFILES_TARGET := tst_bonus_with_yourfiles

# ====== detect GNL sources in parent directory (project root) ======
MANDATORY_GNL_SRCS := $(wildcard ../get_next_line.c ../get_next_line_utils.c)
BONUS_GNL_SRCS := $(wildcard ../get_next_line_bonus.c ../get_next_line_utils_bonus.c)

ifeq ($(strip $(MANDATORY_GNL_SRCS)),)
$(error Mandatory GNL sources not found in parent dir (../get_next_line.c ../get_next_line_utils.c). Place them in project root.)
endif

# choose which GNL sources to compile with
ifeq ($(strip $(BONUS_GNL_SRCS)),)
GNL_SRCS := $(MANDATORY_GNL_SRCS)
HAVE_BONUS := 0
else
GNL_SRCS := $(BONUS_GNL_SRCS)
HAVE_BONUS := 1
endif

.PHONY: all clean re run-mandatory run-bonus run-bonus-yourfiles \
        valgrind-mandatory valgrind-bonus valgrind-bonus-yourfiles

# build everything available
all: $(MANDATORY_TARGET) $(if $(filter 1,$(HAVE_BONUS)),$(BONUS_TARGET) $(BONUS_YOURFILES_TARGET))

# mandatory tester (always built)
$(MANDATORY_TARGET): $(TST_MANDATORY_SRC) $(MANDATORY_GNL_SRCS)
	@echo "Compiling $(MANDATORY_TARGET) (using parent mandatory GNL) BUFFER_SIZE=$(BUFFER)"
	$(CC) $(CFLAGS) $(TST_MANDATORY_SRC) $(MANDATORY_GNL_SRCS) -D BUFFER_SIZE=$(BUFFER) -o $(MANDATORY_TARGET)
	@echo "-> Built $(MANDATORY_TARGET)"

# bonus testers - built only if bonus files exist in parent
ifneq ($(filter 1,$(HAVE_BONUS)),)
$(BONUS_TARGET): $(TST_BONUS_SRC) $(BONUS_GNL_SRCS)
	@echo "Compiling $(BONUS_TARGET) (using parent bonus GNL) BUFFER_SIZE=$(BUFFER)"
	$(CC) $(CFLAGS) $(TST_BONUS_SRC) $(BONUS_GNL_SRCS) -D BUFFER_SIZE=$(BUFFER) -o $(BONUS_TARGET)
	@echo "-> Built $(BONUS_TARGET)"

$(BONUS_YOURFILES_TARGET): $(TST_BONUS_YOURFILES_SRC) $(BONUS_GNL_SRCS)
	@echo "Compiling $(BONUS_YOURFILES_TARGET) (using parent bonus GNL) BUFFER_SIZE=$(BUFFER)"
	$(CC) $(CFLAGS) $(TST_BONUS_YOURFILES_SRC) $(BONUS_GNL_SRCS) -D BUFFER_SIZE=$(BUFFER) -o $(BONUS_YOURFILES_TARGET)
	@echo "-> Built $(BONUS_YOURFILES_TARGET)"
endif

# run helpers
run-mandatory: $(MANDATORY_TARGET)
	./$(MANDATORY_TARGET)

run-bonus: $(if $(filter 1,$(HAVE_BONUS)),$(BONUS_TARGET),)
ifneq ($(filter 1,$(HAVE_BONUS)),)
	./$(BONUS_TARGET)
else
	@echo "Bonus GNL not found in parent directory. No bonus tester to run."
endif

run-bonus-yourfiles: $(if $(filter 1,$(HAVE_BONUS)),$(BONUS_YOURFILES_TARGET),)
ifneq ($(filter 1,$(HAVE_BONUS)),)
	@if [ -z "$(ARGS)" ]; then \
		echo "Usage: make run-bonus-yourfiles ARGS=\"../file1 ../file2 ...\""; \
		exit 1; \
	fi
	@echo "Running $(BONUS_YOURFILES_TARGET) on: $(ARGS)"
	./$(BONUS_YOURFILES_TARGET) $(ARGS)
else
	@echo "Bonus GNL not found in parent directory. No multi-FD tester to run."
endif

# valgrind targets (ensure binary is built first)
valgrind-mandatory: $(MANDATORY_TARGET)
	@printf "\n=== VALGRIND: $(MANDATORY_TARGET) ===\n\n"
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./$(MANDATORY_TARGET)

valgrind-bonus: $(BONUS_TARGET)
	@printf "\n=== VALGRIND: $(BONUS_TARGET) ===\n\n"
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./$(BONUS_TARGET)

valgrind-bonus-yourfiles: $(BONUS_YOURFILES_TARGET)
ifneq ($(filter 1,$(HAVE_BONUS)),)
	@if [ -z "$(ARGS)" ]; then \
		echo "Usage: make valgrind-bonus-yourfiles ARGS=\"../file1 ../file2 ...\""; \
		exit 1; \
	fi
	@printf "\n=== VALGRIND: $(BONUS_YOURFILES_TARGET) ===\n\n"
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./$(BONUS_YOURFILES_TARGET) $(ARGS)
else
	@echo "Bonus GNL not found in parent directory. No valgrind target for bonus-yourfiles."
endif

clean:
	rm -f $(MANDATORY_TARGET) $(BONUS_TARGET) $(BONUS_YOURFILES_TARGET) test_*.txt bonus_test_*.txt smart_test_*.txt
	@echo "Cleaned tester binaries and generated test files."

re: clean all
	@echo "Rebuilt all available testers."

