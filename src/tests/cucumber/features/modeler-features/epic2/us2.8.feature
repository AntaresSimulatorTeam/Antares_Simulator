Feature: 2.8

  Scenario: 2.8.1: One model with one load and two generators, one timestep
    Given the study path is "modeler/1_1"
    When I run antares modeler
    Then the modeler simulation succeeds
    And the objective value is 160
    And the optimal value of variable node1.gen1_p_0 is 80
    And the optimal value of variable node1.gen2_p_0 is 20

  Scenario: 2.8.1: One model with one load and two generators, mix of constant and time dependent parameter, one timestep.
    Given the study path is "modeler/1_2"
    When I run antares modeler
    Then the modeler simulation succeeds
    And the objective value is 160
    And the optimal value of variable node1.gen1_p_0 is 80
    And the optimal value of variable node1.gen2_p_0 is 20

  Scenario: 2.8.1: Exception handling: time series of different lengths.
    Given the study path is "modeler/1_3"
    When I run antares modeler
    Then the simulation fails

  Scenario: 2.8.1: Mixture of constant parameters and time series (2 time steps), With booleans type.
    Given the study path is "modeler/1_4"
    When I run antares modeler
    Then the modeler simulation succeeds
    And the objective value is 765
    And the optimal values of the variables are
      | component | variable | timestep | value  |
      | node1     | gen1_p   | 0-1      | 80-0   |
      | node1     | gen2_p   | 0-1      | 20-100 |

  Scenario: 2.8.1: Exception handling: given time serie for a constant in time parameter.
    Given the study path is "modeler/1_5"
    When I run antares modeler
    Then the simulation fails


  Scenario: 2.8.1: Mixture of constant parameters and time series (10 time steps)
    Given the study path is "modeler/1_7"
    When I run antares modeler
    Then the modeler simulation succeeds
    And the objective value is 16000
    And the optimal values of the variables are
      | component | variable | timestep | value |
      | node1     | gen1_p   | 0-99     | 80    |
      | node1     | gen2_p   | 0-99     | 20    |