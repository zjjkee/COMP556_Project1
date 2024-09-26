CC = gcc
CFLAGS = -Wall -Wextra -g -Iheaders
SERVER_SRC = source/server.c
CLIENT_SRC = source/client_num.c
SERVER_OBJ = server.o
CLIENT_OBJ = client_num.o
TARGET_SERVER = server
TARGET_CLIENT = client_num
TARGET_ALL = $(TARGET_SERVER) $(TARGET_CLIENT)

.PHONY: all clean server client

all: $(TARGET_ALL)

$(SERVER_OBJ): $(SERVER_SRC)
	$(CC) $(CFLAGS) -c $<

$(CLIENT_OBJ): $(CLIENT_SRC)
	$(CC) $(CFLAGS) -c $<

$(TARGET_SERVER): $(SERVER_OBJ)
	$(CC) $(CFLAGS) -o $@ $^

$(TARGET_CLIENT): $(CLIENT_OBJ)
	$(CC) $(CFLAGS) -o $@ $^

server: $(TARGET_SERVER)

client: $(TARGET_CLIENT)

clean:
	rm -f $(SERVER_OBJ) $(CLIENT_OBJ) $(TARGET_ALL)