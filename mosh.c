// 2016 10/21 Horie Yuki


#include "server.h"
#include "client.h"

int main(int argc, char *argv[])
{
    char *option;

    int result;
    int port=0;

    char *srv_ip=NULL; 
    char *usr_str=NULL;    
    int srv_port=0;


    if(argc<2){
        printf("%s\n=========================\n",COPYRIGHT_MSG);        
        printf(" Server Side:: %s -s ポート番号\n",argv[0]);
        printf(" Client Side:: %s -c -h ホスト名 -p ポート番号 -u ユーザ名\n",argv[0]);
        return 0;
    }

    while(( result = getopt(argc, argv, "s:ch:p:u:")) != -1){
 		switch (result){            
        case 's':
            port = atoi(optarg);
            option = "s";
            break;
        case 'c':
            option = "c";
            break;
        case 'h': // サーバのIPアドレスまたはホスト名
            srv_ip = optarg;
            break;
        case 'p': // サーバのポート番号
            srv_port = atoi(optarg);
            break;
        case 'u': // ユーザ名
            usr_str = optarg;
            break;
        default:
            printf("error\n");
            return 1;
        }
    }

	init();	

    if(strcmp(option, "s") == 0){
        server(port);
    }else if(strcmp(option, "c") == 0){
        client(srv_port, srv_ip, usr_str);
    }
    return 0;

}
