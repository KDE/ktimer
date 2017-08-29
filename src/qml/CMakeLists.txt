qt5_add_resources(
    ktimer_qrc_SRCS
    qml.qrc
)

add_library(ktimer_qrc STATIC ${ktimer_qrc_SRCS})
