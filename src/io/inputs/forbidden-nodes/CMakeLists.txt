set(SOURCES
        ForbiddenNodesVisitor.cpp
        ForbiddenNodes.cpp
        include/antares/io/inputs/forbidden-nodes/ForbiddenNodes.h
        include/antares/io/inputs/forbidden-nodes/ForbiddenNodesVisitor.h
)

add_library(forbidden-nodes ${SOURCES})
add_library(Antares::forbidden-nodes ALIAS forbidden-nodes)

target_include_directories(forbidden-nodes
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_link_libraries(forbidden-nodes
        PRIVATE
        Antares::expressions
)

install(DIRECTORY include/antares
        DESTINATION "include"
)
