dist_noinst_DATA=Doxyfile

all-local : html

html-local : html/index.html

html/index.html : Doxyfile
	$(DX_ENV) $(DOXYGEN) $(srcdir)/Doxyfile 

install-data-local : html-local
	test -z "$(docdir)/html" || $(mkdir_p) "$(DESTDIR)$(docdir)/html"
	$(INSTALL_DATA) html/* $(DESTDIR)$(docdir)/html/

uninstall-local :
	-rm -fr $(DESTDIR)$(docdir)/html

clean-local :
	-rm -fr html

maintainer-clean-local :
	-rm -f *~


