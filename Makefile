CC = gcc
CFLAGS = -Wall 
CFLAGS += $(shell pkg-config --cflags json-c)
LDFLAGS += $(shell pkg-config --libs json-c)
objects = chatPrueba.o


chat : $(objects)
	$(CC) $(CFLAGS) $(LDFLAGS) -o chatPrueba $(objects)

chatPrueba.o : chatPrueba.c
	$(CC)  $(CFLAGS) $(LDFLAGS) -c chatPrueba.c

.PHONY : clean
clean :
	-rm chatPrueba $(objects)
