Feature: 2.4 - modeler simple tests with no ports and no timeseries

  Scenario: 2.4.1: One model with one load and two generators, one timestamp
    Given the study path is "modeler/study_2.4.1"
    When I run antares modeler
    Then the simulation succeeds
    And the optimal value of variable node1.gen1_p_0 is 80
    And the optimal value of variable node1.gen2_p_0 is 20

  Scenario: 2.4.2: One model with one load and two generators (minP), three timestamps
    Given the study path is "modeler/study_2.4.2"
    When I run antares modeler
    Then the simulation succeeds
    And the optimal value of variable node1.gen1_up_0 is 1
    And the optimal value of variable node1.gen1_up_1 is 1
    And the optimal value of variable node1.gen1_up_2 is 1
    And the optimal value of variable node1.gen1_p_0 is 60
    And the optimal value of variable node1.gen1_p_1 is 60
    And the optimal value of variable node1.gen1_p_2 is 60
    And the optimal value of variable node1.gen2_up_0 is 1
    And the optimal value of variable node1.gen2_up_1 is 1
    And the optimal value of variable node1.gen2_up_2 is 1
    And the optimal value of variable node1.gen2_p_0 is 40
    And the optimal value of variable node1.gen2_p_1 is 40
    And the optimal value of variable node1.gen2_p_2 is 40

  Scenario: 2.4.3: Two libs, one timestamp
    Given the study path is "modeler/study_2.4.3"
    When I run antares modeler
    Then the simulation succeeds
    And the optimal value of variable node1.gen1_p_0 is 0
    And the optimal value of variable node1.gen2_p_0 is 100
    And the optimal value of variable node2.gen1_p_0 is 500
    And the optimal value of variable node2.gen1_up_0 is 1
    And the optimal value of variable node2.gen2_p_0 is 500
    And the optimal value of variable node2.gen2_up_0 is 1