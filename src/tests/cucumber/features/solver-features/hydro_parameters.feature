Feature: hydro parameters loading (hydro.ini / PartHydro::LoadIniFile)

  # A single base study (hydro-parameters) is reused: a final-levels variant with
  # a monthly-stepped load profile and a 500 MW / 50 €/MWh thermal cluster.
  # Each scenario takes a temporary copy of the study, overrides one hydro.ini
  # section, then runs the solver.
  #
  # Note: parameters that participate only in soft hydro heuristics (the four
  # breakdown / modulation parameters, follow-load, hard-bounds, leeway-low/up,
  # power-to-level, overflow-spilled-cost-difference) do not change the simulation
  # output in this study setup because the final-levels feature pins start/end
  # reservoir levels and the optimizer has enough slack to ignore soft targets.
  # The dedicated unit test in test-hydro-common.cpp covers loader regressions for
  # *every* section; the scenarios below verify end-to-end propagation where it
  # is observable.

  @fast @short
  Scenario Outline: hydro section "<section>" value "<value>" loads and the simulation is stable
    Given the solver study path is a copy of "Antares_Simulator_Tests_NR/hydro/hydro-parameters"
    And in input "hydro/hydro.ini" section "<section>" variable "area" is set to "<value>"
    When I run antares simulator
    Then the simulation succeeds
    And the simulation takes less than 5 seconds
    And the expected value of the annual system cost is <exp_cost>
    And in area "area", during year 1, total hydro production is <prod1> MWh
    And in area "area", during year 2, total hydro production is <prod2> MWh
    And in area "area", during year 1, total hydro pumping is <pump1> MWh
    And in area "area", during year 2, total hydro pumping is <pump2> MWh

    Examples:
      | section                          | value         | exp_cost  | prod1  | prod2  | pump1 | pump2 |
      | inter-daily-breakdown            | 3.000000      | 106823000 | 138751 | 45158  | 38750 | 35670 |
      | intra-daily-modulation           | 3.000000      | 106823000 | 138751 | 45158  | 38750 | 35670 |
      | inter-monthly-breakdown          | 2.000000      | 106823000 | 138751 | 45158  | 38750 | 35670 |
      | reservoir                        | false         | 106830000 | 0      | 109200 | 0     | 0     |
      | follow load                      | true          | 106823000 | 138751 | 45158  | 38750 | 35670 |
      | use water                        | true          | 106827000 | 0      | 114196 | 0     | 4881  |
      | hard bounds                      | true          | 106823000 | 138751 | 45158  | 38750 | 35670 |
      | power to level                   | true          | 106823000 | 138751 | 45158  | 38750 | 35670 |
      | use leeway                       | true          | 106823000 | 100001 | 9488   | 0     | 0     |
      | leeway low                       | 0.500000      | 106823000 | 138751 | 45158  | 38750 | 35670 |
      | leeway up                        | 1.500000      | 106823000 | 138751 | 45158  | 38750 | 35670 |
      | pumping efficiency               | 0.750000      | 106823000 | 100001 | 9488   | 0     | 0     |
      | overflow spilled cost difference | 0.500000      | 106823000 | 138751 | 45158  | 38750 | 35670 |

  @fast @short
  Scenario: section "use heuristic" = false (combined with "use water" = true to remain valid)
    Given the solver study path is a copy of "Antares_Simulator_Tests_NR/hydro/hydro-parameters"
    And in input "hydro/hydro.ini" section "use heuristic" variable "area" is set to "false"
    And in input "hydro/hydro.ini" section "use water" variable "area" is set to "true"
    When I run antares simulator
    Then the simulation succeeds
    And the expected value of the annual system cost is 100830000
    And in area "area", during year 1, total hydro production is 236100 MWh
    And in area "area", during year 2, total hydro production is 155026 MWh
    And in area "area", during year 1, total hydro pumping is 36100 MWh
    And in area "area", during year 2, total hydro pumping is 5850 MWh

  # The two parameters below cannot take a non-default value under the
  # final-levels feature of this base study: any change is rejected at
  # hydro validation time. The simulation failing with the expected error
  # proves the parameter was loaded and applied.

  @fast @short
  Scenario: section "initialize reservoir date" = 6 is rejected by final-levels
    Given the solver study path is a copy of "Antares_Simulator_Tests_NR/hydro/hydro-parameters"
    And in input "hydro/hydro.ini" section "initialize reservoir date" variable "area" is set to "6"
    When I run antares simulator
    Then the simulation fails
    And the message "Hydro allocation must start on the 1st simulation month" is reported in the logs

  @fast @short
  Scenario: section "reservoir capacity" = 350000 is rejected by final-levels
    Given the solver study path is a copy of "Antares_Simulator_Tests_NR/hydro/hydro-parameters"
    And in input "hydro/hydro.ini" section "reservoir capacity" variable "area" is set to "350000.000000"
    When I run antares simulator
    Then the simulation fails
    And the message "Incompatible total inflows" is reported in the logs
