Feature: 7 - End-to-end tests for electrolyser model

  Scenario: 7.1: One system with two nodes : one electric node (3 thermal clusters, 1 load, 1 wind) and one hydrogen node (1 electrolyser, 1 H2 load, 1 H2 back-up production)
    Given the modeler study path is "modeler/7_1"
    When I run antares modeler
    Then the simulation succeeds
    And the objective value is 13466443



 