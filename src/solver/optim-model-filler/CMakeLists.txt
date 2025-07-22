set(PROJ optim-model-filler)

set(SRC_optim_model_filler
        ComponentFiller.cpp
        FullKey.cpp
        LinearExpression.cpp
        PartialKey.cpp
        ReadLinearConstraintVisitor.cpp
        ReadLinearExpressionVisitor.cpp
        TimeDependentLinearExpression.cpp
        VariableDictionary.cpp
        include/antares/solver/optim-model-filler/ComponentFiller.h
        include/antares/solver/optim-model-filler/FullKey.h
        include/antares/solver/optim-model-filler/LinearExpression.h
        include/antares/solver/optim-model-filler/PartialKey.h
        include/antares/solver/optim-model-filler/ReadLinearConstraintVisitor.h
        include/antares/solver/optim-model-filler/ReadLinearExpressionVisitor.h
        include/antares/solver/optim-model-filler/TimeDependentLinearExpression.h
        include/antares/solver/optim-model-filler/VariableDictionary.h
        include/antares/solver/optim-model-filler/scenarioGroupRepo.h
        scenarioGroupRepo.cpp
        scenarioGroupRepoExceptions.cpp
)

add_library(${PROJ} ${SRC_optim_model_filler})
add_library(Antares::${PROJ} ALIAS ${PROJ})

set_target_properties(${PROJ} PROPERTIES LINKER_LANGUAGE CXX)

find_package(fmt REQUIRED)
target_link_libraries(${PROJ}
        PRIVATE
        Antares::expressions
        Antares::antares-study-system-model
        Antares::linear-problem-api
        fmt::fmt
)

target_include_directories(${PROJ}
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)
