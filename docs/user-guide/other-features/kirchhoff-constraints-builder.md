# Kirchhoff's constraint generator

> _**WARNING:**_ this feature is deprecated and will be removed in a future release. If you are still using it, 
> please [get in touch](https://github.com/AntaresSimulatorTeam/Antares_Simulator/issues) with us.

**Executable**: antares-kirchhoff-constraints-builder (currently released for Windows & Ubuntu only)

## Principles

Binding Constraints can take many forms (hourly, daily, weekly), involve flows or thermal generated power, etc. Sets of binding constraints of special interest are those which can be used to model and enforce Kirchhoff's second law on the AC flows.

In other words, it is possible to make Antares work as a genuine DC OPF, provided that consistent binding constraints are written down for each cycle belonging to any cycle basis of the graph made out from all AC components of the power system (V vertices, E edges).

- In a fully connected graph (V, E), there are as many binding constraints to write down as there are cycles in any cycle basis of the graph, which amounts to (E+1-V). The number of different possible basis is equal to that of spanning trees, which can be assessed by the Kirchhoff's theorem [^15]

- Among all cycle basis, some should be preferred to others because they lead to a sparser constraint matrix.

To get around this issue, the KCG is an autonomous Antares module (much like the time-series analyzer) which automatically instantiates a set of adequate binding constraints that will enforce Kirchhoff's law on the AC subgraph of the power system. The graph cycle basis associated with the generated constraints is optimal, in that sense that it leads to a constraint matrix as sparse as possible.
To achieve that, the KCG implements an efficient algorithm yielding a minimal cycle basis [^16] and, for all cycles of the chosen basis, generates constraints of the form:

$$c= 1, ..., C : \sum_{l \in C}{sign(l,c)F_lZ_l} = 0$$

Where $Z_l$ are the impedances (parameters) and $F_l$ are the flows (variables).

Beyond this basic purpose, the KCG is meant to provide additional modeling capacities, so as to allow the representation of two important phenomena:

- As a rule, the power system graph represented in Antares in not fully detailed, it is usually more a "backbone" approximation, in which "vertices" are not equivalent to individual bus-bars. More likely, vertices of the graph stand for whole regions, or even countries: as a consequence, it is highly possible that when all Areas/Vertices have a zero-balance (neither import, nor export), there are real physical flows between them, so-called "loop flows". If assessments of the level of these loop flows are available (and filled out as link input data), the KCG may include them (on user's request) in the binding constraints formulation, which becomes:

$$c= 1, ..., C : \sum_{l \in C}{sign(l,c)F_lZ_l} = \sum_{l \in C}{sign(l,c)\varphi_lZ_l}$$

- To mitigate the effects of actual loop flows, or more generally to allow the transmission assets to give the maximum of their potential, the power system may include components such as phase-shifting transformers, whose function can be modeled by changing the formulation of the binding constraints. Provided that estimates of the shifting capacities ($Y_l^-$, $Y_l^+$) of the installed PST are known and filled out in the link data section, the KCG will (on user's request) automatically reformulate the binding constraints as:

$$c= 1, ..., C : Y_c^- + \sum_{l \in C}{sign(l,c)\varphi_lZ_l} \leq \sum_{l \in C}{sign(l,c)F_lZ_l} \leq Y_c^+ + \sum_{l \in C}{sign(l,c)\varphi_lZ_l}$$

with:

$$Y_c^- = \sum_{l \in C}{Min(sign(l,c)Y_l^-, sign(l,c)Y_l^+)}$$

$$Y_c^+ = \sum_{l \in C}{Max(sign(l,c)Y_l^-, sign(l,c)Y_l^+)}$$

Besides, the KCG takes into account the fact that the "best estimates" of all critical data (loop flows, phase-shifting ratings, or even impedances) may vary in time: In such cases, the KCG formulates as many different binding constraints as necessary to model this operating context diversity, and relax them when appropriate (by setting the right hand sides of the equation to +/- infinite)

From a practical standpoint, assessments of $Y^-, Y^+$ should be derived from knowledge about the actual components installed on the grid, while $Z_l$ and $ \varphi_l $ can be estimated by various methods.

- Choice of a specific period of time for which the constraints should be applied, while completely relaxed at other moments
- Before actual generation of binding constraints, preview of the "minimal length" spanning tree used as starting point for the optimal basis algorithm (left column of the table – links displayed with "0" do not belong to the tree)
- Before actual generation of binding constraints, preview of the "optimal cycle basis" used as starting point for constraints generation (right column of the table – links displayed with "n" appear in n different cycles of the basis)


[^15]: The number of spanning trees is equal to the absolute value of any cofactor of the graph incidence matrix

[^16]: Mehlhorn K., Michail D. (2005) _Implementing Minimum Cycle Basis Algorithms_. In: Experimental and Efficient Algorithms. WEA 2005. Lecture Notes in Computer Science, vol 3503.

## Command-line instructions

The kirchhoff constraint generator is a standalone tool used to automatically create kirchhoff constraints. For more details about this process read:

[7-kirchhoffs_constraint_generator.md](https://github.com/AntaresSimulatorTeam/Antares_Simulator/blob/develop/docs/reference-guide/07-kirchhoffs_constraint_generator.md)

The binary is located in Antares_Simulator/bin/

---

### Usage

```bash
./antares-kirchhoff-constraints-builder [study_path] [option_file_path]
```

### Options

**study_path**: the path of the study, mandatory

**option_file_path**: Optional. The path of the **constraintbuilder.ini** file. It's normally located in *study_path/settings/*. If not provided the constraint builder will take the default values, which are defined at the end of cbuilder.h.

### Results

New constraints generated this way will be stored in *input/bindingconstraints/* with the name uto_0001.txt, 
with incrementing numbers. The file *bindingconstraints.ini* located in the same directory will be updated accordingly.  
> *Note*: the outputs of the Kirchhoff's constraints generator are not printed in the general output files
> but kept within the input files structure, the reason being that they are input data for the proper Antares simulation.  
> The associated data, the so-called binding constraints, bear in their name the prefix "@UTO-".

### Study components needed

The study only needs to include the following components to generate kirchhoff constraints:

- Areas (contains links)
- Binding Constraints
- version number
