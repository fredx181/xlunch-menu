LIBS= `pkg-config --libs glib-2.0 gtk+-2.0 libmenu-cache`
CFLAGS+= -g -Wall -w `pkg-config --cflags glib-2.0 gtk+-2.0 libmenu-cache`
CC=gcc
#-DG_DISABLE_DEPRECATED

# Comment this line if you don't want icons to appear in menu
CFLAGS+=-DWITH_ICONS
# Uncomment this line if Openbox can display SVG icons 
# Check SVG support with '$ ldd /usr/bin/openbox | grep svg', librsvg must appear..
CFLAGS+=-DWITH_SVG

prefix= /usr/local
DESTDIR ?= $(prefix)
BINDIR= ${DESTDIR}/bin

#SRC= $(shell ls *.c 2> /dev/null)
SRC= menu.c
OBJ= $(SRC:.c=.o)

all: $(OBJ) xlunch-menu

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

xlunch-menu: $(OBJ)
	$(CC) $(OBJ) -o xlunch-menu $(LDFLAGS) $(LIBS)

.PHONY: clean install doc changelog check

clean:
	@rm -f *.o xlunch-menu
	@rm -rf doc

install:
	@strip -s xlunch-menu
	@install -Dm 755 xlunch-menu $(BINDIR)/xlunch-menu
	@install -Dm 755 xlunch-menu-gen $(BINDIR)/xlunch-menu-gen
	@install -Dm 755 xlunch-menu-update $(BINDIR)/xlunch-menu-update
	@install -Dm 644 xlunch-applications.menu /etc/xdg/menus/xlunch-applications.menu
	@install -Dm 755 update-entries.desktop /usr/share/applications/update-entries.desktop
	@install -Dm 644 xlunch-logo.png /usr/share/pixmaps/xlunch-logo.png
doc:
	robodoc --src . --doc doc/ --multidoc --index --html --cmode

check: xlunch-menu
	./xlunch-menu xlunch-applications.menu -o entries.dsv 2> /dev/null 

changelog:
	@hg log --style changelog > ChangeLog
