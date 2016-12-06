CFLAGS := -DCURSES -O2 -W -Wall -Wno-unused-parameter -Wno-unused-variable -Wno-unused-result
all:
	gcc -c mosh.c ${CFLAGS}
	gcc -c common.c ${CFLAGS}
	gcc -c client.c ${CFLAGS} 
	gcc -c server.c ${CFLAGS}

	gcc -o mosh mosh.o common.o client.o server.o -lpthread -lncursesw

install:
	cp mosh /usr/local/bin
	chown root /usr/local/bin/mosh

clean:
	rm -fr mosh *.o ./a.out
