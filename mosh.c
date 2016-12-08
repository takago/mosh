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



#include "server.h"
#include "client.h"

int main(int argc, char *argv[])
{
	char *option;

	int result;
	int port = 0;

	char *srv_ip = NULL;
	char *usr_str = NULL;
	int srv_port = 0;


	if (argc < 2) {
		printf("%s\n=========================\n", COPYRIGHT_MSG);
		printf(" Server Side:: %s -s ポート番号\n", argv[0]);
		printf
		  (" Client Side:: %s -c -h ホスト名 -p ポート番号 -u ユーザ名\n",
		   argv[0]);
		return 0;
	}

	while ((result = getopt(argc, argv, "s:ch:p:u:")) != -1) {
		switch (result) {
		case 's':
			port = atoi(optarg);
			option = "s";
			break;
		case 'c':
			option = "c";
			break;
		case 'h':	// サーバのIPアドレスまたはホスト名
			srv_ip = optarg;
			break;
		case 'p':	// サーバのポート番号
			srv_port = atoi(optarg);
			break;
		case 'u':	// ユーザ名
			usr_str = optarg;
			break;
		default:
			printf("error\n");
			return 1;
		}
	}

	init();

	if (strcmp(option, "s") == 0) {
		server(port);
	}
	else if (strcmp(option, "c") == 0) {
		client(srv_port, srv_ip, usr_str);
	}
	return 0;

}
