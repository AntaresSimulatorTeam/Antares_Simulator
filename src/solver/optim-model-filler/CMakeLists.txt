set(PROJ optim-model-filler)

set(SRC_optim_model_filler
        BendersDecomposition.cpp
        ComponentFiller.cpp
        ReadLinearConstraintVisitor.cpp
        ReadLinearExpressionVisitor.cpp
        LinearExpression.cpp
        TimeDependentLinearExpression.cpp
        Dimensions.cpp
        include/antares/solver/optim-model-filler/BendersDecomposition.h
        include/antares/solver/optim-model-filler/ComponentFiller.h
        include/antares/solver/optim-model-filler/ReadLinearConstraintVisitor.h
        include/antares/solver/optim-model-filler/ReadLinearExpressionVisitor.h
        include/antares/solver/optim-model-filler/MCYearAndTime.h
        include/antares/solver/optim-model-filler/Dimensions.h
        include/antares/solver/optim-model-filler/LinearExpression.h
        include/antares/solver/optim-model-filler/TimeDependentLinearExpression.h
)

add_library(${PROJ} ${SRC_optim_model_filler})
add_library(Antares::${PROJ} ALIAS ${PROJ})

set_target_properties(${PROJ} PROPERTIES LINKER_LANGUAGE CXX)

target_link_libraries(${PROJ}
        PRIVATE
        Antares::modeler-optimisation-container
        Antares::expressions
        Antares::exception
        Antares::antares-study-system-model
        Antares::linear-problem-api
        fmt::fmt
)

target_include_directories(${PROJ}
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

install(DIRECTORY include/antares
        DESTINATION "include"
)