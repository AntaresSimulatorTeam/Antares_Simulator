# Adding expressions from GEMS into legacy LP constraints

Every time we want to add an expression from GEMS into an existing legacy LP constaint, this expression 
is evaluated into a time-dependent linear expression.
A time-dependent linear expression is a vector of linear expressions (one for each time step).
A linear expression comprises a linear combination of variables and a unique constant term.
In a linear expression, each variable is associated to a constant coefficient.

So when adding an expression from GEMS into a LP constraint, we merely add coefficients or constants to the constraint.

In the following, writing constraints often implies gathering constant terms in the RHS, 
and variable terms to the LHS.

## Balance constraint

In Antares legacy modeler code, balance constraint is actually written as follows :
 **-P = -(L-Fatal)**, with **P** being productions, **L** being loads and **L-Fatal** being the residual load.
An equivalent form is : **L - P = Fatal**.
Now, let's break **P** into **Pv** (production given as a variable of optimization) and **Pc** (production as parameter, e.g. fatal production) :
**L - Pv = Fatal + Pc**
As a consequence, variable productions from GEMS are added negatively, and constant productions are added positively.

## Fictitious load constraint

This constraint can be written as follows : 

**Spillage - Thermal - Hydro < Fatal**.

As previously, adding variable productions from GEMS is done negatively, and adding constant production is done positively.

## Bounding unsupplied energy constraint

This constraint can be written as follows :

**unsupE - Lv < Lc + Fatal**, **Lv** being variables loads, **Lc** being constant loads.

adding variable loads from GEMS is done negatively, and adding constant loads is done positively.

## Adequacy patch CSR hourly problem

In hybrid studies, the adequacy patch CSR post-processing must account for GEMS components
when computing the bounds of the hourly curtailment sharing problem.

The GEMS contribution is evaluated by reading port field expressions
(`unsupplied_energy_bound` and `spillage_bound`) at the triggered hour,
using `EvalVisitor` on the `OptimEntityContainer` retained from the weekly solve.

The following bounds are modified:

### ENS variable upper bound

The upper bound of the ENS variable for each area inside the adequacy patch
is increased by the evaluated `unsupplied_energy_bound` expression of connected GEMS components:

**Xmax(ENS) = DENS_new + GEMS_unsupplied_energy_bound**

### Fictitious load constraint RHS

The RHS of the fictitious load constraint (bounding spillage) is increased
by the evaluated `spillage_bound` expression of connected GEMS components:

**RHS(fictitious_load) = STt - (1-BT)·STmint + BH·Ht + BF·(Ft - Lt) + STS + GEMS_spillage_bound**

### Max unsupplied energy constraint RHS

The RHS of the max unsupplied energy constraint is increased
by the evaluated `unsupplied_energy_bound` expression:

**RHS(max_unsupE) = Load + GEMS_unsupplied_energy_bound**

### Implementation

The GEMS contribution is encapsulated in the `ActiveGemsPart` class (null object pattern).
In pure Legacy studies, `NullGemsPart` is used and all GEMS methods are no-ops.
The factory `makeGemsPart()` selects the implementation based on the presence of `modelerData`.

### Restrictions in hybrid mode

In hybrid mode, the `price-taking-order` parameter must be set to `DENS`.
The `Load` option is not supported because the Legacy load does not include
the GEMS load, which would result in an incorrect PTO denominator
in the CSR quadratic objective function (Σ ENS²/PTO).

This restriction is enforced at startup in `AdqPatchParams::checkAdqPatchPriceTakingOrderForHybrid()`.
Using `Load` in hybrid mode throws `IncompatiblePriceTakingOrderForHybrid`

Additionally, GEMS areas and links are implicitly considered as outside the adequacy
patch domain. Only Legacy areas can be classified as "physical inside" the patch.
GEMS links are treated as connecting no zone inside the domain, or at least one
virtual zone. The GEMS contribution to the CSR problem is limited to port field
expressions (`unsupplied_energy_bound`, `spillage_bound`) evaluated on Legacy areas
connected to GEMS components via port-to-area connections.