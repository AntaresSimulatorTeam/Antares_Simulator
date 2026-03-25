# =========================
# Target study-utils
# =========================
set(SRC_STUDY_UTILS
        utils.cpp
        include/antares/utils/utils.h
        include/antares/utils/utils.hxx
)

add_library(study-utils ${SRC_STUDY_UTILS}) #utils also used elsewhere
add_library(Antares::study-utils ALIAS study-utils)

target_link_libraries(study-utils
        PRIVATE
        yuni-static-core
        Antares::logs
)

target_include_directories(study-utils
                           PUBLIC
                           $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

# =========================
# Target vector_utils
# =========================

set(SRC_VECTOR_UTILS include/antares/utils/vector-utils.h)

source_group("utils" FILES ${SRC_VECTOR_UTILS})

# Header-only: use an INTERFACE library so CMake does not need to determine a linker language
add_library(vector-utils INTERFACE)
add_library(Antares::vector-utils ALIAS vector-utils)

target_include_directories(vector-utils
                           INTERFACE
                           $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

# =========================

source_group("utils" FILES ${SRC_STUDY_UTILS} ${SRC_VECTOR_UTILS})

install(DIRECTORY include/antares
        DESTINATION "include"
)