Feature: 8 - End-to-end tests for Andromede V1 models - hybrid mode

  @fast @short
  Scenario: 8.1 : Study 028b Electrolyser
    # - V8.6 Small test case with two Legacy nodes (areas). One electric node :  3 thermal clusters, 1 load, 1 wind, one H2 node :  1 H2 load, 1 H2 back-up production, one electrolyser as modeler component
    Given the solver study path is "Antares_Simulator_Tests_NR/hybrid/8_1"
    When I run antares simulator
    Then the simulation succeeds
    # Hour 0
    And in area "west", unsupplied energy on "1 JAN 00:00" of year 1 is of 0 MW
    And in area "west-H2", unsupplied energy on "1 JAN 00:00" of year 1 is of 0 MW
    And in area "west", during year 1, hourly production of "b" for hour 0 is equal to 3020 MWh
    And in area "west", during year 1, hourly production of "sb" for hour 0 is equal to 0 MWh
    And in area "west", during year 1, hourly production of "p" for hour 0 is equal to 0 MWh
    And in area "west-H2", during year 1, hourly production of "SMR" for hour 0 is equal to 10 MWh
    # At hour 32, the electrolyzer provides 94 MW to west-H2
    And in area "west", unsupplied energy on "2 JAN 08:00" of year 1 is of 0 MW
    And in area "west-H2", unsupplied energy on "2 JAN 08:00" of year 1 is of 0 MW
    And in area "west", during year 1, hourly production of "b" for hour 32 is equal to 6000 MWh
    And in area "west", during year 1, hourly production of "sb" for hour 32 is equal to 0 MWh
    And in area "west", during year 1, hourly production of "p" for hour 32 is equal to 0 MWh
    And in area "west-H2", during year 1, hourly production of "SMR" for hour 32 is equal to 126 MWh

  @fast @short
  Scenario: 8.2: Study 028b DSR - V8.6
   # Small test case with one node (3 thermal clusters, 1 load, 1 wind, 1 demand-side response model)
    Given the solver study path is "Antares_Simulator_Tests_NR/hybrid/8_2"
    When I run antares simulator
    Then the simulation succeeds
    And the annual system cost is 13119200
    # Hour 0, DSR curtails 0 MW of load
    And in area "west", unsupplied energy on "1 JAN 00:00" of year 1 is of 0 MW
    And in area "west", during year 1, hourly production of "b" for hour 0 is equal to 3020 MWh
    And in area "west", during year 1, hourly production of "sb" for hour 0 is equal to 0 MWh
    And in area "west", during year 1, hourly production of "p" for hour 0 is equal to 0 MWh
    # At hour 34, DSR curtails 300 MW of load
    And in area "west", unsupplied energy on "2 JAN 08:00" of year 1 is of 0 MW
    And in area "west", during year 1, hourly production of "b" for hour 34 is equal to 6000 MWh
    And in area "west", during year 1, hourly production of "sb" for hour 34 is equal to 775 MWh
    And in area "west", during year 1, hourly production of "p" for hour 34 is equal to 0 MWh

  @fast @short
  Scenario: 8.3: Study 028b Storage - V8.6
   # Small test case with one node (3 thermal clusters, 1 one load, 1 wind, 1 short-term storage)
    Given the solver study path is "Antares_Simulator_Tests_NR/hybrid/8_3"
    When I run antares simulator
    Then the simulation succeeds
    And the annual system cost is 11971300