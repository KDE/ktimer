project(ktimer)

cmake_minimum_required (VERSION 2.8.12 FATAL_ERROR)
set (QT_MIN_VERSION "5.2.0")

find_package (ECM 1.3.0 REQUIRED NO_MODULE)
set (CMAKE_MODULE_PATH ${ECM_MODULE_PATH})

add_definitions(-DTRANSLATION_DOMAIN="kcharselect")

include(KDEInstallDirs)
include(KDECompilerSettings)
include(KDECMakeSettings)
include(ECMInstallIcons)

find_package (Qt5 ${QT_MIN_VERSION} CONFIG REQUIRED COMPONENTS
    Core
    Widgets
)

find_package(KF5 REQUIRED COMPONENTS
    DocTools
    I18n
    WidgetsAddons
    XmlGui
    KDELibs4Support
)


set(ktimer_SRCS main.cpp ktimer.cpp )

ki18n_wrap_ui(ktimer_SRCS prefwidget.ui )

kde4_add_app_icon(ktimer_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/hi*-app-ktimer.png")

add_executable(ktimer ${ktimer_SRCS})

target_link_libraries(ktimer  KF5::KIOCore KF5::KDELibs4Support)

install(TARGETS ktimer  ${INSTALL_TARGETS_DEFAULT_ARGS} )


########### install files ###############

install( PROGRAMS ktimer.desktop  DESTINATION ${XDG_APPS_INSTALL_DIR})

ecm_install_icons( ${ICON_INSTALL_DIR}   )

add_subdirectory(doc)
