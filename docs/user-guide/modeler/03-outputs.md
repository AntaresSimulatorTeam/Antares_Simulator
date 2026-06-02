# Output files

_**This feature is under development and may change frequently**_  

Antares modeler automatically creates an **output** directory under the study root directory, to write output files into it.  
Currently, Antares modeler outputs the simulation table and optionally MPS files for debugging.

## MPS Files

When the `export-mps` parameter is set to `true` in [parameters.yml](04-parameters.md#outputs), Antares modeler exports
the optimization model in MPS format:
- **output/1-1.mps**: The subproblem in MPS format
- **output/master.mps**: The master problem in MPS format (if applicable, e.g., in Benders decomposition mode)
- **output/structure.txt**: A text file describing the Benders decomposition structure (if applicable)

These files are intended for debugging and analysis purposes.

## Simulation Table

Antares Simulator (hybrid and modeler modes) produces detailed optimization results for the modeler's components, 
in the "simulation table", in CSV format:  
- for each optimization run for hybrid mode (simulation_table--optim-nb-1.csv, simulation_table--optim-nb-2.csv)
- unique file for pure modeler (simulation_table_*TimeStamp*.csv).
These files provide a comprehensive view of the variables and constraints involved in the optimization, including their
values and basis statuses.

### Location

These files are saved in the output directory of your study, typically named:

    simulation_table--optim-nb-1.csv (hybrid, for the first optimization)
    simulation_table--optim-nb-2.csv (hybrid, for the second optimization, if performed)
    simulation_table--20250812-1256.csv (pure modeler)

### Structure

Each line in the simulation table corresponds to a variable, constraint,
port or an extra output for a given block, component, scenario, and
time index.

#### CSV Header

The CSV files have the following header:
Code

    block,component,output,absolute_time_index,block_time_index,scenario_index,value,basis_status

#### Column Details

    block:
        This is an integer representing the time block solved within the year.
        If the resolution is weekly, the value is "1" for all lines of the first week, "2" for all lines of the second week, etc.
        If the resolution is daily, the value is "1" for all lines of the first day, "2" for the second day, etc.
        In pure Modeler mode, the resolution is currently annual, so the value is always "1".

    component:
        The ID of the exported component.
        Currently, all variables and constraints from all components are exported.
    output:
        The identifier (within the component) of the exported output.
        In this first version, you can expect to find the values of optimization variables and the basis status of both variables and constraints.
        The variable ID is as it appears in the model; it does not include the component name, scenario number, or time step (these are found in other columns).
    absolute_time_index:
        The time step, starting from the beginning of the year.
        Starts at 1.
    block_time_index:
        The time step, relative to the solved block (starting from 1).
            - If the resolution is weekly, values range from 1 to 168.
            - If the resolution is daily, values range from 1 to 24.
            - In pure Modeler mode, this value is equal to absolute_time_index.

    scenario_index:
        The scenario index (Monte Carlo year).
        In pure Modeler mode, this is currently equal to 1 (or possibly 0).
    value:
        The value of the output (variable). None for constraints.
    basis_status:
        The status in the solver basis for this variable or constraint. Possible values are:
            Free
            At lower bound
            At upper bound
            Fixed value
            Basic
            None (not available or not applicable)

### Example
~~~csv
    block,component,output,absolute_time_index,block_time_index,scenario_index,value,basis_status
    1,GENERATOR,max_p,1,1,1,5900,Basic
    1,GENERATOR,constraint_1,1,1,1,,At lower bound
~~~

### Usage

This output is intended for advanced users who need to analyze all variables and constraints involved in the
optimization process, their time and scenario dependencies, and the solution status from the solver.

### Legacy solver variables (hybrid studies only)

In [hybrid studies](../solver/08-hybrid-studies.md), the legacy solver's optimization variables are also exported to
the simulation table, one row per variable per optimization step.

The legacy solver names its variables internally using the pattern `<Output>::<location>::hour<N>`, for example:

    UnsuppliedEnergy::area<north>::hour<33>

After solving, this is parsed and written to the simulation table as:

| column | value |
|---|---|
| `component` | the location part, e.g. `area<north>` |
| `output` | the output name, optionally remapped via [`variable-names.yml`](02-inputs.md#variable-name-mapping-file), e.g. `unsupplied_energy` |
| `absolute_time_index` | 0-based hour index N + 1 (1-based) |
| `block_time_index` | position of the timestep within the current optimization block, or absent if outside the block |
| `value` | the solver solution value for that variable |
| `basis_status` | always `None` for legacy variables |

#### Example

With the mapping `UnsuppliedEnergy: unsupplied_energy` in `variable-names.yml`, an unsupplied energy of 52 MW on
hour 34 (1-based) of year 1 would appear as:

~~~csv
block,component,output,absolute_time_index,block_time_index,scenario_index,value,basis_status
1,area<north>,unsupplied_energy,34,34,0,52,None
~~~

### Notes
- The simulation table output is only available in hybrid and pure modeler studies.  
- Two files may be generated if a second optimization step is performed (e.g., for feasibility or integer relaxation).  
- If a field is not applicable for a given entry, it will be filled with None.  
- Some outputs (parameters, ports, ...) that are marked as time/scenario-dependent in the model may be time/scenario-independent 
  given the user's input data. They will be treated as time/scenario-independent during the simulation table generation.
