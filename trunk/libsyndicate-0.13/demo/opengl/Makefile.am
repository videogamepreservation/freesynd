maintainer-clean-local :
	-rm -f *~

noinst_PROGRAMS = Main

Main_SOURCES = Main.cc Data.cc Data.h Sight.cc Sight.h Tiles.cc Tiles.h

AM_CXXFLAGS = @SDL_CFLAGS@ -I${top_srcdir}/lib/
LDADD = @SDL_LIBS@ ${top_builddir}/lib/libSyndicate.la -lGL -lGLU
