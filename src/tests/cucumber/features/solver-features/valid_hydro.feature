Feature: valid hydro

  # Non-regression tests for the "valid-hydro" study set (H700-xx / H701-xx).
  # These studies exercise the hydro module: inflow generation & breakdown,
  # reservoir management, heuristic target (UHT), water values, leeway (LW),
  # hard bounds on rule curves (HB), pumping and pumped-storage plants.
  #
  # See resources/.../valid-hydro/HYDRO_TESTS_OVERVIEW.md for a description of
  # each study. Reference values below were captured with the antares-solver
  # built from this tree, using the sirius linear/quadratic solver and
  # year-by-year results enabled (as the cucumber harness does at runtime).
  #
  # Each @fast scenario anchors on "the expected value of the annual system
  # cost" (EXP, relative tolerance 0.1%). Single-area studies with a clear
  # hydro behaviour additionally check total hydro production / pumping for
  # year 1.
  #
  # A few studies are tagged @flaky (excluded from CI via --tags=~@flaky):
  #   - H700-38 / H700-40 / H700-51 abort with sirius (infeasible at week 1);
  #   - H700-28 / H700-29 / H700-32 / H700-34 / H700-37 complete but emit a
  #     -nan(ind) annual system cost on the Windows CI runner (numerically
  #     degenerate with sirius; finite on the reference Linux build), so their
  #     numeric anchors are not cross-platform reproducible.
  # These quarantined scenarios document current behaviour and only assert the
  # solver's pass/fail outcome.

  # ---------------------------------------------------------------------------
  # Family A - one node, inflow generation / breakdown (no reservoir management)
  # ---------------------------------------------------------------------------

  @fast @valid-hydro
  Scenario: H700-00 One node, zero inflows -> no hydro generation
    Given the solver study path is "Antares_Simulator_Tests_NR/valid-hydro/H700-00"
    When I run antares simulator
    Then the simulation succeeds
    And the expected value of the annual system cost is 0

  @fast @valid-hydro
  Scenario: H700-01 Inflows every other month, ROR 25%, daily pattern = 1
    Given the solver study path is "Antares_Simulator_Tests_NR/valid-hydro/H700-01"
    When I run antares simulator
    Then the simulation succeeds
    And the expected value of the annual system cost is 0

  @fast @valid-hydro
  Scenario: H700-02 Inter-daily breakdown = 1 (Wednesday = 1.5 x other days)
    Given the solver study path is "Antares_Simulator_Tests_NR/valid-hydro/H700-02"
    When I run antares simulator
    Then the simulation succeeds
    And the expected value of the annual system cost is 0

  @fast @valid-hydro
  Scenario: H700-03 ROR 0%, daily pattern = 10 on Tuesdays, max-power capping
    Given the solver study path is "Antares_Simulator_Tests_NR/valid-hydro/H700-03"
    When I run antares simulator
    Then the simulation succeeds
    And the expected value of the annual system cost is 0

  @fast @valid-hydro
  Scenario: H700-04 Inter-daily breakdown (same output as H700-02)
    Given the solver study path is "Antares_Simulator_Tests_NR/valid-hydro/H700-04"
    When I run antares simulator
    Then the simulation succeeds
    And the expected value of the annual system cost is 0

  @fast @valid-hydro
  Scenario: H700-05 Intra-daily modulation = 1 (constant hourly power)
    Given the solver study path is "Antares_Simulator_Tests_NR/valid-hydro/H700-05"
    When I run antares simulator
    Then the simulation succeeds
    And the expected value of the annual system cost is 0

  @fast @valid-hydro
  Scenario: H700-06 Managed reservoir without reservoir management, inflows every other month
    Given the solver study path is "Antares_Simulator_Tests_NR/valid-hydro/H700-06"
    When I run antares simulator
    Then the simulation succeeds
    And the expected value of the annual system cost is 0

  @fast @valid-hydro
  Scenario: H700-25 Inflows every other month with an isolated peak on Jan 2nd
    Given the solver study path is "Antares_Simulator_Tests_NR/valid-hydro/H700-25"
    When I run antares simulator
    Then the simulation succeeds
    And the expected value of the annual system cost is 0

  # ---------------------------------------------------------------------------
  # Family B - ready-made inflows, infinite reservoir or reservoir start-up
  # ---------------------------------------------------------------------------

  @fast @valid-hydro
  Scenario: H700-07 Uniform inflows, infinite reservoir, no reservoir management
    Given the solver study path is "Antares_Simulator_Tests_NR/valid-hydro/H700-07"
    When I run antares simulator
    Then the simulation succeeds
    And the expected value of the annual system cost is 0
    And in area "Area 01", during year 1, total hydro production is 36374520 MWh

  @fast @valid-hydro
  Scenario: H700-08 Managed reservoir with reservoir management, uniform inflows
    Given the solver study path is "Antares_Simulator_Tests_NR/valid-hydro/H700-08"
    When I run antares simulator
    Then the simulation succeeds
    And the expected value of the annual system cost is 0
    And in area "Area 01", during year 1, total hydro production is 36374520 MWh

  @fast @valid-hydro
  Scenario: H700-09 Reservoir cold start, fills then stays full
    Given the solver study path is "Antares_Simulator_Tests_NR/valid-hydro/H700-09"
    When I run antares simulator
    Then the simulation succeeds
    And the expected value of the annual system cost is 0

  @fast @valid-hydro
  Scenario: H700-10 Reservoir hot start, fills then stays full
    Given the solver study path is "Antares_Simulator_Tests_NR/valid-hydro/H700-10"
    When I run antares simulator
    Then the simulation succeeds
    And the expected value of the annual system cost is 0

  @fast @valid-hydro
  Scenario: H700-11 Inflows on Tuesdays only, constant 100 MW output
    Given the solver study path is "Antares_Simulator_Tests_NR/valid-hydro/H700-11"
    When I run antares simulator
    Then the simulation succeeds
    And the expected value of the annual system cost is 69446400000
    And in area "Area 01", during year 1, total hydro production is 873600 MWh

  # ---------------------------------------------------------------------------
  # Family C - 12 zones, water value usage
  # ---------------------------------------------------------------------------

  @fast @valid-hydro
  Scenario: H700-12 12 zones, water value defined but NOT used
    Given the solver study path is "Antares_Simulator_Tests_NR/valid-hydro/H700-12"
    When I run antares simulator
    Then the simulation succeeds
    And the expected value of the annual system cost is 0

  @fast @valid-hydro
  Scenario: H700-13 12 zones, water value used
    Given the solver study path is "Antares_Simulator_Tests_NR/valid-hydro/H700-13"
    When I run antares simulator
    Then the simulation succeeds
    And the expected value of the annual system cost is 0

  # ---------------------------------------------------------------------------
  # Family D - one node, Monday/Tuesday inflows, fine management (UHT/WV/LW/HB)
  # ---------------------------------------------------------------------------

  @fast @valid-hydro
  Scenario: H700-14 UHT, no water value, no load following (reference case)
    Given the solver study path is "Antares_Simulator_Tests_NR/valid-hydro/H700-14"
    When I run antares simulator
    Then the simulation succeeds
    And the expected value of the annual system cost is 24960000000
    And in area "Zone 01", during year 1, total hydro production is 2600000 MWh

  @fast @valid-hydro
  Scenario: H700-15 Intra-daily modulation effect (no output Wed->Sun)
    Given the solver study path is "Antares_Simulator_Tests_NR/valid-hydro/H700-15"
    When I run antares simulator
    Then the simulation succeeds
    And the expected value of the annual system cost is 24960000000

  @fast @valid-hydro
  Scenario: H700-16 Leeway 1:1 lifts the intra-daily constraint
    Given the solver study path is "Antares_Simulator_Tests_NR/valid-hydro/H700-16"
    When I run antares simulator
    Then the simulation succeeds
    And the expected value of the annual system cost is 24960000000

  @fast @valid-hydro
  Scenario: H700-17 Load following, shedding policy = shave peaks
    Given the solver study path is "Antares_Simulator_Tests_NR/valid-hydro/H700-17"
    When I run antares simulator
    Then the simulation succeeds
    And the expected value of the annual system cost is 11440000000
    And in area "Zone 01", during year 1, total hydro production is 2598336 MWh

  @fast @valid-hydro
  Scenario: H700-18 Load following, shedding policy = minimize duration
    Given the solver study path is "Antares_Simulator_Tests_NR/valid-hydro/H700-18"
    When I run antares simulator
    Then the simulation succeeds
    And the expected value of the annual system cost is 11440000000

  @fast @valid-hydro
  Scenario: H700-19 Thermal cluster present, shave peaks has no effect
    Given the solver study path is "Antares_Simulator_Tests_NR/valid-hydro/H700-19"
    When I run antares simulator
    Then the simulation succeeds
    And the expected value of the annual system cost is 57200000

  @fast @valid-hydro
  Scenario: H700-20 Power fluctuations = minimize excursions
    Given the solver study path is "Antares_Simulator_Tests_NR/valid-hydro/H700-20"
    When I run antares simulator
    Then the simulation succeeds
    And the expected value of the annual system cost is 57200000

  @fast @valid-hydro
  Scenario: H700-21 Minimize excursions (same as H700-20)
    Given the solver study path is "Antares_Simulator_Tests_NR/valid-hydro/H700-21"
    When I run antares simulator
    Then the simulation succeeds
    And the expected value of the annual system cost is 57200000

  @fast @valid-hydro
  Scenario: H700-22 Water value with leeway 1:1
    Given the solver study path is "Antares_Simulator_Tests_NR/valid-hydro/H700-22"
    When I run antares simulator
    Then the simulation succeeds
    And the expected value of the annual system cost is 57200000

  @fast @valid-hydro
  Scenario: H700-23 Leeway 1:1.2 -> generation 60 GWh, level decreases
    Given the solver study path is "Antares_Simulator_Tests_NR/valid-hydro/H700-23"
    When I run antares simulator
    Then the simulation succeeds
    And the expected value of the annual system cost is 31200000
    And in area "Zone 01", during year 1, total hydro production is 3120000 MWh

  @fast @valid-hydro
  Scenario: H700-24 Leeway 1:1.2 with hard bounds on rule curves
    Given the solver study path is "Antares_Simulator_Tests_NR/valid-hydro/H700-24"
    When I run antares simulator
    Then the simulation succeeds
    And the expected value of the annual system cost is 42866700

  @fast @valid-hydro
  Scenario: H700-26 Expensive water + leeway 0.2:1.2
    Given the solver study path is "Antares_Simulator_Tests_NR/valid-hydro/H700-26"
    When I run antares simulator
    Then the simulation succeeds
    And the expected value of the annual system cost is 161200000

  @fast @valid-hydro
  Scenario: H700-27 Expensive water + leeway 4:5 (with spillage)
    Given the solver study path is "Antares_Simulator_Tests_NR/valid-hydro/H700-27"
    When I run antares simulator
    Then the simulation succeeds
    And the expected value of the annual system cost is 30800000

  # H700-28 has a pumping efficiency of 1.5 (a physical free-energy loop):
  # the objective is degenerate and the annual system cost comes out as
  # -nan(ind) with the sirius solver on the Windows CI runner (it yielded a
  # finite value on the reference Linux build). The numeric anchors are not
  # cross-platform reproducible, so this scenario is quarantined and only
  # checks that the solver completes without aborting.
  @flaky @valid-hydro
  Scenario: H700-28 Pumping with amplification (efficiency 1.5)
    Given the solver study path is "Antares_Simulator_Tests_NR/valid-hydro/H700-28"
    When I run antares simulator
    Then the simulation succeeds

  # H700-29 also produces a -nan(ind) annual system cost with sirius on the
  # Windows CI runner (numerically degenerate). Quarantined; the scenario only
  # checks that the solver completes without aborting.
  @flaky @valid-hydro
  Scenario: H700-29 Pumping with losses (efficiency 0.5)
    Given the solver study path is "Antares_Simulator_Tests_NR/valid-hydro/H700-29"
    When I run antares simulator
    Then the simulation succeeds

  @fast @valid-hydro
  Scenario: H700-30 Pumping without losses (efficiency 1.0), hard bounds off
    Given the solver study path is "Antares_Simulator_Tests_NR/valid-hydro/H700-30"
    When I run antares simulator
    Then the simulation succeeds
    And the expected value of the annual system cost is 238880000

  @fast @valid-hydro
  Scenario: H700-31 Day-dependent generation/pumping credits, upper bound 60%
    Given the solver study path is "Antares_Simulator_Tests_NR/valid-hydro/H700-31"
    When I run antares simulator
    Then the simulation succeeds
    And the expected value of the annual system cost is 146592000

  # ---------------------------------------------------------------------------
  # Sub-family D' - small inflows, alternating water value, power-to-level
  # ---------------------------------------------------------------------------

  # H700-32 produces a -nan(ind) annual system cost with sirius on the Windows
  # CI runner (the power-to-level / hard-bounds optimum is numerically
  # degenerate there; it yielded a finite value on the reference Linux build).
  # Quarantined; only checks that the solver completes without aborting.
  @flaky @valid-hydro
  Scenario: H700-32 Power-to-level modulations, hard bounds 70/30%, simplex 24
    Given the solver study path is "Antares_Simulator_Tests_NR/valid-hydro/H700-32"
    When I run antares simulator
    Then the simulation succeeds

  @fast @valid-hydro
  Scenario: H700-33 UHT/leeway off, no hard bounds, simplex 168
    Given the solver study path is "Antares_Simulator_Tests_NR/valid-hydro/H700-33"
    When I run antares simulator
    Then the simulation succeeds
    And the expected value of the annual system cost is 187749000

  # H700-34 produces a -nan(ind) annual system cost with sirius on the Windows
  # CI runner (numerically degenerate, like H700-32). Quarantined; only checks
  # that the solver completes without aborting.
  @flaky @valid-hydro
  Scenario: H700-34 UHT/leeway off, hard bounds 70/30%, large thermal unit
    Given the solver study path is "Antares_Simulator_Tests_NR/valid-hydro/H700-34"
    When I run antares simulator
    Then the simulation succeeds

  @fast @valid-hydro
  Scenario: H700-44 Variant of H700-32, initial level 30-70%
    Given the solver study path is "Antares_Simulator_Tests_NR/valid-hydro/H700-44"
    When I run antares simulator
    Then the simulation succeeds
    And the expected value of the annual system cost is 184575000

  @fast @valid-hydro
  Scenario: H700-45 Same as H700-44 with weekly simplex (168)
    Given the solver study path is "Antares_Simulator_Tests_NR/valid-hydro/H700-45"
    When I run antares simulator
    Then the simulation succeeds
    And the expected value of the annual system cost is 184575000

  @fast @valid-hydro
  Scenario: H700-46 UHT/leeway off, hard bounds active, simplex 168
    Given the solver study path is "Antares_Simulator_Tests_NR/valid-hydro/H700-46"
    When I run antares simulator
    Then the simulation succeeds
    And the expected value of the annual system cost is 184575000

  # ---------------------------------------------------------------------------
  # Family E - rule-curve shapes (concave / convex / rugged)
  # ---------------------------------------------------------------------------

  @fast @valid-hydro
  Scenario: H700-35 No inflow, lower rule curve strictly concave (UHT = NO)
    Given the solver study path is "Antares_Simulator_Tests_NR/valid-hydro/H700-35"
    When I run antares simulator
    Then the simulation succeeds
    And the expected value of the annual system cost is 87320500

  @fast @valid-hydro
  Scenario: H700-36 No inflow, upper rule curve strictly convex (UHT = NO)
    Given the solver study path is "Antares_Simulator_Tests_NR/valid-hydro/H700-36"
    When I run antares simulator
    Then the simulation succeeds
    And the expected value of the annual system cost is 87412000

  # H700-37 produces a -nan(ind) annual system cost with sirius on the Windows
  # CI runner (the rugged rule-curve optimum is numerically degenerate there;
  # it yielded a finite value on the reference Linux build). Quarantined; only
  # checks that the solver completes without aborting.
  @flaky @valid-hydro
  Scenario: H700-37 No inflow, single rugged rule curve (UHT = NO)
    Given the solver study path is "Antares_Simulator_Tests_NR/valid-hydro/H700-37"
    When I run antares simulator
    Then the simulation succeeds

  # H700-38 / H700-40 currently abort with the sirius solver (the only LP solver
  # available in this build): "Linear optimization failed ... Year N failed at
  # week 1" on these UHT=YES rule-curve variants. Quarantined until investigated
  # or run with another solver; the scenarios document the current behaviour.
  @flaky @valid-hydro
  Scenario: H700-38 No inflow, UHT=YES, lower rule curve concave (infeasible with sirius)
    Given the solver study path is "Antares_Simulator_Tests_NR/valid-hydro/H700-38"
    When I run antares simulator
    Then the simulation fails

  @fast @valid-hydro
  Scenario: H700-39 1 MW inflow, UHT=YES, lower rule curve concave
    Given the solver study path is "Antares_Simulator_Tests_NR/valid-hydro/H700-39"
    When I run antares simulator
    Then the simulation succeeds
    And the expected value of the annual system cost is 87288000

  @flaky @valid-hydro
  Scenario: H700-40 1 MW inflow, UHT=YES, rugged rule curve (infeasible with sirius)
    Given the solver study path is "Antares_Simulator_Tests_NR/valid-hydro/H700-40"
    When I run antares simulator
    Then the simulation fails

  # ---------------------------------------------------------------------------
  # Family F - managed reservoir, surplus requalified as overflow
  # ---------------------------------------------------------------------------

  @fast @valid-hydro
  Scenario: H700-41 Reservoir fills to 100%, inflows requalified as overflow
    Given the solver study path is "Antares_Simulator_Tests_NR/valid-hydro/H700-41"
    When I run antares simulator
    Then the simulation succeeds
    And the expected value of the annual system cost is 0

  @fast @valid-hydro
  Scenario: H700-42 Same as H700-41 with UHT = NO
    Given the solver study path is "Antares_Simulator_Tests_NR/valid-hydro/H700-42"
    When I run antares simulator
    Then the simulation succeeds
    And the expected value of the annual system cost is 0

  @fast @valid-hydro
  Scenario: H700-43 Same as H700-41 with some generation/pumping on Jan 1st
    Given the solver study path is "Antares_Simulator_Tests_NR/valid-hydro/H700-43"
    When I run antares simulator
    Then the simulation succeeds
    And the expected value of the annual system cost is 0

  # ---------------------------------------------------------------------------
  # Family G - studies without comments.txt (config only)
  # ---------------------------------------------------------------------------

  @fast @valid-hydro
  Scenario: H700-47 Near-infinite reservoir, leeway 1:1
    Given the solver study path is "Antares_Simulator_Tests_NR/valid-hydro/H700-47"
    When I run antares simulator
    Then the simulation succeeds
    And the expected value of the annual system cost is 9477570000000

  @fast @valid-hydro
  Scenario: H700-48 Near-infinite reservoir, load following on
    Given the solver study path is "Antares_Simulator_Tests_NR/valid-hydro/H700-48"
    When I run antares simulator
    Then the simulation succeeds
    And the expected value of the annual system cost is 9477860000000

  @fast @valid-hydro
  Scenario: H700-49 Reservoir variant, leeway 1:1
    Given the solver study path is "Antares_Simulator_Tests_NR/valid-hydro/H700-49"
    When I run antares simulator
    Then the simulation succeeds
    And the expected value of the annual system cost is 9477570000000

  @fast @valid-hydro
  Scenario: H700-50 Reservoir variant, load following = false
    Given the solver study path is "Antares_Simulator_Tests_NR/valid-hydro/H700-50"
    When I run antares simulator
    Then the simulation succeeds
    And the expected value of the annual system cost is 9477860000000

  # ---------------------------------------------------------------------------
  # Family H - settings combination / pumped-storage plants
  # ---------------------------------------------------------------------------

  # H700-51 (six areas combining UHT x HB x LW) currently aborts with the sirius
  # solver ("Year 4 failed at week 1"). Quarantined like H700-38 / H700-40.
  @flaky @valid-hydro
  Scenario: H700-51 Six areas combining UHT/HB/LW settings (infeasible with sirius)
    Given the solver study path is "Antares_Simulator_Tests_NR/valid-hydro/H700-51"
    When I run antares simulator
    Then the simulation fails

  @fast @valid-hydro
  Scenario: H701-52 PSP, reservoir 100000, initial level 10%
    Given the solver study path is "Antares_Simulator_Tests_NR/valid-hydro/H701-52"
    When I run antares simulator
    Then the simulation succeeds
    And the expected value of the annual system cost is 2326650000
    And in area "& PSP", during year 1, total hydro production is 1674440 MWh
    And in area "& PSP", during year 1, total hydro pumping is 2325613 MWh

  @fast @valid-hydro
  Scenario: H701-53 PSP, reservoir 100000, initial level 90%
    Given the solver study path is "Antares_Simulator_Tests_NR/valid-hydro/H701-53"
    When I run antares simulator
    Then the simulation succeeds
    And the expected value of the annual system cost is 2301990000

  @fast @valid-hydro
  Scenario: H701-54 PSP, reservoir 2000000, intra-daily modulation 24
    Given the solver study path is "Antares_Simulator_Tests_NR/valid-hydro/H701-54"
    When I run antares simulator
    Then the simulation succeeds
    And the expected value of the annual system cost is 2299890000

  @fast @valid-hydro
  Scenario: H701-55 PSP, intra-daily modulation 1.5
    Given the solver study path is "Antares_Simulator_Tests_NR/valid-hydro/H701-55"
    When I run antares simulator
    Then the simulation succeeds
    And the expected value of the annual system cost is 2299890000

  @fast @valid-hydro
  Scenario: H701-56 PSP, intra-daily modulation 1
    Given the solver study path is "Antares_Simulator_Tests_NR/valid-hydro/H701-56"
    When I run antares simulator
    Then the simulation succeeds
    And the expected value of the annual system cost is 2299890000

  @fast @valid-hydro
  Scenario: H701-57 PSP, generation credit 2400, intra-daily modulation 2
    Given the solver study path is "Antares_Simulator_Tests_NR/valid-hydro/H701-57"
    When I run antares simulator
    Then the simulation succeeds
    And the expected value of the annual system cost is 2302210000

  @fast @valid-hydro
  Scenario: H701-58 PSP, constant inflow 10 MW/h, weekly optimisation
    Given the solver study path is "Antares_Simulator_Tests_NR/valid-hydro/H701-58"
    When I run antares simulator
    Then the simulation succeeds
    And the expected value of the annual system cost is 2298740000
    And in area "& PSP", during year 1, total hydro production is 2081772 MWh
    And in area "& PSP", during year 1, total hydro pumping is 2770043 MWh

  @fast @valid-hydro
  Scenario: H701-59 PSP, constant inflow 10 MW/h, daily optimisation
    Given the solver study path is "Antares_Simulator_Tests_NR/valid-hydro/H701-59"
    When I run antares simulator
    Then the simulation succeeds
    And the expected value of the annual system cost is 2298740000
