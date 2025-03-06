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
  Scenario: 065 Pumped storage plant -explicit model-01
    Given the solver study path is "Antares_Simulator_Tests_NR/short-tests/065 Pumped storage plant -explicit model-01"
    When I run antares simulator
    Then the simulation succeeds
    And the simulation takes less than 20 seconds
    And the expected value of the annual system cost is 2357680000
    And in area "& psp", during year 1, total hydro production is 1027107 MWh
    And in area "& psp", during year 1, total hydro pumping is 1424007 MWh
    And in area "& psp", during year 1, total balance is -396899 MWh
    And in area "& psp", during year 1, total spilled energy is 0 MWh
    And in area "west", during year 1, total spilled energy is 1677128 MWh
    And in area "west", during year 1, total unsupplied energy is 0 MWh
    And in area "west", during year 1, total balance is 4637689 MWh
    And in area "& psp", during year 2, total hydro production is 1124876 MWh
    And in area "& psp", during year 2, total hydro pumping is 1560045 MWh
    And in area "& psp", during year 2, total balance is -454236 MWh
    # Spilled energy is ok bc water value is set to 0
    And in area "& psp", during year 2, total spilled energy is 19079 MWh
    And in area "west", during year 2, total spilled energy is 149644 MWh
    And in area "west", during year 2, total unsupplied energy is 0 MWh
    And in area "west", during year 2, total balance is 571415 MWh

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
    And in area "& psp", during year 1, total hydro production is 1829 MWh
    # This hydro production is actually spilled but it's ok bc water value is set to 0
    And in area "& psp", during year 1, total spilled energy is 1829 MWh
    And in area "& psp", during year 1, total hydro pumping is 0 MWh
    And in area "& psp", during year 1, total balance is 0 MWh
    And in area "west", during year 1, total spilled energy is 2292484 MWh
    And in area "west", during year 1, total unsupplied energy is 0 MWh
    And in area "west", during year 1, total balance is 4171217 MWh
    And in area "& psp", during year 2, total hydro production is 1655 MWh
    # This hydro production is actually spilled but it's ok bc water value is set to 0
    And in area "& psp", during year 2, total spilled energy is 1655 MWh
    And in area "& psp", during year 2, total hydro pumping is 0 MWh
    And in area "& psp", during year 2, total balance is 0 MWh
    And in area "west", during year 2, total spilled energy is 430418 MWh
    And in area "west", during year 2, total unsupplied energy is 0 MWh
    And in area "west", during year 2, total balance is -101739 MWh

  @fast @short @hydro @storage
  Scenario: 066 Pumped storage plant -explicit model-02
    Given the solver study path is "Antares_Simulator_Tests_NR/short-tests/066 Pumped storage plant -explicit model-02"
    When I run antares simulator
    Then the simulation succeeds
    And the simulation takes less than 20 seconds
    And the expected value of the annual system cost is 2356260000
    And in area "& psp", during year 1, total hydro production is 965144 MWh
    And in area "& psp", during year 1, total hydro pumping is 1346274 MWh
    And in area "& psp", during year 1, total balance is -381129 MWh
    And in area "& psp", during year 1, total spilled energy is 0 MWh
    And in area "west", during year 1, total spilled energy is 1637756 MWh
    And in area "west", during year 1, total unsupplied energy is 0 MWh
    And in area "west", during year 1, total balance is 4673517 MWh
    # Hydro has to spill some energy bc of inflows
    And in area "& psp", during year 2, total spilled energy is 13791 MWh
    And in area "& psp", during year 2, total hydro production is 1078892 MWh
    And in area "& psp", during year 2, total hydro pumping is 1504516 MWh
    And in area "& psp", during year 2, total balance is -439406 MWh
    And in area "west", during year 2, total spilled energy is 144877 MWh
    And in area "west", during year 2, total unsupplied energy is 0 MWh
    And in area "west", during year 2, total balance is 621201 MWh

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
    And in area "& psp", during year 1, total hydro production is 0 MWh
    And in area "& psp", during year 1, total hydro pumping is 0 MWh
    And in area "& psp", during year 1, total balance is 0 MWh
    And in area "& psp", during year 1, total spilled energy is 0 MWh
    And in area "west", during year 1, total spilled energy is 2292484 MWh
    And in area "west", during year 1, total unsupplied energy is 0 MWh
    And in area "west", during year 1, total balance is 4171217 MWh
    And in area "& psp", during year 2, total hydro production is 0 MWh
    And in area "& psp", during year 2, total hydro pumping is 0 MWh
    And in area "& psp", during year 2, total balance is 0 MWh
    And in area "& psp", during year 2, total spilled energy is 0 MWh
    And in area "west", during year 2, total spilled energy is 430418 MWh
    And in area "west", during year 2, total unsupplied energy is 0 MWh
    And in area "west", during year 2, total balance is -101739 MWh