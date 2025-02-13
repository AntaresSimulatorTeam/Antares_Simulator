Feature: 2.8

  Scenario: 2.8.1: One model with one load and two generators, one timestep
    Given the study path is "modeler/1_1"
    When I run antares modeler
    Then the simulation succeeds
    And the objective value is 160
    And the optimal value of variable node1.gen1_p_0 is 80
    And the optimal value of variable node1.gen2_p_0 is 20