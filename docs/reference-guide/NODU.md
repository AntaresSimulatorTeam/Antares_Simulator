## Computation of the number of dispatched units (NODU)
NODU is the number of dispatched units, defined for each cluster at every hour for a given year.  The goal of this document is to explain how the number of dispatched units is computed in Antares Simulator.

### Introduction
Thermal clusters are of two types:

* Must-run
* Controllable
    
Must-run clusters are always on. For controllable clusters, a heuristic method is used to determine at each hour its state (ON/OFF) and its production level.

Since an heuristic is used, it is not a result of the optimization. Instead, a specific algorithm is used to compute it from the simulation results. When computing the NODU, the production at each hour and for each cluster is known. What is unknown is how many clusters had to be ON to obtain that production.

Each cluster contains one or multiple units. Each unit has a minimal power level and a maximal power level. Each unit has a startup cost and a fixed cost for having a cluster running for 1 hour at whatever power level. These data are used by the algorithm detailed below.

### Minimal ON duration after which a stop & start is preferable
```cpp
if (currentCluster->fixedCost > 0.)
{
    dur = static_cast<uint>(
      Math::Floor(currentCluster->startupCost / currentCluster->fixedCost));
    if (dur > endHourForCurrentYear)
        dur = endHourForCurrentYear;
}
else
    dur = endHourForCurrentYear;

```
If there are no fixed costs, this duration is set to 1 year: no unit will be restarted. Otherwise, a trade-off is used depending on `startupCost / fixedCost`.

There are three possibilities:

* `dur == 0`: the cluster is very versatile: it is not too costly to restart it.
* `dur == endHourForCurrentYear`: the cluster is either must-run, or very costly to start.
* `0 < dur < endHourForCurrentYear`. Intermediate case: it can sometimes be profitable to restart the cluster, but not always.

### Computation of the minimal number of ON units at each hour
This depends on the unit-comittment mode chosen, the min-stable power, and the nominal capacity with spinning reserve. 
**TODO**
```cpp
switch (unitCommitmentMode)
{
case Antares::Data::UnitCommitmentMode::ucHeuristic:
{
    if (currentCluster->pminOfAGroup[numSpace] > 0.)
    {
        ON_min[h] = Math::Max(
          Math::Min(static_cast<uint>(
                      Math::Floor(thermalClusterPMinOfTheClusterForYear[h]
                                  / currentCluster->pminOfAGroup[numSpace])),
                    static_cast<uint>(
                      Math::Ceil(thermalClusterAvailableProduction
                                 / currentCluster->nominalCapacityWithSpinning))),
          static_cast<uint>(
            Math::Ceil(thermalClusterProduction
                       / currentCluster->nominalCapacityWithSpinning)));
    }
    else // pmin == 0
        ON_min[h] = static_cast<uint>(Math::Ceil(
          thermalClusterProduction / currentCluster->nominalCapacityWithSpinning));
    break;
}
case Antares::Data::UnitCommitmentMode::ucMILP:
{
    ON_min[h] = Math::Max(
      static_cast<uint>(Math::Ceil(thermalClusterProduction
                                   / currentCluster->nominalCapacityWithSpinning)),
      thermalClusterDispatchedUnitsCountForYear[h]); // eq. to thermalClusterON for
                                                     // that hour

    break;
}
}
```
### Computation of the maximal number of ON units at each hour
```cpp
ON_max[h] = static_cast<uint>(Math::Ceil(
  thermalClusterAvailableProduction / currentCluster->nominalCapacityWithSpinning));

if (currentCluster->minStablePower > 0.)
{
    maxUnitNeeded = static_cast<uint>(
      Math::Floor(thermalClusterProduction / currentCluster->minStablePower));
    if (ON_max[h] > maxUnitNeeded)
        ON_max[h] = maxUnitNeeded;
}

if (ON_max[h] < ON_min[h])
    ON_max[h] = ON_min[h];
```

### Computation of the optimal number of ON units at each hour
This is by far the most complicated part. The algorithm is run only if `dur > 0`. **TODO**

### Computation of the NODU for each cluster
```cpp
// For each hour h in a year
if (dur == 0)
    NODU[h] = ON_min[h];
else
    NODU[h] = ON_opt[h];

// NODU cannot be exceed the unit count
if (NODU[h] > currentCluster->unitCount)
    NODU[h] = currentCluster->unitCount;

```

If the cluster is very flexible (`dur == 0`), NODU is the minimal number of clusters. Otherwise, it is the optimal number of clusters from the [optimal ON algorithm](#computation-of-the-optimal-number-of-on-units-at-each-hour).
