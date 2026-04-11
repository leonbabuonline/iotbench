CC     = gcc
CFLAGS = -Wall -Wextra -g
TARGET = iotbench

SRC = main.c

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

clean:
	rm -f $(TARGET)