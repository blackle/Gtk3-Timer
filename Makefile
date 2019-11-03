
all : timer

timer : timer.o resources.o
	gcc -o $@ $^ `pkg-config --libs webkit2gtk-4.0` 

%.o : %.c
	gcc -c $< `pkg-config --cflags webkit2gtk-4.0` -O3 -Wall -Wextra -Werror -std=gnu11

resources.c : resources.xml
	glib-compile-resources resources.xml --generate-source

clean :
	-rm timer resources.gresource resources.c *.o
