add_library(whatcable-plasmoid-plugin SHARED
    plugin/WhatCablePlugin.cpp
    plugin/DeviceModel.cpp
)

target_link_libraries(whatcable-plasmoid-plugin PRIVATE
    whatcablecore
    Qt6::Core
    Qt6::Quick
)

install(TARGETS whatcable-plasmoid-plugin DESTINATION ${KDE_INSTALL_PLUGINDIR}/plasma/applets)

plasma_install_package(. org.kde.whatcable)
