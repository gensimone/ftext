CC = cc
CFLAGS = -Wall -Wextra -g
DEPS = utils.h alloc.h strutil.h ccore.h queue.h
OBJ = ftext.o util.o alloc.o strutil.o ccore.o queue.o
TARGET = ftext

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) -o $@ $^ -lm

%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)
