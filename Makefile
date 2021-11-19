CC = gcc
CFLAGS = -lssl -lcrypto
CPPFLAGS = -g
DEPS = messages.h

%.o: %.c $(DEPS)
	$(CC) $(CPPFLAGS) -c -o $@ $< $(CFLAGS)

servermake: experiment2.o 
	$(CC) $(CPPFLAGS) experiment2.o -o server $(CFLAGS)