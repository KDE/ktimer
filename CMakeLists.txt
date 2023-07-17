# KDE Application Version, managed by release script
set (RELEASE_SERVICE_VERSION_MAJOR "23")
set (RELEASE_SERVICE_VERSION_MINOR "11")
set (RELEASE_SERVICE_VERSION_MICRO "70")
set (RELEASE_SERVICE_VERSION "${RELEASE_SERVICE_VERSION_MAJOR}.${RELEASE_SERVICE_VERSION_MINOR}.${RELEASE_SERVICE_VERSION_MICRO}")
cmake_minimum_required (VERSION 3.16 FATAL_ERROR)
project(ktimer VERSION ${RELEASE_SERVICE_VERSION})

set (QT_MIN_VERSION "5.15.2")
set(KF_MIN_VERSION "5.92.0")

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
if (QT_MAJOR_VERSION STREQUAL "6")
    set(QT_MIN_VERSION "6.4.0")
    set(KF_MIN_VERSION "5.240.0")
    set(KF_MAJOR_VERSION "6")
else()
    set(KF_MAJOR_VERSION "5")
endif()

find_package (Qt${QT_MAJOR_VERSION} ${QT_MIN_VERSION} CONFIG REQUIRED COMPONENTS
    Core
    Widgets
)
if (QT_MAJOR_VERSION STREQUAL "6")
    find_package(Qt6Core5Compat)
endif()

find_package(KF${KF_MAJOR_VERSION} ${KF_MIN_VERSION} REQUIRED COMPONENTS
    I18n
    WidgetsAddons
    KIO
    DBusAddons
    Notifications
    ConfigWidgets
)

find_package(KF${KF_MAJOR_VERSION}DocTools ${KF_MIN_VERSION})
set_package_properties(KF5DocTools PROPERTIES DESCRIPTION
    "Tools to generate documentation"
    TYPE OPTIONAL
)

ecm_set_disabled_deprecation_versions(QT 6.4.0
    KF 5.101
)

add_executable(ktimer)

configure_file(config-ktimer.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config-ktimer.h)


target_sources(ktimer PRIVATE main.cpp ktimer.cpp ktimer.h)

ki18n_wrap_ui(ktimer prefwidget.ui )

file(GLOB ICONS_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/*-apps-ktimer.png")
ecm_add_app_icon(ktimer ICONS ${ICONS_SRCS})


target_link_libraries(ktimer  KF${KF_MAJOR_VERSION}::I18n KF${KF_MAJOR_VERSION}::KIOWidgets KF${KF_MAJOR_VERSION}::ConfigWidgets
    KF${KF_MAJOR_VERSION}::Notifications KF${KF_MAJOR_VERSION}::DBusAddons)

install(TARGETS ktimer  ${KDE_INSTALL_TARGETS_DEFAULT_ARGS} )


########### install files ###############

install( PROGRAMS org.kde.ktimer.desktop  DESTINATION ${KDE_INSTALL_APPDIR})
install(FILES org.kde.ktimer.appdata.xml DESTINATION ${KDE_INSTALL_METAINFODIR})

ecm_install_icons( ICONS 128-apps-ktimer.png  16-apps-ktimer.png  32-apps-ktimer.png  48-apps-ktimer.png DESTINATION ${KDE_INSTALL_ICONDIR} THEME hicolor  )
if(KF${KF_MAJOR_VERSION}DocTools_FOUND)
    kdoctools_install(po)
    add_subdirectory(doc)
endif()
ki18n_install(po)
kde_configure_git_pre_commit_hook(CHECKS CLANG_FORMAT)
feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)
