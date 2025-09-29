add_library(investment-modeler
            include/antares/modeler/investment/masterProblem.h
            masterProblem.cpp)

add_library(Antares::investment-modeler ALIAS investment-modeler)

target_link_libraries(investment-modeler
        PRIVATE
        Antares::exception
        Antares::solverUtils
)

target_include_directories(investment-modeler
        PUBLIC
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)

