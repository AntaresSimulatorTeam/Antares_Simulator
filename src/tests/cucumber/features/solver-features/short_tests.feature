Feature: short tests

  @fast @short
  Scenario: 001 One node - passive
    Given the solver study path is "Antares_Simulator_Tests_NR/short-tests/001 One node - passive"
    When I run antares simulator
    Then the simulation succeeds
    And the simulation takes less than 5 seconds
    And the annual system cost is 0

  @fast @short
  Scenario: 002 Thermal fleet - Base
    Given the solver study path is "Antares_Simulator_Tests_NR/short-tests/002 Thermal fleet - Base"
    When I run antares simulator
    Then the simulation succeeds
    And the simulation takes less than 5 seconds
    And the annual system cost is 2.729e+7
    And in area "AREA", during year 1, loss of load lasts 1 hours
    And in area "AREA", unsupplied energy on "2 JAN 09:00" of year 1 is of 52 MW

  @fast @short
  Scenario: 003 Thermal fleet - Must-run
    Given the solver study path is "Antares_Simulator_Tests_NR/short-tests/003 Thermal fleet - Must-run"
    When I run antares simulator
    Then the simulation succeeds
    And the simulation takes less than 5 seconds
    And the annual system cost is 2.751e+7
    And in area "AREA", during year 1, loss of load lasts 1 hours
    And in area "AREA", unsupplied energy on "2 JAN 09:00" of year 1 is of 52 MW
    And in area "AREA", during year 1, hourly production of "non-dispatchable semi base" is always equal to 300 MWh

  @fast @short
  Scenario: 004 Thermal fleet - Partial must-run
    Given the solver study path is "Antares_Simulator_Tests_NR/short-tests/004 Thermal fleet - Partial must-run"
    When I run antares simulator
    Then the simulation succeeds
    And the simulation takes less than 5 seconds
    And the annual system cost is 2.751e+7
    And in area "AREA", during year 1, loss of load lasts 1 hours
    And in area "AREA", unsupplied energy on "2 JAN 09:00" of year 1 is of 52 MW
    And in area "AREA", during year 1, hourly production of "semi base" is always greater than 300 MWh

  @fast @short
  Scenario: 005 Thermal fleet - Minimum stable power and min up down times
    Given the solver study path is "Antares_Simulator_Tests_NR/short-tests/005 Thermal fleet - Minimum stable power and min up down times"
    When I run antares simulator
    Then the simulation succeeds
    And the simulation takes less than 5 seconds
    And the annual system cost is 2.75816e+07
    And in area "AREA", the units of "base" produce between 400 and 900 MWh hourly
    And in area "AREA", the units of "semi base" produce between 100 and 300 MWh hourly
    And in area "AREA", the units of "peak" produce between 10 and 100 MWh hourly
    # Ideally, we would also check min up & down durations in this test. But is not possible, since clusters defined
    # in this test have a unitcount > 1
    # TODO : create similar tests with unitcount = 1, and implement the following steps:
    # And in area "AREA", unit "base" respects a minimum up duration of 24 hours, and a minimum down duration of 24 hours
    # And in area "AREA", unit "semi base" respects a minimum up duration of 6 hours, and a minimum down duration of 12 hours
    # And in area "AREA", unit "peak" respects a minimum up duration of 2 hours, and a minimum down duration of 2 hours

  @fast @short
  Scenario: 006 Thermal fleet - Extra costs
    # Like previous test, but with extra non-proportional (NP) costs
    # NP costs = 1756400 ; OP costs = 2.75816e+07 (like test 5) => Total cost = 2.9338e+07
    Given the solver study path is "Antares_Simulator_Tests_NR/short-tests/006 Thermal fleet - Extra costs"
    When I run antares simulator
    Then the simulation succeeds
    And the simulation takes less than 5 seconds
    And the annual system cost is 2.9338e+07
    And in area "AREA", during year 1, total non-proportional cost is 1756400
    And in area "AREA", the units of "base" produce between 400 and 900 MWh hourly
    And in area "AREA", the units of "semi base" produce between 100 and 300 MWh hourly
    And in area "AREA", the units of "peak" produce between 10 and 100 MWh hourly
    # Ideally, we would also check min up & down durations in this test. But is not possible, since clusters defined
    # in this test have a unitcount > 1
    # TODO : create similar tests with unitcount = 1, and implement the following steps:
    # And in area "AREA", unit "base" respects a minimum up duration of 24 hours, and a minimum down duration of 24 hours
    # And in area "AREA", unit "semi base" respects a minimum up duration of 6 hours, and a minimum down duration of 12 hours
    # And in area "AREA", unit "peak" respects a minimum up duration of 2 hours, and a minimum down duration of 2 hours

  @fast @short
  Scenario: 007 Thermal fleet - Fast unit commitment
    # This example is the first of a set of two that are comparing the two unit-commitment modes of Antares.
    # Fast mode
    # => overall cost is not great, there are a lot of startups, and min up & down time are considered equal
    Given the solver study path is "Antares_Simulator_Tests_NR/short-tests/007 Thermal fleet - Fast unit commitment"
    When I run antares simulator
    Then the simulation succeeds
    And the simulation takes less than 5 seconds
    And the annual system cost is 2.98912e+07
    And in area "AREA", during year 1, total non-proportional cost is 1861400
    And in area "AREA", the units of "base" produce between 400 and 900 MWh hourly
    And in area "AREA", the units of "semi base" produce between 100 and 300 MWh hourly
    And in area "AREA", the units of "peak" produce between 10 and 100 MWh hourly
    And in area "AREA", during year 1, loss of load lasts 1 hours
    And in area "AREA", unsupplied energy on "2 JAN 09:00" of year 1 is of 52 MW
    # Ideally, we would also check min up & down durations in this test. But is not possible, since clusters defined
    # in this test have a unitcount > 1
    # TODO : create similar tests with unitcount = 1, and implement the following steps:
    # And in area "AREA", unit "base" respects a minimum up duration of 24 hours, and a minimum down duration of 24 hours
    # And in area "AREA", unit "semi base" respects a minimum up duration of 12 hours, and a minimum down duration of 12 hours
    # And in area "AREA", unit "peak" respects a minimum up duration of 2 hours, and a minimum down duration of 2 hours

  @fast @short
  Scenario: 008 Thermal fleet - Accurate unit commitment
    # Like previous test, but with unit commitment
    # => overall cost is better, there are less startups, and min up & down time are not equal
    Given the solver study path is "Antares_Simulator_Tests_NR/short-tests/008 Thermal fleet - Accurate unit commitment"
    When I run antares simulator
    Then the simulation succeeds
    And the simulation takes less than 5 seconds
    And the annual system cost is 2.97339e+07
    And in area "AREA", during year 1, total non-proportional cost is 1680900
    And in area "AREA", the units of "base" produce between 400 and 900 MWh hourly
    And in area "AREA", the units of "semi base" produce between 100 and 300 MWh hourly
    And in area "AREA", the units of "peak" produce between 10 and 100 MWh hourly
    And in area "AREA", during year 1, loss of load lasts 1 hours
    And in area "AREA", unsupplied energy on "2 JAN 09:00" of year 1 is of 52 MW
    # Ideally, we would also check min up & down durations in this test. But is not possible, since clusters defined
    # in this test have a unitcount > 1
    # TODO : create similar tests with unitcount = 1, and implement the following steps:
    # And in area "AREA", unit "base" respects a minimum up duration of 24 hours, and a minimum down duration of 24 hours
    # And in area "AREA", unit "semi base" respects a minimum up duration of 6 hours, and a minimum down duration of 12 hours
    # And in area "AREA", unit "peak" respects a minimum up duration of 2 hours, and a minimum down duration of 2 hours

  @fast @short
  Scenario: 021 Four areas - DC law
    Given the solver study path is "Antares_Simulator_Tests_NR/short-tests/021 Four areas - DC law"
    When I run antares simulator
    Then the simulation succeeds
    And the simulation takes less than 20 seconds
    And the annual system cost is
      | EXP       | STD       | MIN       | MAX       |
      | 7.972e+10 | 2.258e+10 | 5.613e+10 | 1.082e+11 |

  @fast @short @hydro @storage
  Scenario: 015 TS generation - Hydro power
    Given the solver study path is "Antares_Simulator_Tests_NR/short-tests/015 TS generation - Hydro power"
    When I run antares simulator
    Then the simulation succeeds
    And the simulation takes less than 20 seconds
    And the expected value of the annual system cost is 0
    And the annual results are
      | area         | year | hydro production | hydro pumping | balance | spilled energy | unsupplied energy |
      | hydro node 1 | 1    | 29423845         | 0             | 0       | 67243352       | 0                 |
      | hydro node-2 | 1    | 26758144         | 0             | 0       | 63506522       | 0                 |
      | hydro node-3 | 1    | 35188296         | 0             | 0       | 80265517       | 0                 |

  @fast @short @useless
  Scenario: 053 System Map Editor - 1
    # This was meant to be a GUI training scenario. Keeping it simple: only checking if simulation works & overall cost.
    Given the solver study path is "Antares_Simulator_Tests_NR/short-tests/053 System Map Editor - 1"
    When I run antares simulator
    Then the simulation succeeds
    And the simulation takes less than 20 seconds
    And the expected value of the annual system cost is 668214000000

  @fast @short @useless
  Scenario: 054 System Map Editor -2
    # This was meant to be a GUI training scenario. Keeping it simple: only checking if simulation works & overall cost.
    Given the solver study path is "Antares_Simulator_Tests_NR/short-tests/054 System Map Editor -2"
    When I run antares simulator
    Then the simulation succeeds
    And the simulation takes less than 20 seconds
    And the expected value of the annual system cost is 2485600000000

  @fast @short @useless
  Scenario: 055 System Map Editor - 3
    # This was meant to be a GUI training scenario. Keeping it simple: only checking if simulation works & overall cost.
    Given the solver study path is "Antares_Simulator_Tests_NR/short-tests/055 System Map Editor - 3"
    When I run antares simulator
    Then the simulation succeeds
    And the simulation takes less than 20 seconds
    And the expected value of the annual system cost is 760011000000

  @fast @short @useless
  Scenario: 056 System Map Editor - 4
    # This was meant to be a GUI training scenario. Keeping it simple: only checking if simulation works & overall cost.
    Given the solver study path is "Antares_Simulator_Tests_NR/short-tests/056 System Map Editor - 4"
    When I run antares simulator
    Then the simulation succeeds
    And the simulation takes less than 20 seconds
    And the expected value of the annual system cost is 2485600000000

  @fast @short @hydro @storage
  Scenario: 065 Pumped storage plant -explicit model-01
    Given the solver study path is "Antares_Simulator_Tests_NR/short-tests/065 Pumped storage plant -explicit model-01"
    When I run antares simulator
    Then the simulation succeeds
    And the simulation takes less than 20 seconds
    And the expected value of the annual system cost is 2357680000
    And the annual results are
      | area  | year | hydro production | hydro pumping | balance | spilled energy | unsupplied energy |
      | & psp | 1    | 1027107          | 1424007       | -396899 | 0              |                   |
      | west  | 1    |                  |               | 4637689 | 1677128        | 0                 |
      | & psp | 2    | 1124876          | 1560045       | -454236 | 19079          |                   |
      | west  | 2    |                  |               | 571415  | 149644         | 0                 |
    # Spilled energy in "& psp" is ok because water value is set to 0

  @fast @short @hydro @storage
  Scenario: 065.bis Pumped storage plant -explicit model-01 - zero transmission capacity
    # Like the previous case, but with no transmission capacity between "west" and "& psp"
    # This makes overall system cost go up (like described in the study notes)
    Given the solver study path is a copy of "Antares_Simulator_Tests_NR/short-tests/065 Pumped storage plant -explicit model-01"
    And the transmission-capacities of link "& psp" are set to "ignore"
    When I run antares simulator
    Then the simulation succeeds
    And the simulation takes less than 20 seconds
    And the expected value of the annual system cost is 2575240000
    And the annual results are
      | area  | year | hydro production | hydro pumping | balance | spilled energy | unsupplied energy |
      | & psp | 1    | 1829             | 0             | 0       | 1829           |                   |
      | west  | 1    |                  |               | 4171217 | 2292484        | 0                 |
      | & psp | 2    | 1655             | 0             | 0       | 1655           |                   |
      | west  | 2    |                  |               | -101739 | 430418         | 0                 |
    # Hydro production in "& psp" is actually spilled but it's ok because water value is set to 0

  @fast @short @hydro @storage
  Scenario: 066 Pumped storage plant -explicit model-02
    Given the solver study path is "Antares_Simulator_Tests_NR/short-tests/066 Pumped storage plant -explicit model-02"
    When I run antares simulator
    Then the simulation succeeds
    And the simulation takes less than 20 seconds
    And the expected value of the annual system cost is 2356260000
    And the annual results are
      | area  | year | hydro production | hydro pumping | balance | spilled energy | unsupplied energy |
      | & psp | 1    | 965144           | 1346274       | -381129 | 0              |                   |
      | west  | 1    |                  |               | 4673517 | 1637756        | 0                 |
      | & psp | 2    | 1078892          | 1504516       | -439406 | 13791          |                   |
      | west  | 2    |                  |               | 621201  | 144877         | 0                 |
    # Hydro has to spill some energy in "& psp" year 2 because of inflows

  @fast @short @hydro @storage
  Scenario: 066.bis Pumped storage plant -explicit model-02 - zero transmission capacity
    # Like the previous case, but with no transmission capacity between "west" and "& psp"
    # This makes overall system cost go up (like described in the study notes)
    Given the solver study path is a copy of "Antares_Simulator_Tests_NR/short-tests/066 Pumped storage plant -explicit model-02"
    And the transmission-capacities of link "& psp" are set to "ignore"
    When I run antares simulator
    Then the simulation succeeds
    And the simulation takes less than 20 seconds
    And the expected value of the annual system cost is 2575240000
    And the annual results are
      | area  | year | hydro production | hydro pumping | balance | spilled energy | unsupplied energy |
      | & psp | 1    | 0                | 0             | 0       | 0              |                   |
      | west  | 1    |                  |               | 4171217 | 2292484        | 0                 |
      | & psp | 2    | 0                | 0             | 0       | 0              |                   |
      | west  | 2    |                  |               | -101739 | 430418         | 0                 |
