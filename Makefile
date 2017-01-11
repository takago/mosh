CFLAGS := -DCURSES -O2 -W -Wall -Wno-unused-parameter -Wno-unused-variable -Wno-unused-result
all:
	gcc -c mosh.c ${CFLAGS}
	gcc -c common.c ${CFLAGS}
	gcc -c client.c ${CFLAGS} 
	gcc -c server.c ${CFLAGS}

	gcc -o mosh mosh.o common.o client.o server.o -lpthread -lncursesw
	@echo "ビルドが完了しました"	
install:
	cp mosh mosh-tui /usr/local/bin
	chown root /usr/local/bin/mosh /usr/local/bin/mosh-tui
	mkdir -p /usr/local/share/applications/
	cp mosh.desktop /usr/local/share/applications/
	@echo "インストールが完了しました"
test1:
	hexdump -vC /dev/urandom | perl -pe 'use Time::HiRes qw(sleep);sleep(0.1)'
	@echo "!!! 指示があるまで，何もタイプしないで下さい !!!"
test2:
	hexdump -vC /dev/urandom | perl -pe 'use Time::HiRes qw(sleep);sleep(0.05)'
	@echo "!!! 指示があるまで，何もタイプしないで下さい !!!"
test3:
	hexdump -vC /dev/urandom  | perl -pe 'use Time::HiRes qw(sleep);sleep(0.025)'
	@echo "!!! 指示があるまで，何もタイプしないで下さい !!!"
test4:
	hexdump -vC /dev/urandom  | perl -pe 'use Time::HiRes qw(sleep);sleep(0.0125)'
	@echo "!!! 指示があるまで，何もタイプしないで下さい !!!"
test5:
	hexdump -vC /dev/urandom
	@echo "!!! 指示があるまで，何もタイプしないで下さい !!!"

uninstall:
	rm -fr /usr/local/share/applications/mosh.desktop
	rm -fr /usr/local/bin/mosh /usr/local/bin/mosh-tui
	@echo "アンインストールが完了しました"
clean:
	rm -fr mosh *.o ./a.out
