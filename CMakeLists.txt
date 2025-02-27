# KDE Application Version, managed by release script
set (RELEASE_SERVICE_VERSION_MAJOR "24")
set (RELEASE_SERVICE_VERSION_MINOR "12")
set (RELEASE_SERVICE_VERSION_MICRO "3")
set (RELEASE_SERVICE_VERSION "${RELEASE_SERVICE_VERSION_MAJOR}.${RELEASE_SERVICE_VERSION_MINOR}.${RELEASE_SERVICE_VERSION_MICRO}")
cmake_minimum_required (VERSION 3.16 FATAL_ERROR)
project(ktimer VERSION ${RELEASE_SERVICE_VERSION})

set(QT_MIN_VERSION "6.5.0")
set(KF_MIN_VERSION "6.0.0")

find_package (ECM ${KF_MIN_VERSION} REQUIRED NO_MODULE)
set (CMAKE_MODULE_PATH ${ECM_MODULE_PATH})

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)

include(ECMInstallIcons)
include(ECMAddAppIcon)
include(KDEGitCommitHooks)
include(KDEClangFormat)
include(ECMDeprecationSettings)
file(GLOB_RECURSE ALL_CLANG_FORMAT_SOURCE_FILES *.cpp *.h *.c)
kde_clang_format(${ALL_CLANG_FORMAT_SOURCE_FILES})

find_package (Qt6 ${QT_MIN_VERSION} CONFIG REQUIRED COMPONENTS
    Core
    Widgets
)

find_package(KF6 ${KF_MIN_VERSION} REQUIRED COMPONENTS
    I18n
    WidgetsAddons
    KIO
    DBusAddons
    Notifications
    ConfigWidgets
    Crash
)

find_package(KF6StatusNotifierItem ${KF_MIN_VERSION} REQUIRED)

find_package(KF6DocTools ${KF_MIN_VERSION})
set_package_properties(KF5DocTools PROPERTIES DESCRIPTION
    "Tools to generate documentation"
    TYPE OPTIONAL
)

ecm_set_disabled_deprecation_versions(QT 6.8.0
    KF 6.7
)
add_definitions(-DQT_NO_CONTEXTLESS_CONNECT)
add_executable(ktimer)

configure_file(config-ktimer.h.in ${CMAKE_CURRENT_BINARY_DIR}/config-ktimer.h)


target_sources(ktimer PRIVATE main.cpp ktimer.cpp ktimer.h)

ki18n_wrap_ui(ktimer prefwidget.ui )

file(GLOB ICONS_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/*-apps-ktimer.png")
ecm_add_app_icon(ktimer ICONS ${ICONS_SRCS})


target_link_libraries(ktimer  KF6::I18n KF6::KIOWidgets KF6::ConfigWidgets
    KF6::Notifications KF6::DBusAddons KF6::Crash)

target_link_libraries(ktimer KF6::StatusNotifierItem)


install(TARGETS ktimer  ${KDE_INSTALL_TARGETS_DEFAULT_ARGS} )


########### install files ###############

install( PROGRAMS org.kde.ktimer.desktop  DESTINATION ${KDE_INSTALL_APPDIR})
install(FILES org.kde.ktimer.appdata.xml DESTINATION ${KDE_INSTALL_METAINFODIR})

ecm_install_icons( ICONS 128-apps-ktimer.png  16-apps-ktimer.png  32-apps-ktimer.png  48-apps-ktimer.png DESTINATION ${KDE_INSTALL_ICONDIR} THEME hicolor  )
if(KF6DocTools_FOUND)
    kdoctools_install(po)
    add_subdirectory(doc)
endif()
ki18n_install(po)
kde_configure_git_pre_commit_hook(CHECKS CLANG_FORMAT)
feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
