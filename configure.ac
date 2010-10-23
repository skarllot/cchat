#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.54])
AC_INIT([cchat], [0.1.0], [skarllot@gmail.com])
AC_CONFIG_SRCDIR([src])
AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_HEADERS([config.h])
AM_INIT_AUTOMAKE([1.9 -Wall -Werror])
LT_PREREQ([2.2.6])
LT_INIT

# Check build environment
AC_CANONICAL_HOST

# Checks for programs.
AC_PROG_CC

# Checks for libraries.

# ------------------------------------------------------------------------------
# Check OS-specific platform
# ------------------------------------------------------------------------------
AC_MSG_CHECKING([for native Win32])
case "$host_os" in
  *mingw*)
    iswin32=yes
    AC_DEFINE(WIN32_LEAN_AND_MEAN, 1, [Define for faster code generation.])
    ;;
  *)
    iswin32=no
    ;;
esac
AC_MSG_RESULT([$iswin32])

AM_CONDITIONAL(OS_WIN32, [test "$iswin32" = "yes"])

# Checks for header files.

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.

AC_CONFIG_FILES([Makefile
		 tests/Makefile
		 tests/list/Makefile
		 tests/string/Makefile
		 src/Makefile
		 src/client/Makefile
		 src/common/Makefile
		 src/server/Makefile])
AC_OUTPUT
