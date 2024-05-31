SUBDIRS = src

bin_SCRIPTS = unecm

unecm: src/ecm
	cat unecm.in > unecm
	chmod a+x unecm

EXTRA_DIST = unecm.in
CLEANFILES = unecm *.snap
