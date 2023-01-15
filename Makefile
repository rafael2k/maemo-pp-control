CC=gcc
LIBRARIES=gconf-2.0 hildon-1 gtk+-2.0 libosso gdk-2.0 gconf-2.0 glib-2.0
CFLAGS= -Wall -std=gnu17 -g `pkg-config --cflags $(LIBRARIES)`
LDFLAGS=`pkg-config --libs $(LIBRARIES)` -lm

all: suspend

suspend: suspend.o
	$(CC) $(LDFLAGS) suspend.o -o suspend

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
