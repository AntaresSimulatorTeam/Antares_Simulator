# Adequacy Patch calculation

## Foreword

The operational algorithm of Euphémia market coupling implements "de-optimization" rules in order to fix the sharing of “Energy Not Served” (ENS) between the market areas when there is any (in a way to ensure a certain fairness in the sharing of this ENS): this is what we call the adequacy patch.  
The requirement of fairness rules in the sharing of ENS within Antares studies increased with the introduction of flow-based modelling. Previously, this problem was mainly solved with the hurdle cost mechanism (small costs on interconnections) limiting exports from areas where ENS was encountered. However, implicit rules in Antares for sharing ENS, even outside the Flow-Based domain, are not fully satisfactory, even with hurdle costs, because they prioritize the treatment of ENS in countries directly connected to countries with margins, to the detriment of more distant countries. In a context of decommissioning of coal and nuclear plants in France and in Europe, cases of simultaneous ENS over the domain will increase. Consequently, the need of adequacy patch to share fairly this ENS is arising.

It should be noted that, in the current implementation of the patch within Antares, the implementation covers only one of the three main features of the ‘curtailment sharing and minimization’ principles in EUPHEMIA, namely:

1. Local matching constraint.

2. The implemented solution does not yet consider secondary problem of sharing of curtailment by the quadratic term.

3. The implemented solution does not yet consider the feature in EUPHEMIA, related to ‘minimization of curtailment’ in the primal problem which includes an extra penalty term directly in the global objective function in EUPHEMIA by multiplying the maximum curtailment ratio with a large M pre-factor in the total welfare.

## Local matching rule

The rule implemented is called the « Local Matching » rule:

- This rule consists in satisfying **ENS \\(\leq\\) DENS** (Domestic Energy not Served) for all physical areas inside the adequacy patch.
- It specifies that an area cannot export more than its local margin.

This means that an area which can serve local demand with local generation, should never have ENS.

_**Notes**_

- _The local matching rule is only part of the economic mode of Antares (Not the Adequacy or Draft mode);_
- _DENS outputs of Antares, for all selected areas, are available in the General values tab, as year-by-year and synthetic results. All the statistic values for it are calculated as well (min, max, mean, standard deviation). For the areas not part of the patch, the output DENS is set to 0._

### Specifications of the evolution concerning the optimization problem

To calculate the DENS used to constrain the ENS variations of each area, a first iteration is performed. During this first iteration, the usual optimization problem is slightly modified before being solved. The objective of this first iteration is to get an ENS of each area which will represent its DENS. In order to achieve this, links between areas are forced to have a capacity of 0. To ensure that customizable models will still work using this new feature, different categories are created for the areas, enabling different behaviours as needed. The different categories are:

- "virtual": The area is a virtual area,
- "physical outside": The area is a physical area not included in the adequacy patch domain,
- "physical inside": The area is a physical area included in the adequacy patch domain.

In case Flow-Based is represented in the study, the "hub" node introduced for the Flow-based modelling should also be included in the list of physical areas (this one should be declared by the user in its list of physical nodes).

#### First iteration

In the first iteration, the following link capacities in the following direction will be set to 0 MW:

- node "physical inside" <-> node "physical inside"
- node "physical inside" -> node "physical outside"
- node "physical outside" -> node "physical inside": optional/user defined (default `true`),
- node "physical outside" <-> node "physical outside": optional/user defined (default `true`).

Once the first iteration is completed, we have access to the DENS value, which represents the ability of an area to fulfil its local load thanks to its local production capacities:
- DENS = ENS for each physical area inside the adequacy patch perimeter

#### Second iteration

During the second iteration, all link capacities between physical nodes are set to the values provided as input data (as it is done in the current Antares version). The only change compared to a simulation without the adequacy patch is that the upper bound of the ENS on the areas included in the patch is now constrained by the DENS found during the first iteration. This is introduced in the optimization problem for physical areas that are declared in the adequacy patch domain (areas declared as "physical inside"):

- ENS <= DENS
 (for all physical areas inside the adequacy patch).

## Curtailment sharing rule
The curtailment sharing rule will be implemented in Antares Simulator 8.4. It will include an hourly post-processing phase.
