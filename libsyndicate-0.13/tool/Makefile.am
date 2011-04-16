
maintainer-clean-local :
	-rm -f *~ *.BIN *.DAT *.txt *.xpm

EXTRA_DIST = README difference

noinst_PROGRAMS = MemshotFe

MemshotFe_SOURCES = MemshotFe.cc Memshot.cc Memshot.h Screenshot.cc Screenshot.h

##AM_CXXFLAGS = -I/usr/X11R6/include/ 
##-L/usr/X11R6/lib/ 
LDADD = -lX11 -lXmu -lXpm
