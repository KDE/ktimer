INCLUDES = $(all_includes)
LDFLAGS	= $(all_libraries)
LDADD =  $(LIB_KDEUI) $(LIB_KFILE)

bin_PROGRAMS = ktimer

METASOURCES = AUTO

ktimer_SOURCES =  main.cpp ktimer.cpp prefwidget.ui
# ktimer.cpp pref.cpp
noinst_HEADERS = ktimer.h

KDE_ICON = AUTO

appsdir = $(kde_appsdir)/Utilities/More
apps_DATA = ktimer.desktop

install-data-local: uninstall.desktop
	$(mkinstalldirs) $(DESTDIR)$(kde_appsdir)/Utilities
	$(INSTALL_DATA) $(srcdir)/uninstall.desktop $(DESTDIR)$(kde_appsdir)/Utilities/ktimer.desktop

messages: rc.cpp
	$(XGETTEXT) *.cpp -o $(podir)/ktimer.pot


srcdoc:
	kdoc -a -p -H -d $(HOME)/web/src/ktimer ktimer *.h -lkdecore -lkdeui -lqt


