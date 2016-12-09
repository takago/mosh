CFLAGS := -DCURSES -O2 -W -Wall -Wno-unused-parameter -Wno-unused-variable -Wno-unused-result
all:
	gcc -c mosh.c ${CFLAGS}
	gcc -c common.c ${CFLAGS}
	gcc -c client.c ${CFLAGS} 
	gcc -c server.c ${CFLAGS}

	gcc -o mosh mosh.o common.o client.o server.o -lpthread -lncursesw

install:
	cp mosh mosh-tui /usr/local/bin
	chown root /usr/local/bin/mosh /usr/local/bin/mosh-tui
	mkdir -p /usr/local/share/applications/
	cp mosh.desktop /usr/local/share/applications/

clean:
	rm -fr mosh *.o ./a.out
