Feature: 4 - Simple end-to-end tests to test temporal expression management

  Scenario: 4.1: One model with one load and two generators, 100 timesteps
    Given the modeler study path is "modeler/4_1"
    When I run antares modeler
    Then the simulation succeeds
    And the objective value is 4000
    And the optimal values of the variables are
      | component | variable | timestep | value |
      | node1     | gen1_p   | 0-99  | 80    |
      | node1     | gen2_p   | 0-99     | 20    |

  Scenario: 4.2: One model with one load and two generators, two timesteps ; fuel constraint on one generator
    Given the modeler study path is "modeler/4_2"
    When I run antares modeler
    Then the simulation succeeds
    And the objective value is 420
    And the optimal values of the variables are
      | component | variable | timestep | value |
      | node1     | gen1_p   | 0     | 80    |
      | node1     | gen2_p   | 0     | 0    |
      | node1     | gen1_p   | 1     | 20    |
      | node1     | gen2_p   | 1     | 60    |

  Scenario: 4.3: One model with one load and two generators, one timestep ; 
    Given the modeler study path is "modeler/4_3"
    When I run antares modeler
    Then the simulation succeeds
    And the objective value is 160
    And the optimal values of the variables are
      | component           | variable | timestep | value |
      | node1     | gen1_p   | 0      | 80    |
      | node1     | gen2_p   | 0     | 20    |
    

 