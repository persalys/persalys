
add_subdirectory ( include )
add_subdirectory ( src )
add_subdirectory ( test )

install ( FILES ${HEADERFILES}
          DESTINATION ${OTGUI_INCLUDE_PATH}
        )

