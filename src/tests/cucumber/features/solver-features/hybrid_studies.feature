Feature: hybrid (simulator+modeler) studies

  @fast @short
  Scenario: 001 One node - passive + modeler test 1_1
    Given the study path is "hybrid/001 One node - passive"
    When I run antares simulator
    Then the simulation succeeds
    And the simulation takes less than 5 seconds
    And the annual system cost is 0