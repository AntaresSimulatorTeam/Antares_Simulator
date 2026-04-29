# This file adds custom targets to generate headers & sources from ANTLR4 grammar files

# To generate all headers and sources
# cmake --build <build> --target antlr_generate_all
# NOTE Environment variable ANTLR_JAR_PATH must be set

macro(add_antlr_target target_name directory grammar_file)
    # Stamp file to track generation
    set(stamp_file ${directory}/${target_name}.stamp)

    # ANTLR generation command
    add_custom_command(
        OUTPUT ${stamp_file}
	WORKING_DIRECTORY ${directory}
	COMMAND ${CMAKE_COMMAND} -E env java -jar "${ANTLR_JAR_PATH}"
            -Dlanguage=Cpp
            -visitor
            -no-listener
            -o .
            ${grammar_file}

        COMMAND ${CMAKE_COMMAND} -E touch ${stamp_file}
        DEPENDS ${directory}/${grammar_file}
        COMMENT "Generating C++ files from ${grammar_file}"
        VERBATIM
    )

    # Target to trigger generation
    add_custom_target(${target_name} DEPENDS ${stamp_file})
endmacro()

add_antlr_target(antlr_generate_hours_field
  ${CMAKE_SOURCE_DIR}/libs/antares/additionalConstraintRhsExpression
  HoursField.g4)

add_antlr_target(antlr_generate_expression
  ${CMAKE_SOURCE_DIR}/expressions/antlr-interface
  Expr.g4)

add_antlr_target(antlr_generate_scbuilder
  ${CMAKE_SOURCE_DIR}/libs/antares/scenarioGroupParser/scenarioBuilderExpression
  ScenarioBuilder.g4)

add_custom_target(antlr_generate_all
  DEPENDS
  antlr_generate_hours_field
  antlr_generate_expression
  antlr_generate_scbuilder)
