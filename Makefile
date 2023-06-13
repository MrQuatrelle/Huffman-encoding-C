CC = gcc

# compiler flags:
#  -g    adds debugging information to the executable file
#  -Wall turns on most, but not all, compiler warnings
TEST_FLAGS  = -g -Wall -Werror -std=c17
RELEASE_FLAGS = -Wall -Werror -std=c17 -O3

# the build target executable:
TARGET = main

all: $(TARGET)

$(TARGET): $(TARGET).c
	$(CC) $(TEST_FLAGS) -o beta $(TARGET).c

release:
	$(CC) $(RELEASE_FLAGS) -o huffman $(TARGET).c

clean:
	$(RM) $(TARGET)
