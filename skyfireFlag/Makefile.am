lib_LTLIBRARIES = skyfireFlag.la

skyfireFlag_la_SOURCES = skyfireFlag.cpp
skyfireFlag_la_CPPFLAGS= -I$(top_srcdir)/include -I$(top_srcdir)/plugins/plugin_utils
skyfireFlag_la_LDFLAGS = -module -avoid-version -shared
skyfireFlag_la_LIBADD = $(top_builddir)/plugins/plugin_utils/libplugin_utils.la

AM_CPPFLAGS = $(CONF_CPPFLAGS)
AM_CFLAGS = $(CONF_CFLAGS)
AM_CXXFLAGS = $(CONF_CXXFLAGS)

EXTRA_DIST = \
	README.skyfireFlag.txt \
	skyfireFlag.sln \
	skyfireFlag.vcxproj	\
	skyfireFlag.vcxproj.filters

MAINTAINERCLEANFILES =	\
	Makefile.in
