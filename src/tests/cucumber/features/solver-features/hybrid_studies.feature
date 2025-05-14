Feature: hybrid (simulator+modeler) studies

  @fast @short
  Scenario: 001 One node - passive + modeler test 1_1
    Given the solver study path is "Antares_Simulator_Tests_NR/hybrid/001 One node - passive"
    When I run antares simulator
    Then the simulation succeeds

  @fast @short
  Scenario: 002 Thermal fleet - Base + modeler test 1_1
    Given the solver study path is "Antares_Simulator_Tests_NR/hybrid/002 Thermal fleet - Base"
    When I run antares simulator
    Then the simulation succeeds
    And the annual system cost is 27288600
    And in area "AREA", during year 1, loss of load lasts 1 hours
    And in area "AREA", unsupplied energy on "2 JAN 09:00" of year 1 is of 52 MW

  @fast @short
  Scenario: Empty legacy node with one generator component and one load component (24h simplex)
    Given the solver study path is "Antares_Simulator_Tests_NR/hybrid/3_6_0"
    When I run antares simulator
    Then the simulation succeeds
    And the simulation takes less than 5 seconds
    # 100MW @ 0.3€/MW/h, for 1 week = 5040 €
    # for now, modeler costs does not figure in system cost txt
    And the annual system cost is 0
    And in area "NODE", during year 1, loss of load lasts 0 hours

  @fast @short
  Scenario: Legacy node with one legacy load (up to 5952 MW) and wind, and one generator component (max_p=6200) (168h simplex)
    # copy of short test 002, with no legacy thermal cluster, replaced by one component
    Given the solver study path is "Antares_Simulator_Tests_NR/hybrid/3_6_1"
    When I run antares simulator
    Then the simulation succeeds
    And the simulation takes less than 5 seconds
    # for now, modeler costs does not figure in system cost txt
    And the annual system cost is 0
    And in area "AREA", during year 1, loss of load lasts 0 hours

  @fast @short
  Scenario: Legacy node with one legacy load (up to 5952 MW) and wind, and one generator component (max_p=5900) (168h simplex)
    # copy of previous case, with reduced max_p on generator => loss of load of 52MW on 1 hour
    Given the solver study path is "Antares_Simulator_Tests_NR/hybrid/3_6_2"
    When I run antares simulator
    Then the simulation succeeds
    And the simulation takes less than 5 seconds
    # for now, modeler costs does not figure in system cost txt
    And the annual system cost is 520000
    And in area "AREA", during year 1, loss of load lasts 1 hours
    And in area "AREA", during year 1, total unsupplied energy is 52 MWh

  @fast @short
  Scenario: Legacy node with one legacy thermal cluster, and one load component (constant load of 3000 MW)
    Given the solver study path is "Antares_Simulator_Tests_NR/hybrid/3_6_3"
    When I run antares simulator
    Then the simulation succeeds
    And the simulation takes less than 5 seconds
    # for now, modeler costs does not figure in system cost txt
    And the annual system cost is 17640000.0
    And in area "AREA", during year 1, loss of load lasts 0 hours
    And in area "AREA", during year 1, hourly production of "base" is always equal to 3000 MWh