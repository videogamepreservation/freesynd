maintainer-clean-local :
	-rm -f *~

if BUILD_DOC_LATEX
  DOC_LATEX=latex
endif

if BUILD_DOC_DOXYGEN_HTML
  DOC_DOXYGEN=doxygen
endif

SUBDIRS = $(DOC_DOXYGEN) $(DOC_LATEX)
