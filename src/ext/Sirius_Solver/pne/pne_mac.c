/*
** Copyright 2007-2018 RTE
** Author: Robert Gonzalez
**
** This file is part of Sirius_Solver.
** This program and the accompanying materials are made available under the
** terms of the Eclipse Public License 2.0 which is available at
** http://www.eclipse.org/legal/epl-2.0.
**
** This Source Code may also be made available under the following Secondary
** Licenses when the conditions for such availability set forth in the Eclipse
** Public License, v. 2.0 are satisfied: GNU General Public License, version 3
** or later, which is available at <http://www.gnu.org/licenses/>.
**
** SPDX-License-Identifier: EPL-2.0 OR GPL-3.0
*/
#include <stdio.h>              /* Standard I/O */
#include <stdlib.h>             /* Standard Library */
#include <errno.h>              /* Error number and related */

#	if defined(__linux) || defined(linux) || defined(__linux__)

/* On ne controle que dans le cas de linux */

#define ENUMS
#include <sys/socket.h>
#include <net/route.h>
#include <net/if.h>
#include <features.h>           /* for the glibc version number */
#if __GLIBC__ >= 2 && __GLIBC_MINOR >= 1
#include <netpacket/packet.h>
#include <net/ethernet.h>       /* the L2 protocols */
#else
#include <asm/types.h>
#include <linux/if_packet.h>
#include <linux/if_ether.h>     /* The L2 protocols */
#endif
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/un.h>
#include <sys/ioctl.h>
#include <netdb.h>

# include "pne_fonctions.h"

# define AUCUN_CONTROLE  0
# define RECUPERATION_ADRESSE_MAC  1
# define CONTROLE_ADRESSE_MAC      2

# define MODE   AUCUN_CONTROLE   

/*----------------------------------------------------------------------------*/

void PNE_ControleMacAdresse( void )
{
int i, j, nb; int sck; char * pt; 
struct ifconf ifc; struct ifreq *ifr;
char buffer[1024]; char buff[256]; char AdresseMac[] = "00:25:90:E4:5A:08";

# if MODE == AUCUN_CONTROLE
  return;
# endif

/* On commence par enumerer les interfaces */
sck = socket(AF_INET, SOCK_STREAM, 0);
if ( sck < 0 ) {
  /* Unable to create socket */
	printf("Unable to detect authorized computer (step 1)\n");
	return;
}

ifc.ifc_len = sizeof(buffer);
ifc.ifc_buf = buffer;
if ( ioctl(sck, SIOCGIFCONF, &ifc) < 0 ) {
	/* Erreur sur ioctl(SIOCGIFCONF) */
	printf("Unable to detect authorized computer (step 2)\n");
}

ifr = ifc.ifc_req;
nb = ifc.ifc_len / sizeof(struct ifreq);
for ( i = 0 ; i < nb ; i++ ) {
  if ( ioctl(sck, SIOCGIFHWADDR, &ifr[i]) < 0 ) {
    /* ioctl(SIOCGIFHWADDR) */
	  printf("Unable to detect authorized computer (step 3)\n");
    return;
	}

  # if MODE == RECUPERATION_ADRESSE_MAC
	  printf("Adresse mac de %s: ", ifr[i].ifr_name);
	# endif
	pt = buff;
  for ( j = 0 ; j < IFHWADDRLEN ; j++ ) {	
    /* printf("%02X", (unsigned int) (unsigned char) (ifr[i].ifr_hwaddr.sa_data[j])); */
		sprintf(pt,"%02X", (unsigned int) (unsigned char) (ifr[i].ifr_hwaddr.sa_data[j]));
		pt += 2;
    if (j != IFHWADDRLEN-1) {		
		  /*putchar(':');*/
		  *pt = ':';
			pt++;
	  }
  }
  # if MODE == RECUPERATION_ADRESSE_MAC
    putchar('\n');
	  printf("%s\n",buff);
	# endif
  # if MODE == CONTROLE_ADRESSE_MAC	
	if ( strcmp( AdresseMac , buff ) == 0 ) {
	  /*printf("Utilisation du solveur permise\n");*/
		return;
	}
	# endif		
}
# if MODE == RECUPERATION_ADRESSE_MAC
  exit(0);
# endif

# if MODE == CONTROLE_ADRESSE_MAC	
  /* On n'a pas trouve l'adresse mac recherchee */
  printf("Please, contact RTE to use this solver for your own purpose\n");
  exit(0);
# endif

return;
}

# else

void PNE_ControleMacAdresse( void )
{
return;
}

# endif
