Feature: 2 - Simple end-to-end tests to test temporal expression management : TimeEval operator

  Scenario: 2.1: One model with one load and two generators, two timesteps ; TimEval operator in the load satsifaction constraint (on variables only)
    Given the modeler study path is "modeler/2_1"
    When I run antares modeler
    Then the simulation succeeds
    And the objective value is 320
    And the optimal values of the variables are
      | component | variable | timestep | value |
      | node1     | gen1_p   | 0-1     | 80    |
      | node1     | gen2_p   | 0-1     | 20    |
  
  Scenario: 2.2: One model with one load and two generators, two timesteps ; TimeEval operator in the objective (on variables and constant param)
    Given the modeler study path is "modeler/2_2"
    When I run antares modeler
    Then the simulation succeeds
    And the objective value is 320
    And the optimal values of the variables are
      | component | variable | timestep | value |
      | node1     | gen1_p   | 0-1     | 80    |
      | node1     | gen2_p   | 0-1     | 20    |

  
  Scenario: 2.3: One model with one load and two generators, two timesteps ; TimeEval operator in the objective (on variables and timeseries param)
    Given the modeler study path is "modeler/2_3"
    When I run antares modeler
    Then the simulation succeeds
    And the objective value is 320
    And the optimal values of the variables are
      | component | variable | timestep | value |
      | node1     | gen1_p   | 0-1     | 80    |
      | node1     | gen2_p   | 0-1     | 20    |
  
  Scenario: 2.4: One model with one load and two generators, two timesteps ; TimeEval operator in the objective (on arithmetic expression)
    Given the modeler study path is "modeler/2_4"
    When I run antares modeler
    Then the simulation succeeds
    And the objective value is 320
    And the optimal values of the variables are
      | component | variable | timestep | value |
      | node1     | gen1_p   | 0-1     | 80    |
      | node1     | gen2_p   | 0-1     | 20    |