Feature: 3 - Simple end-to-end tests to test temporal expression management : TimeShift

  @fast
  Scenario: 3.1: One model with one load and two generators, two timesteps ; TimeShift operator in a constraint => generator1 should have a constant power
    Given the modeler study path is "modeler/3_1"
    When I run antares modeler
    Then the simulation succeeds
    And the objective value is 840
    And the modeler outputs contain the following entries
      | component | output | timestep | value |
      | node1     | gen1_p | 1-2      | 80    |
      | node1     | gen2_p | 1-2      | 20    |


  @fast
  Scenario: 3.2: One model with one load and two generators, three timesteps ; TimeShift operator in a constraint => gen1_p <= Min(Pmax[t], 1.5*Pmax[t-1])
    Given the modeler study path is "modeler/3_2"
    When I run antares modeler
    Then the simulation succeeds
    And the objective value is 672.5
    And the modeler outputs contain the following entries
      | component | output | timestep | value |
      | node1     | gen1_p | 1        | 50    |
      | node1     | gen2_p | 1        | 50    |
      | node1     | gen1_p | 2        | 75    |
      | node1     | gen2_p | 2        | 25    |
      | node1     | gen1_p | 3        | 80    |
      | node1     | gen2_p | 3        | 20    |

#TODO test input contains illegal expression, to be updated
  @fast
  Scenario: 3.3: One model with one load and two generators, two timesteps ; TimeShift operator in a constraint => generator1 should have a constant power. The TimeShift operator is applied to an expression.
    Given the modeler study path is "modeler/3_3"
    When I run antares modeler
    Then the simulation succeeds
    And the objective value is 840
    And the modeler outputs contain the following entries
      | component | output | timestep | value |
      | node1     | gen1_p | 1-2      | 80    |
      | node1     | gen2_p | 1-2      | 20    |

