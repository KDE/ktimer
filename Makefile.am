INCLUDES = $(all_includes)
LDFLAGS	= $(all_libraries)
LDADD =  $(LIB_KDEUI) $(LIB_KFILE)

bin_PROGRAMS = ktimer

METASOURCES = AUTO

ktimer_SOURCES =  main.cpp ktimer.cpp prefwidget.ui
# ktimer.cpp pref.cpp
noinst_HEADERS = ktimer.h

KDE_ICON = ktimer AUTO

appsdir = $(kde_appsdir)/Utilities

install-data-local:
	$(mkinstalldirs) $(appsdir)
	$(INSTALL_DATA) $(srcdir)/support/ktimer.desktop $(appsdir)

uninstall-local:
	rm -f $(kde_appsdir)/ktimer.kdesktop

messages: rc.cpp
	$(XGETTEXT) *.cpp -o $(podir)/ktimer.pot


srcdoc:
	kdoc -a -p -H -d $(HOME)/web/src/ktimer ktimer *.h -lkdecore -lkdeui -lqt


