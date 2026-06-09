# Header-only library for enum utilities
add_library(enums)
add_library(Antares::enums ALIAS enums)

target_sources(enums PRIVATE
        include/antares/enums/class_name.h
        include/antares/enums/Enum.hpp
        include/antares/enums/Enum.hxx
        class_name.cpp
)

target_include_directories(enums
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

install(DIRECTORY include/antares
        DESTINATION "include"
)

target_link_libraries(enums
        PUBLIC
        Antares::exception
)
