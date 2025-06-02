ifndef PREFIX
	PREFIX := /usr/local
endif

CC := gcc
MACROS :=
CLIBS := $(shell pkg-config --cflags --libs wayland-client)
CFLAGS := -Wextra -Wall -Wshadow -Wcast-align=strict -Wno-format-truncation -std=gnu11 $(MACROS)
DEBUG_FLAGS := -ggdb -g3 -DDEBUG -fsanitize=address,undefined -fanalyzer
REL_FLAGS := -O2

REL_PREFIX := release
DEBUG_PREFIX := debug

ifeq ($(RELEASE), 1)
	BUILD_DIR := build/$(REL_PREFIX)
	CFLAGS += $(REL_FLAGS)
else
	BUILD_DIR := build/$(DEBUG_PREFIX)
	CFLAGS += $(DEBUG_FLAGS)
endif

BIN_PATH := $(BUILD_DIR)/bin
OBJ_PATH := $(BUILD_DIR)/bin
DEP_PATH := $(BUILD_DIR)/dep

SRC_PATH := src
INCLUDE_PATH := ./src/include

TARGET_NAME := wayclipper
TARGET := $(BIN_PATH)/$(TARGET_NAME)

SRC := main.c ext-data-control-unstable-v1.c wlr-data-control-unstable-v1.c virtual-keyboard-unstable-v1.c
OBJ := $(addprefix $(OBJ_PATH)/, $(SRC:.c=.o))
DEPS := $(addprefix $(DEP_PATH)/, $(notdir $(OBJ:.o=.d)))

all: prebuild $(TARGET)

prebuild:
	@mkdir -p $(BIN_PATH) $(SRC_PATH) $(DEP_PATH) $(OBJ_PATH) $(INCLUDE_PATH) $(BUILD_PREFIX)

rebuild: clean all

clean:
	rm -fr build/release/*/*
	rm -fr build/debug/*/*

run: all
	./$(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(CLIBS)

$(OBJ_PATH)/%.o: $(SRC_PATH)/%.c
	$(CC) $(CFLAGS) -I$(INCLUDE_PATH) -MD -MP -MF $(DEP_PATH)/$(notdir $(basename $@).d) -o $@ -c $< $(CLIBS)

-include $(DEPS)

.PHONY: all clean prebuild rebuild run
