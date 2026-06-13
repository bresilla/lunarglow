add_custom_command(OUTPUT lunarglow.1 COMMAND pod2man --section=1 --center="Lunarglow Manual" --name="LUNARGLOW" --release="lunarglow 2.7.1" ${CMAKE_CURRENT_SOURCE_DIR}/README.pod > lunarglow.1)
add_custom_target(docs ALL DEPENDS lunarglow.1)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/lunarglow.1 DESTINATION ${CMAKE_INSTALL_MANDIR}/man1)
