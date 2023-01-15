CC=gcc
LIBRARIES=hildon-1 gconf-2.0 gtk+-2.0 glib-2.0 x11
CFLAGS= -Wall -std=gnu17 -g `pkg-config --cflags $(LIBRARIES)`
LDFLAGS=`pkg-config --libs $(LIBRARIES)` -lm

all: suspend

suspend: suspend.o
	$(CC) suspend.o -o suspend -fPIC $(LDFLAGS)

suspend.o: suspend.c
	$(CC) $(CFLAGS) -c -o suspend.o suspend.c

install: suspend
	install -d /usr/bin
	install suspend /usr/bin/maemo-suspend
	install -d /usr/share/icons/
	install suspend.png /usr/share/icons/
	install -d /usr/share/applications/hildon/
	install suspend.desktop /usr/share/applications/hildon/

clean:
	rm -f suspend.o suspend
