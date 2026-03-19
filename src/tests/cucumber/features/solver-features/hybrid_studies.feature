Feature: hybrid (simulator+modeler) studies

  @fast @short
  Scenario: 001 One node - passive + modeler test 1_1
    Given the solver study path is "Antares_Simulator_Tests_NR/hybrid/001 One node - passive"
    When I run antares simulator
    Then the simulation succeeds

  @fast @short
  Scenario: 002 Thermal fleet - Base + modeler test 1_1
    Given the solver study path is "Antares_Simulator_Tests_NR/hybrid/002 Thermal fleet - Base"
    When I run antares simulator
    Then the simulation succeeds
    And the annual system cost is 27288600
    And in area "AREA", during year 1, loss of load lasts 1 hours
    And in area "AREA", unsupplied energy on "2 JAN 09:00" of year 1 is of 52 MW

  @fast @short
  Scenario: Empty legacy node with one generator component and one load component (168h simplex)
    Given the solver study path is "Antares_Simulator_Tests_NR/hybrid/3_6_0"
    When I run antares simulator
    Then the simulation succeeds
    And the simulation takes less than 10 seconds
    # 100MW @ 0.3€/MW/h, for 1 week = 5040 €
    # for now, modeler costs does not figure in system cost txt
    And the annual system cost is 0
    And in area "NODE", during year 1, loss of load lasts 0 hours
    And the modeler outputs contain the following entries
      | block | component | output                 | timestep  | scenario | value |
      |       | gen1      | generation.flow_field  | 1-24      | 0-4      | 100   |
      |       | gen1      | generation.flow_field  | 6184-6230 | 0-4      | 100   |
      | 1-52  | load1     | consumption.flow_field |           | 0-4      | -100  |

  @fast @short
  Scenario: Legacy node with one legacy load (up to 5952 MW) and wind, and one generator component (max_p=6200) (168h simplex)
    # copy of short test 002, with no legacy thermal cluster, replaced by one component
    Given the solver study path is "Antares_Simulator_Tests_NR/hybrid/3_6_1"
    When I run antares simulator
    Then the simulation succeeds
    And the simulation takes less than 5 seconds
    # for now, modeler costs does not figure in system cost txt
    And the annual system cost is 0
    And in area "AREA", during year 1, loss of load lasts 0 hours
    And simulation tables match the references

  @fast @short
  Scenario: Legacy node with one legacy load (up to 5952 MW) and wind, and one generator component (max_p=5900) (168h simplex)
    # copy of previous case, with reduced max_p on generator => loss of load of 52MW on 1 hour
    Given the solver study path is "Antares_Simulator_Tests_NR/hybrid/3_6_2"
    When I run antares simulator
    Then the simulation succeeds
    And the simulation takes less than 5 seconds
    # for now, modeler costs does not figure in system cost txt
    And the annual system cost is 520000
    And in area "AREA", during year 1, loss of load lasts 1 hours
    And in area "AREA", during year 1, total unsupplied energy is 52 MWh

  @fast @short
  Scenario: Legacy node with one legacy thermal cluster, and one load component (constant load of 3000 MW)
    Given the solver study path is "Antares_Simulator_Tests_NR/hybrid/3_6_3"
    When I run antares simulator
    Then the simulation succeeds
    And the simulation takes less than 5 seconds
    # for now, modeler costs does not figure in system cost txt
    And the annual system cost is 17640000.0
    And in area "AREA", during year 1, loss of load lasts 0 hours
    And in area "AREA", during year 1, hourly production of "base" is always equal to 3000 MWh

  @fast @short
  Scenario: Legacy node with one legacy load (up to 5952 MW) and wind, and one generator component (168h simplex)
    # copy of test 3_6_1, both weeks are clones
    # except max_p of the generator component is 6200 in first week, 5900 in second
    Given the solver study path is "Antares_Simulator_Tests_NR/hybrid/3_6_4"
    When I run antares simulator
    Then the simulation succeeds
    And the simulation takes less than 5 seconds
    # for now, modeler costs does not figure in system cost txt
    And the annual system cost is 520000
    And in area "AREA", during year 1, week 1, loss of load lasts 0 hours
    And in area "AREA", during year 1, week 2, loss of load lasts 1 hours
    And in area "AREA", during year 1, total unsupplied energy is 52 MWh

  @fast @short
  Scenario: Invalid study - scenario-independent variable
    Given the solver study path is "Antares_Simulator_Tests_NR/invalid-studies/hybrid/Scenario-independent variable"
    When I run antares simulator
    Then the simulation fails
    And the message "Scenario-independent variables are not supported in hybrid studies" is reported in the logs

  @fast @short
  Scenario: Two studies with same structure should have the same objective value at each time step
    Given the study path 1 is "Antares_Simulator_Tests_NR/hybrid/14_1/five_steps_hybrid_fixed_load"
    And the study path 2 is "Antares_Simulator_Tests_NR/hybrid/14_1/five_steps_hybrid_flexible_load"
    When I run antares simulator on all studies
    Then all simulations succeed
    And for each time step, all studies have the same objective value


  @fast @short
  Scenario: Thermal capacity connection: on one node (= area, thermal cluster = continuous_generator_candidate), one timestep and one scenario with one candidate.

    Given the solver study path is "Antares_Simulator_Tests_NR/hybrid/13_1_thermal_add_on/"
    When I run antares simulator with --named-mps-problems
    Then the simulation succeeds
    And the simulation takes less than 15 seconds
    Then for each weekly problem, verify the "MaxGenerationFromCapacity" constraint exists for all time steps for thermal cluster continuous_generator_candidate in area area.
    And enforces: DispatchableProduction - thermal_add_on.p_max < 0 for the thermal capacity connection between GEMS and the continuous_generator_candidate thermal cluster in area area.

  @fast @short
  Scenario: Thermal capacity connection: on one node (= area, thermal cluster = continuous_generator_candidate), one timestep and one scenario with two candidates: one continuous and one discrete..

    Given the solver study path is "Antares_Simulator_Tests_NR/hybrid/13_2_thermal_add_on/"
    When I run antares simulator with --named-mps-problems
#    Then the simulation succeeds
#    And the simulation takes less than 15 seconds
    Then for each weekly problem, verify the "MaxGenerationFromCapacity" constraint exists for all time steps for thermal cluster continuous_generator_candidate in area area.
    And for each weekly problem, verify the "MaxGenerationFromCapacity" constraint exists for all time steps for thermal cluster discrete_generator_candidate in area area.
    And enforces: DispatchableProduction - thermal_add_on_continuous.p_max < 0 for the thermal capacity connection between GEMS and the continuous_generator_candidate thermal cluster in area area.
    And enforces: DispatchableProduction - thermal_add_on_discrete.p_max < 0 for the thermal capacity connection between GEMS and the discrete_generator_candidate thermal cluster in area area.

  @fast @short
  Scenario: A load from GEMS is taken into account in balance constraint

    Given the solver study path is "Antares_Simulator_Tests_NR/hybrid/3_8/"
    When I run antares simulator with --named-mps-problems
    Then the simulation succeeds
    And the simulation takes less than 10 seconds
	And the objective value is 186360
	Then for first week, area balance RHS (for area unique) is first -12, -13, -14, -15, -16, then equals constant -11