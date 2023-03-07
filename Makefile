CC=gcc
LIBRARIES=hildon-1 gconf-2.0 gtk+-2.0 glib-2.0 x11
CFLAGS= -Wall -std=gnu17 -g `pkg-config --cflags $(LIBRARIES)`
LDFLAGS=`pkg-config --libs $(LIBRARIES)` -lm

all: maemo-pp-control

maemo-pp-control: maemo-pp-control.o
	$(CC) maemo-pp-control.o -o maemo-pp-control -fPIC $(LDFLAGS)

maemo-pp-control.o: maemo-pp-control.c
	$(CC) $(CFLAGS) -c -o maemo-pp-control.o maemo-pp-control.c

install: maemo-pp-control
	install -d /usr/bin
	install maemo-pp-control /usr/bin/maemo-maemo-pp-control
	install -d /usr/share/icons/
	install maemo-pp-control.png /usr/share/icons/
	install -d /usr/share/applications/hildon/
	install maemo-pp-control.desktop /usr/share/applications/hildon/

clean:
	rm -f maemo-pp-control.o maemo-pp-control
