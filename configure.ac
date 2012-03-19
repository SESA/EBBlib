#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.61)
AC_INIT([EBBLib],[0.1],[sesa-developers@googlegroups.com])
AC_CONFIG_AUX_DIR([build-aux])
AM_INIT_AUTOMAKE([foreign subdir-objects -Wall -Werror -Wno-portability])
AC_CONFIG_SRCDIR([l0/l0_start.c])
AC_CONFIG_HEADER([config.h])
#AC_CONFIG_MACRO_DIR([m4])
m4_include([m4/check_gnu_make.m4])

AC_CANONICAL_HOST
AS_CASE([$host_cpu], [x86_64], 
		     [AC_DEFINE([ARCH_AMD64], [1], [x86_64 architecture])],
		     [powerpc], 
		     [AC_DEFINE([ARCH_PPC], [1], [powerpc32 architecture])],
		     [powerpc64], 
		     [AC_DEFINE([ARCH_PPC64], [1], [powerpc64 architecture])], 
		     [AC_MSG_ERROR([not on a supported architecture])])
AM_CONDITIONAL([x86_64], [test x$host_cpu == xx86_64])
AM_CONDITIONAL([powerpc], [test x$host_cpu == xpowerpc])
AM_CONDITIONAL([powerpc64], [test x$host_cpu == xpowerpc64])

AC_ARG_ENABLE([baremetal],
  AS_HELP_STRING([--enable-baremetal], [Enable baremetal build]),
  [lrt=bare],
  [lrt=ulnx])

AS_CASE([$lrt], [bare], [AC_DEFINE([LRT_BARE], [1], [Baremetal LRT])],
		[ulnx], [AC_DEFINE([LRT_ULNX], [1], [Baremetal ULNX])])

AM_CONDITIONAL(LRT_BARE, test x$lrt = xbare)
AM_CONDITIONAL(LRT_ULNX, test x$lrt = xulnx)


# Checks for programs.
AC_PROG_CC
AM_PROG_CC_C_O
AC_PROG_CXX
AM_PROG_AS
AC_PROG_RANLIB
AC_CHECK_GNU_MAKE

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

# C feature checks
AC_LANG_C

ac_save_CFLAGS="$CFLAGS"
CFLAGS="$CFLAGS -std=gnu99"
AC_COMPILE_IFELSE(
	[AC_LANG_PROGRAM([[]],[[;]])],
	[std_gnu_99=yes],
	[AC_MSG_WARN([no --std=gnu99])])
CFLAGS="$ac_save_CFLAGS"
AM_CONDITIONAL(STD_GNU_99, test x$std_gnu_99 = xyes)

ac_save_CFLAGS="$CFLAGS"
CFLAGS="$CFLAGS -std=gnu99"
AC_COMPILE_IFELSE(
	[AC_LANG_PROGRAM([[_Static_assert(1,"");]],[[;]])],
	[cc_static_assert=yes],
	[AC_MSG_WARN([no c static assertions])])
CFLAGS="$ac_save_CFLAGS"
AS_IF([test x$cc_static_assert = xyes], 
	    [AC_DEFINE([HAS_C_STATIC_ASSERT], [1], [can use _Static_assert in C])])

# CXX feature checks
AC_LANG_CPLUSPLUS
ac_save_CXXFLAGS="$CXXFLAGS"
CXXFLAGS="$CXXFLAGS -std=gnu++0x"
AC_COMPILE_IFELSE(
	[AC_LANG_PROGRAM([[]],[[;]])],
	[std_gnuxx_0x=yes],
	[AC_MSG_WARN([no --std=gnu++0x])])
CXXFLAGS="$ac_save_CXXFLAGS"
AM_CONDITIONAL(STD_GNUXX_0x, test x$std_gnuxx_0x = xyes)

ac_save_CXXFLAGS="$CXXFLAGS"
CXXFLAGS="$CXXFLAGS -ffreestanding -Wall -Werror"
AC_COMPILE_IFELSE(
	[AC_LANG_PROGRAM([[]],[[;]])],
	[gxx_ffreestanding=yes],
	[AC_MSG_WARN([no -ffreestanding])])
CXXFLAGS="$ac_save_CXXFLAGS"
AM_CONDITIONAL(CXX_FFREESTANDING, test x$gxx_ffreestanding = xyes)

ac_save_CXXFLAGS="$CXXFLAGS"
CXXFLAGS="$CXXFLAGS -std=gnu++0x"
AC_COMPILE_IFELSE(
	[AC_LANG_PROGRAM([[static_assert(1,"");]],[[;]])],
	[cxx_static_assert=yes],
	[AC_MSG_WARN([no c++ static assertions])])
CXXFLAGS="$ac_save_CXXFLAGS"
AS_IF([test x$cxx_static_assert = xyes], 
	    [AC_DEFINE([HAS_CXX_STATIC_ASSERT], [1], [can use static_assert in C++])])

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

# Optional features
memmgrprim=none
AC_ARG_ENABLE([stupid-prim-alloc],
  AS_HELP_STRING([--enable-stupid-prim-alloc], [use the stupid primitive allocator (disabled by default)]),
  memmgrprim=stupid,)
AC_ARG_ENABLE([simple-prim-alloc],
  AS_HELP_STRING([--enable-simple-prim-alloc], [use the simple primitive allocator (disabled by default)]),
  memmgrprim=simple,)

AM_CONDITIONAL([MemMgrPrimStupid], [test x$memmgrprim = xstupid])
AM_CONDITIONAL([MemMgrPrimSimple], [test x$memmgrprim = xsimple])

AS_IF([test x$memmgrprim = xnone], AC_MSG_ERROR([You must choose exactly one primitive memory allocator.]),)

AC_OUTPUT
