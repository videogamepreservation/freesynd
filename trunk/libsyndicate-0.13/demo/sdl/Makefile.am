maintainer-clean-local :
	-rm -f *~

noinst_PROGRAMS = Main

Main_SOURCES = Main.cc Game.cc Game.h Sdl.cc Sdl.h

AM_CXXFLAGS = @SDL_CFLAGS@ -I${top_srcdir}/lib/
LDADD = @SDL_LIBS@ ${top_builddir}/lib/libSyndicate.la
