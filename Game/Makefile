CC?=clang
CFLAGS=-c -Wall -Werror -ggdb -std=gnu99
SOURCES= spielfeldAusgabe.c prozessSync.c logger.c performConnection.c network.c sharedmemory.c prolog.c config.c denkmal.c pipe.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=client

all: $(SOURCES)  $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) -o $@

.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -rf *.o $(EXECUTABLE)
