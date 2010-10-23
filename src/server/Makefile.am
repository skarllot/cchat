# vim:set ts=8 sw=8 noexpandtab nowrap textwidth=80

AM_CFLAGS = -Wall -I $(top_srcdir)/src

bin_PROGRAMS = cchat-server

cchat_server_SOURCES = \
	main.c \
	chatserver.h \
	chatserver.c

cchat_server_LDADD = \
	$(top_builddir)/src/common/libcchat-common.la
cchat_server_LDFLAGS = -pthread

