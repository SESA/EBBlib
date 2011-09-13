/* Copyright ©2007-2008 Kris Maglione <fbsdaemon@gmail.com>
 * Copyright ©2004-2006 Anselm R. Garbe <garbeam at gmail dot com>
 * See LICENSE file for license details.
 */
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include "ixp_local.h"
#include "raweth.h"

/* Note: These functions modify the strings that they are passed.
 *   The lookup function duplicates the original string, so it is
 *   not modified.
 */

/* From FreeBSD's sys/su.h */
#define SUN_LEN(su) \
	(sizeof(*(su)) - sizeof((su)->sun_path) + strlen((su)->sun_path))

typedef struct addrinfo addrinfo;
typedef struct sockaddr sockaddr;
typedef struct sockaddr_un sockaddr_un;
typedef struct sockaddr_in sockaddr_in;

static char*
get_port(char *addr) {
	char *s;

	s = strchr(addr, '!');
	if(s == nil) {
		werrstr("no port provided");
		return nil;
	}

	*s++ = '\0';
	if(*s == '\0') {
		werrstr("invalid port number");
		return nil;
	}
	return s;
}

/* converts c (which must be a hexidecimal digit ([0-9A-Za-z]) or the results
 * are undefined) to it's value in as a number. */
static unsigned char
xdigit(char c){
	if (c <= '9' && c >= '0')
		return c - '0';
	else if (c <= 'f' && c >= 'a')
		return c - 'a' + 10;
	else
		return c - 'A' + 10;
}

/* dest must have length ETH_ALEN. read_mac then parses address as a mac address according to these rules:
 * 1. colons are skipped.
 * 2. if a non-hexidecimal character is encountered (other than colon) it is
 * an error.
 * 3. the rest of the characters are interpreted as hexidecimal digits in the
 * mac address.
 * 
 * in the event of an error, read_mac will return -1, otherwise it will return
 * 0, and dest will be filled out as initSend expects. (see eth.c included
 * above)
 */ 
static int
read_mac(char *address, unsigned char *dest){
	int i;
	for (i = 0; i < ETH_ALEN; i++) {
		while(*address == ':')
			address++;
		if(!isxdigit(*address))
			return -1;
		dest[i] = xdigit(*address++) * 16;
		if(!isxdigit(*address))
			return -1;
		dest[i] = xdigit(*address++);
	}
}

static int
dial_eth(char *address) {
	unsigned char dest_mac[ETH_ALEN];
	net_handle *handle;
	char *macstr = strchr(address, '!');
	if(macstr == NULL)
		return -1;
	*macstr++ = '\0';
	if(read_mac(macstr, dest_mac) == -1)
		return -1;
	handle = emalloc(sizeof(net_handle));
	initSend(handle, address, dest_mac);
	ixp_shadowregister(handle->fd, handle);
	return handle->fd;
}

static int
announce_eth(char *address){
	net_handle *handle = emalloc(sizeof(net_handle));
	initRecv(handle, address);
	ixp_shadowregister(handle->fd, handle);
	return handle->fd;
}

static int
sock_unix(char *address, sockaddr_un *sa, socklen_t *salen) {
	int fd;

	memset(sa, 0, sizeof *sa);

	sa->sun_family = AF_UNIX;
	strncpy(sa->sun_path, address, sizeof sa->sun_path);
	*salen = SUN_LEN(sa);

	fd = socket(AF_UNIX, SOCK_STREAM, 0);
	if(fd < 0)
		return -1;
	return fd;
}

static int
dial_unix(char *address) {
	sockaddr_un sa;
	socklen_t salen;
	int fd;

	fd = sock_unix(address, &sa, &salen);
	if(fd == -1)
		return fd;

	if(connect(fd, (sockaddr*) &sa, salen)) {
		close(fd);
		return -1;
	}
	return fd;
}

static int
announce_unix(char *file) {
	const int yes = 1;
	sockaddr_un sa;
	socklen_t salen;
	int fd;

	signal(SIGPIPE, SIG_IGN);

	fd = sock_unix(file, &sa, &salen);
	if(fd == -1)
		return fd;

	if(setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (void*)&yes, sizeof yes) < 0)
		goto fail;

	unlink(file);
	if(bind(fd, (sockaddr*)&sa, salen) < 0)
		goto fail;

	chmod(file, S_IRWXU);
	if(listen(fd, IXP_MAX_CACHE) < 0)
		goto fail;

	return fd;

fail:
	close(fd);
	return -1;
}

static addrinfo*
alookup(char *host, int announce) {
	addrinfo hints, *ret;
	char *port;
	int err;

	/* Truncates host at '!' */
	port = get_port(host);
	if(port == nil)
		return nil;

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;

	if(announce) {
		hints.ai_flags = AI_PASSIVE;
		if(!strcmp(host, "*"))
			host = nil;
	}

	err = getaddrinfo(host, port, &hints, &ret);
	if(err) {
		werrstr("getaddrinfo: %s", gai_strerror(err));
		return nil;
	}
	return ret;
}

static int
ai_socket(addrinfo *ai) {
	return socket(ai->ai_family, ai->ai_socktype, ai->ai_protocol);
}

static int
dial_tcp(char *host) {
	addrinfo *ai, *aip;
	int fd;

	aip = alookup(host, 0);
	if(aip == nil)
		return -1;

	SET(fd);
	for(ai = aip; ai; ai = ai->ai_next) {
		fd = ai_socket(ai);
		if(fd == -1) {
			werrstr("socket: %s", strerror(errno));
			continue;
		}

		if(connect(fd, ai->ai_addr, ai->ai_addrlen) == 0)
			break;

		werrstr("connect: %s", strerror(errno));
		close(fd);
		fd = -1;
	}

	freeaddrinfo(aip);
	return fd;
}

static int
announce_tcp(char *host) {
	addrinfo *ai, *aip;
	int fd;

	aip = alookup(host, 1);
	if(aip == nil)
		return -1;

	/* Probably don't need to loop */
	SET(fd);
	for(ai = aip; ai; ai = ai->ai_next) {
		fd = ai_socket(ai);
		if(fd == -1)
			continue;

		if(bind(fd, ai->ai_addr, ai->ai_addrlen) < 0)
			goto fail;

		if(listen(fd, IXP_MAX_CACHE) < 0)
			goto fail;
		break;
	fail:
		close(fd);
		fd = -1;
	}

	freeaddrinfo(aip);
	return fd;
}

typedef struct addrtab addrtab;
static
struct addrtab {
	char *type;
	int (*fn)(char*);
} dtab[] = {
	{"tcp", dial_tcp},
	{"unix", dial_unix},
	{"eth", dial_eth},
	{0, 0}
}, atab[] = {
	{"tcp", announce_tcp},
	{"unix", announce_unix},
	{"eth", announce_eth},
	{0, 0}
};

static int
lookup(const char *address, addrtab *tab) {
	char *addr, *type;
	int ret;

	ret = -1;
	type = estrdup(address);

	addr = strchr(type, '!');
	if(addr == nil)
		werrstr("no address type defined");
	else {
		*addr++ = '\0';
		for(; tab->type; tab++)
			if(strcmp(tab->type, type) == 0) break;
		if(tab->type == nil)
			werrstr("unsupported address type");
		else
			ret = tab->fn(addr);
	}

	free(type);
	return ret;
}

/**
 * Function: ixp_dial
 * Function: ixp_announce
 *
 * Params:
 *	address - An address on which to connect or listen,
 *		  specified in the Plan 9 resources
 *		  specification format
 *		  (<protocol>!address[!<port>])
 *
 * These functions hide some of the ugliness of Berkely
 * Sockets. ixp_dial connects to the resource at P<address>,
 * while ixp_announce begins listening on P<address>.
 *
 * Returns:
 *	These functions return file descriptors on success,
 * and -1 on failure. ixp_errbuf(3) may be inspected on
 * failure.
 */

int
ixp_dial(const char *address) {
	return lookup(address, dtab);
}

int
ixp_announce(const char *address) {
	return lookup(address, atab);
}

