Feature: short tests

  @fast @short
  Scenario: 001 One node - passive
    Given the study path is "short-tests/001 One node - passive"
    When I run antares simulator
    Then the simulation takes less than 5 seconds
    And the simulation succeeds
    And the annual system cost is 0

  @fast @short
  Scenario: 002 Thermal fleet - Base
    Given the study path is "short-tests/002 Thermal fleet - Base"
    When I run antares simulator
    Then the simulation takes less than 5 seconds
    And the simulation succeeds
    And the annual system cost is 2.729e+7
    And in area "AREA", during year 1, loss of load lasts 1 hours
    And in area "AREA", unsupplied energy on "02 JAN 09:00" of year 1 is of 52 MW

  @fast @short
  Scenario: 003 Thermal fleet - Must-run
    Given the study path is "short-tests/003 Thermal fleet - Must-run"
    When I run antares simulator
    Then the simulation takes less than 5 seconds
    And the simulation succeeds
    And the annual system cost is 2.751e+7
    And in area "AREA", during year 1, loss of load lasts 1 hours
    And in area "AREA", unsupplied energy on "02 JAN 09:00" of year 1 is of 52 MW
    And in area "AREA", during year 1, hourly production on "non-dispatchable semi base" is always equal to 300 MWh

  @fast @short
  Scenario: 004 Thermal fleet - Partial must-run
    Given the study path is "short-tests/004 Thermal fleet - Partial must-run"
    When I run antares simulator
    Then the simulation takes less than 5 seconds
    And the simulation succeeds
    And the annual system cost is 2.751e+7
    And in area "AREA", during year 1, loss of load lasts 1 hours
    And in area "AREA", unsupplied energy on "02 JAN 09:00" of year 1 is of 52 MW
    And in area "AREA", during year 1, hourly production on "semi base" is always greater than 300 MWh

  @fast @short
  Scenario: 005 Thermal fleet - Minimum stable power and min up down times
    Given the study path is "short-tests/005 Thermal fleet - Minimum stable power and min up down times"
    When I run antares simulator
    Then the simulation takes less than 5 seconds
    And the simulation succeeds
    And the annual system cost is 2.75816e+07
    And in area "AREA", hourly production on "base" is always zero or greater than 900 MWh
    And in area "AREA", unit "base" respects a minimum up duration of 24 hours, and a minimum down duration of 24 hours
    And in area "AREA", hourly production on "semi base" is always zero or greater than 100 MWh
    And in area "AREA", unit "semi base" respects a minimum up duration of 6 hours, and a minimum down duration of 12 hours
    And in area "AREA", hourly production on "peak" is always zero or greater than 10 MWh
    And in area "AREA", unit "peak" respects a minimum up duration of 2 hours, and a minimum down duration of 2 hours

  @fast @short
  Scenario: 006 Thermal fleet - Extra costs
    # Like previous test, but with extra non-proportional (NP) costs
    # NP costs = 1756400 ; OP costs = 2.75816e+07 (like test 5) => Total cost = 2.9338e+07
    Given the study path is "short-tests/006 Thermal fleet - Extra costs"
    When I run antares simulator
    Then the simulation takes less than 5 seconds
    And the simulation succeeds
    And the annual system cost is 2.9338e+07
    And in area "AREA", during year 1, total non-proportional cost is 1756400
    And in area "AREA", hourly production on "base" is always zero or greater than 900 MWh
    And in area "AREA", unit "base" respects a minimum up duration of 24 hours, and a minimum down duration of 24 hours
    And in area "AREA", hourly production on "semi base" is always zero or greater than 100 MWh
    And in area "AREA", unit "semi base" respects a minimum up duration of 6 hours, and a minimum down duration of 12 hours
    And in area "AREA", hourly production on "peak" is always zero or greater than 10 MWh
    And in area "AREA", unit "peak" respects a minimum up duration of 2 hours, and a minimum down duration of 2 hours

  @fast @short
  Scenario: 021 Four areas - DC law
    Given the study path is "short-tests/021 Four areas - DC law"
    When I run antares simulator
    Then the simulation takes less than 20 seconds
    And the simulation succeeds
    And the annual system cost is
      | EXP       | STD       | MIN       | MAX       |
      | 7.972e+10 | 2.258e+10 | 5.613e+10 | 1.082e+11 |