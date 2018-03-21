CC = gcc
CFLAGS += -g -O3 -std=c99 -pedantic -Wall -Werror -Wno-error=unused-function -Wno-error=unused-but-set-variable -pthread
CPPFLAGS += -D_XOPEN_SOURCE=700 -D_FORTIFY_SOURCE=2

all: nvgpg symlinks norm

nvgpg: nvgpg.o reocol.o playfair.o
	$(CC) $(LDFLAGS) $(CFLAGS) -o $@ $^

norm: norm.o
	$(CC) $(LDFLAGS) $(CFLAGS) -o $@ $^

symlinks:
	[ -L nvgpg-encrypt ] || ln -s nvgpg nvgpg-encrypt
	[ -L nvgpg-decrypt ] || ln -s nvgpg nvgpg-decrypt

%.o: %.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $<

clean:
	rm -f *.o nvgpg nvgpg-decrypt nvgpg-encrypt

.PHONY: clean all symlinks
