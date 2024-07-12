CC=gcc
CFLAGS=-g -ggdb3 -Wall -Wextra -std=gnu99 #deleted -Werror after -Wall
LDFLAGS=-pthread
EXECUTABLES=httpserver forkserver threadserver poolserver
SOURCE=httpserver.c libhttp.c wq.c

all: $(EXECUTABLES)

httpserver: $(SOURCE)
	$(CC) $(CFLAGS) $(LDFLAGS) -D BASICSERVER $(SOURCE) -o $@
forkserver: $(SOURCE)
	$(CC) $(CFLAGS) $(LDFLAGS) -D FORKSERVER $(SOURCE) -o $@
threadserver: $(SOURCE)
	$(CC) $(CFLAGS) $(LDFLAGS) -D THREADSERVER $(SOURCE) -o $@
poolserver: $(SOURCE)
	$(CC) $(CFLAGS) $(LDFLAGS) -D POOLSERVER $(SOURCE) -o $@

clean:
	rm -f $(EXECUTABLES)

test:
	make clean
	make
	./httpserver --files www

connect: 
	curl -v http://0.0.0.0:8000/


