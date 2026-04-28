Feature: Out-of-bounds processing

	@fast
	Scenario: cyclic mode with literal timeshift
		Given the modeler study path is "modeler/simple_system_cyclic"
		When I run antares modeler
		Then the simulation succeeds
		And the objective value is 1500
  
	@fast
	Scenario: drop mode with literal timeshift
		Given the modeler study path is "modeler/simple_system_drop"
		When I run antares modeler
		Then the simulation succeeds
		And the objective value is 300

	# Expected number of active constraint rows per (study_id, constraint_name).
	# Constraints: 2 components (gen_1, gen_2) × 3 timesteps = 6 potential instances each.
	#
	# system_cyclic_with_param_in_shift — no drop mode, all instances present:
	#   is_on_dynamics (lb + ub): 6 each
	#   min_up_duration (ub only): 6
	#   min_down_duration (ub only): 6
	#
	# system_drop_with_param_in_shift — drop mode active:
	#   is_on_dynamics: shift -1 (from is_on[t-1]) → dropped at t=0 for all → 4
	#   min_up_duration: gen_1 has d_min_up=2 → range [-1,0] → dropped at t=0 for gen_1;
	#                    gen_2 has d_min_up=1 → range [0,0] → kept at t=0 → 2+3=5
	#   min_down_duration: both have d_min_down=1 → range [0,0] → never dropped → 6

	@fast
	Scenario: cyclic mode with parameter in sum shift bounds
		Given the modeler study path is "modeler/system_cyclic_with_param_in_shift"
		When I run antares modeler
		Then the simulation succeeds
		And the objective value is 10700
		And the subproblem contains the following number of active constraint rows
			| constraint_name   | count |
			| is_on_dynamics    | 6     |
			| min_up_duration   | 6     |
			| min_down_duration | 6     |

	@fast
	Scenario: drop mode with parameter in sum shift bounds
		Given the modeler study path is "modeler/system_drop_with_param_in_shift"
		When I run antares modeler
		Then the simulation succeeds
		And the objective value is 9800
		And the subproblem contains the following number of active constraint rows
			| constraint_name   | count |
			| is_on_dynamics    | 4     |
			| min_up_duration   | 5     |
			| min_down_duration | 6     |
