set(HEADERS
        include/antares/application/application.h
)
set(SRC_APPLICATION
        ${HEADERS}
        application.cpp
        process-priority.cpp
)
source_group("application" FILES ${SRC_APPLICATION})

add_library(application
        ${SRC_APPLICATION}
)
add_library(Antares::application ALIAS application)

target_link_libraries(application
        PUBLIC
        yuni-static-core
        Antares::study
        Antares::infoCollection
        solver-lib
        PRIVATE
        Antares::logs
        Antares::sys
        Antares::solverUtils
)

target_include_directories(application
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

set_target_properties(application PROPERTIES PUBLIC_HEADER "${HEADERS}")
include(GNUInstallDirs)
install(TARGETS application
        EXPORT "applicationTargets"
        # these get default values from GNUInstallDirs, no need to set them
        #RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} # bin
        #LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} # lib
        #ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} # lib
        # except for public headers, as we want them to be inside a library folder
        PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/antares # include/SomeLibrary
        INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR} # include
)