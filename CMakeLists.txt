kde4_header()
project(ktimer)

include_directories( ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}  )


########### next target ###############

set(ktimer_SRCS main.cpp ktimer.cpp )

kde4_automoc(${ktimer_SRCS})

kde4_add_ui_files(ktimer_SRCS prefwidget.ui )

kde4_add_executable(ktimer ${ktimer_SRCS})

target_link_libraries(ktimer  ${LIB_KIO} )

install_targets(/bin ktimer )


########### install files ###############

install_files( ${XDG_APPS_DIR} FILES ktimer.desktop )

kde4_install_icons( ${ICON_INSTALL_DIR}  hicolor )

kde4_footer()



#original Makefile.am contents follow:

#INCLUDES = $(all_includes)
#AM_LDFLAGS	= $(all_libraries) $(KDE_RPATH)
#LDADD =  $(LIB_KDEUI) $(LIB_KFILE)
#
#bin_PROGRAMS = ktimer
#
#METASOURCES = AUTO
#
#ktimer_SOURCES =  main.cpp ktimer.cpp prefwidget.ui
## ktimer.cpp pref.cpp
#noinst_HEADERS = ktimer.h
#
#KDE_ICON = ktimer
#
#xdg_apps_DATA = ktimer.desktop
#
#messages: rc.cpp
#	$(XGETTEXT) *.cpp -o $(podir)/ktimer.pot
#
#
#srcdoc:
#	kdoc -a -p -H -d $$HOME/web/src/ktimer ktimer *.h -lkdecore -lkdeui -lqt
#
#
