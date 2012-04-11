/*
 * Copyright (C) 2011 by Project SESA, Boston University
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <lrt/ulnx/io.h>

typedef int FDType;
typedef int PortType;

enum {LISTENQ = 1024};

static int
net_listen(FDType fd) 
{
  if (listen(fd, LISTENQ) < 0) {
    fprintf(stderr, "Error: EventListen listen failed (%d)\n", errno);
    return -1;
  }
  return 1;
}

static FDType
net_accept(FDType fd)
{
  FDType nfd;


  nfd=accept(fd, NULL, NULL);

#ifdef __APPLE__
  // might need this on OSX to avoid sigpipe on read and write
  // rather than using gobal ignore of sigpipe 
  {
    int i=1;
    if (nfd>=0) {
      setsockopt(nfd, SOL_SOCKET, SO_NOSIGPIPE, (void *)&i, sizeof(int));
    }
  }
#endif

  return nfd;
}

static int
net_setup_listen_socket(FDType *fd, PortType *port)
{
    struct sockaddr_in name;
    int one = 1;
    FDType fd_listen;
    socklen_t addrLen = sizeof(struct sockaddr);
    int rc;

    fd_listen = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (fd_listen < 0) {
        fprintf(stderr, "Error: server socket create failed (%d)\n", errno);
        exit(1);
    }

    rc = setsockopt(fd_listen, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one));
    if (rc != 0) {
        fprintf(stderr, "setsockopt(SO_REUSEADDR): %s\n", strerror(errno));
        exit(1);
    }

    bzero(&name, sizeof(name));
    name.sin_family      = AF_INET;
    name.sin_addr.s_addr = htonl(INADDR_ANY);
    if (*port) name.sin_port = htons(*port);
    else name.sin_port = 0;

    rc = bind(fd_listen, (struct sockaddr *)&name, addrLen);
    if (rc < 0) {
        fprintf(stderr, "Error: server bind failed (%d)\n", errno);
        close (fd_listen);
        exit(1);
    }

    if (*port == 0) {
        rc = getsockname(fd_listen, (struct sockaddr *)&name, &addrLen);
        if (rc < 0) {
            fprintf(stderr, "Error server getsocketname failed (%d)\n", errno);
            close(fd_listen);
            exit(1);
        }
        *port = ntohs(name.sin_port);
    }
    *fd = fd_listen;
    return 1;
}

static int
net_setup_connection(FDType *fd, char *host, PortType port)
{
    int rc;
    struct sockaddr_in localAddr, servAddr;
    struct hostent *h;

    h = gethostbyname(host);
    if (h == NULL) {
        fprintf(stderr, "unknown host '%s'\n", host);
        return -1;
    }

    servAddr.sin_family = h->h_addrtype;
    memcpy((char *) &servAddr.sin_addr.s_addr, h->h_addr_list[0], h->h_length);
    servAddr.sin_port = htons(port);

    *fd = socket(AF_INET, SOCK_STREAM, 0);
    if (*fd<0) {
        fprintf(stderr, "unable to create socket");
        perror("oops ");
        return -1;
    }

    localAddr.sin_family = AF_INET;
    localAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    localAddr.sin_port = htons(0);

    rc = bind(*fd, (struct sockaddr *)&localAddr, sizeof(localAddr));
    if (rc<0) {
        fprintf(stderr, "cannot bind port TCP %u\n", port);
        perror("oops ");
        return -1;
    }

    rc = connect(*fd, (struct sockaddr *) &servAddr, sizeof(servAddr));
    if (rc<0) {
        perror("cannot connect ");
        return -1;
    }
    return 1;
}

/* FOLLOWING CODE COMES FROM THE UNP BOOK */

/* Write "n" bytes to a descriptor. */
static ssize_t		
net_writen(FDType fd, const void *vptr, size_t n)
{
  size_t		nleft;
  ssize_t		nwritten;
  const char	*ptr;
  
  ptr = vptr;
  nleft = n;
  while (nleft > 0) {
#ifndef __APPLE__
    if ( (nwritten = send(fd, ptr, nleft, MSG_NOSIGNAL)) <= 0) {
#else
    // MIGHT NEED THIS VERSION ON OSX  : SIGPIPE IGNORED VIA 
    // SOCKOPT SO_NOSIGPIPE
    if ( (nwritten = write(fd, ptr, nleft)) <= 0) {
#endif
      if (nwritten < 0 && errno == EINTR)
	nwritten = 0;	/* and call write() again */
      else
	return(-1);	/* error */
    }
    
    nleft -= nwritten;
    ptr   += nwritten;
  }
  return(n);
}

/* Read "n" bytes from a descriptor. */
static ssize_t    
net_readn(FDType fd, void *vptr, size_t n)
{
  size_t  nleft;
  ssize_t nread;
  char    *ptr;

  ptr = vptr;
  nleft = n;
  while (nleft > 0) {
#ifndef __APPLE__
    if ( (nread = recv(fd, ptr, nleft, MSG_NOSIGNAL)) < 0) {
#else
    // MIGHT NEED THIS VERSION ON OSX  : SIGPIPE IGNORED VIA 
    // SOCKOPT SO_NOSIGPIPE
    if ( (nread = read(fd, ptr, nleft)) < 0) {
#endif
      if (errno == EINTR)
	nread = 0;              /* and call read() again */
      else
	return(-1);
    } else if (nread == 0)
      break;                          /* EOF */

    nleft -= nread;
    ptr   += nread;
  }
  return(n - nleft);              /* return >= 0 */
}
/* end readn */

/* for the moment assume we can simply open the BootInfo from the local file system */
/* otherwise use the above to read in from socket */
int
lrt_getBootInfoPage(char *page, int *size)
{
  int fd;

  fd = open("BootInfo", O_RDONLY);
  if (fd == -1) return 0;
  *size=read(fd, page, *size);
  close(fd);

  return *size;
}
