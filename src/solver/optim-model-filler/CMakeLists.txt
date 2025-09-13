set(PROJ optim-model-filler)
find_package(Eigen3 CONFIG REQUIRED)

set(SRC_optim_model_filler
        ComponentFiller.cpp
        EvaluationContextProvider.cpp
        FullKey.cpp
        PartialKey.cpp
        ReadLinearConstraintVisitor.cpp
        ReadLinearExpressionVisitor.cpp
        VariableDictionary.cpp
        LinearExpressionEigen.cpp
        include/antares/solver/optim-model-filler/ComponentFiller.h
        include/antares/solver/optim-model-filler/EvaluationContextProvider.h
        include/antares/solver/optim-model-filler/FullKey.h
        include/antares/solver/optim-model-filler/PartialKey.h
        include/antares/solver/optim-model-filler/ReadLinearConstraintVisitor.h
        include/antares/solver/optim-model-filler/ReadLinearExpressionVisitor.h
        include/antares/solver/optim-model-filler/MCYearAndTime.h
        include/antares/solver/optim-model-filler/VariableDictionary.h
        include/antares/solver/optim-model-filler/scenarioGroupRepo.h
        include/antares/solver/optim-model-filler/LinearExpressionEigen.h
        include/antares/solver/optim-model-filler/VariableContainer.h
        scenarioGroupRepo.cpp
        scenarioGroupRepoExceptions.cpp
        VariableContainer.cpp
)

add_library(${PROJ} ${SRC_optim_model_filler})
add_library(Antares::${PROJ} ALIAS ${PROJ})

set_target_properties(${PROJ} PROPERTIES LINKER_LANGUAGE CXX)

target_link_libraries(${PROJ}
        PRIVATE
        Antares::expressions
        Antares::exception
        Antares::antares-study-system-model
        Antares::linear-problem-api
        fmt::fmt
        Eigen3::Eigen
)

target_include_directories(${PROJ}
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)
