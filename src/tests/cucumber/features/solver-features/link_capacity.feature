Feature: link capacity connections (hybrid studies)
  # Study hybrid/link_capacity: legacy areas a and b joined by a legacy link a - b whose NTC is
  # null, so its direct (a->b) and indirect (b->a) capacities come only from the link_ab
  # component: direct = 20 MW, indirect = 150 MW.
  #   load_a = 100 MW, gen_a: 200 MW @ 10, load_b = 50 MW, gen_b: 200 MW @ 1
  # Importing into a is cheaper than local generation, up to the indirect capacity.

  @short
  Scenario: Link capacity connection - the component gives capacity to a legacy link with a null NTC
    # The legacy NTC of link a - b is 0 in both directions: its limits come only from the
    # link_ab component (direct = 20 MW a->b, indirect = 150 MW b->a).
    # a needs 100 MW. Importing from b (cost 1) is cheaper than gen_a (cost 10) and fits in the
    # 150 MW indirect capacity: flow a->b = -100, gen_b = 100 + 50 (load_b) = 150, gen_a = 0.
    Given the solver study path is "Antares_Simulator_Tests_NR/hybrid/link_capacity"
    When I run antares simulator with --output=all
    Then the simulation succeeds
    And the modeler outputs contain the following entries
      | block | component | output                                | timestep | scenario | value |
      |       | a_b_link  | flow                                  | 0-167    | 0        | -100  |
      |       | gen_b     | p                                     | 0-167    | 0        | 150   |
      |       | gen_a     | p                                     | 0-167    | 0        | 0     |
      |       | a_b_link  | is_indirectly_congested               | 0-167    | 0        | 0     |
    And on link "a - b", during year 1, the hourly "CONG. PROB +" is 0 for every hour
    And on link "a - b", during year 1, the hourly "CONG. PROB -" is 0 for every hour

  @short
  Scenario: Link capacity connection - a binding indirect capacity limits the flow and sets the congestion
    # Same study with the indirect capacity reduced to 60 MW: a imports 60 MW only, the 40 MW
    # left come from gen_a (cost 10). Prices: 1 in b, 10 in a, so the congestion fee is
    # (10 - 1) * 60 = 540 and the link is congested in the indirect direction at every hour.
    # CONG. PROB must use the solved capacity (60 MW), not the null legacy NTC.
    Given the solver study path is a copy of "Antares_Simulator_Tests_NR/hybrid/link_capacity"
    And in input "system.yml" the parameter "indirect" of component "link_ab" is set to "60"
    When I run antares simulator with --output=all
    Then the simulation succeeds
    And the modeler outputs contain the following entries with relative tolerance 0.001
      | block | component | output                  | timestep | scenario | value |
      |       | a_b_link  | flow                    | 0-167    | 0        | -60   |
      |       | gen_b     | p                       | 0-167    | 0        | 110   |
      |       | gen_a     | p                       | 0-167    | 0        | 40    |
      |       | a_node    | price                   | 0-167    | 0        | 10    |
      |       | b_node    | price                   | 0-167    | 0        | 1     |
      |       | a_b_link  | is_indirectly_congested | 0-167    | 0        | 1     |
      |       | a_b_link  | is_directly_congested   | 0-167    | 0        | 0     |
      |       | a_b_link  | alg_congestion_fee      | 0-167    | 0        | 540   |
    And on link "a - b", during year 1, the hourly "CONG. PROB +" is 0 for every hour
    And on link "a - b", during year 1, the hourly "CONG. PROB -" is 100 for every hour

  @short
  Scenario: Link capacity connection - the simulation tables match the legacy mc-ind outputs
    Given the solver study path is "Antares_Simulator_Tests_NR/hybrid/link_capacity"
    When I run antares simulator with --output=all
    Then the simulation succeeds
    And the simulation table matches the legacy mc-ind output for year 1

  # ---- Bounds and direction ----------------------------------------------------------------

  @short
  Scenario: Link capacity connection - a binding direct capacity limits the export
    # gen_a becomes cheaper (0.5) than gen_b (1): a exports to b up to the direct capacity
    # (20 MW). gen_a = 100 + 20, gen_b = 50 - 20.
    Given the solver study path is a copy of "Antares_Simulator_Tests_NR/hybrid/link_capacity"
    And in input "system.yml" the parameter "prop_cost" of component "gen_a" is set to "0.5"
    When I run antares simulator with --output=all
    Then the simulation succeeds
    And the modeler outputs contain the following entries with relative tolerance 0.001
      | block | component | output                  | timestep | scenario | value |
      |       | a_b_link  | flow                    | 0-167    | 0        | 20    |
      |       | gen_a     | p                       | 0-167    | 0        | 120   |
      |       | gen_b     | p                       | 0-167    | 0        | 30    |
      |       | a_b_link  | is_directly_congested   | 0-167    | 0        | 1     |
      |       | a_b_link  | is_indirectly_congested | 0-167    | 0        | 0     |
    And on link "a - b", during year 1, the hourly "CONG. PROB +" is 100 for every hour
    And on link "a - b", during year 1, the hourly "CONG. PROB -" is 0 for every hour

  @short
  Scenario: Link capacity connection - a null capacity in one direction forbids that flow
    Given the solver study path is a copy of "Antares_Simulator_Tests_NR/hybrid/link_capacity"
    And in input "system.yml" the parameter "indirect" of component "link_ab" is set to "0"
    When I run antares simulator with --output=all
    Then the simulation succeeds
    And the modeler outputs contain the following entries with relative tolerance 0.001
      | block | component | output | timestep | scenario | value |
      |       | a_b_link  | flow   | 0-167    | 0        | 0     |
      |       | gen_a     | p      | 0-167    | 0        | 100   |
      |       | gen_b     | p      | 0-167    | 0        | 50    |

  @short
  Scenario: Link capacity connection - the marginal cost of a congested link is the price spread
    Given the solver study path is a copy of "Antares_Simulator_Tests_NR/hybrid/link_capacity"
    And in input "system.yml" the parameter "indirect" of component "link_ab" is set to "60"
    When I run antares simulator with --output=all
    Then the simulation succeeds
    And on link "a - b", during year 1, the hourly "MARG. COST" is 9 for every hour

  @short
  Scenario: Link capacity connection - a capacity too low for a demand without slack is infeasible
    # Modeler loads have no unsupplied-energy variable: with no local generation and only 60 MW
    # of import capacity, the 100 MW of load_a cannot be met.
    Given the solver study path is a copy of "Antares_Simulator_Tests_NR/hybrid/link_capacity"
    And in input "system.yml" the parameter "max_p" of component "gen_a" is set to "0"
    And in input "system.yml" the parameter "indirect" of component "link_ab" is set to "60"
    When I run antares simulator
    Then the simulation fails

  # ---- Replacement of the legacy NTC ---------------------------------------------------------

  @short
  Scenario Outline: Link capacity connection - the component capacity replaces a non-null legacy NTC
    # The legacy NTC (<legacy> MW both ways) is ignored in favour of the component capacity.
    Given the solver study path is a copy of "Antares_Simulator_Tests_NR/hybrid/link_capacity"
    And in input "links/a/b.txt" the 8760 lines are all set to "<legacy>\t<legacy>\t0\t0\t0\t0\t0\t0"
    And in input "links/a/capacities/b_direct.txt" the 8760 lines are all set to "<legacy>"
    And in input "links/a/capacities/b_indirect.txt" the 8760 lines are all set to "<legacy>"
    And in input "system.yml" the parameter "indirect" of component "link_ab" is set to "<capacity>"
    When I run antares simulator with --output=all
    Then the simulation succeeds
    And the modeler outputs contain the following entries with relative tolerance 0.001
      | block | component | output | timestep | scenario | value  |
      |       | a_b_link  | flow   | 0-167    | 0        | <flow> |

    Examples: legacy NTC larger or smaller than the component capacity
      | legacy | capacity | flow |
      | 500    | 60       | -60  |
      | 30     | 150      | -100 |

  # ---- Hurdle costs --------------------------------------------------------------------------

  @short
  Scenario: Link capacity connection - a hurdle cost makes local generation cheaper than importing
    # Importing costs 1 (gen_b) + 12 (hurdle on the indirect direction) = 13 > 10 (gen_a).
    Given the solver study path is a copy of "Antares_Simulator_Tests_NR/hybrid/link_capacity"
    And in input "links/a/properties.ini" section "b" variable "hurdles-cost" is set to "true"
    And in input "links/a/b_parameters.txt" the 8760 lines are all set to "1\t12\t0\t0\t0\t0"
    When I run antares simulator with --output=all
    Then the simulation succeeds
    And the modeler outputs contain the following entries with relative tolerance 0.001
      | block | component | output | timestep | scenario | value |
      |       | a_b_link  | flow   | 0-167    | 0        | 0     |
      |       | gen_a     | p      | 0-167    | 0        | 100   |
      |       | gen_b     | p      | 0-167    | 0        | 50    |

  @short
  Scenario: Link capacity connection - hurdle costs are charged on the capacity-bounded flow
    # indirect hurdle = 5: importing (1 + 5 = 6) still beats gen_a (10). The import is capped at
    # 60 MW by the component, so the hurdle cost is 60 * 5 = 300 per hour.
    Given the solver study path is a copy of "Antares_Simulator_Tests_NR/hybrid/link_capacity"
    And in input "links/a/properties.ini" section "b" variable "hurdles-cost" is set to "true"
    And in input "links/a/b_parameters.txt" the 8760 lines are all set to "1\t5\t0\t0\t0\t0"
    And in input "system.yml" the parameter "indirect" of component "link_ab" is set to "60"
    When I run antares simulator with --output=all
    Then the simulation succeeds
    And the modeler outputs contain the following entries with relative tolerance 0.001
      | block | component | output | timestep | scenario | value |
      |       | a_b_link  | flow   | 0-167    | 0        | -60   |
      |       | gen_a     | p      | 0-167    | 0        | 40    |
    And on link "a - b", during year 1, the hourly "HURDLE COST" is 300 for every hour

  # ---- Invalid studies -----------------------------------------------------------------------

  @short
  Scenario: Invalid study - link capacity connection with the reverse orientation of the link
    Given the solver study path is a copy of "Antares_Simulator_Tests_NR/hybrid/link_capacity"
    And in input "system.yml" the text "      from: a\n      to: b" is replaced by "      from: b\n      to: a"
    When I run antares simulator
    Then the simulation fails
    And the message "use Antares lexicographical direction" is reported in the logs

  @short
  Scenario: Invalid study - link capacity connection to an unknown link
    Given the solver study path is a copy of "Antares_Simulator_Tests_NR/hybrid/link_capacity"
    And in input "system.yml" the text "      to: b" is replaced by "      to: unknown"
    When I run antares simulator
    Then the simulation fails
    And the message "Unknown link 'a -> unknown'" is reported in the logs

  @short
  Scenario: Invalid study - link capacity connection on a port type without link-capacity-connection fields
    Given the solver study path is a copy of "Antares_Simulator_Tests_NR/hybrid/link_capacity"
    And in input "system.yml" the text "  - component: link_ab\n    port: capacity_port" is replaced by "  - component: load_a\n    port: consumption"
    When I run antares simulator
    Then the simulation fails
    And the message "has no link-capacity-connection field IDs defined" is reported in the logs

  @short
  Scenario: Invalid study - the same link connected twice
    Given the solver study path is a copy of "Antares_Simulator_Tests_NR/hybrid/link_capacity"
    And in input "system.yml" the text "      from: a\n      to: b" is replaced by "      from: a\n      to: b\n  - component: link_ab\n    port: capacity_port\n    link-component:\n      from: a\n      to: b"
    When I run antares simulator
    Then the simulation fails
    And the message "has several link-capacity connections" is reported in the logs

  @short
  Scenario: Invalid study - two components giving capacity to the same link
    # Only one component may bring capacity to a link: the capacities must be gathered in one model
    Given the solver study path is a copy of "Antares_Simulator_Tests_NR/hybrid/link_capacity"
    And in input "system.yml" the text "  area-connections:" is replaced by "  - id: link_ab_bis\n    model: lib_link_capacity.link_capacity\n    parameters:\n    - id: direct\n      time-dependent: false\n      scenario-dependent: false\n      value: 10\n    - id: indirect\n      time-dependent: false\n      scenario-dependent: false\n      value: 10\n\n  area-connections:"
    And in input "system.yml" the text "      from: a\n      to: b" is replaced by "      from: a\n      to: b\n  - component: link_ab_bis\n    port: capacity_port\n    link-component:\n      from: a\n      to: b"
    When I run antares simulator
    Then the simulation fails
    And the message "has several link-capacity connections" is reported in the logs

  @short
  Scenario Outline: Invalid study - link capacity connection on a link whose transmission capacities are not used
    Given the solver study path is a copy of "Antares_Simulator_Tests_NR/hybrid/link_capacity"
    And the transmission-capacities of link "a" are set to "<mode>"
    When I run antares simulator
    Then the simulation fails
    And the message "requires its transmission capacities to be used" is reported in the logs

    Examples: transmission capacities modes
      | mode   |
      | ignore |
      | infinite |

  @short
  Scenario: Invalid study - FLOW QUAD. (selected by default) with link capacity connections and a non-null impedance
    Given the solver study path is a copy of "Antares_Simulator_Tests_NR/hybrid/link_capacity"
    And in input "links/a/b_parameters.txt" the 8760 lines are all set to "1\t1\t0.5\t0\t0\t0"
    When I run antares simulator
    Then the simulation fails
    And the message "is not supported in studies with link capacity connections" is reported in the logs

  @short
  Scenario: Link capacity connection with a non-null impedance runs when FLOW QUAD. is deselected
    Given the solver study path is a copy of "Antares_Simulator_Tests_NR/hybrid/link_capacity"
    And in input "links/a/b_parameters.txt" the 8760 lines are all set to "1\t1\t0.5\t0\t0\t0"
    And the output variable "FLOW QUAD." is deselected
    When I run antares simulator
    Then the simulation succeeds

  # ---- Time horizon and resolution modes -----------------------------------------------------

  @short
  Scenario: Link capacity connection - the capacity applies to every week of a full year
    # Checks the hours of the first week, a middle week and the last week of the year.
    Given the solver study path is a copy of "Antares_Simulator_Tests_NR/hybrid/link_capacity"
    And the general parameter "simulation.end" is set to "365"
    And in input "system.yml" the parameter "indirect" of component "link_ab" is set to "60"
    When I run antares simulator with --output=all
    Then the simulation succeeds
    And the modeler outputs contain the following entries with relative tolerance 0.001
      | block | component | output | timestep  | scenario | value |
      |       | a_b_link  | flow   | 0-167     | 0        | -60   |
      |       | a_b_link  | flow   | 4200-4367 | 0        | -60   |
      |       | a_b_link  | flow   | 8568-8735 | 0        | -60   |
      |       | gen_a     | p      | 8568-8735 | 0        | 40    |
    And on link "a - b", during year 1, the hourly "CONG. PROB -" is 100 for every hour

  @short
  Scenario: Link capacity connection - every Monte Carlo year gets the capacity
    Given the solver study path is a copy of "Antares_Simulator_Tests_NR/hybrid/link_capacity"
    And the general parameter "nbyears" is set to "3"
    And in input "system.yml" the parameter "indirect" of component "link_ab" is set to "60"
    When I run antares simulator with --output=all
    Then the simulation succeeds
    And the modeler outputs contain the following entries with relative tolerance 0.001
      | block | component | output | timestep | scenario | value |
      |       | a_b_link  | flow   | 0-167    | 0-2      | -60   |
    And on link "a - b", during year 1, the hourly "CONG. PROB -" is 100 for every hour
    And on link "a - b", during year 3, the hourly "CONG. PROB -" is 100 for every hour

  @short
  Scenario: Link capacity connection - MILP resolution (no dual values)
    Given the solver study path is a copy of "Antares_Simulator_Tests_NR/hybrid/link_capacity"
    And the unit commitment mode is "milp"
    And the linear solver is highs
    And in input "system.yml" the parameter "indirect" of component "link_ab" is set to "60"
    When I run antares simulator with --output=all
    Then the simulation succeeds
    And the modeler outputs contain the following entries with relative tolerance 0.001
      | block | component | output | timestep | scenario | value |
      |       | a_b_link  | flow   | 0-167    | 0        | -60   |
      |       | gen_a     | p      | 0-167    | 0        | 40    |
    And on link "a - b", during year 1, the hourly "CONG. PROB -" is 100 for every hour

  # ---- Capacity as a time series -------------------------------------------------------------

  @short
  Scenario: Link capacity connection - time-dependent capacity
    # indirect capacity: 150 MW for the first 84 hours of each week, 60 MW for the next 84 hours
    Given the solver study path is a copy of "Antares_Simulator_Tests_NR/hybrid/link_capacity"
    And in input "data-series/indirect_capacity.csv" the 8760 lines cycle through "150*84|60*84"
    And in input "model-libraries/lib_link_capacity.yml" the text "    - id: indirect\n      time-dependent: false" is replaced by "    - id: indirect\n      time-dependent: true"
    And in input "system.yml" the text "      time-dependent: false\n      scenario-dependent: false\n      value: 150" is replaced by "      time-dependent: true\n      scenario-dependent: false\n      value: indirect_capacity"
    When I run antares simulator with --output=all
    Then the simulation succeeds
    And the modeler outputs contain the following entries with relative tolerance 0.001
      | block | component | output | timestep | scenario | value |
      |       | a_b_link  | flow   | 0-83     | 0        | -100  |
      |       | a_b_link  | flow   | 84-167   | 0        | -60   |
      |       | gen_a     | p      | 0-83     | 0        | 0     |
      |       | gen_a     | p      | 84-167   | 0        | 40    |

  @short
  Scenario: Link capacity connection - scenario-dependent capacity
    # indirect capacity series with one column per Monte Carlo year: 150 MW then 60 MW
    Given the solver study path is a copy of "Antares_Simulator_Tests_NR/hybrid/link_capacity"
    And the general parameter "nbyears" is set to "2"
    And in input "data-series/indirect_capacity.tsv" the 8760 lines cycle through "150\t60"
    And in input "data-series/modeler-scenariobuilder.dat" the 2 lines cycle through "link_group, 0 = 1|link_group, 1 = 2"
    And in input "model-libraries/lib_link_capacity.yml" the text "    - id: indirect\n      time-dependent: false\n      scenario-dependent: false" is replaced by "    - id: indirect\n      time-dependent: true\n      scenario-dependent: true"
    And in input "system.yml" the text "  - id: link_ab\n    model: lib_link_capacity.link_capacity" is replaced by "  - id: link_ab\n    model: lib_link_capacity.link_capacity\n    scenario-group: link_group"
    And in input "system.yml" the text "      time-dependent: false\n      scenario-dependent: false\n      value: 150" is replaced by "      time-dependent: true\n      scenario-dependent: true\n      value: indirect_capacity"
    When I run antares simulator with --output=all
    Then the simulation succeeds
    And the modeler outputs contain the following entries with relative tolerance 0.001
      | block | component | output | timestep | scenario | value |
      |       | a_b_link  | flow   | 0-167    | 0        | -100  |
      |       | a_b_link  | flow   | 0-167    | 1        | -60   |
      |       | gen_a     | p      | 0-167    | 0        | 0     |
      |       | gen_a     | p      | 0-167    | 1        | 40    |
    And on link "a - b", during year 1, the hourly "CONG. PROB -" is 0 for every hour
    And on link "a - b", during year 2, the hourly "CONG. PROB -" is 100 for every hour

  # ---- Equivalence with a legacy NTC ---------------------------------------------------------
  # The two scenarios below describe the same link (direct 20 MW, indirect 60 MW, hurdle cost 1,
  # loop flow 10 MW), once through the component capacity and once through the legacy NTC. The
  # expected numbers are identical.

  @short
  Scenario: Link capacity connection - hurdle costs and loop flow, capacity from the component
    # The legacy NTC (100 MW) only has to be consistent with the loop flow; it is replaced.
    Given the solver study path is a copy of "Antares_Simulator_Tests_NR/hybrid/link_capacity"
    And in input "links/a/properties.ini" section "b" variable "hurdles-cost" is set to "true"
    And in input "links/a/properties.ini" section "b" variable "loop-flow" is set to "true"
    And in input "links/a/b_parameters.txt" the 8760 lines are all set to "1\t1\t0\t10\t0\t0"
    And in input "links/a/b.txt" the 8760 lines are all set to "100\t100\t0\t0\t0\t0\t0\t0"
    And in input "links/a/capacities/b_direct.txt" the 8760 lines are all set to "100"
    And in input "links/a/capacities/b_indirect.txt" the 8760 lines are all set to "100"
    And in input "system.yml" the parameter "indirect" of component "link_ab" is set to "60"
    When I run antares simulator with --output=all
    Then the simulation succeeds
    And the modeler outputs contain the following entries with relative tolerance 0.001
      | block | component | output           | timestep | scenario | value |
      |       | a_b_link  | flow             | 0-167    | 0        | -60   |
      |       | a_b_link  | actual_loop_flow | 0-167    | 0        | 10    |
      |       | gen_a     | p                | 0-167    | 0        | 40    |
    And on link "a - b", during year 1, the hourly "HURDLE COST" is 70 for every hour
    And on link "a - b", during year 1, the hourly "MARG. COST" is 8 for every hour
    And on link "a - b", during year 1, the hourly "CONG. PROB -" is 100 for every hour

  @short
  Scenario: Link capacity connection - hurdle costs and loop flow, same link with a legacy NTC
    Given the solver study path is a copy of "Antares_Simulator_Tests_NR/hybrid/link_capacity"
    And in input "system.yml" the text "  link-capacity-connections:\n  - component: link_ab\n    port: capacity_port\n    link-component:\n      from: a\n      to: b" is replaced by "  # no link capacity connection: the link keeps its legacy NTC"
    And in input "links/a/properties.ini" section "b" variable "hurdles-cost" is set to "true"
    And in input "links/a/properties.ini" section "b" variable "loop-flow" is set to "true"
    And in input "links/a/b_parameters.txt" the 8760 lines are all set to "1\t1\t0\t10\t0\t0"
    And in input "links/a/b.txt" the 8760 lines are all set to "20\t60\t0\t0\t0\t0\t0\t0"
    And in input "links/a/capacities/b_direct.txt" the 8760 lines are all set to "20"
    And in input "links/a/capacities/b_indirect.txt" the 8760 lines are all set to "60"
    When I run antares simulator with --output=all
    Then the simulation succeeds
    And the modeler outputs contain the following entries with relative tolerance 0.001
      | block | component | output           | timestep | scenario | value |
      |       | a_b_link  | flow             | 0-167    | 0        | -60   |
      |       | a_b_link  | actual_loop_flow | 0-167    | 0        | 10    |
      |       | gen_a     | p                | 0-167    | 0        | 40    |
    And on link "a - b", during year 1, the hourly "HURDLE COST" is 70 for every hour
    And on link "a - b", during year 1, the hourly "MARG. COST" is 8 for every hour
    And on link "a - b", during year 1, the hourly "CONG. PROB -" is 100 for every hour

  # ---- Capacity chosen by the optimisation ---------------------------------------------------
  # Study hybrid/link_capacity_investment: same system as above, but the capacity of the link
  # (same in both directions) is a variable of the link_investment component, bounded by 1000 MW
  # and costing invest_cost per MW and per week. Importing into a saves 9 * 168 = 1512 per MW and
  # per week.

  @short
  Scenario: Link capacity connection - the optimisation invests in the link when it pays off
    # invest_cost = 1 < 1512: the capacity is raised to the 100 MW needed by a, not more.
    Given the solver study path is "Antares_Simulator_Tests_NR/hybrid/link_capacity_investment"
    When I run antares simulator with --output=all
    Then the simulation succeeds
    And the modeler outputs contain the following entries with relative tolerance 0.001
      | block | component | output   | timestep | scenario | value |
      | 0     | link_ab   | capacity |          | 0        | 100   |
      |       | a_b_link  | flow     | 0-167    | 0        | -100  |
      |       | gen_a     | p        | 0-167    | 0        | 0     |

  @short
  Scenario: Link capacity connection - the optimisation does not invest in the link when it does not pay off
    # invest_cost = 3000 > 1512: no capacity, a is supplied by gen_a only.
    Given the solver study path is a copy of "Antares_Simulator_Tests_NR/hybrid/link_capacity_investment"
    And in input "system.yml" the parameter "invest_cost" of component "link_ab" is set to "3000"
    When I run antares simulator with --output=all
    Then the simulation succeeds
    And the modeler outputs contain the following entries with relative tolerance 0.001
      | block | component | output   | timestep | scenario | value |
      | 0     | link_ab   | capacity |          | 0        | 0     |
      |       | a_b_link  | flow     | 0-167    | 0        | 0     |
      |       | gen_a     | p        | 0-167    | 0        | 100   |

  # ---- Adequacy patch ------------------------------------------------------------------------

  @short
  Scenario: Link capacity connection - the adequacy patch (CSR) uses the solved link capacity
    # Area a gets a legacy load of 100 MW (the modeler load_a is set to 0) and no generation, so
    # it can only import: 60 MW through the link (indirect capacity), 40 MW are unsupplied.
    # The legacy NTC of the link is null: if the CSR problem used it instead of the solved
    # capacity, it would forbid the import (the CSR bounds are relaxed by a few 1e-3 MW only).
    Given the solver study path is a copy of "Antares_Simulator_Tests_NR/hybrid/link_capacity"
    And the general parameter "include-adq-patch" is set to "true"
    And in input "areas/a/adequacy_patch.ini" section "adequacy-patch" variable "adequacy-patch-mode" is set to "inside"
    And in input "areas/b/adequacy_patch.ini" section "adequacy-patch" variable "adequacy-patch-mode" is set to "inside"
    And in input "load/series/load_a.txt" the 8760 lines are all set to "100"
    And in input "system.yml" the parameter "load_value" of component "load_a" is set to "0"
    And in input "system.yml" the parameter "max_p" of component "gen_a" is set to "0"
    And in input "system.yml" the parameter "indirect" of component "link_ab" is set to "60"
    When I run antares simulator with --output=all
    Then the simulation succeeds
    And the modeler outputs from stage "optim-nb-1" contain the following entries with relative tolerance 0.001
      | block | component | output             | timestep | scenario | value |
      |       | a_b_link  | flow               | 0-167    | 0        | -60   |
      |       | a_node    | unsupplied_energy  | 0-167    | 0        | 40    |
    And the modeler outputs from stage "adq-patch" contain the following entries with relative tolerance 0.001
      | block | component | output                     | timestep | scenario | value |
      |       | a_b_link  | flow                       | 0-167    | 0        | -60   |
      |       | a_node    | unsupplied_energy          | 0-167    | 0        | 40    |
      |       | a_node    | domestic_unsupplied_energy | 0-167    | 0        | 100   |

  # ---- Interaction with legacy binding constraints -------------------------------------------

  @short
  Scenario: Link capacity connection - a legacy binding constraint on the link flow is still enforced
    # flow(a->b) >= -30 is tighter than the 150 MW indirect capacity: a imports 30 MW only
    Given the solver study path is a copy of "Antares_Simulator_Tests_NR/hybrid/link_capacity"
    And in input "bindingconstraints/bindingconstraints.ini" section "0" variable "name" is set to "min_flow"
    And in input "bindingconstraints/bindingconstraints.ini" section "0" variable "id" is set to "min_flow"
    And in input "bindingconstraints/bindingconstraints.ini" section "0" variable "enabled" is set to "true"
    And in input "bindingconstraints/bindingconstraints.ini" section "0" variable "type" is set to "hourly"
    And in input "bindingconstraints/bindingconstraints.ini" section "0" variable "operator" is set to "greater"
    And in input "bindingconstraints/bindingconstraints.ini" section "0" variable "filter-year-by-year" is set to "hourly"
    And in input "bindingconstraints/bindingconstraints.ini" section "0" variable "filter-synthesis" is set to "hourly"
    And in input "bindingconstraints/bindingconstraints.ini" section "0" variable "a%b" is set to "1"
    And in input "bindingconstraints/min_flow.txt" the 8784 lines are all set to "0\t-30\t0"
    When I run antares simulator with --output=all
    Then the simulation succeeds
    And the modeler outputs contain the following entries with relative tolerance 0.001
      | block | component | output | timestep | scenario | value |
      |       | a_b_link  | flow   | 0-167    | 0        | -30   |
      |       | gen_a     | p      | 0-167    | 0        | 70    |
      |       | gen_b     | p      | 0-167    | 0        | 80    |
