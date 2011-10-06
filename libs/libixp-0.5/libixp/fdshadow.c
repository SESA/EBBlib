#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include "ixp_local.h"
#include "raweth.h"

static Intmap fdshadow;
static int have_fdshadow_init = 0;

#define HASH_SIZE 16

static void ensure_fdshadow_init(void){
	if(have_fdshadow_init)
		return;
	void *hash = emalloc(sizeof(void *) * HASH_SIZE);
	initmap(&fdshadow, HASH_SIZE, hash);
	have_fdshadow_init = 1;
}

int ixp_shadowaccept(int sockfd, struct sockaddr *addr, socklen_t *addrlen) {
	ensure_fdshadow_init();
	EthFD *ethfd = lookupkey(&fdshadow, sockfd);
	if(!ethfd)
		return accept(sockfd, addr, addrlen);
	return sockfd;
}

ssize_t ixp_shadowread(int fd, void *buf, size_t count){
	ensure_fdshadow_init();
	EthFD *ethfd = lookupkey(&fdshadow, fd);
	if(!ethfd) /* not an ethernet socket. use the syscall. */
		return read(fd, buf, count);
	return ethRecv(ethfd, buf, count);
		
}

ssize_t ixp_shadowwrite(int fd, const void *buf, size_t count) {
	ensure_fdshadow_init();
	EthFD *ethfd = lookupkey(&fdshadow, fd);
	if (!ethfd)
		return write(fd, buf, count);
	return ethSend(ethfd, buf, count);
}

void ixp_shadowregister(int fd, void *hnd) {
	ensure_fdshadow_init();
	EthFD *ethfd = EthFD_init(hnd);
	insertkey(&fdshadow, fd, ethfd);
}
