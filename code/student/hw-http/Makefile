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

basic_test:
	make clean
	make
	./httpserver --files www

p_test: #proxy test #remember to do port forwarding (or tunneling): ssh -L 8000:127.0.0.1:8000 docker162, or replace docker162 with 127.0.0.1
	make clean
	make 
	./httpserver --proxy inst.eecs.berkeley.edu:81

t_test:
	make clean
	make 
	./threadserver --proxy inst.eecs.berkeley.edu:81

connect: 
	curl -v http://0.0.0.0:8000/


