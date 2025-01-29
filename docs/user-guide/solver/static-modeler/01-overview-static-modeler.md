# Overview

_**This section is under construction**_

The *Solver* is *Antares Simulator*'s main feature.

**Monte Carlo Simulation** Runs either an economy simulation or an adequacy simulation
depending on the values of the [parameters](04-parameters.md).
If hardware resources and simulation settings allow it, simulations can benefit from [multi-threading](../optional-features/multi-threading.md).


## Antares at one glance

This section gives a summary of the whole simulation process followed by Antares in Economy simulations (Adequacy being simplified variant of it):

1. Load or Generate [stochastic generators] Time-series of every kind for all system areas

2. For each Monte-Carlo year, pick up at random or not [scenario builder] one time-series of each kind for each area/link

3. For each area and each reservoir:

    1. Split up the annual overall hydro storage inflows into monthly hydro storage generation, taking into account reservoir constraints, hydro management policy and operation conditions (demand, must-run generation, etc.) [heuristic + optimizer]

    2. For every day of each month, break down the monthly hydro energy into daily blocks, taking into account hydro management policy and operation conditions (demand, must-run generation, etc.) [heuristic + optimizer]. Aggregate daily blocks back into weekly hydro storage energy credits (used if the final optimization is run with full weekly 168-hour span)

    3. For each week of the year (daily/weekly hydro energy credits are now known in every area), run a three-stage 168-hour optimization cycle (or seven 24-hour optimizations, if the optimization preference is set to "daily"). This aim of this cycle is to minimize the sum of all costs throughout the optimization period. This sum may include regular proportional fuel costs, start-up and no-load heat costs, unsupplied and spilled energy costs, and hurdle costs on interconnection. The solution has to respect minimum and maximum limits on the power output of each plant, minimum up and down durations, as well as interconnection capacity limits and "binding constraints" at large (which may be technical – e.g. DC flow rules – or commercial – e.g. contracts). Note that an accurate resolution of this problem requires mixed integer linear programming (because of dynamic constraints on thermal units). A simplified implementation of this approach is used when the advanced parameter "Unit commitment" is set on "accurate". This high quality option may imply long calculation times. This is why, when "Unit commitment" is set on "fast", Antares makes further simplifications that save a lot of time (starting costs are not taken into account within the optimization step but are simply added afterwards, units within a thermal cluster are subject to starting up/shutting down constraints more stringent than the minimum up/down durations). In both cases, the general optimization sequence is as follows:

       i. Minimization of the overall system cost throughout the week in a continuous relaxed linear optimization. Prior to the optimization, an 8760-hourly vector of operating reserve R3 (see next section) may be added to the load vector (this will lead in step (ii) to identify plants that would not be called if there were no reserve requirements. Their actual output will be that found in step (iii), wherein the load used in the computations takes back its original value)

       ii. So as to accommodate the schedule resulting from (i), search for integer values of the on/off variables that satisfy the dynamic constraints with the smallest possible cost increase.

       iii. Take into account the integer variables found in (ii) and solve again the optimal schedule problem for the week.

## Operating reserves modeling

Many definitions may be encountered regarding the different operating reserves (spinning / non-spinning, fast / delayed, primary-secondary-tertiary, frequency containment reserve – frequency restoration reserve – replacement reserve, etc.).

Besides, all of them need not be modeled with the same level of accuracy in a simulator such as Antares. Furthermore, the best way to use the concept is not always quite the same in pure Adequacy studies and in Economy studies.

Several classes of reserves may therefore be used in Antares; how to use them at best depend on the kind and quality of operational data at hand, and on the aim of the studies to carry out; though all kinds of reserves may always be defined in the INPUT dataset, the set of reserves that will effectively be used depends on the kind of simulations to run. Note that any or all classes of reserves may be ignored in a given simulation (without being removed from the INPUT dataset) by setting the matching "optimization preference" to "ignore reserve X":

- **Pre-allocated reserve on dispatchable thermal plants (R0)** <br/>
  This reserve (which corresponds to the parameter "spinning" attached to the thermal plants) is expressed as a percentage of the nominal capacity of the plants. It is simply used as a derating parameter: for instance, a 1000 MW plant with a 2.5% spinning parameter will not be able to generate more than 975 MW. It is important to notice that, if the plant is not scheduled on, it will NOT contribute to the spinning reserve (to be effectively available, the 25 MW of reserve would need the plant to be started). This first class of reserve is available for **Adequacy** as well as for **Economy**.

- **Day-ahead reserve (R3):** <br/>
  This reserve is available in **Adequacy** and **Economy** simulations, with the following meaning:
  "For any day D, to be able to accommodate last-minute random variations of the expected demand and/or generation (as they were seen from day D -1), a certain amount of power (R3) should be ready to be available at short notice".
  <br/>
  In actual operating terms, R3 is a complex (spinning/non-spinning) mix as well as (hydro/thermal) mix. It may involve or not part of the primary and secondary power/frequency regulation reserves. R3 may represent as much as the overall amount of frequency containment reserve, frequency restoration reserve and replacement reserve required for operation on day D, as seen from day D-1.
  <br/>
  In the simulations, R3 is construed as a "virtual" increase of the load to serve, which influences the optimal unit commitment and dispatch (because of minimum stable power levels and minimum On / Down times).

**IMPORTANT:**

The optimization makes sure that, should the need arise, reserve R3 will actually be available where it is needed **BUT** there is no commitment regarding whether this service should be provided by an increase of local generation, a decrease of exports or even an increase of imports: the optimizer will choose the mix leading to the minimal cost for the system.

Note that this "standard" feature of Antares makes it possible to assess the potential value of keeping some headroom in interconnections for the purpose of transferring operating reserves, when "remote" reserves are less expensive than domestic ones.

The table below gives an overview of the different reserves available in Antares

|      | _Economy_ | _Adequacy_ |
|------|-----------|------------|
| _R0_ | _Yes_     | _Yes_      |
| _R3_ | _Yes_     | _Yes_      |