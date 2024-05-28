AUTOMAKE_OPTIONS = foreign
SUBDIRS = src

bin_PROGRAMS = ecm
ecm_SOURCES = src/ecm.c src/encode.c src/decode.c
ecm_CFLAGS = -I$(top_srcdir)/include

EXTRA_DIST = unecm
CLEANFILES = unecm
