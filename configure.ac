AC_INIT([ServerAuth], [1.1], [])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])
AC_PROG_CC
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])

AC_PROG_CC
LT_INIT
AC_SUBST([LIBTOOL_DEPS])

AC_CHECK_LIB([crypto], [RSA_public_encrypt], [], AC_MSG_ERROR([*** libcrypto not found.]))

AC_CONFIG_FILES([
  Makefile
  src/Makefile
  include/Makefile
])

AC_OUTPUT
