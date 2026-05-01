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

# Build a kpackagetool6-installable package directory under build/
set(PKG_DIR "${CMAKE_BINARY_DIR}/pkg/org.kde.whatcable")
add_custom_command(TARGET whatcable-plasmoid-plugin POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "${PKG_DIR}/contents/ui"
    COMMAND ${CMAKE_COMMAND} -E copy
        "${CMAKE_CURRENT_SOURCE_DIR}/metadata.json" "${PKG_DIR}/metadata.json"
    COMMAND ${CMAKE_COMMAND} -E copy
        "${CMAKE_CURRENT_SOURCE_DIR}/contents/ui/main.qml"
        "${CMAKE_CURRENT_SOURCE_DIR}/contents/ui/CompactView.qml"
        "${CMAKE_CURRENT_SOURCE_DIR}/contents/ui/DeviceCard.qml"
        "${CMAKE_CURRENT_SOURCE_DIR}/contents/ui/PowerSourceList.qml"
        "${PKG_DIR}/contents/ui/"
    COMMENT "Assembling plasmoid package in build/pkg/org.kde.whatcable"
)
