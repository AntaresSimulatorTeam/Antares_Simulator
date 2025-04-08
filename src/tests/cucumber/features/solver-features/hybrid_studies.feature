Feature: hybrid (simulator+modeler) studies

  @fast @short
  Scenario: 001 One node - passive + modeler test 1_1
    Given the study path is "Antares_Simulator_Tests_NR/hybrid/001 One node - passive"
    When I run antares simulator
    Then the simulation succeeds

  @fast @short
  Scenario: 002 Thermal fleet - Base + modeler test 1_1
    Given the study path is "Antares_Simulator_Tests_NR/hybrid/002 Thermal fleet - Base"
    When I run antares simulator
    Then the simulation succeeds
    And the annual system cost is 2.729e+7
    And in area "AREA", during year 1, loss of load lasts 1 hours
    And in area "AREA", unsupplied energy on "2 JAN 09:00" of year 1 is of 52 MW
