Feature: 3 - Simple end-to-end tests to test temporal expression management : TimeShift

  Scenario: 3.1: One model with one load and two generators, two timesteps ; TimeShift operator in a constraint => generator1 should have a constant power
    Given the study path is "modeler/3_1"
    When I run antares modeler
    Then the simulation succeeds
    And the objective value is 840
    And the optimal values of the variables are
      | component | variable | timestep | value |
      | node1     | gen1_p   | 0-1      | 80    |
      | node1     | gen2_p   | 0-1      | 20    |


  Scenario: 3.2: One model with one load and two generators, three timesteps ; TimeShift operator in a constraint => gen1_p <= Min(Pmax[t], 1.5*Pmax[t-1])
    Given the study path is "modeler/3_2"
    When I run antares modeler
    Then the simulation succeeds
    And the objective value is 672.5
    And the optimal values of the variables are
      | component | variable | timestep | value |
      | node1     | gen1_p   | 0        | 50    |
      | node1     | gen2_p   | 0        | 50    |
      | node1     | gen1_p   | 1        | 75    |
      | node1     | gen2_p   | 1        | 25    |
      | node1     | gen1_p   | 2        | 80    |
      | node1     | gen2_p   | 2        | 20    |

#TODO test input contains illegal expression, to be updated
  Scenario: 3.3: One model with one load and two generators, two timesteps ; TimeShift operator in a constraint => generator1 should have a constant power. The TimeShift operator is applied to an expression.
    Given the study path is "modeler/3_3"
    When I run antares modeler
    Then the simulation succeeds
    And the objective value is 840
    And the optimal values of the variables are
      | component | variable | timestep | value |
      | node1     | gen1_p   | 0-1      | 80    |
      | node1     | gen2_p   | 0-1      | 20    |

