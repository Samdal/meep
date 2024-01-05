all: meep

meep: meep.c Makefile
	gcc `pkg-config --cflags libnotify x11 xi` meep.c `pkg-config --libs libnotify x11 xi` -o meep

clean:
	rm meep

.phony: all clean
