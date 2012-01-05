project(ktimer)

if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_CURRENT_SOURCE_DIR})
    find_package(KDE4)
    include( KDE4Defaults )
    include_directories(${KDE4_INCLUDES})

    add_definitions(${QT_DEFINITIONS} ${KDE4_DEFINITIONS})
    add_definitions(-DQT_USE_FAST_CONCATENATION -DQT_USE_FAST_OPERATOR_PLUS)

    # If definitions like -D_GNU_SOURCE are needed for these checks they
    # should be added to _KDE4_PLATFORM_DEFINITIONS when it is originally
    # defined outside this file.  Here we include these definitions in
    # CMAKE_REQUIRED_DEFINITIONS so they will be included in the build of
    # checks below.
    set( CMAKE_REQUIRED_DEFINITIONS ${_KDE4_PLATFORM_DEFINITIONS} )
endif()

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
