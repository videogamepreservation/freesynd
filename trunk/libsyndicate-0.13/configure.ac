#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.60)
AC_INIT([libSyndicate], [0.13], [valefor@icculus.org])
AC_CANONICAL_TARGET([]) # for SDL
AM_INIT_AUTOMAKE
AC_CONFIG_SRCDIR([lib/MapData.cc])
AC_CONFIG_HEADER([lib/Syndicate/Data/config.h])

# Checks for parts to compile
AC_ARG_ENABLE([demo],
              [AS_HELP_STRING([--enable-demo], [compile demo [default=no]])],
              [build_demo=${enableval}],
              [build_demo=no])

AC_ARG_ENABLE([example],
              [AS_HELP_STRING([--enable-example], [compile example [default=no]])],
              [build_example=${enableval}],
              [build_example=no])

AC_ARG_ENABLE([doc-latex-pdf],
              [AS_HELP_STRING([--enable-doc-latex-pdf], [compile Latex documentation (pdf output) [default=no]])],
              [build_doc_latex_pdf=${enableval}],
              [build_doc_latex_pdf=no])

AC_ARG_ENABLE([doc-latex-html],
              [AS_HELP_STRING([--enable-doc-latex-html], [compile Latex documentation (html output) [default=no]])],
              [build_doc_latex_html=${enableval}],
              [build_doc_latex_html=no])

AC_ARG_ENABLE([doc-doxygen-html],
              [AS_HELP_STRING([--enable-doc-doxygen-html], [compile Doxygen documentation (html output) [default=no]])],
              [build_doc_doxygen_html=${enableval}],
              [build_doc_doxygen_html=no])

AC_ARG_ENABLE([tool],
              [AS_HELP_STRING([--enable-tool], [compile tool [default=no]])],
              [build_tool=${enableval}],
              [build_tool=no])

AC_ARG_ENABLE([all],
              [AS_HELP_STRING([--enable-all], [alias for --enable-demo --enable-example --enable-doc-latex-pdf --enable-doc-latex-html --enable-doc-doxygen-html --enable-tool [default=no]])],
              [build_all=${enableval}],
              [build_all=no])

AC_ARG_ENABLE([debug],
              [AS_HELP_STRING([--enable-debug], [enable traces [default=no]])],
              [debug=${enableval}],
              [debug=no])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_MAKE_SET
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
AC_PROG_LIBTOOL

#AC_CHECK_PROGS ([syndicate_data_files], [COL01.DAT GAME01.DAT], [notfound], $SYNDICATE_DATA_PATH)
#if test "x$syndicate_data_files" = "xnotfound"
#then
#	AC_MSG_WARN([Data files not found in $SYNDICATE_DATA_PATH])
#fi

if test "x$build_all" = "xyes"
then
	build_demo=yes
	build_example=yes
	build_doc_latex_pdf=yes
	build_doc_latex_html=yes
	build_doc_doxygen_html=yes
	build_tool=yes
fi

if test "x$build_demo" = "xyes" -o "x$build_example" = "xyes"
then
	AM_PATH_SDL(1.0.0, [have_sdl=yes], [have_sdl=no])

	if test "x$have_sdl" != "xyes"
	then
		build_demo=no
		build_example=no
		AC_MSG_WARN([Demo and example can't be build])
	fi
fi	

if test "x$build_doc_latex_pdf" = "xyes" -o "x$build_doc_latex_html" = "xyes" 
then
	AC_PATH_PROG(LATEX, latex, [], $PATH)
	if test "x$LATEX" = "x"; 
	then
		build_doc_latex_pdf=no
		build_doc_latex_html=no
		AC_MSG_WARN([latex not found : latex doc can't be build])
	fi

	AC_PATH_PROG(DIA, dia, [], $PATH)
	if test "x$DIA" = "x"; 
	then
		build_doc_latex_pdf=no
		build_doc_latex_html=no
		AC_MSG_WARN([dia not found : latex doc can't be build])
	fi

	AC_PATH_PROG(CONVERT, convert, [], $PATH)
	if test "x$CONVERT" = "x"; 
	then
		build_doc_latex_pdf=no
		build_doc_latex_html=no
		AC_MSG_WARN([convert not found : latex doc can't be build])
	fi

	if test "x$build_doc_latex_pdf" = "xyes"
	then
		AC_PATH_PROG(PDFLATEX, pdflatex, [], $PATH)
		if test "x$PDFLATEX" = "x"; 
		then
			build_doc_latex_pdf=no
			AC_MSG_WARN([pdflatex not found : latex doc can't be build])
		fi

		AC_PATH_PROG(EPSTOPDF, epstopdf, [], $PATH)
		if test "x$EPSTOPDF" = "x"; 
		then
			build_doc_latex_pdf=no
			AC_MSG_WARN([epstopdf not found : latex doc can't be build])
		fi
	fi

	if test "x$build_doc_latex_html" = "xyes"
	then
		AC_PATH_PROG(HEVEA, hevea, [], $PATH)
		if test "x$HEVEA" = "x"; 
		then
			build_doc_latex_html=no
			AC_MSG_WARN([hevea not found : html output from tex will not be build.])
		fi
	fi
fi

if test "x$build_doc_doxygen_html" = "xyes"
then
	AC_PATH_PROG(DOXYGEN, doxygen, [], $PATH)
	if test "x$DOXYGEN" = "x"; 
	then
		build_doc_doxygen_html=no
		AC_MSG_WARN([doxygen not found : doxygen doc can't be build])
	fi
fi

if test "x$build_tool" = "xyes"
then
	AC_PATH_X
fi

# Checks for libraries.

# Checks for header files.
AC_HEADER_STDC
if test "x$build_tool" = "xyes"
then
	AC_HEADER_SYS_WAIT
fi
AC_CHECK_HEADERS([fcntl.h inttypes.h limits.h stdlib.h string.h sys/time.h unistd.h endian.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_CONST
AC_TYPE_INT16_T
AC_TYPE_INT32_T
AC_TYPE_INT8_T
AC_TYPE_OFF_T
AC_HEADER_TIME
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T
AC_TYPE_UINT8_T
AC_C_BIGENDIAN
if test "x$build_tool" = "xyes"
then
	AC_TYPE_PID_T
	AC_TYPE_SIZE_T
	AC_TYPE_SIGNAL
fi

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_MEMCMP
AC_FUNC_MMAP
AC_FUNC_REALLOC
AC_FUNC_SELECT_ARGTYPES
AC_FUNC_ERROR_AT_LINE
AC_FUNC_VPRINTF
AC_CHECK_FUNCS([atexit gettimeofday memset munmap select strerror])
if test "x$build_tool" = "xyes"
then
	AC_FUNC_STRTOD
	AC_CHECK_FUNCS([strrchr strstr strtoul strtol])
fi

# Set the parts to compile
AM_CONDITIONAL([BUILD_DEMO], [test x$build_demo = xyes])

AM_CONDITIONAL([BUILD_EXAMPLE], [test x$build_example = xyes])

AM_CONDITIONAL([BUILD_DOC_LATEX_PDF], [test x$build_doc_latex_pdf = xyes])

AM_CONDITIONAL([BUILD_DOC_LATEX_HTML], [test x$build_doc_latex_html = xyes])

AM_CONDITIONAL([BUILD_DOC_LATEX], [test x$build_doc_latex_pdf = xyes -o x$build_doc_latex_html = xyes])

AM_CONDITIONAL([BUILD_DOC_DOXYGEN_HTML], [test x$build_doc_doxygen_html = xyes])

AM_CONDITIONAL([BUILD_TOOL], [test x$build_tool = xyes])

if test "x$debug" = "xyes"
then
	AC_DEFINE([DEBUG], [1], [Enable debug traces])
fi

# Define automake constants

DX_ENV='PROJECT=$(PACKAGE_NAME) VERSION=$(PACKAGE_VERSION) OUTPUT_DIR=$(top_builddir)/doc/doxygen INPUT_DIR=$(top_srcdir)/lib/Syndicate STRIP_DIR=$(top_srcdir)/lib '
AC_SUBST(DX_ENV)

# Output
AC_CONFIG_FILES([Makefile])
AC_CONFIG_FILES([lib/Makefile])
AC_CONFIG_FILES([test/Makefile])
AC_CONFIG_FILES([demo/Makefile])
AC_CONFIG_FILES([demo/opengl/Makefile])
AC_CONFIG_FILES([demo/sdl/Makefile])
AC_CONFIG_FILES([example/Makefile])
AC_CONFIG_FILES([doc/Makefile])
AC_CONFIG_FILES([doc/doxygen/Makefile])
AC_CONFIG_FILES([doc/latex/Makefile])
AC_CONFIG_FILES([doc/latex/picts/Makefile])
AC_CONFIG_FILES([tool/Makefile])

AC_OUTPUT

echo "
Configuration :
	Build demo		${build_demo}
	Build example		${build_example}
	Build doc doxygen html	${build_doc_doxygen_html}
	Build doc latex pdf	${build_doc_latex_pdf}
	Build doc latex html	${build_doc_latex_html}
	Build tool		${build_tool}

You should export SYNDICATE_DATA_PATH to point to your syndicate data files.

Now type :
        make to build
	make check to check
"
