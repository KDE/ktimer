INCLUDES = $(all_includes)
AM_LDFLAGS	= $(all_libraries) $(KDE_RPATH)
LDADD =  $(LIB_KDEUI) $(LIB_KFILE)

bin_PROGRAMS = ktimer

METASOURCES = AUTO

ktimer_SOURCES =  main.cpp ktimer.cpp prefwidget.ui
# ktimer.cpp pref.cpp
noinst_HEADERS = ktimer.h

KDE_ICON = ktimer

xdg_apps_DATA = ktimer.desktop

messages: rc.cpp
	$(XGETTEXT) *.cpp -o $(podir)/ktimer.pot


srcdoc:
	kdoc -a -p -H -d $$HOME/web/src/ktimer ktimer *.h -lkdecore -lkdeui -lqt


