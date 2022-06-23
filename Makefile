BINDIR := $(PREFIX)/bin

CFLAGS := -Os -Wall -Wfatal-errors
LDFLAGS := -Wl,--as-needed
LDLIBS := -lcrypto

totp: totp.c Makefile
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(filter %.c,$^) $(LDLIBS)

install: totp
	mkdir -p $(DESTDIR)$(BINDIR)
	install -s $^ $(DESTDIR)$(BINDIR)

clean:
	rm -f totp

.PHONY: install clean
