if(NOT DEFINED ASSETS_SOURCE)
    message(FATAL_ERROR "ASSETS_SOURCE is not set")
endif()

if(NOT DEFINED ASSETS_DEST)
    message(FATAL_ERROR "ASSETS_DEST is not set")
endif()

if(NOT EXISTS "${ASSETS_DEST}")
    message(STATUS "Copying assets to ${ASSETS_DEST}")
    execute_process(
        COMMAND "${CMAKE_COMMAND}" -E copy_directory "${ASSETS_SOURCE}" "${ASSETS_DEST}"
        RESULT_VARIABLE copy_result
    )
    if(NOT copy_result EQUAL 0)
        message(FATAL_ERROR "Failed to copy assets from ${ASSETS_SOURCE} to ${ASSETS_DEST}")
    endif()
endif()
