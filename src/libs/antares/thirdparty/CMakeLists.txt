set(PROJ antares-third-party)
set(SRC_PROJ
  thirdparty.cpp
  include/antares/thirdparty/thirdparty.h
  # TODO NEW CMAKE TARGET
  sample.cpp
  include/antares/thirdparty/sample.h
)

add_library(${PROJ} ${SRC_PROJ})

target_link_libraries(${PROJ}
        PRIVATE
        yuni-static-core
	PUBLIC
	modeler_api
)

target_include_directories(${PROJ}
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

install(DIRECTORY include/antares
        DESTINATION "include"
)


