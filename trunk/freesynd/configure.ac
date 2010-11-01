#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.65])
AC_INIT([freesynd], [0.5], [freesynd-user@lists.sourceforge.net])
AM_INIT_AUTOMAKE
AC_CONFIG_SRCDIR([src/freesynd.cpp])

# Checks for programs.
AC_PROG_CXX

SDL_VERSION=1.2.0
AM_PATH_SDL($SDL_VERSION,
            :,
            AC_MSG_ERROR([*** SDL version $SDL_VERSION not found!])
)

AC_CONFIG_FILES([Makefile
                 src/Makefile])
AC_OUTPUT
