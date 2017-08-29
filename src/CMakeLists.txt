add_subdirectory(qml)

set(ktimer_SRCS
    main.cpp
    ktimer.cpp
    ktimermodel.cpp
    ktimerjob.cpp
)

ki18n_wrap_ui(ktimer_SRCS prefwidget.ui )

add_executable(ktimer ${ktimer_SRCS})
add_dependencies(ktimer ktimer_icons ktimer_qrc)

target_link_libraries(ktimer
    ktimer_icons
    ktimer_qrc
    KF5::I18n
    KF5::KIOWidgets
    KF5::ConfigWidgets
    KF5::Notifications
    KF5::DBusAddons
    Qt5::Quick
)

install(TARGETS ktimer ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})
