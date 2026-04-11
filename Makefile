CC     = gcc
CFLAGS = -Wall -Wextra -g
LIBS   = -lmosquitto -lrt -lpthread
TARGET = iotbench

SRC = main.c device.c

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC) $(LIBS)

clean:
	rm -f $(TARGET)