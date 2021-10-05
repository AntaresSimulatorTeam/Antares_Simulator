# Formulation of the optimisation problems

**_Antares\_Simulator Modeling and Optimization_**

**Document available on : [https://antares-simulator.org](https://antares-simulator.org/)**


## 1 Introduction

The purpose of this document is to give every user of the **Antares\_Simulator** model (regardless of its version number), a detailed and comprehensive formulation of the main optimization problems solved by the application's inner optimization engine.

The aim of the information presented hereafter is to provide a transparent access to the inner workings of the software from a **formal** standpoint. Note that, aside from this conceptual transparency, the software itself offers an option that makes it possible for the user to print, in a standardized format, any or all of the optimization problems actually solved in the course of an **Antares\_Simulator** session

Used together with the elements developed in the next pages, this **practical** access to the internal model implemented the tool allows fair and open benchmarking with comparable software. Besides, another important issue regarding transparency is addressed by the release of **Antares\_Simulator** as an Open Source Gnu GPL 3.0 application.

So as to delimit the scope of the present document with as much clarity as possible, it is important to notice that a typical **Antares\_Simulator** session involves different steps that are usually run in sequence, either automatically or with some degree of man-in-the-loop control, depending on the kind of study to perform.

These steps most often involve:

1. GUI session dedicated to the initialization or to the updating of various input data sections (load time-series, grid topology, wind speed probability distribution, etc.)
2. GUI session dedicated to the definition of simulation contexts (definition of the number and consistency of the ";Monte-Carlo years"; to simulate)
3. Simulation session producing actual numeric scenarios following the directives defined in (b)
4. Optimization session aiming at solving all of the optimization problems associated with each of the scenarios produced in (c).
5. GUI session dedicated to the exploitation of the detailed results yielded by (d)

The scope of this document is exclusively devoted to step (d). Note that equivalent information regarding the other steps may be found in other documents made available either:

- Within the application itself, in the "; ?"; menu
- On the **Antares\_Simulator** website (download section) : [https://antares-simulator.org](https://antares-simulator.org/)
- In technical publications referenced in the bibliography section of the website

The following picture gives a functional view of all that is involved in steps (a) to (e). In this illustration, Step (d), whose goal is to solve the problems introduced in this document, is materialized by the red box.

![](RackMultipart20201119-4-hpmtl9_html_ca6164c8f4d9c041.gif)

The number and the size of the individual problems to solve (a least-cost hydro-thermal unit-commitment and power schedule, with an hourly resolution and throughout a week, over a large interconnected system) make optimization sessions often computer-intensive. Note that the content of the blue ";hydro energy manager"; box appearing on the previous figure, whose purpose is to deal with energy storage issues at the seasonal scale, is not detailed in the present document but in the ";General Reference Guide";.

Depending on user-defined results accuracy requirements, various practical options allow to simplify either the formulation of the weekly UC & dispatch problems (e.g. do not account for constraints associated with operational reserves) or their resolution (i.e. find, for the native MILP, an approximate solution based on two successive LPs). For the sake of simplicity and clarity, the way these options are used to revise the primary problem formulation is not detailed hereafter. Likewise, many simplifications are introduced to keep notations as light as possible. This is why, for instance, the overall sum of load, wind power generation, solar power generation, run of the river generation, and all other kinds of so-called ";must-run"; generation is simply denoted ";load"; in the present document.

## 2 Typology of the problems solved

In terms of power studies, the different fields of application Antares has been designed for are the following:

- **Generation adequacy problems :** assessment of the need for new generating plants so as to keep the security of supply above a given critical threshold

What is most important in these studies is to survey a great number of scenarios that represent well enough the random factors that may affect the balance between load and generation. Economic parameters do not play as much a critical role as they do in the other kinds of studies, since the stakes are mainly to know if and when supply security is likely to be jeopardized (detailed costs incurred in more ordinary conditions are of comparatively lower importance). In these studies, the default Antares option to use is the ";Adequacy"; simulation mode, or the ";Draft"; simulation mode (which is extremely fast but which produces crude results).

- **Transmission project profitability :** assessment of the savings brought by a specific reinforcement of the grid, in terms of decrease of the overall system generation cost (using an assumption of fair and perfect market) and/or improvement of the security of supply (reduction of the loss-of-load expectation).

In these studies, economic parameters and the physical modeling of the dynamic constraints bearing on the generating units are of paramount importance. Though a thorough survey of many ";Monte-Carlo years"; is still required, the number of scenarios to simulate is not as large as in generation adequacy studies. In these studies, the default Antares option to use is the ";Economy"; simulation mode.

- **Generation and/or Transmission expansion planning:** rough assessment of the location and consistency of profitable reinforcements of the generating fleet and/or of the grid at a given horizon, on the basis of relevant reference costs and taking into account feasibility local constraints (bounds on the capacity of realistic reinforcements).

These long term studies clearly differ from the previous ones in the sense that the generation and transmission assets that define the consistency of the power system are no longer passive parameters but are given the status of active problem variables. In the light of both the nature and the magnitude of the economic stakes, there is comparatively a lesser need than before for an accurate physical modeling of fine operational constraints or for an intensive exploration of a great many random scenarios. The computer intensiveness of expansion studies is, however, much higher than that of the other types because the generic optimization problem to address happens to be much larger.

The common rationale of the modeling used in all of these studies is, whenever it is possible, to decompose the general issue (representation of the system behavior throughout many years, with a time step of one hour) into a series of standardized smaller problems.

In **Antares\_Simulator**, the ";elementary "; optimization problem resulting from this approach is that of the minimization of the overall system operation cost over a week, taking into account all proportional and non-proportional generation costs, as well as transmission charges and ";external"; costs such as that of the unsupplied energy (generation shortage) or, conversely, that of the spilled energy (generation excess).

In this light, carrying out generation adequacy studies or transmission projects studies means formulating and solving a series of a great many week-long operation problems (one for each week of each Monte-Carlo year ), assumed to be independent. This generic optimization problem will be further denoted **,** where is an index encompassing all weeks of all Monte-Carlo years.

Note that this independency assumption may sometimes be too lax, because in many contexts weekly problems are actually coupled to some degree, as a result of energy constraints (management of reservoir-type hydro resources, refueling of nuclear power plants, etc.). When appropriate, these effects are therefore dealt with before the actual decomposition in weekly problems takes place, this being done either of the following way (depending on simulation options):

1. Use of an economic signal (typically, a shadow ";water value";) yielded by an external preliminary stochastic dynamic programming optimization of the use of energy-constrained resources.
2. Use of heuristics that provide an assessment of the relevant energy credits that should be used for each period, fitted so as to accommodate with sufficient versatility different operational rules.

Quite different is the situation that prevails in expansion studies, in which weekly problems cannot at all be separated from a formal standpoint, because new assets should be paid for all year-long, regardless of the fact that they are used or not during such or such week : the generic expansion problem encompasses therefore all the weeks of all the Monte-Carlo years at the same time. It will be further denoted \\(\mathcal{P}\\).

The next sections of this document develop the following subjects:

- Notations used for \\(\mathcal{P}^k\\) and \\(\mathcal{P}\\)

- Formulation of \\(\mathcal{P}^k\\)

- Formulation of \\(\mathcal{P}\\)

- Complements to the standard problems (how to make **Antares\_Simulator** work as a SCOPF )

- Miscellaneous complements to the standard problems

## 3 Notations

### 3.1 General notations
**TODO : add units**

| Notation             | Explanation                                                                                                        |
| ------------         | -------------                                                                                                      |
| \\( k \in  K \\)     | optimization periods (weeks) over which \\(P\\) and \\(P^k\\) are defined (omitted for simplicity)                 |
| \\(t \in T\\)        | individual time steps of any optimization period \\( k\in K\\) (hours of the week)                                 |
| \\(G(N,L)\\)         | undirected graph of the power system (connected)                                                                   |
| \\(n \in N\\)        | vertices of \\(G\\), \\(N\\) is an ordered set                                                                     |
| \\(l \in L\\)        | edges of \\(G\\)                                                                                                   |
| \\(A\\)              | incidence matrix of \\(G\\), dimension \\(N\times L\\)                                                             |
| \\(g\\)              | spanning tree of \\(G\\)                                                                                           |
| \\(C_g\\)            | cycle basis associated with \\(g\\), dimension \\(L\times (L+1-N)\\)                                               |
| \\(L_n^+\subset L\\) | set of edges for which \\(n\\) is the upstream vertex                                                              |
| \\(L_n^-\subset L\\) | set of edges for which \\(n\\) is the downstream vertex                                                            |
| \\(u_l \in N\\)      | vertex upstream from \\(l\\)                                                                                       |
| \\(d_l \in N\\)      | vertex downstream from \\(l\\)                                                                                     |
| \\(u \cdot v\\)      | inner product of vectors \\(u\\) and \\(v\\)                                                                       |
| \\(u_\uparrow^p\\)   | vector resulting from the permutation on \\(u \in \mathbb{R}^s\\) : $$ u\_\uparrow^p(i)=u(i+p\, \mathrm{mod}\,s)$$ |


### 3.2 Grid
**TODO : add units**

| Notation                                      | Explanation                                                                                            |
| ------------                                  | -------------                                                                                          |
| \\(C_l^+ \in \mathbb{R}^T_+\\)                | initial transmission capacity from \\(u_l\\) to \\(d_l\\) (variable of \\(P\\) and \\(P^k\\))          |
| \\( \overline{C}\_l^+ \in \mathbb{R}^T\_+ \\) | maximum transmission capacity from \\(u_l\\) to \\(d_l\\) (variable of \\(P\\), not used in \\(P^k\\)) |
| \\(C_l^- \in \mathbb{R}^T_+\\)                | initial transmission capacity from \\(d_l\\) to \\(u_l\\) (variable of \\(P\\) and \\(P^k\\))          |
| \\( \overline{C}^{-}\_l\in \mathbb{R}^T\_{+} \\)   | maximum transmission capacity from \\(d_l\\) to \\(u_l\\) (variable of \\(P\\), not used in \\(P^k\\)) |
| \\(\Psi_l \in \mathbb{R}_+\\)                 | weekly cost of a maximum capacity investment                                                           |
| \\(x_l \in [0,1]\\)                           | transmission capacity investment level                                                                 |
| \\(F_l^+ \in \mathbb{R}^T_+\\)                | power flow through \\(l\\), from \\(u_l\\) to \\(d_l\\)                                                |
| \\(F_l^- \in \mathbb{R}^T_+\\)                | power flow through \\(l\\), from \\(d_l\\) to \\(u_l\\)                                                |
| \\(F_l\in \mathbb{R}^T\\)                     | total power flow through \\(l\\), \\(F_l=F_l^+-F_l^-\\)                                                |
| \\(\tilde{F}_t \in \mathbb{R}^T\\)            | system flow snapshot at time \\(t\\)                                                                   |
| \\(\gamma_l^+\in \mathbb{R}^T\\)              | transmission cost through \\(l\\), from \\(u_l\\) to \\(d_l\\). Proportional to the power flow         |
| \\(\gamma_l^-\in \mathbb{R}^T\\)              | transmission cost through \\(l\\), from \\(d_l\\) to \\(u_l\\). Proportional to the power flow         |
| \\(Z_l \in \mathbb{R}\_+\\)                   | overall impedance of \\(l\\)                                                                           |

### 3.3 Thermal units
**TODO : add units**

| Notation                                                      | Explanation                                                                            |
| ------------                                                  | -------------                                                                          |
| \\(\theta \in \Theta_n\\)                                     | thermal clusters (sets of identical units) installed in node \\(n\\)                   |
| \\(\Theta\\)                                                  | set of all thermal clusters of the power system \\(\Theta = \cup_{n\in N} \Theta_n\\)  |
| \\(\overline{P}\_\theta \in \mathbb{R}^T_+\\)                 | maximum power output from cluster \\(\theta\\), depends on units availability          |
| \\(\underline{P}\_\theta \in \mathbb{R}^T_+\\)                | mimimum power output from cluster \\(\theta\\), units availability allowing            |
| \\(P_\theta \in \mathbb{R}^T_+\\)                             | power output from cluster \\(\theta\\)                                                 |
| \\(\chi_\theta \in \mathbb{R}^T\\)                            | power output from cluster \\(\theta\\)                                                 |
| \\(\sigma_\theta^+ \in \mathbb{R}^T\\)                        | startup cost of a single unit in cluster \\(\theta\\)                                  |
| \\(\tau_\theta \in \mathbb{R}^T\\)                            | running unit in \\(\theta\\) : cost independent from output level (aka NoLoadHeatCost) |
| \\(l_\theta \in \mathbb{R}_+\\)                               | unit in \\(\theta\\) : minimum stable power output when running                        |
| \\(u_\theta \in \mathbb{R}_+\\)                               | unit in \\(\theta\\) : maximum net power output when running                           |
| \\(\Delta_\theta^+ \in \lbrace 1,\dots, \|T\|\rbrace\\)       | unit in \\(\theta\\) : minumum on time when running                                    |
| \\(\Delta_\theta^- \in \lbrace 1,\dots, \|T\|\rbrace\\)        | unit in \\(\theta\\) : minumum off time when not running                               |
| \\(\Delta_\theta = \max(\Delta_\theta^-, \Delta_\theta^+) \\) | duration above which both state changes are allowed                                    |
| \\(M_\theta \in \mathbb{N}^T\\)                               | number of running units in cluster \\(\theta\\)                                        |
| \\(\overline{M}_\theta \in \mathbb{N}^T\\)                    | maximum number of running units in cluster \\(\theta\\)                                |
| \\(\underline{M}_\theta \in \mathbb{N}^T\\)                   | minimum number of running units in cluster \\(\theta\\)                                |
| \\(M_\theta^+ \in \mathbb{N}^T\\)                             | number of units in cluster changing from state off to state on in cluster \\(\theta\\) |
| \\(M_\theta^- \in \mathbb{N}^T\\)                             | number of units in cluster changing from state on to state off in cluster \\(\theta\\) |
| \\(M_\theta^{--} \in \mathbb{N}^T\\)                                   | number of units in cluster changing from state on to state outage cluster \\(\theta\\) |

### 3.4 Reservoir-type hydropower units (or other power storage facilities)
**TODO : add units**

| Notation                                         | Explanation                                                                                                                          |
| ------------                                     | -------------                
| \\(\lambda \in \Lambda_n\\)                      | reservoirs connected to node \\(n\\)                                                                                                 |
| \\(\Sigma_\lambda \in \mathbb{R}_+\\)            | size of reservoir \\(\lambda\\) : amount of energy that can be stored in \\(\lambda\\)                                               |
| \\(Q\in \mathbb{N}\\)                                     | number of discrete levels defined in reservoir                                                                                       |
| \\(\overline{W}\_\lambda \in \mathbb{R}_+\\)     | maximum energy output from \\(\lambda\\) throughout the optimization period                                                          |
| \\(\underline{W}\_\lambda \in \mathbb{R}_+\\)    | minimum energy output from \\(\lambda\\) throughout the optimization period                                                          |
| \\(\overline{H}\_\lambda \in \mathbb{R}_+^T\\)   | maximum power output from reservoir \\(\lambda\\). Note : \\(\sum_{t\in T} \overline{H}\_{\lambda\_t} \geq \underline{W}\_\lambda\\) |
| \\(\underline{H}\_\lambda \in \mathbb{R}_+^T\\)  | minimum power output from reservoir \\(\lambda\\). Note : \\(\sum_{t\in T} \underline{H}\_{\lambda\_t} \leq \overline{W}\_\lambda\\) |
| \\(H\_\lambda \in \mathbb{R}_+^T\\)              | power output from reservoir \\(\lambda\\)                                                                                            |
| \\(r\_\lambda \in \mathbb{R}_+\\)                | maximum ratio between output power daily peak and daily average (\\(1 \leq r\_\lambda \leq 24\\))                                    |
| \\(\varepsilon\_\lambda \in \mathbb{R}\\)                 | reference water value associated with the reservoir's initial state (date, level)                                                    |
| \\(\varepsilon^*\_\lambda \in \mathbb{R}\\)               | random component added to the water value (dispatch smoothing effect)                                                                |
| \\(\eta\_\lambda \in \mathbb{R}^Q\\)             | reference water value associated with the reservoir's final state (date)                                                             |
| \\(\rho\_\lambda \in \mathbb{R}_+\\)             | efficiency ratio of pumping units (or equivalent devices) available in reservoir \\(\lambda\\)                                       |
| \\(\overline{\Pi}\_\lambda \in \mathbb{R}_+^T\\) | maximum power absorbed by pumps of reservoir \\(\lambda\\)                                                                           |
| \\(\Pi\_\lambda \in \mathbb{R}_+^T\\)            | power absorbed by pumps of reservoir \\(\lambda\\)                                                                                   |
| \\(I\_\lambda \in \mathbb{R}^T_+\\)              | natural power inflow to reservoir \\(\lambda\\)                                                                                      |
| \\(O\_\lambda \in \mathbb{R}_+^T\\)              | power overflowing from reservoir \\(\lambda\\) : part of inflow that cannot be stored                                                |
| \\(\overline{R}\_\lambda \in \mathbb{R}_+^T\\)   | upper bound of the admissible level in reservoir \\(\lambda\\)                                                                       |
| \\(\underline{R}\_\lambda \in \mathbb{R}_+^T\\)  | lower bound of the admissible level in reservoir \\(\lambda\\)                                                                       |
| \\(R\_\lambda \in \mathbb{R}^T_+\\)              | stored energy level in reservoir \\(\lambda\\)                                                                                       |
| \\(\mathfrak{R}\_{\lambda_q} \in \mathbb{R}_+\\) | filling level of reservoir layer \\(q\\) at time \\(T\\) (end of the week)                                                           |

### 3.5 Binding constraints

In both \\(\mathcal{P}^k\\) and \\(\mathcal{P}\\), the need for a versatile modelling of the power system calls for the introduction of an arbitrary number of linear binding constraints between system's variables throughout the grid, expressed either in terms of hourly power, daily energies or weekly energies.
These constraints may bind together synchronous flows as well as thermal units power outputs. They may be related to synchronous values or bear on different times.
Herebelow, the generic notation size is used for the relevant dimension of the set to which parameters belong.

These dimensions stand as follow

\\(\mathrm{size}=T=168\\) : applicable to lower and upper bounds of constraints between hourly powers
\\(\mathrm{size}=\frac{T}{7}=24\\) : applicable to lower and upper bounds of constraints between daily energies
\\(\mathrm{size}=\frac{T}{168}=1\\) : applicable to lower and upper bounds of constraints between weekly energies

Generic notations for binding constraints :

| Notation                                 | Explanation                                                                                                 |
| ------------                             | -------------                                                                                               |
| \\(e \in E\\)                            | set of all grid interconnections and thermal clusters. \\(E = L \cup \Theta\\)                              |
| \\(b \in B\\)                            | binding constraints                                                                                         |
| \\(B_h \subset B\\)                      | subset of \\(B\\) containing the binding constraints between hourly powers                                  |
| \\(B_d \subset B\\)                      | subset of \\(B\\) containing the binding constraints between daily energies                                 |
| \\(B_w \subset B\\)                      | subset of \\(B\\) containing the binding constraints between weekly energies                                |
| \\(\alpha_e^b \in \mathbb{R}\\)          | weight of \\(e\\) (flow within \\(e\\) or output from \\(e\\)) in the expression of constraint \\(b\\)      |
| \\(o_e^b \in \mathbb{N}\\)               | time offset of \\(e\\) (flow within \\(e\\) or output from \\(e\\)) in the expression of constraint \\(b\\) |
| \\(u^b \in \mathbb{R}^{\mathrm{size}}\\) | upper bound of binding constraint \\(b\\)                                                                   |
| \\(l^b \in \mathbb{R}^{\mathrm{size}}\\) | lower bound of binding constraint \\(b\\)                                                                   |

### 3.6 Demand, security uplift, unsupplied and spilled energies
| Notation                          | Explanation                                                                            |
| ------------                      | -------------                                                                          |
| \\(D_n \in \mathbb{R}^T\\)        | net power demand expressed in node \\(n\\), including must-run generation              |
| \\(S_n \in \mathbb{R}^T_+\\)      | demand security uplift to be faced in node \\(n\\), by activation of security reserves |
| \\(\delta_n^+ \in \mathbb{R}^T\\) | normative unsupplied energy value in node \\(n\\). Value of lost load - VOLL           |
| \\(G_n^+ \in \mathbb{R}^T_+\\)    | unsupplied power in the nominal state                                                  |
| \\(\delta_n^- \in \mathbb{R}^T\\) | normative spilled energy value in node \\(n\\) (value of wasted energy)                |
| \\(G_n^- \in \mathbb{R}^T_+\\)    | spilled power in the nominal state                                                     |


## 4 Formulation of problem \\(\mathcal{P}^k\\)

Superscript k is implicit in all subsequent notations of this section (omitted for simplicity's sake)

## 4.1 Objective
$$
    \min\_{M\_\theta \in \mathrm{Argmin} \Omega\_{\mathrm{unit com}}}(\Omega\_{\mathrm{dispatch}})
$$


with


\\(
\displaystyle \Omega\_{\mathrm{dispatch}} = \Omega\_{\mathrm{transmission}}+\Omega\_{\mathrm{hydro}}+\Omega\_{\mathrm{thermal}}+\Omega\_{\mathrm{unsupplied}}+\Omega\_{\mathrm{spillage}}
\\)


\\(
\displaystyle \Omega\_{\mathrm{transmission}}=\sum_{l \in L} \gamma_l^+ \cdot F_l^+ + \gamma_l^- \cdot F_l^-
\\)


\\(
\displaystyle \Omega\_{\mathrm{hydro}} = \sum\_{n \in N} \sum\_{\lambda in \Lambda\_n} (\varepsilon\_\lambda + \varepsilon^*\_\lambda)\cdot(H\_\lambda - \rho\_\lambda \Pi\_\lambda + O\_\lambda) - \sum\_{n \in N} \sum\_{\lambda \in \Lambda\_n}\sum\_{q=1}^Q \eta\_{\lambda\_q} \mathfrak{R}\_{\lambda_q}
\\)


\\(
\displaystyle \Omega\_{\mathrm{thermal}}=\sum\_{n \in N} \sum\_{\theta \in \Theta\_n} \chi\_\theta \cdot P\_\theta + \sigma\_\theta^+ \cdot M\_\theta^+ + \tau\_\theta \cdot M\_\theta
\\)


\\(
\displaystyle \Omega\_{\mathrm{unsupplied}}=\sum\_{n \in N} \delta_n^+ \cdot G_n^+
\\)


\\(
\displaystyle \Omega\_{\mathrm{spillage}}=\sum\_{n \in N} \delta_n^- \cdot G_n^-
\\)

\\(\Omega\_{\mathrm{unit com}}\\) is the expression derived from \\(\Omega\_{\mathrm{dispatch}}\\) by replacing all variables that depend on the system's state by their equivalent in the uplifted state.

## 4.2 Constraints related to the nominal system state

Balance between load and generation:

First Kirchhoff's law:

\\(
\displaystyle  \forall n \in N, \sum\_{l \in L\_n^+} F_l - \sum\_{l \in L\_n^-} F_l = \left( G\_n^+ + \sum\_{\lambda \in \Lambda\_n}(H\_\lambda - \Pi\_\lambda) + \sum\_{\theta \ \in \Theta\_n} P\_\theta\right)-(G\_n^-+D\_n)
\\)


On each node, the unsupplied power is bounded by the net positive demand:

\\(
\displaystyle \forall n \in N, 0 \leq G\_n^+ \leq \max(0, D_n)
\\)

On each node, the spilled power is bounded by the overall generation of the node (must-run + dispatchable power):

\\(
\displaystyle \forall n \in N, 0 \leq G_n^- \leq -\min(0, D_n) + \sum\_{\lambda \in \Lambda\_n}H\_\lambda + \sum\_{\theta \ \in \Theta\_n} P\_\theta
\\)

Flows on the grid:

\\(
\displaystyle \forall l \in L, 0 \leq F\_l^+ \leq C\_l^+ +(\overline{C}^{+}\_l - C\_l^+)x\_l
\\)

\\(
\displaystyle \forall l \in L, 0 \leq F\_l^- \leq C\_l^- +(\overline{C}^{-}\_l - C\_l^-)x\_l
\\)

\\(
\displaystyle \forall l \in L, F\_l = F\_l^+ - F\_l^-
\\)

Flows are bounded by the sum of an initial capacity and of a complement brought by investment

Binding constraints :

\\(
\displaystyle \forall b \in B\_h, l^b \leq \sum\_{e \in E} \alpha\_e^b (F\_e)\_{\uparrow}^{o\_e^b} \leq u^b
\\)

\\(
\displaystyle \forall b \in B\_d, \forall k \in \lbrace 0,\dots,6\rbrace, l^b \leq \sum\_{e \in E} \alpha\_e^b \sum\_{t \in \lbrace 1,\dots,24\rbrace} (F\_e)\_{\uparrow {24k+t}}^{o\_e^b} \leq u^b
\\)

\\(
\displaystyle \forall b \in B\_w, l^b \leq \sum\_{e \in E} \alpha\_e^b \sum\_{t \in T} F\_{e\_t} \leq u^b
\\)

Reservoir-type Hydro power:

The energy generated throughout the optimization period is bounded

\\(
\displaystyle \forall n \in N, \forall \lambda \in \Lambda\_n, \underline{W}\_{\lambda} \ leq \sum\_{t\in T} H\_{\lambda\_t} \leq \overline{W}\_{\lambda}
\\)

FIXME : RHS
\\(
\displaystyle \forall n \in N, \forall \lambda \in \Lambda\_n, \sum\_{t\in T} H\_{\lambda\_t} - \sum\_{t\in T} \rho\_t \Pi\_{\lambda\_t} = \overline{W}\_{\lambda}
\\)

Instantaneous generating power is bounded

\\(
\displaystyle \forall n \in N, \forall \lambda \in \Lambda\_n, \underline{H}\_{\lambda} \leq H\_{\lambda} \leq \overline{H}\_{\lambda}
\\)

Intra-daily power modulations are bounded

\\(
\displaystyle \forall n \in N, \forall \lambda \in \Lambda\_n, \forall k \in \lbrace 1, \ldots, 6 \rbrace, \frac{\max\_{t \in \lbrace 24k+1,\ldots, 24k+24 \rbrace} H\_{\lambda\_t}}{\sum\_{t \in \lbrace 24k+1,\ldots, 24k+24 \rbrace} H\_{\lambda\_t}} \leq r\_{\lambda}
\\)

Instantaneous pumping power is bounded

\\(
\displaystyle \forall n \in N, \forall \lambda \in \Lambda\_n, 0 \leq \Pi\_{\lambda} \leq \overline{\Pi}\_{\lambda}
\\)

Reservoir level evolution depends on generating power, pumping power, pumping efficiency, natural inflows and overflows

Reservoir level is bounded by admissible lower and upper bounds (rule curves)

Thermal units :

Power output is bounded by must-run commitments and power availability

The number of running units is bounded

Power output remains within limits set by minimum stable power and maximum capacity thresholds

Minimum running and not-running durations contribute to the unit-commitment plan. Note that this modeling requires that one at least of the following conditions is met:

### 4.3 Constraints related to the uplifted system state (activation of security reserves)

All constraints to previously defined for regular operation conditions are repeated with replacement of all variables by their twins when they exist.

Besides, in the expression of constraints , all occurrences of are replaced by

## 5 Formulation of problem

### 5.1 Objective
FIXME

### 5.2 Constraints

## 6 Antares as a SCOPF ("FB model")

When problems and do not include any instance of so-called ";binding constraints"; and if no market pools are defined, the flows within the grid are only committed to meet the bounds set on the initial transmission capacities, potentially reinforced by investments (problem ).In other words, there are no electrical laws enforcing any particular pattern on the flows, even though hurdles costs and may influence flow directions through an economic signal.

In the general case, such a raw backbone model is a very simplified representation of a real power system whose topology and consistency are much more complex. While the full detailed modeling of the system within Antares is most often out of the question, it may happen that additional data and/or observations can be incorporated in the problems solved by the software.

In a particularly favorable case, various upstream studies, taking account the detailed system characteristics in different operation conditions (generating units outages and/or grid components outages N, N-1 , N-k,…) may prove able to provide a translation of all relevant system limits as a set of additional linear constraints on the power flowing on the graph handled by Antares.

These can therefore be readily translated as ";hourly binding constraints";, without any loss of information. This kind of model will be further referred to as a "FB model". Its potential downside is the fact that data may prove to be volatile in short-term studies and difficult to assess in long-term studies.

## 7 Antares as a SCOPF ("KL model")

When a full FB model cannot be set up (lack of robust data for the relevant horizon), it remains possible that classical power system studies carried on the detailed system yield sufficient information to enrich the raw backbone model. An occurrence of particular interest is when these studies show that the physics of the active power flow within the real system can be valuably approached by considering that the edges of behave as simple impedances .This model can be further improved if a residual (passive) loop flow is to be expected on the real system when all nodes have a zero net import and export balance (situation typically encountered when individual nodes actually represent large regions of the real system). This passive loop flow should therefore be added to the classical flow dictated by Kirchhoff's rules on the basis of impedances . This model will be further referred to as a "KL model". Different categories of binding constraints, presented hereafter, make it possible to implement this feature in and

### 7.1 Implementation of Kirchhoff's second law

The implementation ofKirchhoff's second law for the reference state calls for the following additional hourly binding constraints:

### 7.2 Implementation of a passive loop flow

In cases where a residual passive loop flow should be incorporated in the model to complete the enforcement of regular Kirchhoff's rules, the binding constraints mentioned in 7.1 should be replaced by:

### 7.3 Modelling of phase-shifting transformers

In cases where the power system is equipped with phase-shifting transformers whose ratings are known, ad hoc classical power studies can be carried out to identify the minimum and maximum flow deviations and phase-shift that each component may induce on the grid. The following additional notations are in order:

The enhancement of the model with a representation of the phase-shifting components of the real system then requires to re-formulate as follows the binding constraints defined in 7.2:

### 7.4 Modelling of DC components

When the power system graph contains edges that represent DC components, additional notations need be defined:

The proper modeling of the system then requires that all constraints identified in 7.1, 7.2, 7.3 be formulated using notations instead of

### 7.5 Implementation of security rules N-1,..., N-k

Upstream power system classical calculations on the detailed system are assumed to have provided appropriate estimates for line outage distribution factors (LODFs) for all components involved in the contingency situations to consider. The following additional notations will be further used:

The implementation of security rules for the chosen situations requires the following additional binding constraints:

## 8 Antares as a SCOPF (";KL+FB model";)

If the information context is rich enough, it is possible to set up a hybrid model based on both previous approaches: on the one hand, a ";KL layer"; makes use of the best available estimates for grid impedances and loop flows, so as to instantiate physically plausible flow patterns; on the other hand, a ";FB layer"; sets multiple kinds of limits on the admissible flow amplitudes, as a result of various operation commitments.

To work appropriately, such a hybrid model needs an additional auxiliary layer that performs a mapping between the two ";twin"; FB and KL grid layers.

## 9 Miscellaneous

### 9.1 Modelling of generation investments in

The assessment of the desired level of expansion of any segment of the generating fleet can be carried out with a model in which the potential reinforcements of the fleet are assumed to be actually commissioned from the start but located on virtual nodes connected to the real system through virtual lines with a zero initial capacity. Relevant generation assets costs and capacities should then be assigned to the virtual connections, and the investment in new generation can be optimized ";by proxy"; through the identification of the optimal expansion of the virtual connections.

### 9.2 Modelling of pumped storage power plants

A number of specific equipments, as well as particular operation rules or commercial agreements, can be modelled with appropriate use of binding constraints. A typical case is that of a pumped storage power plant operated with a daily or weekly cycle. Such a component can be modelled as a set of two virtual nodes connected to the real grid through one-way lines. On one node is attached a virtual load with zero VOLL, which may absorb power from the system. On the other node is installed a virtual generating unit with zero operation cost, which may send power to the system. The flows on the two virtual lines are bound together by a daily or weekly constraint (depending on the PSP cycle duration), with a weight set to fit the PSP efficiency ratio. Besides, time offsets may be included in the constraints to take into account considerations regarding the volume of the PSP reservoir (additional energy constraint).

[1](#sdfootnote1anc) The development of the product is a continuous process resulting in the dissemination of a new version each year. As a rule, version N brings various improvements on the code implemented in version N-1 and enhances the functional perimeter of the tool. This document presents the general optimization problem formulation as it is formalized so far in the last version of disseminated version of **Antares\_Simulator** (V7).

[2](#sdfootnote2anc) Reference guide , section « optimization preferences : ";export mps problems";

[3](#sdfootnote3anc) For the sake of simplicity, the **Antares\_Simulator** application will be further denoted « Antares »

[4](#sdfootnote4anc)See «hydro» sections of the General Reference Guide (";hydro"; standing as a generic name for all types of energy storage facilities)

[5](#sdfootnote5anc)This does not actually limit the model's field of application: all datasets can easily be put in a format that meets this commitment

[6](#sdfootnote6anc) FB stands for « flow-based », denomination used in the framework given to the internal electricity market of western Europe

[7](#sdfootnote7anc) KL stands for ";Kirchhoff- and-Loop";. Such a model was used in the European E-Highway project ([http://www.e-highway2050.eu](http://www.e-highway2050.eu/))

[8](#sdfootnote8anc) A common situation is that KL and FB are defined at different spatial scales and describe different topologies: the KL layer has typically a fairly large number of small-sized regions, while the FB layer consists of fewer wide market zones. The role of the auxiliary layer is to implement the appropriate relationship between physical regions and trade zones.

Copyright © RTE 2007-2019 – Version 7.1.0

Last Rev : M. Doquet - 16 OCT 2019
