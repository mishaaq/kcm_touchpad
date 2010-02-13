########### D-Bus interfaces  ###############

qt4_generate_dbus_interface(ksyndaemon.h)

########### ksyndaemon binary ###############

set(ksyndaemon_SRCS
    ksyndaemon.cpp
    main.cpp
)

include_directories( ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR})

qt4_add_dbus_adaptor(ksyndaemon_SRCS ${CMAKE_CURRENT_BINARY_DIR}/ksyndaemon.xml ksyndaemon.h KSyndaemon)

kde4_add_executable( ksyndaemon ${ksyndaemon_SRCS})

target_link_libraries( ksyndaemon ${KDE4_KDEUI_LIBS} )

install( TARGETS ksyndaemon RUNTIME DESTINATION ${LIBEXEC_INSTALL_DIR} )

install( FILES ksyndaemon.desktop  DESTINATION  ${SERVICES_INSTALL_DIR} )

########### D-Bus Autostart Services #########
configure_file(org.kde.ksyndaemon.service.cmake
	${CMAKE_CURRENT_BINARY_DIR}/org.kde.ksyndaemon.service)

install( FILES ${CMAKE_CURRENT_BINARY_DIR}/org.kde.ksyndaemon.service DESTINATION ${DBUS_SERVICES_INSTALL_DIR} )
