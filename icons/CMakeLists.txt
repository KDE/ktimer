set(ICONS_SRCS
 128-apps-ktimer.png
 16-apps-ktimer.png
 32-apps-ktimer.png
 48-apps-ktimer.png
)

ecm_add_app_icon(ktimer_ECM_ICONS ICONS ${ICONS_SRCS})

ecm_install_icons(ICONS
    ${ICONS_SRCS}
    DESTINATION
    ${KDE_INSTALL_ICONDIR}
    THEME hicolor
)

add_library(ktimer_icons ${ktimer_ECM_ICONS} STATIC)
