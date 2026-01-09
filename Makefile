CC = cc
CFLAGS = -Wall -Wextra -g -pthread
DEPS = utils.h alloc.h strutil.h core.h queue.h thread.h
OBJ = ftext.o util.o alloc.o strutil.o core.o queue.o thread.o
TARGET = ftext

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) -o $@ $^ -lm

%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)
