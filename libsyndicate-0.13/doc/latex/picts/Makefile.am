dist_noinst_DATA = \
	src/Tile.dia \
	src/Menus.dia \
	src/Map.dia \
	src/Animation.dia \
	src/SyndicateCover.png

VPATH=$(srcdir):$(srcdir)/src

%.pdf : %.eps
	$(EPSTOPDF) $< > $@

%.eps : %.dia
	$(DIA) --export=$@ $<

%.eps : %.png
	$(CONVERT) $< $@ 

clean-local :
	-rm -f *.eps *.pdf

maintainer-clean-local :
	-rm -f *~ src/*~

