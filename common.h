#ifndef _COMMON
#define _COMMON
#define SOFTWARE_NAME "MOSH"
#define SOFTWARE_VERSION  "2.0(20161206)"
#define COPYRIGHT_MSG SOFTWARE_NAME " " SOFTWARE_VERSION "\nCopyright (C) 2015 Daisuke Takago(takago@neptune.kanazawa-it.ac.jp)\n                   Yuuki Horie(b1334513@planet.kanazwa-it.ac.jp)"

struct sockaddr_in saddr;
void init();
void ssocket(int port, char *ip_str);

#define MAXDATA 65536

#endif
