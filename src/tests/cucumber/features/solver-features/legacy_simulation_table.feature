Feature: Legacy variables in simulation table
  # The legacy solver populates PROBLEME_ANTARES_A_RESOUDRE::NomDesVariables
  # with names of the form `<Output>::<location>::hour<N>`. After the per-week
  # solve, FillLegacySimulationTable parses those names, applies LegacyNameMapper
  # to the output, and pushes one row per parsed variable into the per-numSpace
  # SimulationTable. This scenario checks that:
  #   - the simulation table file is produced for a pure-legacy study
  #   - the LegacyNameMapper transform (UnsuppliedEnergy -> unsupplied_energy)
  #     is visible in the CSV output
  #   - the row carries the value the legacy solver computed for that hour

  @short
  Scenario: Legacy UnsuppliedEnergy is mapped to unsupplied_energy and carries the solver value
    # "002 Thermal fleet - Base" has a single area (id "area") with a known
    # shortfall of 52 MW on "2 JAN 09:00" of year 1, i.e. absolute hour 33
    # (0-based), which lives in week 0 -> block 0. Antares lowercases area
    # ids, so the legacy variable produced by the solver is
    # `UnsuppliedEnergy::area<area>::hour<33>` (0-based legacy hour); is stored as
    # component=area, output=unsupplied_energy, absolute_time_index=33.
    Given the solver study path is "Antares_Simulator_Tests_NR/hybrid/002 Thermal fleet - Base"
    When I run antares simulator with --output=all
    Then the simulation succeeds
    And in area "AREA", unsupplied energy on "2 JAN 09:00" of year 1 is of 52 MW
    And the modeler outputs contain the following entries
      | block | component  | output            | timestep | scenario | value |
      | 0     | area_node | unsupplied_energy | 33       | 0        | 52    |

  @short
  Scenario: Extra outputs prop_cost and imbalance_cost are derived from the legacy solution
    # Same study, same shortfall hour (absolute hour 33, block 0). With 52 MW
    # of unsupplied energy every thermal cluster is at its maximum, so:
    #   - prop_cost = marginal_cost * generated_power per cluster:
    #       base:      35 * (4 * 900) = 126000
    #       semi base: 50 * (5 * 300) =  75000
    #       peak:      80 * (8 * 100) =  64000
    #   - imbalance_cost = unsupplied_energy_cost * unsupplied_energy
    #     + spilled_energy_cost * spilled_energy = 10000 * 52 + 0 * 0 = 520000
    #   - is_loss_of_load = 1 since unsupplied energy (52) is positive, and
    #     is_significant_loss_of_load = 1 since it exceeds the 0.5 MW threshold
    #   - price = -dual(balance) = 10000: with unsupplied energy strictly
    #     between its bounds, the marginal MW is served at the unsupplied
    #     energy cost
    #   - is_near_loss_of_load = 1 since price (10000) > 10000 - 5
    #   - actual_load = raw load = 6111 MW (input/load/series line 34). The
    #     extra output reads the residual load and adds the must-run generation
    #     back, recovering the raw load series; it is anchored on the area's
    #     UnsuppliedEnergy variable, which exists in fast mode too.
    # actual_num_units_on is asserted on a separate accurate-mode scenario; the
    # NODU legacy variable is only created when unit-commitment-mode != fast
    # (opt_construction_variables_optimisees_lineaire.cpp guards the call on
    # problemeHebdo->OptimisationNotFastMode), and this study runs in fast mode.

    # The derived costs are computed from the objective coefficients, which
    # carry the anti-degeneracy noise the legacy solver always adds to costs
    # (PrepareRandomNumbers in common-eco-adq.cpp: |noise| is forced into
    # [5e-4, 6e-4] per thermal cluster cost even with spread-cost = 0, and
    # this study sets spread-unsupplied-energy-cost = 0.01). Hence the relaxed
    # tolerance: relative 1e-4 covers a noise of 6e-4 on the cheapest cost
    # (35) while still pinning the value to the theoretical formula.
    Given the solver study path is "Antares_Simulator_Tests_NR/hybrid/002 Thermal fleet - Base"
    When I run antares simulator with --output=simulation-tables
    Then the simulation succeeds
    And the modeler outputs contain the following entries with relative tolerance 1e-4
      | block | component | output         | timestep | scenario | value  |
      | 0     | area_thermal_base      | prop_cost      | 33       | 0        | 126000 |
      | 0     | area_thermal_semi base | prop_cost      | 33       | 0        | 75000  |
      | 0     | area_thermal_peak      | prop_cost      | 33       | 0        | 64000  |
      | 0     | area_node      | imbalance_cost | 33       | 0        | 520000 |
      | 0     | area_node      | is_loss_of_load | 33      | 0        | 1      |
      | 0     | area_node      | is_significant_loss_of_load | 33 | 0  | 1      |
      | 0     | area_node      | price          | 33       | 0        | 10000  |
      | 0     | area_node      | is_near_loss_of_load | 33  | 0        | 1      |
      | 0     | area_load      | actual_load    | 33       | 0        | 6111   |

  @fast @short
  Scenario: Link extra outputs are derived from the legacy flow variables and duals
    # Study "Hurdle-cost link" is the first 21 days of short-tests/025 Hurdle
    # costs - 2. Two areas (east, west) joined by an AC link with constant
    # hurdle costs of 1 EUR/MWh in both directions; this is the only short
    # legacy study with hurdles-cost = true. In weeks 1 and 2 each area covers
    # its own load so the link is unused; in week 3 (block 3) east starts
    # exporting to west. At absolute hour 426 (block 3, hour 90 of the block):
    #   - abs_flow = |DirectFlow| = 213.45 MW (the link carries this much
    #     east -> west).
    #   - minus_flow = -DirectFlow = -213.45 MW; the flow is positive (east
    #     exporting to west), so its negation is negative.
    #   - actual_loop_flow = 0: this fixture configures no loop flow on the
    #     link, so the loop-flow input series is zero and the extra output is
    #     emitted as 0.
    #   - prop_cost (link) = direct_hurdle_cost * positive_direct_flow
    #                        + indirect_hurdle_cost * positive_indirect_flow
    #                      = 1 * 213.45 + 1 * 0 = 213.45.
    #   - capacity_shadow_price = |dual(FlowDissociation)| approx 1.0; the
    #     constraint is binding when the link is hurdle-managed and flowing,
    #     and the dual matches the hurdle differential. The FlowDissociation
    #     constraint (and PositiveDirectFlow variable) only exist when the
    #     link has hurdles-cost = true, see ConstraintsList.cpp.
    #
    # Per-area prices at the same hour bracket the hurdle cost:
    #   price west = price east + 1 EUR/MWh.
    Given the solver study path is "Antares_Simulator_Tests_NR/hybrid/Hurdle-cost link"
    When I run antares simulator with --output=simulation-tables
    Then the simulation succeeds
    And the modeler outputs contain the following entries with relative tolerance 1e-4
      | block | component  | output                  | timestep | scenario | value   |
      | 2     | east_west_link | abs_flow                | 425      | 0        | 213.452 |
      | 2     | east_west_link | minus_flow              | 425      | 0        | -213.452 |
      | 2     | east_west_link | actual_loop_flow        | 425      | 0        | 0       |
      | 2     | east_west_link | prop_cost               | 425      | 0        | 213.452 |
      | 2     | east_west_link | capacity_shadow_price   | 425      | 0        | 1.0     |
      | 2     | east_node       | price                   | 425      | 0        | 44.926  |
      | 2     | west_node       | price                   | 425      | 0        | 45.926  |

      # is_directly/indirectly_congested compare the (signed) DirectFlow to the
      # link's per-hour capacity (3000 MW direct and indirect, constant in this
      # study). At 213.45 MW the link is far from either bound, so both
      # indicators are 0; the saturation case (= 1) is covered by the unit
      # tests, since no realistic single-week dispatch on this fixture pushes
      # the link to 3000 MW.
      | 2     | east_west_link | is_directly_congested   | 425      | 0        | 0       |
      | 2     | east_west_link | is_indirectly_congested | 425      | 0        | 0       |

  @fast @short
  Scenario: hydro_shadow_price is emitted when hydro-pricing-mode is accurate
    # Study "Accurate hydro pricing" is the first 7 days of
    # valid-hydroPricing/Accurate/YHT-one_node_different styles/hydro-pricing-HE.
    # One area "HE" with a reservoir and hydro-pricing-mode = accurate; this
    # turns on the FinalStockExpression constraint in the legacy weekly
    # problem (one per area per week, anchored at the last hour of the week),
    # which the legacy extra outputs map to hydro_shadow_price.
    #
    # The constraint expresses end-of-week water value; the dual at the
    # weekly optimum equals the reservoir's marginal value (about
    # -56 EUR/MWh for this fixture, set by the calibrated water values).
    # The legacy extra output writes the dual as-is (no sign flip).
    Given the solver study path is "Antares_Simulator_Tests_NR/hybrid/Accurate hydro pricing"
    When I run antares simulator with --output=simulation-tables
    Then the simulation succeeds
    And the modeler outputs contain the following entries
      | block | component | output             | timestep | scenario | value |
      | 0     | he_hydro_storage        | hydro_shadow_price | 167      | 0        | 56   |
      # level_percentage = HydroLevel / reservoir_capacity * 100. The "he" area has a
      # 10 000 000 MWh reservoir and the initial level for hour 1 of week 1 is
      # 5 110 638.139 MWh => 51.10638138.
      | 0     | he_hydro_storage        | level_percentage   | 0        | 0        | 51.10638138    |
      # actual_inflows = round(inflows). The "he" area has empty mod.txt/ror.txt,
      # but hydro TS generation is enabled for this study (generate = hydro,
      # seed-tsgen-hydro = 5489), so the inflow used is the generated series,
      # not the empty input files: 1873 MWh at hour 1 of MC year 0.
      | 0     | he_hydro_storage        | actual_inflows     | 0        | 0        | 1873          |

  @fast @short
  Scenario: Input-generation rows are absent for components whose series are entirely zero
    # Study "002 Thermal fleet - Base" runs with renewable-generation-modelling
    # = aggregated. Its "area" has real wind data (input/wind/series/wind_area.txt),
    # but empty solar, misc-gen and run-of-river series
    # (input/solar/series/solar_area.txt, input/misc-gen/miscgen-area.txt,
    # input/hydro/series/area/ror.txt are all 0-line files, which Antares
    # loads as an all-zero series) -- exactly the "no meaningful production"
    # case this filtering targets. Before the fix these still produced
    # generation_power/minus_generation/balance_port.flow rows valued 0;
    # after it, the components are omitted entirely, while the (non-zero)
    # wind component is untouched.
    Given the solver study path is "Antares_Simulator_Tests_NR/hybrid/002 Thermal fleet - Base"
    When I run antares simulator with --output=simulation-tables
    Then the simulation succeeds
    And the modeler outputs contain entries for component "area_wind"
    And the modeler outputs contain no entries for component "area_solar"
    And the modeler outputs contain no entries for component "area_run_of_river"
    And the modeler outputs contain no entries for component "area_combined_heat_power"
    And the modeler outputs contain no entries for component "area_biomass"
    And the modeler outputs contain no entries for component "area_biogas"
    And the modeler outputs contain no entries for component "area_waste"
    And the modeler outputs contain no entries for component "area_geothermal"
    And the modeler outputs contain no entries for component "area_other"
    And the modeler outputs contain no entries for component "area_pumped_storage_power"
    And the modeler outputs contain no entries for component "area_rest_world"

  @fast @short
  Scenario: actual_load and its balance-port row are absent for an area with an all-zero load series
    # Same base study as above, on a temporary copy with its load series
    # emptied (the same "empty file -> all-zero series" convention). With no
    # load at all, area_load's actual_load and balance_port.flow rows should
    # be suppressed like any other all-zero-series component.
    Given the solver study path is a copy of "Antares_Simulator_Tests_NR/hybrid/002 Thermal fleet - Base"
    And in input "load/series/load_area.txt" the time series is emptied
    When I run antares simulator with --output=simulation-tables
    Then the simulation succeeds
    And the modeler outputs contain no entries for component "area_load"

  @fast @short
  Scenario: hydro balance_port.flow is absent when reservoir is unmanaged and inflow is entirely zero
    # Base study "hydro/hydro-parameters" with reservoir management disabled
    # (reservoir=false) AND its inflow series (mod.txt) emptied -- both
    # conditions together, since reservoir=false alone (with real inflow)
    # still produces legitimate turbine generation, as the existing
    # hydro_parameters.feature scenario outline shows (year-2 production of
    # 109200 MWh with reservoir=false but real inflow). "area_hydro" only
    # ever carries the balance_port.flow row, so its absence here is a
    # precise check that this specific row was suppressed.
    Given the solver study path is a copy of "Antares_Simulator_Tests_NR/hydro/hydro-parameters"
    And in input "hydro/hydro.ini" section "reservoir" variable "area" is set to "false"
    And in input "hydro/series/area/mod.txt" the time series is emptied
    When I run antares simulator with --output=simulation-tables
    Then the simulation succeeds
    And the modeler outputs contain no entries for component "area_hydro"

  @fast @short
  Scenario: derived link outputs are absent when both NTC directions are zero across the whole study
    # Same base study as the "Link extra outputs" scenario above (east-west,
    # hurdle-cost link), on a temporary copy with both direction capacity
    # series emptied (the "empty file -> all-zero series" convention). No
    # explicit "disabled" flag exists on links, so a link with zero capacity
    # in both directions across the whole study is treated as inactive.
    #
    # This only suppresses the *derived* extra-output rows produced by
    # AddLegacyExtraOutputs (abs_flow, prop_cost, congestion indicators,
    # port fields, ...) -- it does not touch the separate *raw* per-variable
    # rows (flow, direct_flow, indirect_flow), which are out of this
    # feature's scope and still exist as long as the link's LP variables do.
    Given the solver study path is a copy of "Antares_Simulator_Tests_NR/hybrid/Hurdle-cost link"
    And in input "links/east/capacities/west_direct.txt" the time series is emptied
    And in input "links/east/capacities/west_indirect.txt" the time series is emptied
    When I run antares simulator with --output=simulation-tables
    Then the simulation succeeds
    And the modeler outputs contain no "abs_flow" entries for component "east_west_link"
    And the modeler outputs contain no "minus_flow" entries for component "east_west_link"
    And the modeler outputs contain no "out_port.flow" entries for component "east_west_link"
    And the modeler outputs contain no "in_port.flow" entries for component "east_west_link"
    And the modeler outputs contain no "prop_cost" entries for component "east_west_link"
    And the modeler outputs contain no "capacity_shadow_price" entries for component "east_west_link"
    And the modeler outputs contain no "is_directly_congested" entries for component "east_west_link"
    And the modeler outputs contain no "is_indirectly_congested" entries for component "east_west_link"

  @fast @short
  Scenario: actual_num_units_on is emitted in accurate unit-commitment mode
    # Study "008 Thermal fleet - Accurate unit commitment" is the accurate-mode
    # twin of 002 (same area, same clusters, same shortfall on absolute hour 34
    # of year 1). The NODU legacy variable is only registered when
    # OptimisationNotFastMode is true, which is set from
    # unitCommitment.ucMode != ucHeuristicFast in sim_calcul_economique.cpp;
    # accurate mode satisfies this, so for every dispatched cluster the legacy
    # extra output actual_num_units_on = ceil(NODU) is written.
    #
    # At the shortfall hour the area is at full capacity, so NODU equals the
    # unitcount for each cluster: base = 4, semi base = 5, peak = 8. NODU is
    # integer in accurate mode so the ceil() is a no-op; we still assert the
    # value to lock in the extra-output transform.
    #
    # non_prop_cost = startup_cost * units_started_since_t-1
    #               + fixed_cost * ceil(NODU).
    # The "base" cluster is the cheapest (marginal 35) and the load never drops
    # below the energy its 4 committed units provide, so all 4 base units run
    # from hour 1 onwards (min-up-time 24h) and none start at hour 34. Its
    # non_prop_cost is therefore the fixed-cost term only: fixed-cost (1700) *
    # 4 units = 6800. fixed-cost is the objective coefficient on the NODU
    # variable and carries the legacy anti-degeneracy noise, hence the relaxed
    # tolerance.
    #
    # co2_emissions = generation_power * co2_emissions_rate. At the shortfall
    # hour every cluster is at maximum, so generation is unitcount*nominalcapacity
    # (base 4*900=3600, semi base 5*300=1500, peak 8*100=800) and the CO2 rates
    # from list.ini are base 1.2, semi base 0.6, peak 0.7:
    #   base:      3600 * 1.2 = 4320
    #   semi base: 1500 * 0.6 =  900
    #   peak:       800 * 0.7 =  560
    # Unlike prop_cost the factor is read straight from study data (not an
    # objective coefficient), so the value carries no anti-degeneracy noise and
    # the default tolerance applies. The other pollutant rates are all 0, so
    # those rows would be 0 and are not asserted here.
    #
    # Thermal margins at the same hour. The thermal availability series are empty,
    # so each cluster's available power defaults to unitcount*nominalcapacity
    # (base 3600, semi base 1500, peak 800); at the shortfall hour every cluster
    # is fully dispatched, so generation_power == availability.
    #   cluster_availability = max(availability,
    #                              min_stable_power * ceil(availability/unit_size))
    #     base:      max(3600, 400*ceil(3600/900)=1600) = 3600
    #     semi base: max(1500, 100*ceil(1500/300)= 500) = 1500
    #     peak:      max( 800,  10*ceil( 800/100)=  80) =  800
    #   up_margin = cluster_availability - generation_power = 0 for every cluster
    #     (full dispatch).
    # The min-gen-modulation column of the modulation matrix is 0 at this hour,
    # so the min-generation floor is 0:
    #   min_gen_power = min(generation_power, 0) = 0
    #   down_margin   = generation_power - min(cluster_availability, 0)
    #                 = generation_power (3600 / 1500 / 800)
    # These are quantities / study data, so no anti-degeneracy noise applies.
    Given the solver study path is "Antares_Simulator_Tests_NR/short-tests/008 Thermal fleet - Accurate unit commitment"
    When I run antares simulator with --output=simulation-tables
    Then the simulation succeeds
    And the modeler outputs contain the following entries
      | block | component | output              | timestep | scenario | value |
      | 0     | area_thermal_base      | actual_num_units_on | 33       | 0        | 4     |
      | 0     | area_thermal_semi base | actual_num_units_on | 33       | 0        | 5     |
      | 0     | area_thermal_peak      | actual_num_units_on | 33       | 0        | 8     |
      | 0     | area_node      | is_loss_of_load     | 33       | 0        | 1     |
      | 0     | area_node      | is_significant_loss_of_load | 33 | 0     | 1     |
      | 0     | area_thermal_base      | co2_emissions        | 33       | 0        | 4320  |
      | 0     | area_thermal_semi base | co2_emissions        | 33       | 0        | 900   |
      | 0     | area_thermal_peak      | co2_emissions        | 33       | 0        | 560   |
      | 0     | area_thermal_base      | cluster_availability | 33       | 0        | 3600  |
      | 0     | area_thermal_semi base | cluster_availability | 33       | 0        | 1500  |
      | 0     | area_thermal_peak      | cluster_availability | 33       | 0        | 800   |
      | 0     | area_thermal_base      | up_margin            | 33       | 0        | 0     |
      | 0     | area_thermal_semi base | up_margin            | 33       | 0        | 0     |
      | 0     | area_thermal_peak      | up_margin            | 33       | 0        | 0     |
      | 0     | area_thermal_base      | min_gen_power        | 33       | 0        | 0     |
      | 0     | area_thermal_base      | down_margin          | 33       | 0        | 3600  |
    # prop_cost and imbalance_cost are derived from objective coefficients, which
    # carry the legacy anti-degeneracy noise (PrepareRandomNumbers forces |noise|
    # into [5e-4, 6e-4] per cost), so they need the relaxed tolerance: relative
    # 1e-4 covers a few-EUR drift on a 126000 cost while still pinning the value.
    And the modeler outputs contain the following entries with relative tolerance 1e-4
      | block | component | output         | timestep | scenario | value  |
      | 0     | area_thermal_base      | prop_cost      | 33       | 0        | 126000 |
      | 0     | area_thermal_semi base | prop_cost      | 33       | 0        | 75000  |
      | 0     | area_thermal_peak      | prop_cost      | 33       | 0        | 64000  |
      | 0     | area_thermal_base      | non_prop_cost | 33       | 0        | 6800  |
      | 0     | area_node      | imbalance_cost | 33       | 0        | 1040000 |


  @short
  Scenario: Per-stage simulation tables show what the adequacy patch moved
    # A week is not resolved in one shot: the two optimisation passes are
    # followed by post-treatments that mutate PROBLEME_HEBDO's result
    # structures without re-solving. One simulation table is written per stage,
    # named after it (see docs/architecture/legacy-extra-outputs.md §8).
    #
    # "adq-patch-CSR-test-case-v02" is an Economy study with
    # include-adq-patch = true, four areas: areain-1 / areain-2 are *inside*
    # the patch, areaout-1 / areaout-2 outside. It has no managed hydro, so
    # remix hydro changes nothing and the remix-hydro stage is a faithful copy
    # of optim-nb-2 -- which is exactly what makes it a check of the dump
    # itself rather than of what a post-process did. The CSR treatment, on the
    # other hand, moves 389 values in year 0 alone.
    #
    # Everything below is at block 0, absolute hour 0, scenario (year) 0.
    #
    # After optim 2 the whole 400 MW shortfall sits on areain-2, because its
    # unsupplied energy cost (800) is the cheaper of the two; the link carries
    # 101 MW from areain-2 to areain-1 (negative = indirect).
    Given the solver study path is "Antares_Simulator_Tests_NR/adequacy-patch-CSR/adq-patch-CSR-test-case-v02"
    When I run antares simulator with --output=simulation-tables
    Then the simulation succeeds
    And the simulation tables cover exactly the stages "optim-nb-1, optim-nb-2, remix-hydro, adq-patch-csr"
    And the modeler outputs are read from stage "optim-nb-2"
    And the modeler outputs contain the following entries with relative tolerance 1e-4
      | block | component              | output            | timestep | scenario | value  |
      | 0     | areain-1_node          | unsupplied_energy | 0        | 0        | 0      |
      | 0     | areain-2_node          | unsupplied_energy | 0        | 0        | 400    |
      | 0     | areain-1_areain-2_link | flow              | 0        | 0        | -101   |
      | 0     | areain-1_node          | price             | 0        | 0        | 800    |
      | 0     | areain-2_node          | price             | 0        | 0        | 800    |

    # No managed hydro in this study, so shave-peaks / remix hydro has nothing
    # to move: the stage exists and reproduces optim-nb-2 exactly.
    And the modeler outputs are read from stage "remix-hydro"
    And the modeler outputs contain the following entries with relative tolerance 1e-4
      | block | component              | output            | timestep | scenario | value  |
      | 0     | areain-1_node          | unsupplied_energy | 0        | 0        | 0      |
      | 0     | areain-2_node          | unsupplied_energy | 0        | 0        | 400    |
      | 0     | areain-1_areain-2_link | flow              | 0        | 0        | -101   |

    # Curtailment sharing redistributes the shortfall between the two inside
    # areas, conserving the total: 199.9526 + 200.0474 = 400.0000. The link
    # flow moves by exactly the energy areain-1 now leaves unsupplied:
    # -101 + 199.9526 = 98.9526. These are quantities, i.e. they come from the
    # X refresh (X[i] = *AdresseOuPlacerLaValeurDesVariablesOptimisees[i]).
    #
    # CSR is a separate LP, so the last digits are solver-dependent; hence the
    # relative tolerance on the quantities.
    And the modeler outputs are read from stage "adq-patch-csr"
    And the modeler outputs contain the following entries with relative tolerance 1e-4
      | block | component              | output            | timestep | scenario | value    |
      | 0     | areain-1_node          | unsupplied_energy | 0        | 0        | 199.9526 |
      | 0     | areain-2_node          | unsupplied_energy | 0        | 0        | 200.0474 |
      | 0     | areain-1_areain-2_link | flow              | 0        | 0        | 98.9526  |

    # price exercises the *other* half of the refresh: the duals, read back
    # through AdresseOuPlacerLaValeurDesCoutsMarginaux, which
    # UpdateMrgPriceAfterCSRcmd overwrites in place. Each area's price becomes
    # its own unserverdenergycost from input/thermal/areas.ini (areain-1 = 1000,
    # areain-2 = 800) instead of the single 800.0006 both carried after optim 2.
    # The .0006 disappearing is the anti-degeneracy noise PrepareRandomNumbers
    # adds to the optimisation costs: the CSR price update writes the un-noised
    # study cost, so these are exact and need no tolerance.
    And the modeler outputs contain the following entries
      | block | component     | output | timestep | scenario | value |
      | 0     | areain-1_node | price  | 0        | 0        | 1000  |
      | 0     | areain-2_node | price  | 0        | 0        | 800   |


  @short
  Scenario: Only the selected stages get a simulation table
    # Producing every stage costs memory and time for tables nobody reads, so
    # --simulation-table-stages restricts the run to the stages asked for.
    # The selection is purely subtractive: a stage that is still asked for is
    # filled exactly as it would have been in a full run -- the values below are
    # the same ones the previous scenario measures on the complete set.
    Given the solver study path is "Antares_Simulator_Tests_NR/adequacy-patch-CSR/adq-patch-CSR-test-case-v02"
    When I run antares simulator with --output=simulation-tables --simulation-table-stages=optim-nb-2,adq-patch-csr
    Then the simulation succeeds
    And the simulation tables cover exactly the stages "optim-nb-2, adq-patch-csr"
    And the modeler outputs are read from stage "optim-nb-2"
    And the modeler outputs contain the following entries with relative tolerance 1e-4
      | block | component              | output            | timestep | scenario | value |
      | 0     | areain-2_node          | unsupplied_energy | 0        | 0        | 400   |
      | 0     | areain-1_areain-2_link | flow              | 0        | 0        | -101  |
    And the modeler outputs are read from stage "adq-patch-csr"
    And the modeler outputs contain the following entries with relative tolerance 1e-4
      | block | component              | output            | timestep | scenario | value    |
      | 0     | areain-2_node          | unsupplied_energy | 0        | 0        | 200.0474 |
      | 0     | areain-1_areain-2_link | flow              | 0        | 0        | 98.9526  |


  @short
  Scenario: A single stage can be selected on its own
    Given the solver study path is "Antares_Simulator_Tests_NR/adequacy-patch-CSR/adq-patch-CSR-test-case-v02"
    When I run antares simulator with --output=simulation-tables --simulation-table-stages=remix-hydro
    Then the simulation succeeds
    And the simulation tables cover exactly the stages "remix-hydro"


  @short
  Scenario: The study can ask for its stages in generaldata.ini
    # simulation-table-stages in the [output] section, so a study carries its
    # own selection instead of every run having to pass the flag. Works on a
    # copy: the step edits generaldata.ini in place.
    Given the solver study path is a copy of "Antares_Simulator_Tests_NR/adequacy-patch-CSR/adq-patch-CSR-test-case-v02"
    And the study asks for the simulation table stages "optim-nb-1, remix-hydro"
    When I run antares simulator with --output=simulation-tables
    Then the simulation succeeds
    And the simulation tables cover exactly the stages "optim-nb-1, remix-hydro"


  @short
  Scenario: The command line overrides the stages the study asks for
    # Including --simulation-table-stages=all, which is how a study that
    # restricts the stages is put back to the full set for a single run.
    Given the solver study path is a copy of "Antares_Simulator_Tests_NR/adequacy-patch-CSR/adq-patch-CSR-test-case-v02"
    And the study asks for the simulation table stages "optim-nb-1"
    When I run antares simulator with --output=simulation-tables --simulation-table-stages=all
    Then the simulation succeeds
    And the simulation tables cover exactly the stages "optim-nb-1, optim-nb-2, remix-hydro, adq-patch-csr"
