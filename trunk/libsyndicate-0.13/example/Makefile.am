
maintainer-clean-local :
	-rm -f *~

noinst_PROGRAMS = \
	main_anim \
	main_file \
	main_font \
	main_frame \
	main_game \
	main_hreq \
	main_map \
	main_mapcol \
	main_mapdata \
	main_maptile \
	main_palette \
	main_raw \
	main_sprite

main_anim_SOURCES = main_anim.cc Animation.cc Animation.h
main_file_SOURCES = main_file.cc
main_font_SOURCES = main_font.cc
main_frame_SOURCES = main_frame.cc Animation.cc Animation.h
main_game_SOURCES = main_game.cc
main_hreq_SOURCES = main_hreq.cc
main_map_SOURCES = main_map.cc Map.cc Map.h
main_mapcol_SOURCES = main_mapcol.cc
main_mapdata_SOURCES = main_mapdata.cc
main_maptile_SOURCES = main_maptile.cc
main_palette_SOURCES = main_palette.cc
main_raw_SOURCES = main_raw.cc
main_sprite_SOURCES = main_sprite.cc

AM_CXXFLAGS = @SDL_CFLAGS@ -I${top_srcdir}/lib/
LDADD = @SDL_LIBS@ ${top_builddir}/lib/libSyndicate.la
