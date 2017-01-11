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
#include "common.h"
#include <signal.h>

#define MAXDATA 65536
struct termios tios0;			// 端末設定(復元用)

struct info
{
	int ene;					// 0:未使用，1:使用中
	char ip_addr[100];
	char usr_str[100];
	pid_t x_pid;
	char m_name[100];
};

int server(int port);
