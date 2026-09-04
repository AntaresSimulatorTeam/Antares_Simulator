Feature: Multi-scenario studies (one simulation table per scenario)

  @short
  Scenario: 2 simple : Two scenarios with a scenario-dependent load produce one simulation table per scenario
    Given the modeler study path is "modeler/scenario_scope_2"
    When I run antares modeler
    Then the simulation succeeds
    And the objective value is 25
    And the simulation tables are written for the following scenarios
      | scenario |
      | 0        |
      | 1        |
    And the modeler outputs contain the following entries
      | component | output | timestep | scenario | value |
      | NG        | gen1_p | 0        | 0        | 50    |
      | NG        | gen2_p | 0        | 0        | 0     |
      | NG        | gen1_p | 0        | 1        | 80    |
      | NG        | gen2_p | 0        | 1        | 20    |

  @short
  Scenario: 15.2: Two scenarios, parquet output format, one parquet table per scenario
    Given the modeler study path is "modeler/scenario_scope_2"
    When I run antares modeler with parquet
    Then the simulation succeeds
    And the objective value is 25
    And the simulation tables are written for the following scenarios
      | scenario |
      | 0        |
      | 1        |
    And the modeler outputs contain the following entries
      | component | output | timestep | scenario | value |
      | NG        | gen1_p | 0        | 0        | 50    |
      | NG        | gen2_p | 0        | 1        | 20    |
