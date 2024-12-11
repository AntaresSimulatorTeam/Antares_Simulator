# OR-Tools integration

[OR-Tools](https://developers.google.com/optimization) is an open-source software suite for optimization developed by Google. 
It is compatible with most of existing open-source & commercial solvers.  
*Antares Simulator* uses OR-Tools as an interface, in order to ensure its own compatibility with multiple solvers.

## Available solvers

These are the solver names and enum, defined in the [OR-Tools API](https://github.com/google/or-tools/blob/stable/ortools/linear_solver/linear_solver.cc).

### Linear optimization

| OR-Tools solver name | Enum                      | Usage       | Website                                                                                                            |
|:---------------------|---------------------------|-------------|--------------------------------------------------------------------------------------------------------------------|
| `clp`                | CLP_LINEAR_PROGRAMMING    | Free-to-use | [https://github.com/coin-or/Clp](https://github.com/coin-or/Clp)                                                   |
| `glpk_lp`            | GLPK_LINEAR_PROGRAMMING   | Free-to-use | [https://www.gnu.org/software/glpk](https://www.gnu.org/software/glpk)                                             |
| `glop`               | GLOP_LINEAR_PROGRAMMING   | Free-to-use | [https://developers.google.com/optimization/lp/glop](https://developers.google.com/optimization/lp/glop)           |
| `highs_lp`           | HIGHS_LINEAR_PROGRAMMING  | Free-to-use | [https://highs.dev](https://highs.dev)                                                                             |
| `pdlp`               | PDLP_LINEAR_PROGRAMMING   | Free-to-use | [https://developers.google.com/optimization/lp/pdlp_math](https://developers.google.com/optimization/lp/pdlp_math) |
| `sirius_lp` [^1]     | SIRIUS_LINEAR_PROGRAMMING | Free-to-use | [https://github.com/rte-france/sirius-solver](https://github.com/rte-france/sirius-solver)                         |
| `gurobi_lp`          | GUROBI_LINEAR_PROGRAMMING | Commercial  | [https://www.gurobi.com](https://www.gurobi.com)                                                                   |
| `cplex_lp`           | CPLEX_LINEAR_PROGRAMMING  | Commercial  | [https://www.ibm.com/fr-fr/analytics/cplex-optimizer](https://www.ibm.com/fr-fr/analytics/cplex-optimizer)         |
| `xpress_lp`          | XPRESS_LINEAR_PROGRAMMING | Commercial  | [https://www.fico.com/en/products/fico-xpress-solver](https://www.fico.com/en/products/fico-xpress-solver)         |

[^1]: SIRIUS solver is only supported in [RTE's builds](https://github.com/rte-france/or-tools-rte/releases) of OR-Tools.

### Mixed real integer optimization

| OR-Tools solver name | Enum                             | Usage       | Website                                                                                                    |
|:---------------------|----------------------------------|-------------|------------------------------------------------------------------------------------------------------------|
| `scip`               | SCIP_MIXED_INTEGER_PROGRAMMING   | Free-to-use | [https://www.scipopt.org](https://www.scipopt.org)                                                         |
| `glpk`               | GLPK_MIXED_INTEGER_PROGRAMMING   | Free-to-use | [https://www.gnu.org/software/glpk](https://www.gnu.org/software/glpk)                                     |
| `cbc`                | CBC_MIXED_INTEGER_PROGRAMMING    | Free-to-use | [https://github.com/coin-or/Cbc](https://github.com/coin-or/Cbc)                                           |
| `highs`              | HIGHS_MIXED_INTEGER_PROGRAMMING  | Free-to-use | [https://highs.dev](https://highs.dev)                                                                     |
| `gurobi`             | GUROBI_MIXED_INTEGER_PROGRAMMING | Commercial  | [https://www.gurobi.com](https://www.gurobi.com)                                                           |
| `cplex`              | CPLEX_MIXED_INTEGER_PROGRAMMING  | Commercial  | [https://www.ibm.com/fr-fr/analytics/cplex-optimizer](https://www.ibm.com/fr-fr/analytics/cplex-optimizer) |
| `xpress`             | XPRESS_MIXED_INTEGER_PROGRAMMING | Commercial  | [https://www.fico.com/en/products/fico-xpress-solver](https://www.fico.com/en/products/fico-xpress-solver) |

### Constraint optimization

| OR-Tools solver name | Enum                               | Usage       | Optimization type |
|:---------------------|------------------------------------|-------------|-------------------|
| `bop`                | BOP_INTEGER_PROGRAMMING            | Free-to-use | Boolean           |
| `sat`                | SAT_INTEGER_PROGRAMMING            | Free-to-use | Boolean integer   |
| `knapsack`           | KNAPSACK_MIXED_INTEGER_PROGRAMMING | Free-to-use |                   |

## Integration in Antares Simulator

*Antares Simulator* needs 2 types of solvers:

* A linear solver
* A mixed real integer solver

While the OR-Tools interface allows using multiple solvers, Antares restricts this usage.  
The *Antares Simulator* user can select the solvers using the `solver` command-line option. Here are the allowed
values:

| `solver`   | Linear solver                  | Mixed real integer solver        |
|:-------------------|--------------------------------|----------------------------------|
| `sirius` (default) | SIRIUS_LINEAR_PROGRAMMING      | SIRIUS_MIXED_INTEGER_PROGRAMMING |
| `coin`             | CLP_LINEAR_PROGRAMMING         | CBC_MIXED_INTEGER_PROGRAMMING    |
| `xpress`           | XPRESS_LINEAR_PROGRAMMING      | XPRESS_MIXED_INTEGER_PROGRAMMING |
| `glpk`             | GLPK_LINEAR_PROGRAMMING        | GLPK_MIXED_INTEGER_PROGRAMMING   |
| `scip`             | SCIP_MIXED_INTEGER_PROGRAMMING | SCIP_MIXED_INTEGER_PROGRAMMING   |

The following commercial solvers are not yet supported by *Antares Simulator*, because of unmet pre-requisites:

| `solver` | Linear solver             | Mixed real integer               | Waiting for                                         |
|:-----------------|---------------------------|----------------------------------|-----------------------------------------------------|
| `cplex`          | CPLEX_LINEAR_PROGRAMMING  | CPLEX_MIXED_INTEGER_PROGRAMMING  | Update OR-Tools building process for CPLEX support  |
| `gurobi`         | GUROBI_LINEAR_PROGRAMMING | GUROBI_MIXED_INTEGER_PROGRAMMING | Update OR-Tools building process for GUROBI support |
