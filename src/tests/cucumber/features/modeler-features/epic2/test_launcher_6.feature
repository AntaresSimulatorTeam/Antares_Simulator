Feature: 6 - Simple end-to-end tests to test port management

  Scenario: 6.1: One system with three components: one balance node, one load and one 2 gen-component
    Given the study path is "modeler/6_1"
    When I run antares modeler
    Then the simulation succeeds
    And the objective value is 160
    And the optimal values of the variables are
      | component | variable | timestep | value |
      | NG        | gen1_p   | 0        | 80    |
      | NG        | gen2_p   | 0        | 20    |