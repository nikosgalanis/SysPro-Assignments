TARGET_EXEC = diseaseAggregator

CC = gcc

BUILD_DIR = ./build_aggregator
SRC_DIRS = . ../modules
INC_DIRS = ../include

SRCS := $(shell find $(SRC_DIRS) -name "*.c")
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)
DEPS := $(OBJS:.o=.d)

CC_FLAGS = -Wall -g -I$(INC_DIRS)

$(TARGET_EXEC): $(OBJS)
	$(CC) $(OBJS) -o $@

# c source
$(BUILD_DIR)/%.c.o: %.c
	$(MKDIR_P) $(dir $@)
	$(CC) -c $(CC_FLAGS) $< -o $@ -lm


.PHONY: clean

clean:
	$(RM) -r $(BUILD_DIR)
	$(RM) 

run:
	./$(TARGET_EXEC) -w 10 -b 100 -i input

valgrind:
	valgrind ./$(TARGET_EXEC)
	

-include $(DEPS)

MKDIR_P ?= mkdir -p