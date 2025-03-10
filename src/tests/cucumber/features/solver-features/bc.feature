Feature: bc

  @fast @short
  Scenario: BC enable
    Given the study path is "Antares_Simulator_Tests_NR/valid-v830/BindConst--infinity-on-thCluster"
    When I run antares simulator
    Then the simulation succeeds
    And the annual system cost is 0

  @fast @short
  Scenario: BC enable
    Given the study path is "Antares_Simulator_Tests_NR/valid-v830/BindConst-disableCluster"
    When I run antares simulator
    Then the simulation succeeds
    And the annual system cost is 0

  @fast @short
  Scenario: BC enable
    Given the study path is "Antares_Simulator_Tests_NR/valid-v830/BindConst-mustRunCluster"
    When I run antares simulator
    Then the simulation succeeds
    And the annual system cost is 0
