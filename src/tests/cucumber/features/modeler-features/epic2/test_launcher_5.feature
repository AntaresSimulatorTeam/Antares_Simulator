Feature: 5 - Simple end-to-end tests to test temporal expression management - TimeSum operator
#TODO
#  Scenario: 5.1: One model with one load and two generators, 3 timesteps, temporal constraint on generator 1 linking consecutive timesteps sum(t..t+1,expr)
#    Given the study path is "modeler/5_1"
#    When I run antares modeler
#    Then the simulation succeeds
#    And the objective value is 580
#    And the optimal values of the variables are
#      | component | variable | timestep | value |
#      | node1     | gen1_p   | 0        | 80    |
#      | node1     | gen1_p   | 1        | 20    |
#      | node1     | gen1_p   | 2        | 80    |
#      | node1     | gen2_p   | 0        | 0    |
#      | node1     | gen2_p   | 1        | 60    |
#      | node1     | gen2_p   | 2        | 0    |