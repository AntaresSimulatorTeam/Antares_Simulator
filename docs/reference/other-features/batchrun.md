---
hide:
 - toc
---

# Batch Runner


> _**WARNING:**_ this feature is deprecated and will be removed in a future release. If you are still using it,
> please [get in touch](https://github.com/AntaresSimulatorTeam/Antares_Simulator/issues) with us.


**Executable**: antares-batchrun (currently released for Windows & Ubuntu only)

- Studies

| command           | meaning                                                                 |
|:------------------|:------------------------------------------------------------------------|
| -i, --input=VALUE | The input folder, where to look for studies on which to run simulations |

- Simulation mode

| command     | meaning                                   |
|:------------|:------------------------------------------|
| --expansion | Force the simulation(s) in expansion mode |
| --economy   | Force the simulation(s) in economy mode   |
| --adequacy  | Force the simulation(s) in adequacy mode  |

- Parameters

| command          | meaning                                              |
|:-----------------|:-----------------------------------------------------|
| -n, --name=VALUE | Set the name of the new simulation outputs           |
| -y, --year=VALUE | Override the number of MC years                      |
| -f, --force      | Ignore all warnings at loading                       |
| --no-output      | Do not write the results in the output folder        |
| --year-by-year   | Force the writing of the result output for each year |

- Optimization

| command          | meaning                                         |
|:-----------------|:------------------------------------------------|
| --no-ts-import   | Do not import timeseries into the input folder. |
| --no-constraints | Ignore all binding constraints                  |

- Extras

| command                | meaning                                                  |
|:-----------------------|:---------------------------------------------------------|
| --solver=VALUE         | Specify the antares-solver location                      |
| --parallel             | Enable the parallel computation of MC years              |
| --force-parallel=VALUE | Override the max number of years computed simultaneously |
| --verbose              | Display detailed logs for each simulation to run         |