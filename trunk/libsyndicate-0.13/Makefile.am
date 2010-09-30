DISTCHECK_CONFIGURE_FLAGS = --enable-all

if BUILD_EXAMPLE
  EXAMPLE=example
endif

if BUILD_TOOL
  TOOL=tool
endif

if BUILD_DEMO
  DEMO=demo
endif

SUBDIRS = lib $(DEMO) doc $(EXAMPLE) test $(TOOL)

dist_doc_DATA = README
EXTRA_DIST = bootstrap

clean-local :
	-rm -f *.tar.gz

maintainer-clean-local :
	-rm -f *~ .*~ 

