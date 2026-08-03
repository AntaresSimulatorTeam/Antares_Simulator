Feature: Parquet output format tests for antares-modeler

  @fast
  Scenario: Parquet: One model with one load and two generators, two timesteps ; TimEval operator
    Given the modeler study path is "modeler/2_1"
    When I run antares modeler with parquet
    Then the simulation succeeds
    And the objective value is 320
    And the system-for-views.yml is correctly exported
    And the modeler outputs contain the following entries
      | component | output | timestep | value |
      | node1     | gen1_p | 0-1      | 80    |
      | node1     | gen2_p | 0-1      | 20    |
