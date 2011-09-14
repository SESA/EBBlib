#include <unistd.h>
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

ssize_t ixp_shadowread(int fd, void *buf, size_t count){
	ensure_fdshadow_init();
	void *recvbuf;
	net_handle *hnd = lookupkey(&fdshadow, fd);
	if(!hnd) /* not an ethernet socket. use the syscall. */
		return read(fd, buf, count);
	recvbuf = recvWrapper(hnd, &count);
	memcpy(buf, recvbuf, count);
	return count;
}

ssize_t ixp_shadowwrite(int fd, const void *buf, size_t count) {
	ensure_fdshadow_init();
	net_handle *hnd = lookupkey(&fdshadow, fd);
	if (!hnd)
		return write(fd, buf, count);
	sendWrapper(hnd, buf, count);
	return count;
}

void ixp_shadowregister(int fd, void *hnd) {
	ensure_fdshadow_init();
	insertkey(&fdshadow, fd, hnd);
}
