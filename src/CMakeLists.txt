set(ktimer_SRCS
    main.cpp
    ktimer.cpp
    ktimermodel.cpp
)

ki18n_wrap_ui(ktimer_SRCS prefwidget.ui )

add_executable(ktimer ${ktimer_SRCS})
add_dependencies(ktimer ktimer_icons)

target_link_libraries(ktimer
    ktimer_icons
    KF5::I18n
    KF5::KIOWidgets
    KF5::ConfigWidgets
    KF5::Notifications
    KF5::DBusAddons
)

install(TARGETS ktimer ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})
