/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
 *  
 *                       Copyright 2015 TAKAGO_LAB.
 *                   Daisuke Takago(takago@neptune.kanazawa-it.ac.jp)
 *                   Yuuki Horie(b1334513@planet.kanazwa-it.ac.jp)
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 *  This file is part of  "MOSH"
 *
 *  MOSH is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  MOSH program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 * 
 *  You should have received a copy of the GNU General Public License
 *  along with MOSH.  If not, see <http://www.gnu.org/licenses/>.
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


#define _XOPEN_SOURCE	// これが無いとptsname()がSEGVを起こす
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/select.h>
#include <stdio.h>
#include <stdlib.h>	/* exit() */
#include <string.h>	/* bzero() */
#include <sys/types.h>
#include <sys/socket.h>	/* socket(), bind(), listen(), accept(), recv() */
#include <netinet/in.h>	/* htons() */
#include <unistd.h>
#include <sys/wait.h>
#include <strings.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <netdb.h>
#include <arpa/inet.h>
#include "common.h"
#include "client.h"

int client(int srv_port, char *srv_ip, char *usr_str)
{

	int ssc, csc;				// ソケットディスクリプタ
	int mfd, sfd;				// 端末（マスタ・スレーブ）ディスクリプタ
	socklen_t len = sizeof(struct sockaddr_in);
	int rsize;
	char buf[MAXDATA];			/* 受信バッファ */
	fd_set rfds;
	char point[MAXDATA] = "CON:";

	// ユーザの使用シェル
	char usr_shell[100];

	puts(COPYRIGHT_MSG);

	if (usr_str == NULL)
		usr_str = "John Doe";

//擬似端末の確保
	mfd = open("/dev/ptmx", O_RDWR);	// 擬似端末オープン（マスタ側）
//      printf("Open New Psudo Terminal: %s\r\n",ptsname(mfd));
	grantpt(mfd);	//スレーブ擬似端末へのアクセス許可
	unlockpt(mfd);	//擬似端末マスタ、スレーブのペアロック解除


	if (0 == fork()) {	// シェルを起動する．
		strcpy(usr_shell, getenv("SHELL"));


		sfd = open(ptsname(mfd), O_RDWR);	// 擬似端末オープン（スレーブ側  ptsname()--スレーブ擬似端末の名前取得
		close(mfd);
		setsid();
//          ioctl(sfd, TIOCSCTTY, 0); // 効果不明

		dup2(sfd, 0);
		dup2(sfd, 1);
		dup2(sfd, 2);
		close(sfd);
		close(mfd);
		execlp(usr_shell, usr_shell, NULL);
		exit(1);	// 起動に失敗した時
	}

	char *ip_str =
	  inet_ntoa(*(struct in_addr *)(gethostbyname(srv_ip)->h_addr_list[0]));

	ssc = socket(AF_INET, SOCK_STREAM, 0);

	ssocket(srv_port, ip_str);

	if (-1 == connect(ssc, (struct sockaddr *)&saddr, len)) {
		goto err;
	}
	strcat(point, usr_str);
	printf("%s\n", point);
	send(ssc, point, strlen(point), MSG_NOSIGNAL);	// 最初に送るメッセージ

	//send(ssc, usr_str, strlen(usr_str), MSG_NOSIGNAL);
	printf("サーバに接続しました．\r\n");
	//sprintf(buf,"======== ユーザ：%s の端末を開きました =========\n",usr_str);
	//send(ssc, usr_str, strlen(usr_str), MSG_NOSIGNAL);
	//printf("%s\n",usr_str);
	//printf("%s\n",point);

	send(ssc, buf, strlen(buf), MSG_NOSIGNAL);
	recv(ssc, buf, MAXDATA, MSG_NOSIGNAL);	// 受信バッファを空にする(ノンブロックにしてあること)
	//for(i=4; i<10; i++){

	// 途中でブロックしないように
	fcntl(0, F_SETFL, O_NONBLOCK);
	fcntl(ssc, F_SETFL, O_NONBLOCK);
	fcntl(mfd, F_SETFL, O_NONBLOCK);


	while (1) {
		FD_ZERO(&rfds);
		FD_SET(0, &rfds);	// 標準入力を監視
		FD_SET(ssc, &rfds);	// ソケットを監視
		FD_SET(mfd, &rfds);	// マスタ端末を監視
		select(100, &rfds, NULL, NULL, NULL);
		if (FD_ISSET(0, &rfds)) {
			rsize = read(0, buf, MAXDATA);
			if (rsize <= 0) {
				break;
			}
			write(mfd, buf, rsize);
		}
		if (FD_ISSET(ssc, &rfds)) {
			rsize = recv(ssc, buf, MAXDATA, MSG_NOSIGNAL);
			if (rsize <= 0) {
				break;
			}
			write(mfd, buf, rsize);
		}
		if (FD_ISSET(mfd, &rfds)) {
			rsize = read(mfd, buf, MAXDATA);
			if (rsize <= 0) {
				break;
			}
			send(ssc, buf, rsize, MSG_NOSIGNAL);
			write(1, buf, rsize);	// 画面に表示           
		}
	}
  err:
	close(ssc);
	close(mfd);
	wait(NULL);
	tcflush(STDIN_FILENO, TCIFLUSH);
	tcsetattr(STDIN_FILENO, TCSANOW, &tios0);
	printf("\n切断しました．\n");
	return 0;
}
