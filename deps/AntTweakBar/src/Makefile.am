noinst_LTLIBRARIES = \
	libanttweakbar.la

noinst_HEADERS = \
	AntPerfTimer.h \
	LoadOGL.h \
	TwBar.h \
	TwColors.h \
	TwFonts.h \
	TwGraph.h \
	TwMgr.h \
	TwOpenGL.h \
	TwPrecomp.h \
	resource.h

libanttweakbar_la_SOURCES = \
	LoadOGL.cpp \
	TwBar.cpp \
	TwColors.cpp \
	TwEventSDL2.c \
	TwFonts.cpp \
	TwMgr.cpp \
	TwOpenGL.cpp

libanttweakbar_la_CXXFLAGS = \
	-I../include \
	@BASE_CXXFLAGS@ \
	@ANTTWEAKBAR_CXXFLAGS@ \
	@OPENGL_CFLAGS@ \
	@SDL2_CFLAGS@

libanttweakbar_la_CFLAGS = \
	-I../include \
	@BASE_CFLAGS@ \
	@ANTTWEAKBAR_CFLAGS@ \
	@OPENGL_CFLAGS@ \
	@SDL2_CFLAGS@

libanttweakbar_la_LIBADD = \
	@BASE_LIBS@ \
	@ANTTWEAKBAR_LIBS@ \
	@OPENGL_LIBS@ \
	@SDL2_LIBS@ \
	-lstdc++
