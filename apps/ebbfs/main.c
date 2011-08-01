/* © 2009 JGL (yiyus) <yiyu.jgl@gmail.com>
 * See COPYING for details.
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <err.h>
#include <errno.h>
#include <sys/stat.h>

#include <ixp.h>

#include "ebbfs.h"

extern Ixp9Srv p9srv;
char *message;
char hellomsg[12] = "Hello world!";

#define GETARG() (cp-*argv == strlen(*argv)-1) ? *++argv : cp+1

int
main(int argc, char **argv)
{
	IxpServer srv = {0};
	char *address, *cp, buf[512];
	int fd, i;

	address = getenv("IXP_ADDRESS");
	while(*++argv) {
		if(strcmp(*argv, "--") == 0 || !(**argv == '-')) {
			break;
		}
		for(cp=*argv+1; cp<*argv+strlen(*argv); ++cp) {
			if(*cp == 'a') {
				address = GETARG();
				break;
			} else if(*cp == 'm') {
				message = GETARG();
				break;
			} else {
				errx(1, "usage: hi [ -a ADDRESS ] [-m MESSAGE]");
			}
		}
	}
	if(!address) {
		char *nsdir = ixp_namespace();
		if(mkdir(nsdir, 0700) == -1 && errno != EEXIST) {
			err(1, "mkdir: %s", nsdir);
		}
		snprintf(buf, sizeof(buf), "unix!%s/hi", nsdir);
		address = buf;
	}

	fd = ixp_announce(address);
	if(fd < 0) {
		err(1, "ixp_announce");
	}

	ixp_listen(&srv, fd, &p9srv, serve_9pcon, NULL);

	if(!message)
		message = hellomsg;
	i = ixp_serverloop(&srv);

	return i;
}
