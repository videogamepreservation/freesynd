
maintainer-clean-local :
	-rm -f *~

check_PROGRAMS=TestInt TestBlock TestMapTile TestSpriteTab TestMapData TestPalette TestFont

TestInt_SOURCES = TestInt.cc
TestBlock_SOURCES = TestBlock.cc TestBlock.h
TestMapTile_SOURCES = TestMapTile.cc TestMapTile.h
TestSpriteTab_SOURCES = TestSpriteTab.cc TestSpriteTab.h
TestMapData_SOURCES = TestMapData.cc TestMapData.h
TestPalette_SOURCES = TestPalette.cc TestPalette.h
TestFont_SOURCES = TestFont.cc TestFont.h

TESTS = $(check_PROGRAMS)

AM_CXXFLAGS = -I$(top_srcdir)/lib/
LDADD = $(top_builddir)/lib/libSyndicate.la
