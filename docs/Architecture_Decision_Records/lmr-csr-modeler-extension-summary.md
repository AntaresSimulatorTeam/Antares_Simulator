# LMR/CSR Modeler Extension — Summary

The CSR post-processing (adequacy patch) computes upper bounds on ENS and Spillage from legacy-only generation sub-components, making it blind to Modeler (GEMS) objects connected to areas.
The fix adds two new arrays (`ModelerSpillageContrib`, `ModelerEnsContrib`) to `PROBLEME_HEBDO`, filled inside `OPT_TryToCallSimplex` (after the main LP solve) by evaluating each Modeler component's `spillage_bound` and `unsupplied_energy_bound` port-field expressions at the LP solution values.
These stored values are then added to the existing CSR RHS formulas in `setRHSfictitiousLoadValue` and `setRHSMaxEnsLoadValue` (`construct_problem_constraints_RHS.cpp`), without touching any legacy BT/BH/BF logic.
The LMR step (DENS computation) requires no change: it reads net solution outputs (`ENS_init`, link flows, `Spillage_init`) that already embed Modeler contributions through the main LP.
Four files are modified: `sim_structure_probleme_economique.h`, `sim_alloc_probleme_hebdo.cpp`, `opt_appel_solveur_lineaire.cpp`, and `construct_problem_constraints_RHS.cpp`.
