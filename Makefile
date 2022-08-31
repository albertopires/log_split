CC=gcc
CFLAGS=-Wall -Wextra -g -pedantic -std=c11 -D_GNU_SOURCE -O2
export VERS='"v1.1.1 - 31/Ago/2022"'
export GIT_HASH=\"`git log --pretty=format:'%h' -n 1`\"

all: log_split

clean:
	find . -name "*.o" -exec rm {} \;
	rm -f log_split

log_split: log_split.o
	$(CC) $(CFLAGS) $(LFLAGS) -o log_split log_split.o -Wl,-rpath,'$$ORIGIN'/local_libs

log_split.o: log_split.c
	$(CC) $(CFLAGS) -DVER=$(VERS) -DBUILD=$(GIT_HASH) -c $*.c -o $@
