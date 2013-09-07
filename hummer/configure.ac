#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.59)
AC_INIT(MobileComputationService, 0.1, www.taotaosou.com)
AM_INIT_AUTOMAKE(msc, 0.1)

#AC_ARG_WITH([release],
#	AS_HELP_STRING([--with-release],
#		[use optimize (default is NO)]),
#		[
#		if test "$withval"="yes";then
#			CXXFLAGS="-02 -Wall -


AC_CONFIG_SRCDIR([config.h.in])
AC_CONFIG_HEADER([config.h])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_RANLIB
#AC_PROG_LIBTOOL

# Checks for libraries.
# FIXME: Replace `main' with a function in `-lpthread':
AC_CHECK_LIB([pthread], [pthread_create])

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([limits.h stdlib.h string.h sys/time.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_CONST
AC_C_INLINE
AC_TYPE_SIZE_T
AC_HEADER_TIME
AC_STRUCT_TM
AC_C_VOLATILE

CPPFLAGS="$CPPFLAGS -Wall -Wextra -DMUDUO_STD_STRING -D_REENTRANT -D_GNU_SOURCE -g -ggdb"
# Checks for library functions.
AC_FUNC_ERROR_AT_LINE
AC_CHECK_FUNCS([memset])

AC_CONFIG_FILES([Makefile
                 src/Makefile
                 src/common/Makefile
                 src/common/pubsub/Makefile
                 src/common/log/Makefile
                 src/common/commdef/Makefile
				 src/common/protobuf/Makefile
				 src/configserver/Makefile
				 src/common/hbaseclient/Makefile
				 share/Makefile
				 test/Makefile
				 test/simpletest/Makefile
				 src/core/Makefile])
AC_OUTPUT
