# Output files

_**This feature is under development**_  

Antares modeler automatically creates an **output** directory under the study root directory, to write output files into it.  
Currently, Antares modeler only outputs two file, one containing optimal values of the objective function and all the 
optimization problem's variables, and one containing the optimization model.   

## Optimization results

### Description

The optimization results are written under **output/solution.tsv**.  
The file is in TSV format, with values seperated by a tab, and the floating point represented by the `.` character.  

The first column contains "objective" or the ID of a variable, composed by **componentID.variableID_s_t**, where:  

- **componentID** is the ID of the component as defined in the [system file](02-inputs.md#system-file)
- **variableID** is the ID of the variable as defined in the component's [model](02-inputs.md#models)
- **s** is the index of the scenario (starts at 0), if the variable is scenario-dependent
- **t** is the index of the timestamp (starts at 0), if the variable is time-dependent

The second column contains optimal variable values.

### Example
Output file for a simulation with 2 scenarios and 3 timestamps.
~~~
objective 1565
generator1.is_on_0_0 1
generator1.is_on_0_1 1
generator1.is_on_0_2 1
generator1.is_on_1_0 1
generator1.is_on_1_1 0
generator1.is_on_1_2 0
generator1.active_power_0_0 10
generator1.active_power_0_1 510
generator1.active_power_0_2 340.5
generator1.active_power_1_0 100
generator1.active_power_1_1 0
generator1.active_power_1_2 0
generator2.is_on_0_0 0
generator2.is_on_0_1 1
generator2.is_on_0_2 1
generator2.is_on_1_0 1
generator2.is_on_1_1 1
generator2.is_on_1_2 1
generator2.active_power_0_0 0
generator2.active_power_0_1 1023
generator2.active_power_0_2 45
generator2.active_power_1_0 23
generator2.active_power_1_1 562
generator2.active_power_1_2 10.6534
~~~

## Optimization model

The optimization model solved by Antares modeler is written in the human-readable LP format, 
under **output/problem.lp**. It is only meant to be used for debugging.