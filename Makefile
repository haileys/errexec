INSTALL_PREFIX=/usr/local

CFLAGS = -Wall -Wextra -pedantic -Werror -std=c99 -O3

all: errexec

.PHONY: install uninstall

install: errexec
	mkdir -p $(INSTALL_PREFIX)/bin
	cp errexec $(INSTALL_PREFIX)/bin/errexec

uninstall:
	rm -f $(INSTALL_PREFIX)/bin/errexec
