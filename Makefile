
all : timer

timer : timer.o duration_buffer.o resources.o
	gcc -o $@ $^ `pkg-config --libs gtk+-3.0` -lm -rdynamic

%.o : %.c
	gcc -c $< `pkg-config --cflags gtk+-3.0` -g -O3 -Wall -Wextra -Werror -std=gnu11 -rdynamic

resources.c : resources.xml main.glade
	glib-compile-resources resources.xml --generate-source

clean :
	-rm timer resources.gresource resources.c *.o
