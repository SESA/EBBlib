#include <config.h>
#include <stdint.h>

#include <l0/lrt/types.h>
#include <l0/cobj/cobj.h>
#include <lrt/io.h>
#include <l0/lrt/trans.h>
#include <l0/types.h>
#include <l0/sys/trans.h>
#include <lrt/assert.h>

#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <termios.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <signal.h>

/* FROM : Advanced Programming in the UNIX Environment, Addison-Wesley,
   1992, ISBN 0-201-56317-7
   http://www.kohala.com/start/apue.linux.tar.Z
*/

static struct termios	save_termios;
static int				ttysavefd = -1;
static enum { RESET, RAW, CBREAK }	ttystate = RESET;

static int
tty_cbreak(int fd)	/* put terminal into a cbreak mode */
{
	struct termios	buf;

	if (tcgetattr(fd, &save_termios) < 0)
		return(-1);

	buf = save_termios;	/* structure copy */

	buf.c_lflag &= ~(ECHO | ICANON);
					/* echo off, canonical mode off */

	buf.c_cc[VMIN] = 1;	/* Case B: 1 byte at a time, no timer */
	buf.c_cc[VTIME] = 0;

	if (tcsetattr(fd, TCSAFLUSH, &buf) < 0)
		return(-1);
	ttystate = CBREAK;
	ttysavefd = fd;
	return(0);
}

static int tty_raw(int fd) __attribute__ ((unused));
static int
tty_raw(int fd)		/* put terminal into a raw mode */
{
	struct termios	buf;

	if (tcgetattr(fd, &save_termios) < 0)
		return(-1);

	buf = save_termios;	/* structure copy */

	buf.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
					/* echo off, canonical mode off, extended input
					   processing off, signal chars off */

	buf.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
					/* no SIGINT on BREAK, CR-to-NL off, input parity
					   check off, don't strip 8th bit on input,
					   output flow control off */

	buf.c_cflag &= ~(CSIZE | PARENB);
					/* clear size bits, parity checking off */
	buf.c_cflag |= CS8;
					/* set 8 bits/char */

	buf.c_oflag &= ~(OPOST);
					/* output processing off */

	buf.c_cc[VMIN] = 1;	/* Case B: 1 byte at a time, no timer */
	buf.c_cc[VTIME] = 0;

	if (tcsetattr(fd, TCSAFLUSH, &buf) < 0)
		return(-1);
	ttystate = RAW;
	ttysavefd = fd;
	return(0);
}

static int
tty_reset(int fd)		/* restore terminal's mode */
{
	if (ttystate != CBREAK && ttystate != RAW)
		return(0);

	if (tcsetattr(fd, TCSAFLUSH, &save_termios) < 0)
		return(-1);
	ttystate = RESET;
	return(0);
}

static void
tty_atexit(void)		/* can be set up by atexit(tty_atexit) */
{
	if (ttysavefd >= 0)
		tty_reset(ttysavefd);
}

static struct termios * tty_termios(void) __attribute__ ((unused));

static struct termios *
tty_termios(void)		/* let caller see original tty state */
{
	return(&save_termios);
}

/* modified by Jonathan Appavoo */
static void
sig_catch( int signo )
{
  tty_reset( STDIN_FILENO );
  exit(0);
}

static void
tty_init(int fd) {  

  if ( signal( SIGTERM, sig_catch ) == SIG_ERR ) {
    printf("signal(SIGTERM) error\n");
    exit(1);
  }

  if ( signal( SIGQUIT, sig_catch ) == SIG_ERR ) {
    printf("signal(SIGQUIT) error\n");
    exit(1);
  }

  atexit(tty_atexit);

  tty_cbreak(fd);
}
// back to my test code

EBBRC 
LRTConsoleRead(lrt_pic_src *in, char *buf, int len, int *n)
{
  *n = read(in->unix_pic_src.fd, buf, len);
  return (*n>=0) ? EBBRC_OK : EBBRC_GENERIC_FAILURE;
}

EBBRC 
LRTConsoleWrite(lrt_pic_src *out, char *buf, int len, int *n)
{
  *n = write(out->unix_pic_src.fd, buf, len);
  return (*n>=0) ? EBBRC_OK : EBBRC_GENERIC_FAILURE;
}

EBBRC 
LRTConsoleInit(lrt_pic_src *in, lrt_pic_src *out, lrt_pic_src *err)
{
  int opt = 1;

  tty_init(STDIN_FILENO);

  ioctl(STDIN_FILENO, FIONBIO, &opt);

  // FIXME: JA: A little concerned about nonblock on out and err 
  //            this may have to improve write path
  //            to pay attention to number of bytes and count down
  //             until write is complete
  ioctl(STDOUT_FILENO, FIONBIO, &opt);
  ioctl(STDERR_FILENO, FIONBIO, &opt);

  in->unix_pic_src.fd  = STDIN_FILENO;
  in->unix_pic_src.flags = (LRT_ULNX_PICFLAG_READ | LRT_ULNX_PICFLAG_ERROR);

  out->unix_pic_src.fd = STDOUT_FILENO;
  out->unix_pic_src.flags = (LRT_ULNX_PICFLAG_WRITE | LRT_ULNX_PICFLAG_ERROR);
  
  err->unix_pic_src.fd = STDERR_FILENO;
  err->unix_pic_src.flags = (LRT_ULNX_PICFLAG_WRITE | LRT_ULNX_PICFLAG_ERROR);

  return EBBRC_OK;
}
