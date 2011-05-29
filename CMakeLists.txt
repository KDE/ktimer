project(ktimer)

find_package(KDE4)
include( KDE4Defaults )
include_directories(${KDE4_INCLUDES})

set(ktimer_SRCS main.cpp ktimer.cpp )

kde4_add_ui_files(ktimer_SRCS prefwidget.ui )

kde4_add_app_icon(ktimer_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/hi*-app-ktimer.png")

kde4_add_executable(ktimer ${ktimer_SRCS})

target_link_libraries(ktimer  ${KDE4_KIO_LIBS})

install(TARGETS ktimer  ${INSTALL_TARGETS_DEFAULT_ARGS} )


########### install files ###############

install( PROGRAMS ktimer.desktop  DESTINATION ${XDG_APPS_INSTALL_DIR})

kde4_install_icons( ${ICON_INSTALL_DIR}   )

add_subdirectory(doc)
