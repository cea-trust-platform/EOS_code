#
message(STATUS "Add target index-eos")
add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/index.eos
                   COMMAND sh -c "${CMAKE_CURRENT_SOURCE_DIR}/mk_index_eos ${CMAKE_CURRENT_BINARY_DIR} index.eos"
                   WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)
add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/index.eos.conv
                   COMMAND sh -c "${CMAKE_CURRENT_SOURCE_DIR}/mk_index_eos_conv ${CMAKE_CURRENT_BINARY_DIR} index.eos"
                   WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                   DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/index.eos
)
add_custom_target(index-eos ALL
                  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                  DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/index.eos.conv
                  DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/index.eos
)