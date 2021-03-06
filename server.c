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
#define _BSD_SOURCE	// strsep()
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
#include <sys/ioctl.h>
#include <termios.h>
#include<signal.h>
#include "common.h"
#include "server.h"
#include <arpa/inet.h>
#include <libgen.h>	// basename 必須う
#include <pthread.h>
#include <ncurses.h>
#include <locale.h>	// setlocale() マルチバイト表示用
#include <ctype.h>
#define PROWS 100

#define PCOL 0	// PIDの始まりのカラム
#define UCOL 6	// USRの始まりのカラム
#define ICOL 14	// ITカウント表示の始まりのカラム
#define CCOL 29	// カウント表示の始まりのカラム
#define SCOL 36	// ストロークの始まりのカラム


int xx, yy;						// メインスクリーンの大きさ
int px = 0, py = 0;				// パッド内のカーソル座標
int poy = 0, pox = 0;			// パッド画面の切り出しの左上座標

WINDOW *setup()
{
	setlocale(LC_ALL, "");	// setlocale() マルチバイト表示用
	initscr();
	clear();
#if 0
	raw();	// 割り込みも無視
#else
	cbreak();
#endif
	noecho();	// タイプした文字列を表示させない
	nodelay(stdscr, TRUE);	// ノンブロックモード
	keypad(stdscr, TRUE);	// キーパッド有効化（カーソルキーなどが使えるようになる)
	curs_set(0);	// カーソルOFF

	start_color();
	init_pair(1, COLOR_WHITE, COLOR_BLUE);
	init_pair(2, COLOR_BLUE, COLOR_WHITE);

	bkgd(COLOR_PAIR(1));	// 背景色を設定
	getmaxyx(stdscr, yy, xx);	// 標準スクリーンのサイズを取得   
	mvprintw(0, 0,
			 "Monitorized Shell Server (TAKAGO Lab. 2016)    [d]delete [o]deiconify [i]iconify");
	mvprintw(1, 0, "%3s %-10s%15s%13s%12s", "FD", "USER", "CLI_IP_ADDR", "TTY",
			 "PID");
	/*
	   パッドの作成(大きさを自由に決められる画面    
	 */
	WINDOW *pad = newpad(PROWS, xx);
	wbkgd(pad, COLOR_PAIR(2));	// パッドの背景色を設定
	int m;
	for (m = 0; m < PROWS; m++)
		mvwprintw(pad, m, 0, "---");
	wmove(pad, 0, 0);
	refresh();

	return pad;
}

void highlight(int ene, WINDOW * w, int y)
{
	char str[1024];
	wmove(w, y, 0);

	if (ene) {	// 色設定を反転
		wattrset(w, COLOR_PAIR(2) | A_REVERSE);
	}

	winnstr(w, str, xx);	// カーソルのところの文字列を読み取り，
	mvwprintw(w, y, 0, "%s", str);	// 再描画 
	wattrset(w, COLOR_PAIR(2));	// 色設定を元に戻す
}


// パッド内カーソルの移動（必要に応じてパッド画面をスクロール）
void next_line()
{
	py++;
	if (PROWS - 1 < py)
		py = PROWS - 1;
	if (poy - 3 + (yy - yy / 4) < py)
		poy++;
	if (PROWS + 3 - yy < poy)	// パッド画面の下になったら固定
		poy = PROWS + 3 - yy;
}

// パッド内カーソルの移動（必要に応じてパッド画面をスクロール）
void pre_line()
{
	py--;
	if (py < 0)
		py = 0;
	if (poy + (yy / 4) > py)
		poy--;
	if (py < poy)
		poy = 0;
}

struct sockaddr_in clntAddr;
struct info st_inf[200];

int ssc;
char buf[MAXDATA];				/* 受信バッファ */

fd_set rfds;
int rsize;

void *srv_th(void *csc1)
{

	int mfd;
	int sfd;
	int csc = *((int *)csc1);

	pthread_detach(pthread_self());

	mfd = open("/dev/ptmx", O_RDWR);	// 擬似端末オープン（マスタ側）
	grantpt(mfd);
	unlockpt(mfd);

	sfd = open(ptsname(mfd), O_RDWR);

	strcpy(st_inf[csc].m_name, ptsname(mfd));
	SetTerminal(sfd); // これを実行しないとXterm起動時に変な数字が表示される

	char title_str[256];
	// 6
	sprintf(title_str, "mosh %s@%s:%d",  st_inf[csc].usr_str,
			inet_ntoa(clntAddr.sin_addr), ntohs(clntAddr.sin_port));
	int n;
	pid_t pid;




	pid = fork();	//xtermのプロセス番号  親プロセスには子プロセスのPID、子プロセスには0が返される

	if (0 == pid) {	// 子プロセスでxtermを起動
		setsid();	// ★重要： この新プロセスをセッションリーダにする（元の制御端末と切り離せる） ★

		char buf[128];
		strcpy(buf, ptsname(mfd));
		sprintf(buf, "-S%s/%d", basename(buf), mfd);	//最後の数字sfdだと?;

		close(sfd);
		// xterm -fa Inconsolata -fd IPAGothic:style=Regular -fs 16
		execlp("xterm", "xterm", "-iconic",
			   "-xrm","xterm*allowTitleOps: false",
			   "-xrm","xterm*allowFontOps: true",
			   "-xrm","xterm*allowWindowOps: true",
               "-xrm","xterm*initialFont: 2", 
			   "-fa", "Inconsolata", "-bg","lightgray", "-fg","black",
			   "-fd", "Noto Sans CJK JP Light",
			   "-fs", "16", "-n", title_str, "-T", title_str, buf, NULL);
		printf("(x_x)\n");
		exit(1);
	}

	st_inf[csc].x_pid = pid;
	close(mfd);
    
    struct timeval tv; //重要： selectでタイムアウトを設定しておかないと，まれにブロックしたままになる
	while (1) {
        tv.tv_sec = 0;
        tv.tv_usec = 500*1000; // 500ms
        
		FD_ZERO(&rfds);
		FD_SET(csc, &rfds);
		FD_SET(sfd, &rfds);

		select(100, &rfds, NULL, NULL, &tv);
		if (FD_ISSET(sfd, &rfds)) {
			rsize = read(sfd, buf, MAXDATA);	// キー入力を送信
			if (rsize <= 0) {
				break;
			}
            send(csc, buf, rsize, MSG_NOSIGNAL);
		}
		if (FD_ISSET(csc, &rfds)) {
			rsize = recv(csc, buf, MAXDATA, MSG_NOSIGNAL);
			if (rsize <= 0) {
				kill(pid, SIGKILL);
				waitpid(pid, NULL, 0);
				break;
			}
			rsize = write(sfd, buf, rsize);	// 受信文字を画面に表示
		}
	}
	st_inf[csc].ene = 0;
	close(csc);
	close(sfd);
	return NULL;
}

//　管理画面スレッド
#ifdef CURSES
void *list()
{
	int fd = 0;
	int c = 0;

	pthread_detach(pthread_self());

	WINDOW *pad = setup();		// スクリーンにパッドを貼り付ける 


	while (1) {

		for (fd = 0; fd < 200; fd++) {
			if (st_inf[fd].ene) {
				mvwprintw(pad, fd, 0, "%3d %-10s%15s%15s%10d\n",
						  fd, st_inf[fd].usr_str, st_inf[fd].ip_addr,
						  st_inf[fd].m_name, st_inf[fd].x_pid);
			}
			else {
				mvwprintw(pad, fd, 0, "%3d ---\n", fd);
			}
		}
		highlight(1, pad, py);
		prefresh(pad, poy, pox, 2, 0, yy - 2, xx);
		c = getch();
		highlight(0, pad, py);
		switch (c) {
		case KEY_DOWN:	// "↓"が押されたら，現在行を移動
			next_line();
			break;
		case KEY_UP:	// "↑"が押されたら，現在行を移動
			pre_line();
			break;
		case 'i':	// アイコン化
			if (st_inf[py].ene) {
				sprintf(buf, "/bin/echo -ne '\e[2t'> %s", st_inf[py].m_name);
				system(buf);
			}
			break;
		case 'o':	// 非アイコン化
			if (st_inf[py].ene) {
				sprintf(buf, "/bin/echo -ne '\e[1t'> %s", st_inf[py].m_name);
				system(buf);
			}
			break;
		case 'd':	// kill           
			if (st_inf[py].ene) {
				kill(st_inf[py].x_pid, SIGKILL);
			}
			break;
		default:
			break;
		}
		usleep(20 * 1000);	// CPUの負荷を下げるためのsleep
	}
	pthread_exit(0);
}
#else
void *list()
{
	while (1) {
		printf("Hello\n");
		sleep(1);
	}
}
#endif

int server(int port)
{

	void *srv_th(void *csc1);
	void *list();
	pthread_t th1, th2;

	int csc;
	int opt;

	//文字列分割用の変数
	char usr_name[MAXDATA];		//受信バッファ
	char *sep_p;
	int name_len;				//usr_nameの長さ

	struct sigaction sac1;

	sac1.sa_handler = SIG_IGN;	//
	sac1.sa_flags = SA_RESTART;

	sigaction(SIGCHLD, &sac1, NULL);	//

	if (port == 0)
		port = 60000;

	ssc = socket(AF_INET, SOCK_STREAM, 0);
	opt = 1;
	setsockopt(ssc, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(int));

	ssocket(port, "0.0.0.0");
	socklen_t len = sizeof(struct sockaddr_in);
	if( bind(ssc, (struct sockaddr *)&saddr, len) < 0 ){		
		printf("サーバがすでに起動しています\n");
		exit(1);		
	}
	
	listen(ssc, 200);

	puts(COPYRIGHT_MSG);

	printf("\n サーバを起動しました \n");
	pthread_create(&th2, NULL, list, NULL);	//管理画面表示

	while (1) {
		//struct sockaddr_in clntAddr;  /* クライアントアドレス *///
		unsigned int clntLen = sizeof(clntAddr);

		if (-1 == (csc = accept(ssc, (struct sockaddr *)&clntAddr, &clntLen))) {	// 学生を待つ //
			continue;
		};

		name_len = recv(csc, usr_name, MAXDATA, 0);

		if (strncmp(usr_name, "CON", 3) != 0) {	// クライアントチェック
			continue;
		}

		//文字列分割              
		sep_p = usr_name;
		usr_name[name_len] = '\0';
		strsep(&sep_p, ":");
		strcpy(st_inf[csc].usr_str, sep_p);
		st_inf[csc].ene = 1;


		strcpy(st_inf[csc].ip_addr, inet_ntoa(clntAddr.sin_addr));	//IPアドレス取得

		fcntl(csc, F_SETFL, O_NONBLOCK);	// クライアントはノンブロック      
		recv(csc, buf, MAXDATA, MSG_NOSIGNAL);	// 受信バッファを空にする(ノンブロックにしてあること)


		pthread_create(&th1, NULL, srv_th, &csc);

	}
	printf("\n - - - 切断しました - - -\n");
	close(csc);
	exit(1);
}
