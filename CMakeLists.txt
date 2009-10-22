find_package( Qt4 REQUIRED )
find_package( KDE4 REQUIRED )
find_package( Msgfmt REQUIRED )
find_package( Gettext REQUIRED )

########### KCM ###############

set( kcm_touchpad_PART_SRCS
    kcmtouchpad.cpp
    touchpad.cpp
)

include_directories( ${CMAKE_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_BUILD_DIR}
    ${QT_INCLUDES}
    ${KDE4_INCLUDES}
)

kde4_add_ui_files( kcm_touchpad_PART_SRCS kcmtouchpadwidget.ui )

kde4_add_plugin( kcm_touchpad ${kcm_touchpad_PART_SRCS} )

target_link_libraries( kcm_touchpad
    ${KDE4_KIO_LIBS}
)

target_link_libraries( kcm_touchpad ${X11_LIBRARIES} m X11 Xi )

add_subdirectory ( po )

install( TARGETS kcm_touchpad  DESTINATION ${PLUGIN_INSTALL_DIR} )

install( FILES touchpad.desktop DESTINATION  ${SERVICES_INSTALL_DIR} )

install( FILES AUTHORS README LICENSE DESTINATION ${SHARE_INSTALL_PREFIX}/doc/kcm_touchpad )

#install( FILES ${QM_FILES} DESTINATION ${CMAKE_INSTALL_PREFIX}/translations )
