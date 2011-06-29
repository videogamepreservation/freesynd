#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.61])
AC_INIT([freesynd], [0.6], [freesynd-user@lists.sourceforge.net])
AM_INIT_AUTOMAKE
AC_CONFIG_SRCDIR([src/freesynd.cpp])

# to find OS X's png.h without configure arguments:
CPPFLAGS="$CPPFLAGS -I/usr/X11/include"

# Checks for programs.
AC_PROG_CXX

SDL_VERSION=1.2.0
AM_PATH_SDL($SDL_VERSION,
            :,
            AC_MSG_ERROR([*** SDL version $SDL_VERSION not found!])
)

AC_CHECK_HEADER(SDL/SDL_image.h,
                :,
                AC_MSG_ERROR([*** SDL_image not found!])
)

AC_CHECK_HEADER(SDL/SDL_mixer.h,
                :,
                AC_MSG_ERROR([*** SDL_mixer not found!])
)

AC_CHECK_HEADER(png.h,
                :,
                AC_MSG_ERROR([*** PNG header not found!])
)

AC_CONFIG_FILES([Makefile
                 src/Makefile])
AC_OUTPUT
