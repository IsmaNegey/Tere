
CC = gcc

TARGET = build/tere
SRC_DIR = src
INC_DIR = include
BUILD_DIR = build

CFLAGS = -Wall -Wextra -Werror -std=c11 -D_DEFAULT_SOURCE -g -Iinclude
LDFLAGS =
LDLIBS =

SRC = $(wildcard $(SRC_DIR)/*.c)
OBJ = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRC))
DEP = $(OBJ:.o=.d)

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(BUILD_DIR):
	mkdir -p $@

clean:
	rm -rf $(BUILD_DIR)

re: clean all

run: $(TARGET)
	./$(TARGET)

-include $(DEP)
