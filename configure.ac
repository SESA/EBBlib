#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.61)
AC_INIT([EBBLib],[0.1],[sesa@lists.xstack.org])
AC_CONFIG_AUX_DIR([build-aux])
AM_INIT_AUTOMAKE([foreign subdir-objects -Wall -Werror ])
AC_CONFIG_SRCDIR([EBBStart.c])
AC_CONFIG_HEADER([config.h])

AC_DEFINE(LRT_ULNX, [], [Set to indicate that we are running the userland Linux low-level runtime])

# Checks for programs.
AC_PROG_CC
AM_PROG_CC_C_O
AM_PROG_AS

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
AC_SEARCH_LIBS([pthread_create], [pthread], [],
				 [AC_MSG_FAILURE([Pthread library missing])])
AC_SEARCH_LIBS([pcap_freealldevs], [pcap], [],
				 [AC_MSG_FAILURE([Pcap library missing])])
AC_CHECK_FUNCS([bzero dup2 gethostbyname memset socket strerror])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
