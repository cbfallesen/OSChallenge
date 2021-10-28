CC = gcc
CFLAGS = -lssl -lcrypto
CPPFLAGS = -g
DEPS = messages.h

%.o: %.c $(DEPS)
	$(CC) $(CPPFLAGS) -c -o $@ $< $(CFLAGS)

servermake: server.o 
	$(CC) $(CPPFLAGS) server.o -o server $(CFLAGS)