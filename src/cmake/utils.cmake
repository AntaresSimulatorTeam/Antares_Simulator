macro(copy_dependency deps target)

        if("${CMAKE_BUILD_TYPE}" STREQUAL "release")

            get_target_property( DEP_SHARED_LIB_PATH ${deps} IMPORTED_LOCATION_RELEASE )
        else()

            get_target_property( DEP_SHARED_LIB_PATH ${deps} IMPORTED_LOCATION_DEBUG )
        endif()

	if (NOT "${DEP_SHARED_LIB_PATH}" STREQUAL "DEP_SHARED_LIB_PATH-NOTFOUND")

            # Copy the shared lib file
            add_custom_command(TARGET ${target} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${DEP_SHARED_LIB_PATH} $<TARGET_FILE_DIR:${target}>)

	endif()

endmacro()

