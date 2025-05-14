Feature: 8 - End-to-end tests for Andromede V1 models - hybrid mode

  Scenario: 8.1: One system with two nodes : one electric node (3 thermal clusters, 1 load, 1 wind) and one hydrogen node (1 H2 load, 1 H2 back-up production) in legacy mode - electrolyser as modeler component
    Given the solver study path is "Antares_Simulator_Tests_NR/hybrid/8_1"
    When I run antares simulator
    Then the simulation succeeds
    And the annual system cost is 13466443
 