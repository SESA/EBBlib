#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.61)
AC_INIT([EBBLib],[0.1],[sesa@lists.xstack.org])
AC_CONFIG_AUX_DIR([build-aux])
AM_INIT_AUTOMAKE([foreign subdir-objects -Wall -Werror])
AC_CONFIG_SRCDIR([l0/l0_start.c])
AC_CONFIG_HEADER([config.h])

AC_CANONICAL_HOST
AS_CASE([$host_cpu], [x86_64], [], [powerpc], [], [AC_MSG_ERROR([not on a supported architecture])])
AM_CONDITIONAL([x86_64], [test x$host_cpu == xx86_64])
AM_CONDITIONAL([powerpc], [test x$host_cpu == xpowerpc])

AC_ARG_ENABLE([baremetal],
  AS_HELP_STRING([--enable-baremetal], [Enable baremetal build]),
  [lrt=bare],
  [lrt=ulnx])

AM_CONDITIONAL(LRT_BARE, test x$lrt = xbare)
AM_CONDITIONAL(LRT_ULNX, test x$lrt = xulnx)


# Checks for programs.
AC_PROG_CC
AM_PROG_CC_C_O
AC_PROG_CXX
AM_PROG_AS
AC_PROG_RANLIB

# Checks for libraries.

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([arpa/inet.h fcntl.h netdb.h netinet/in.h stdlib.h string.h sys/socket.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_C_INLINE
AC_TYPE_SIZE_T
AC_TYPE_SSIZE_T
AC_C_VOLATILE

AC_TYPE_INT8_T
AC_TYPE_INT16_T
AC_TYPE_INT32_T
AC_TYPE_INT64_T
AC_TYPE_INTMAX_T
AC_TYPE_INTPTR_T

AC_TYPE_UINT8_T
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T
AC_TYPE_UINT64_T
AC_TYPE_UINTMAX_T
AC_TYPE_UINTPTR_T


# Checks for library functions.
AS_IF([test x$lrt = xulnx],
	    [AC_SEARCH_LIBS([pthread_create],
	      [pthread],
	      [],
	      [AC_MSG_ERROR([pthread library missing])])])
AS_IF([test x$lrt = xulnx],
	    [AC_SEARCH_LIBS([pcap_freealldevs],
	      [pcap],
	      [],
	      [AC_MSG_ERROR([pcap library missing])])])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
