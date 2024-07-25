## Process this file with automake to produce Makefile.in ##

dist_doc_DATA = doc/BUGS doc/LICENSE doc/NOTICE doc/README doc/THANKS doc/TODO \
		man/send2.1 man/btl.1

SUBDIRS = aux/src
DIST_SUBDIRS = $(SUBDIRS)

#TEST_SRC = $(wildcard $(top_srcdir)/t/*.t)
#TEST_OBJ = $(TEST_SRC:.t=.bats)

all-local:

anew: clean all

%.bats : %.t
	$(GPP) $< > $@

clean-local: tidy

tidy:
	$(RM) config.h~
	$(RM) config.h.in~
	$(RM) configure.scan
