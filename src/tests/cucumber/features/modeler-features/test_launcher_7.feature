Feature: 7 - End-to-end tests for electrolyser model

  Scenario: 7.1: One system with two nodes : one electric node (3 thermal clusters, 1 load, 1 wind) and one hydrogen node (1 electrolyser, 1 H2 load, 1 H2 back-up production)
    Given the modeler study path is "modeler/7_1"
    When I run antares modeler
    Then the simulation succeeds
    And the objective value is greater than 13466393 and lower than 13466493

  Scenario: 7.2: One system with one electric node (3 thermal clusters, 1 load, 1 wind, 1 DSR)
    Given the modeler study path is "modeler/7_2"
    When I run antares modeler
    Then the simulation succeeds
    And the objective value is greater than 13360159 and lower than 13360259

  Scenario: 7.3: One system with one electric node (3 thermal clusters, 1 load, 1 wind, 1 short-term storage)
    Given the modeler study path is "modeler/7_3"
    When I run antares modeler
    Then the simulation succeeds
    And the objective value is greater than 11971236 and lower than 11971336

  Scenario: 7.4: One system with two nodes : one electric node (3 thermal clusters, 1 load, 1 wind, 1 DSR, 1 short-term storage) and one hydrogen node (1 electrolyser, 1 H2 load, 1 H2 back-up production)
    Given the modeler study path is "modeler/7_4"
    When I run antares modeler
    Then the simulation succeeds
    And the objective value is greater than 14515576 and lower than 14515676
    