PREFIX =
BINDIR = ${PREFIX}/bin
DESTDIR =

CC = gcc
CFLAGS = -Os -Wall -pedantic -std=gnu99
LDFLAGS = -Wl,--as-needed
LDLIBS = -lcrypto

BINARIES = totp

all: ${BINARIES}

install: ${BINARIES}
	mkdir -p ${DESTDIR}${BINDIR}
	install -s ${BINARIES} ${DESTDIR}${BINDIR}

clean:
	rm -f ${BINARIES}

.PHONY: all install clean
