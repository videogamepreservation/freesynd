lib_LTLIBRARIES = libSyndicate.la

nobase_include_HEADERS = \
	Syndicate/Data/Bitstream.h \
	Syndicate/Data/Block.h \
	Syndicate/Data/Colors.h \
	Syndicate/Data/Debug.h \
	Syndicate/Data/File.h \
	Syndicate/Data/Font.h \
	Syndicate/Data/Game.h \
	Syndicate/Data/HuffmanTable.h \
	Syndicate/Data/Int.h \
	Syndicate/Data/MapColumn.h \
	Syndicate/Data/MapData.h \
	Syndicate/Data/MapSubTile.h \
	Syndicate/Data/MapTile.h \
	Syndicate/Data/Palette.h \
	Syndicate/Data/Raw.h \
	Syndicate/Data/Req.h \
	Syndicate/Data/Rnc.h \
	Syndicate/Data/SpriteAnim.h \
	Syndicate/Data/SpriteData.h \
	Syndicate/Data/SpriteElement.h \
	Syndicate/Data/SpriteFrame.h \
	Syndicate/Data/SpriteTab.h 

libSyndicate_la_SOURCES = $(nobase_include_HEADERS) \
	Bitstream.cc \
	Block.cc \
	Colors.cc \
	Debug.cc \
	File.cc \
	Font.cc \
	Game.cc \
	HuffmanTable.cc \
	Main.cc \
	MapColumn.cc \
	MapData.cc \
	MapSubTile.cc \
	MapTile.cc \
	Palette.cc \
	Raw.cc \
	Req.cc \
	Rnc.cc \
	SpriteAnim.cc \
	SpriteData.cc \
	SpriteElement.cc \
	SpriteFrame.cc \
	SpriteTab.cc

libSyndicate_la_LDFLAGS = -version-info 0:0:0

maintainer-clean-local :
	-rm -f *~


