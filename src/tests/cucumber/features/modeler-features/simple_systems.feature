Feature: Simple modeler systems

  @fast
  Scenario: cyclic
    Given the modeler study path is "modeler/simple_system_cyclic"
    When I run antares modeler
    Then the simulation succeeds
    And the objective value is 1500

  @fast
  Scenario: drop
    Given the modeler study path is "modeler/simple_system_drop"
    When I run antares modeler
    Then the simulation succeeds
    And the objective value is 300
