# Compiler and flags
CC       := gcc
CFLAGS   := -Wall -Wextra -O2 -std=c99 -Iinclude -MMD
LDFLAGS  := -lm  

CFLAGS += -msse4.1

SRC_DIR  := src
INC_DIR  := include
BUILD_DIR:= build
BIN_DIR  := bin

TARGET   := gol

SRCS := $(wildcard $(SRC_DIR)/*.c)
OBJS := $(SRCS:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)
DEPS := $(OBJS:.o=.d)

.PHONY: all
all: $(BIN_DIR)/$(TARGET)

$(BIN_DIR)/$(TARGET): $(OBJS) | $(BIN_DIR)
	$(CC) $(CFLAGS) $(OBJS) -o $@ $(LDFLAGS)
	@echo "Build complete: $@"

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

.PHONY: clean
clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)/$(TARGET)
	@echo "All build artifacts cleaned."

.PHONY: run
run: all
	./$(BIN_DIR)/$(TARGET)

-include $(DEPS)
