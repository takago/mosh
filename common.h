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

#ifndef _COMMON
#   define _COMMON
#   define SOFTWARE_NAME "MOSH"
#   define SOFTWARE_VERSION  "2.2(20170120)"
#   define COPYRIGHT_MSG SOFTWARE_NAME " " SOFTWARE_VERSION "\nCopyright (C) 2015 Daisuke Takago(takago@neptune.kanazawa-it.ac.jp)\n                   Yuuki Horie(b1334513@planet.kanazwa-it.ac.jp)"

struct sockaddr_in saddr;
void init();
void ssocket(int port, char *ip_str);
void SetTerminal(int fd);

#   define MAXDATA 65536

#endif
