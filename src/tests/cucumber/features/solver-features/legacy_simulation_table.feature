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
    When I run antares simulator
    Then the simulation succeeds
    And in area "AREA", unsupplied energy on "2 JAN 09:00" of year 1 is of 52 MW
    And the modeler outputs contain the following entries
      | block | component  | output            | timestep | scenario | value |
      | 0     | area | unsupplied_energy | 33       | 0        | 52    |

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
    #   - is_loss_of_load = 1 since unsupplied energy (52) exceeds the 0.5 MW
    #     threshold
    #   - price = -dual(balance) = 10000: with unsupplied energy strictly
    #     between its bounds, the marginal MW is served at the unsupplied
    #     energy cost
    #   - is_near_loss_of_load = 1 since price (10000) > 10000 - 5
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
    When I run antares simulator
    Then the simulation succeeds
    And the modeler outputs contain the following entries with relative tolerance 1e-4
      | block | component | output         | timestep | scenario | value  |
      | 0     | base      | prop_cost      | 33       | 0        | 126000 |
      | 0     | semi base | prop_cost      | 33       | 0        | 75000  |
      | 0     | peak      | prop_cost      | 33       | 0        | 64000  |
      | 0     | area      | imbalance_cost | 33       | 0        | 520000 |
      | 0     | area      | is_loss_of_load | 33      | 0        | 1      |
      | 0     | area      | price          | 33       | 0        | 10000  |
      | 0     | area      | is_near_loss_of_load | 33  | 0        | 1      |

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
    When I run antares simulator
    Then the simulation succeeds
    And the modeler outputs contain the following entries with relative tolerance 1e-4
      | block | component  | output                | timestep | scenario | value   |
      | 2     | east$$west | abs_flow              | 425      | 0        | 213.452 |
      | 2     | east$$west | prop_cost             | 425      | 0        | 213.452 |
      | 2     | east$$west | capacity_shadow_price | 425      | 0        | 1.0     |
      | 2     | east       | price                 | 425      | 0        | 44.926  |
      | 2     | west       | price                 | 425      | 0        | 45.926  |

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
    When I run antares simulator
    Then the simulation succeeds
    And the modeler outputs contain the following entries
      | block | component | output             | timestep | scenario | value |
      | 0     | he        | hydro_shadow_price | 167      | 0        | -56   |

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
    Given the solver study path is "Antares_Simulator_Tests_NR/short-tests/008 Thermal fleet - Accurate unit commitment"
    When I run antares simulator
    Then the simulation succeeds
    And the modeler outputs contain the following entries
      | block | component | output              | timestep | scenario | value |
      | 0     | base      | actual_num_units_on | 33       | 0        | 4     |
      | 0     | semi base | actual_num_units_on | 33       | 0        | 5     |
      | 0     | peak      | actual_num_units_on | 33       | 0        | 8     |
      | 0     | area      | is_loss_of_load     | 33       | 0        | 1     |
    # prop_cost and imbalance_cost are derived from objective coefficients, which
    # carry the legacy anti-degeneracy noise (PrepareRandomNumbers forces |noise|
    # into [5e-4, 6e-4] per cost), so they need the relaxed tolerance: relative
    # 1e-4 covers a few-EUR drift on a 126000 cost while still pinning the value.
    And the modeler outputs contain the following entries with relative tolerance 1e-4
      | block | component | output         | timestep | scenario | value  |
      | 0     | base      | prop_cost      | 33       | 0        | 126000 |
      | 0     | semi base | prop_cost      | 33       | 0        | 75000  |
      | 0     | peak      | prop_cost      | 33       | 0        | 64000  |
      | 0     | area      | imbalance_cost | 33       | 0        | 1040000 |

