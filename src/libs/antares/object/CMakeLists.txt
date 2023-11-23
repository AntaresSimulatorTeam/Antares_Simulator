set(SRC_OBJECT
        include/antares/object/intrusive-reference.h
        include/antares/object/intrusive-reference.hxx
        include/antares/object/object.h
        include/antares/object/object.hxx
        object.cpp
        include/antares/object/ref.h
)
source_group("object" FILES ${SRC_OBJECT})

add_library(object
        ${SRC_OBJECT})
add_library(Antares::object ALIAS object)

target_link_libraries(object
        PUBLIC
        antares-core
        PRIVATE
        yuni-static-core
        yuni-static-uuid
)

target_include_directories(
        object
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

install(DIRECTORY include/antares
        DESTINATION "include"
)