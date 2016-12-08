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


#include <stdio.h>
#include <stdlib.h>	/* exit() */
#include <string.h>	/* bzero() */
#include <unistd.h>
#include <sys/wait.h>
#include <strings.h>
#include <termios.h>
#include <arpa/inet.h>
#include <ncurses.h>
#include "common.h"
struct termios tios0;			// 端末設定(復元用)
void sigterm_handler(int signum);

void sigterm_handler(int signum)
{
#ifdef CURSES
	endwin();	// ncurses が使っていたメモリを開放
#endif
	tcflush(STDIN_FILENO, TCIFLUSH);
	tcsetattr(STDIN_FILENO, TCSANOW, &tios0);	// 端末設定を復元
	printf("(o_o) Bye!!\n");


	exit(0);
}

/* ターミナル設定 */

void SetTerminal()
{
	struct termios t;
	tcgetattr(STDIN_FILENO, &t);
	// エコーバック無効，非カノニカルモード，シグナル無視
	//  t.c_lflag &= ~(ECHO|ICANON|ISIG);
	t.c_lflag &= ~(ECHO | ICANON);
	t.c_iflag &= ~ICRNL;
	t.c_cc[VTIME] = 0;
	t.c_cc[VMIN] = 1;
	tcflush(STDIN_FILENO, TCIFLUSH);
	tcsetattr(STDIN_FILENO, TCSANOW, &t);
}

void init()
{
	signal(SIGTERM, sigterm_handler);
	signal(SIGINT, sigterm_handler);
	tcgetattr(STDIN_FILENO, &tios0);	// 現在の設定を退避
	SetTerminal();
}

void ssocket(int port, char *ip_str)
{
	bzero((char *)&saddr, sizeof(saddr));
	saddr.sin_family = PF_INET;
	saddr.sin_addr.s_addr = inet_addr(ip_str);
	saddr.sin_port = htons(port);
}
