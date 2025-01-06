set(PROJ optim-model-filler)

set(SRC_optim_model_filler
        include/antares/solver/optim-model-filler/ComponentFiller.h
        include/antares/solver/optim-model-filler/LinearExpression.h
        include/antares/solver/optim-model-filler/ReadLinearConstraintVisitor.h
        include/antares/solver/optim-model-filler/ReadLinearExpressionVisitor.h
        ComponentFiller.cpp
        LinearExpression.cpp
        ReadLinearConstraintVisitor.cpp
        ReadLinearExpressionVisitor.cpp
)

add_library(${PROJ} ${SRC_optim_model_filler})
add_library(Antares::${PROJ} ALIAS ${PROJ})

set_target_properties(${PROJ} PROPERTIES LINKER_LANGUAGE CXX)

target_link_libraries(${PROJ}
        PRIVATE
        Antares::solver-expressions
        Antares::antares-study-system-model
        Antares::modeler_api
)

target_include_directories(${PROJ}
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)
