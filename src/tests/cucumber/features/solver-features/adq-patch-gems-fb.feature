Feature: adequacy-patch GEMS flow-based constraints

  # Positive path: a valid CCR overlay with two PTDF constraints is assembled on top
  # of the reference adequacy-patch study, the flag is enabled, and the solver must
  # exit cleanly (the CSR QP with extra RAM rows is feasible).
  @gems_fb
  Scenario: GEMS flow-based constraints are injected into the CSR and simulation succeeds
    Given the solver study path is a copy of "adequacy-patch-CSR/adq-patch-CSR-test-case-v01"
    And the GEMS overlay "adequacy-patch-CSR/adq-patch-gems-fb-demo" is applied to the study
    And the generaldata setting "enable-gems-fb-constraints" in "adequacy patch" is "true"
    When I run antares simulator
    Then the simulation succeeds

  # Negative path: the GEMS model-library file is syntactically invalid YAML.
  # The solver must detect the malformed input and exit with a non-zero status rather
  # than crashing or producing silently wrong results.
  @gems_fb
  Scenario: Malformed GEMS model library causes graceful solver failure
    Given the solver study path is a copy of "adequacy-patch-CSR/adq-patch-CSR-test-case-v01"
    And the GEMS overlay "adequacy-patch-CSR/adq-patch-gems-fb-bad-yaml" is applied to the study
    And the generaldata setting "enable-gems-fb-constraints" in "adequacy patch" is "true"
    When I run antares simulator
    Then the simulation fails