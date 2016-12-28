AM_CPPFLAGS = -I$(top_srcdir)/include/
AM_CFLAGS = ${SERVERAUTH_CFLAGS} ${libssl_CFLAGS} ${libcrypto_CFLAGS}
ACLOCAL_AMFLAGS='-I m4'
bin_PROGRAMS = authserver
authserver_SOURCES = authencrypt.c hashcheck.c serverauth.c server.c httpencrypt.c encode.c
