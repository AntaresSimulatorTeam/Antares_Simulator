Feature: medium tests

  @fast @medium @incomplete
  Scenario: 035 Mixed Expansion - Smart grid model 2
    Given the study path is "Antares_Simulator_Tests_NR/medium-tests/035 Mixed Expansion - Smart grid model 2"
    When I run antares simulator
    Then the simulation takes less than 30 seconds
    And the simulation succeeds
    And the expected value of the annual system cost is 3.725e+10
    And the minimum annual system cost is 3.642e+10
    And the maximum annual system cost is 4.011e+10
    And the annual system cost is
      | EXP       | STD       | MIN       | MAX       |
      | 3.725e+10 | 1.063e+09 | 3.642e+10 | 4.011e+10 |
    # TODO : add steps when we understand what this test is supposed to test