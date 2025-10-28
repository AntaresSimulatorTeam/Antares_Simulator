# Input files

## Study structure

Like the Antares [solver](../solver), the Antares modeler input is a directory, that should be organized by the user as
follows:

- _**root directory with any name**_
    - **input**: a directory that contains all input files
        - **model-libraries**: a directory that contains all [model libraries](#model-libraries) needed by the study
        - **data-series**: a directory that contains all [data series](#data-series) needed by the study
            - Contains **modeler-scenariobuilder.dat** file, which is used to map scenarios to data series
        - **system.yml**: the [system file](#system-file) describing the simulated energy system
    - **parameters.yml**: the [parameters](04-parameters.md) file

Note that Antares will automatically create an **output** directory to write the modeler [outputs](03-outputs.md).

## Model libraries

The model libraries directory shall contain all the model library files needed to run the study.  
A model library is a collection of [models](05-model.md#models) that are published together in one source.  
The study can rely on only one library, or can mix models from different libraries.

Currently, Antares modeler supports libraries that are each defined in a single yml file.

The header of the yml file must contain exactly one "library" key at the root level.  
The **library** object contains one **id**, one **description**, one **port-types** collection, and one **models**
collection.  
Unless stated otherwise, all listed fields are mandatory.

### ID & description

Example:

~~~yaml
library:
  id: my_library_id
  description: my library is great!
~~~

- **id**: the unique ID for you library. Beware that if you are using many libraries in your study, every library must
  have a unique ID. This ID will be used inside the [system description file](#system-file) in order to reference the
  library's objects. It must respect [these rules](#rules-for-ids).
- **description** _(optional)_: a free description of your library. Has no effect on the simulation.

### Port types

The **port-types** collection lists the possible types of [ports](05-model.md#ports-and-connections) inside the library,
that can be used by models/components to communicate with each-other.  
This field is optional: you can develop a library with no ports, even though this would have limited interest (the
models would not be able to communicate with each-other).  
Example:

~~~yaml
port-types:
- id: dc_port
  description: A port which transfers power flow value
  fields:
  - id: flow
- id: ac_port
  description: A port which transfers power flow and voltage angle values
  fields:
  - id: flow
  - id: angle
~~~

- **id**: the ID for the port type. Must be unique inside the scope of the library, and
  respect [these rules](#rules-for-ids).
- **description** _(optional)_: a free description of your port type. Has no effect on the simulation.
- **fields**: a collection of coherent fields that transit through this port type. A field holds a single floating
  number.
    - **id**: the ID of the field. Must be unique in the scope of the port type, and
      respect [these rules](#rules-for-ids).
- **area-connection** _(optional)_: used only for hybrid (solver x modeler) studies, ignored
  by [antares-modeler](08-command-line.md).
  For more information on hybrid
  studies, [see the relevant documentation](../solver/08-hybrid-studies.md#selecting-the-area-connection-port-fields).

### Models

The **models** collection lists all the [models](05-model.md#models) that can be instantiated using your library.  
Example:

~~~yaml
models:
- id: generator_dc
  description: A simple DC model of a generator
  parameters:
  - id: min_active_power_setpoint
    time-dependent: false
    scenario-dependent: false
  - id: max_active_power_setpoint
    time-dependent: true
    scenario-dependent: true
  - id: proportional_cost
    time-dependent: false
    scenario-dependent: true

  variables:
  - id: is_on
    variable-type: boolean
    lower-bound: 0
    upper-bound: 1
    time-dependent: true
    scenario-dependent: true
  - id: active_power
    variable-type: continuous
    lower-bound: 0
    upper-bound: max_active_power_setpoint

  constraints:
  - id: respect_min_p
    expression: active_power >= is_on * min_active_power_setpoint

  objective-contributions:
  - id: objective
    expression: active_power * proportional_cost

  ports:
  - id: injection
    type: dc_port

  port-field-definitions:
  - port: injection
    field: flow
    definition: active_power

  extra-outputs:
  - id: total_cost_in_millions
    expression: sum(active_power * proportional_cost) / 1000000

- id: node
  description: A balance node with injections (productions and loads)
  ports:
  - id: injections
    type: dc_port
  binding-constraints:
  - id: balance
    expression: sum_connections(injections.flow) = 0
~~~

- **id**: an ID for the model. Must be unique inside the scope of the library, and
  respect [these rules](#rules-for-ids).
- **description** _(optional)_: a free description of the model. Has no effect on the simulation.
- **parameters** _(optional)_: a collection of parameters for the model. The values for these parameters will be set in
  the [system file](#system-file).
    - **id**: an ID for the parameter. Must be unique inside the scope of the model, and
      respect [these rules](#rules-for-ids).
    - **time-dependent** _(optional)_: `true` or `false`, indicates whether the parameter depends on time or is constant 
      across the whole simulation horizon. Defaults to `true`.
    - **scenario-dependent** _(optional)_: `true` or `false`, indicates whether the parameter changes depending on the 
      simulated scenario, or is the same for all scenarios. Defaults to `true`.
- **variables** _(optional)_: a collection of optimization variables that are defined for this model
    - **id**: an ID for the variable. Must be unique inside the scope of the model, and
      respect [these rules](#rules-for-ids).
    - **variable-type**: `continuous`, `integer`, or `binary`
    - **lower-bound** _(optional)_: an [expression](#expressions) representing the lower bound of the variable. The
      expression inside the parentheses must evaluate to a scalar.
      and/or parameters only. If missing, defaults to -inf for continuous and integer types, or 0 for binary.
    - **upper-bound** _(optional)_: an [expression](#expressions) representing the upper bound of the variable. The
      expression inside the parentheses must evaluate to a scalar.
      and/or parameters only. If missing, defaults to +inf for continuous and integer types, or 1 for binary.
    - **time-dependent** _(optional)_: `true` or `false`, indicates whether the variable depends on time or is constant 
      across the whole simulation horizon. Defaults to `true`.
    - **scenario-dependent** _(optional)_: `true` or `false`, indicates whether the parameter changes depending on the 
      simulated scenario, or is the same for all scenarios. Defaults to `true`.
- **constraints** _(optional)_: a collection of "internal" optimization constraints set by the model
    - **id**: an ID for the constraint. Must be unique inside the scope of the model, and
      respect [these rules](#rules-for-ids).
    - **expression**: an [expression](#expressions) representing the constraint. Can use scalars, parameters, internal
      variables, time and scenario operators.
      Must contain exactly one comparison operator (**=**, **<=**, or **>=**).
- **binding-constraints** _(optional)_: a collection of "external" optimization constraints set by the model, that use
  ports. While these have no
  real difference with "internal constraints", it is best practice to separate internal and external constraints in
  order to make the model more readable.
    - **id**: an ID for the constraint. Must be unique inside the scope of the model, and
      respect [these rules](#rules-for-ids).
    - **expression**: an [expression](#expressions) representing the constraint. Can use scalars, parameters, internal
      variables, ports, and time, scenario, and port operators.
- **objective-contributions** _(optional)_: an collection of contributions to the objective function
    - **id**: an ID for the objective contribution. Must be unique inside the scope of the model, and
      respect [these rules](#rules-for-ids).
    - **expression**: an [expression](#expressions) representing the (additive) participation of the model to
      the optimization objective.
      Note that **minimization** is implied. The expression can use scalars, parameters and variables of the model.
- **ports** _(optional)_: a collection of ports exposed by the model, either as input or output
    - **id**: an ID for the port. Must be unique in the scope of the model, and respect [these rules](#rules-for-ids).
    - **type**: the type of the port. Must refer to the ID of a port type defined in the [port types](#port-types)
      section.
- **port-field-definitions** _(optional)_: a collection of definitions for ports output by this model. Note that if the
  model is to define a port, then it must define all fields of this port.
    - **port**: the ID of a port exposed by the model (defined in the **ports** section above)
    - **field**: the field to define (refers to a field ID defined in the port type)
    - **definition**: an [expression](#expressions) representing the definition of the field. Can use scalars,
      parameters, and variables of the model.
- **extra-outputs** _(optional)_: a collection of outputs relevant to the model, that can be computed after optimization
  (i.e. using the optimal values of the variables). 
  The values of these extra outputs will appear in the [output files](03-outputs.md) along with variable and port values.
    - **id**: an ID for the extra-output. Must be unique inside the scope of the model, and
      respect [these rules](#rules-for-ids).
    - **expression**: the [expression](#expressions) of the output. Can use all operators, as long as the expression can be 
      evaluated after optimization. Note that using an output's id in another output's expression is not allowed. 

### Expressions

An expression is a human-readable equation that allows a large flexibility in defining objects of optimization.

#### Arithmetic operators

The following operators are allowed between two elements:

- **+**: addition of two elements
- **-**: subtraction
- __*__: multiplication
- **/**: division
- **=**: equality, only allowed for constraint definitions
- **<=**: less or equal to, only allowed for constraint definitions
- **>=**: greater or equal to, only allowed for constraint definitions

#### Scalars

You can use simple floating-point scalars anywhere. The character `.` represents the floating point.  
Example:

~~~yaml
expression: 3 * 67.43 - 5 / 3.14
~~~

#### Parameters

You can use a parameter by using its ID. Note that if the parameter is time-dependent (resp. scenario-dependent), then
it can be used only for variables or constraints that are time-dependent (resp. scenario-dependent), and that its values
will be implicitly unfolded during the interpretation of the expression.  
Example:

~~~yaml
expression: 3 * parameter_1 + 6.345 / parameter_2
~~~

#### Variables

You can use a parameter by using its ID. Note that if the variable is time-dependent (resp. scenario-dependent), then
it can be used only for constraints that are time-dependent (resp. scenario-dependent), and that its values
will be implicitly unfolded during the interpretation of the expression.  
Example:

~~~yaml
expression: 3 * parameter_1 * variable_a + variable_b + 56.4 <= variable_4 * 439
~~~

Also note that all expressions must be linear with respect to variables.  
Examples of prohibited expressions:

~~~yaml
(X) expression: variable_a * variable_b
~~~

~~~yaml
(X) expression: 3 / variable_a
~~~

#### Ports

You can use a port field in the expression, using its ID composed by: **port_ID.field_ID**. Note that if the
port is time-dependent (resp. scenario-dependent), which is deduced from the variables defining it, then
it can be used only for constraints that are time-dependent (resp. scenario-dependent), and that its values
will be implicitly unfolded during the interpretation of the expression. Unless, of course, you use time (resp.
scenario)
aggregators to aggregate it into a time-constant (resp. scenario-constant) value.  
Example:

~~~yaml
expression: 45.4 * port_3.field_6 + 5.4 * variable_6 - 9
~~~

Note that, like with variables, all expressions must be linear with respect to ports.

#### Time operators

For time-dependent parameters, variables, and port fields, you can use these time operators:

- **[t]** suffix: this operator is implied, but can be used if you like to explicit your intent
- **[N]** suffix: where N is any expression resolving to an integer (using only scalars and parameters), this selects
  the value of the element at the N-th timestamp.
- **[t+N]** suffix: where N is any expression resolving to an integer (using only scalars and parameters), this is a
  forward shift operator of N timestamps.
- **[t-N]** suffix: where N is any expression resolving to an integer (using only scalars and parameters), this is a
  backward shift operator of N timestamps.
- **sum(X)** aggregator: where X is the time-dependent operand, this operator sums the operand on the whole optimization
  horizon.
- **sum(S .. E, X)** aggregator: where X is the time-dependent operand, this operator sums the operand between S and E (
  included), where:
    - **S** represents the first timestamp, either as an expression resolving to an integer, or a time-shift expression
      like the ones defined above
    - **E** represents the last timestamp, either as an expression resolving to an integer, or a time-shift expression
      like the ones defined above

Examples:

~~~yaml
expression: a[t] + b[t + 5] * c[t - 3 - 65 * parameter_1] - sum(a)
~~~

~~~yaml
expression: sum(4 .. 87, c) - sum(t - 3 * parameter_15 + 5 .. t, d)
~~~

#### Scenario operators

For scenario-dependent parameters, variables, and port fields, you can use this operator:

- **expec(X)** aggregator: where X is the scenario-dependent operand, this operator computes its expected value (i.e.
  its scenario-wise average).

#### Port operators

You can aggregate incoming ports using the following operator:

- **sum_connections(port.field)**: where "port" is the port ID and "field" is the field ID, this operator computes the
  sum of values of this port field, on all incoming connections from other models.  
  Note that the resulting sum can be time-dependent and/or scenario-dependent, depending on the port definition.

Examples:

~~~yaml
expression: sum_connections(dc_port.flow) = 0
~~~

## System file

The system file describes the energy system that is to be simulated, by listing the [components](05-model.md#components)
and the [port connections](05-model.md#ports-and-connections).  
Currently, Antares modeler supports importing the system from a yaml file.

The header of the yml file must contain exactly one "system" key at the root level.  
The **system** object contains one **id**, one **description**, one **model-libraries** collection, one **components**
collection.  
Unless stated otherwise, all listed fields are mandatory.

### ID, description, and model libraries

Example:

~~~yaml
system:
  id: my_system
  description: my system is even greater!
  model-libraries: my_library_id, my_other_library_id
~~~

- **id**: an ID for your system. Has no effect on the simulation.
- **description** _(optional)_: a free description of your system. Has no effect on the simulation.
- **model-libraries**: a collection of model libraries needed for your system. Must contain at least one element, and
  refer to IDs of model libraries found in the **input/model-libraries" directory. Beware that the ID of the library is
  one defined in its header, not the name of the file.

### Components

The **components** section lists all the [components](05-model.md#components) of your simulated system.  
Example:

~~~yaml
components:
- id: generator1
  model: my_lib_id.dc_generator
  scenario-group: thermal_group
  parameters:
  - id: min_active_power_setpoint
    time-dependent: false
    scenario-dependent: false
    value: 100
  - id: max_active_power_setpoint
    time-dependent: true
    scenario-dependent: true
    value: generator1_max_p
  - id: proportional_cost
    time-dependent: false
    scenario-dependent: true
    value: generator1_cost
- id: generator2
  model: my_lib_id.dc_generator
  scenario-group: hydro_group
  parameters:
  - id: min_active_power_setpoint
    time-dependent: false
    scenario-dependent: false
    value: 20
  - id: max_active_power_setpoint
    time-dependent: true
    scenario-dependent: false
    value: generator2_max_p
  - id: proportional_cost
    time-dependent: false
    scenario-dependent: false
    value: 0.5
- id: node1
  model: my_lib_id.node
~~~

- **id**: an ID for the component. Must be unique in the scope of the system, and respect [these rules](#rules-for-ids).
- **model**: the ID of the model to use for the component, composed as "library_id.model_id", where "library_id" is the
  ID of the model library (must be listed in the [required model libraries](#id-description-and-model-libraries)), and
  "model_id" is the ID of the model as it is defined inside the [model library](#models).
- **scenario-group** _(only needed if the model has scenario-dependent parameters)_: the ID of the scenario group this
  component belongs to. Must be correctly configured in the [scenario builder](#scenario-builder), and
  respect [these rules](#rules-for-ids). Default value is "" (empty string).
- **parameters** _(not needed if model has no parameters)_: a collection of values for the model's parameters. Note that
  all the parameters of the model should have their values assigned by the component.
    - **id**: the ID of the parameter, as defined by the [model](#models)
    - **time-dependent**: `true` or `false`, indicates whether the parameter depends on time or is constant across the
      whole simulation horizon. If the model parameter is not time-dependent, this can't be set to true.
    - **scenario-dependent**: `true` or `false`, indicates whether the parameter changes depending on the simulated
      scenario, or is the same for all scenarios. If the model parameter is not scenario-dependent, this can't be set to
      true.
    - **value**: the value of the parameter:
        - If the parameter is constant, then this is a numerical value (using a data-series ID is not allowed in this
          case)
        - If the parameter is time-dependent, then this is the ID of a time-dependent [data serie](#data-series)
        - If the parameter is scenario-dependent, then this is the ID of a scenario-dependent [data serie](#data-series)
        - If the parameter is time and scenario-dependent, then this is the ID of a
          time-and-scenario-dependent [data serie](#data-series)
      
        Note that using an expression is not allowed (e.g. neither `3 * 4`, nor `6 * some_time_series`).

### Port connections

The **connections** section lists the port connections between components.  
Example:

~~~yaml
connections:
- component1: generator1
  port1: injection
  component2: node1
  port2: injections
- component1: generator2
  port1: injection
  component2: node1
  port2: injections
~~~

- **component1**, **component2**: the IDs of the components to connect together
- **port1**, **port2**: the IDs of the respective ports to connect (as defined by the two models). Note that exactly one
  of the two models must define the port (in the "port-field-definition" section).

### Area connections

The **area-connections** section is used only for hybrid (solver x modeler) studies, and is ignored
by [antares-modeler](08-command-line.md).  
For more information on hybrid
studies, [see the relevant documentation](../solver/08-hybrid-studies.md#connecting-modeler-components-to-legacy-areas).

## Data series

The **input/data-series** directory contains all data-series needed by the [system description](#system-file) to define
component parameter values.

Currently, Antares modeler supports defining data-series using either tab or space seperated values files. Values must
be separated
using tabs, and the character `.` represents the floating point.

### Naming

TSV files inside the directory should respect the "XXX.tsv" or the "XXX.csv" naming template, where "XXX" is the ID of
the data-series. Thus, this ID **must be unique**, and is the one to be used in the [system file](#system-file).

### Time-dependent series

To define a time-dependent series, define a column vector, where every timestamp is represented by a row.  
Example file for a simulation with 6 timestamps:

~~~
10
15
34
56
34
65
~~~

Note that Antares modeler currently does not conduct quality checks on data-series, and that it is up to you to ensure
that the rows cover the [time horizon](04-parameters.md#horizon) of the simulation.

### Scenario-dependent series

To define a parameter value that changes depending on the scenario, define a row vector, where every data set is
represented by a column.  
Currently, Antares modeler does not support scenario building. Thus, please ensure that every scenario you want to
simulate has an associated column in the scenario-dependent series file.  
In the future, you will be able to use the [scenario builder](#scenario-builder) to map different scenarios to the data
sets, in order to avoid duplicating data.  
Example file for a simulation with 4 scenarios:

~~~
54 67.5 23.652 253
~~~

### Time and scenario-dependent series

Use the two methods described above.  
Example file for a simulation with 2 timestamps and 3 scenarios:

~~~
2345 1243 123
2378 748  0
~~~

## Rules for IDs

All IDs in the model library and system file must respect the following:

- Alphanumeric characters are allowed, as well as the underscore (`_`) character
- All other characters are prohibited
- Only lower-case is allowed

### Scenario builder

The **modeler-scenariobuilder.dat** file, located in the **data-series** directory, is used to map scenarios to data
series.
Each line consists of the association of a groupe name and Monte-carlo year -referred to as _year_- to a data series ID
-referred to as _time serie number_-.

Example:

~~~
thermal_group, 0 = 1
thermal_group, 1 = 5
hydro_group, 2 = 7
~~~

* For thermal_group the year 0 is associated with the time serie number 1 whereas the year 1 is associated with the time
  serie number 5.
* For hydro_group the year 2 is associated with the time serie number 7.


* A _year_ is a integer, starting at 0.
* A _time serie number_ is a integer, starting at 1, and refers to the column number in the corresponding
  data series file.
* Group IDs refer to groups defined in the [components](#components) description.
* All years of the simulation require an association to a time serie number

## Full examples

You can find some simple to more complex studies in
our [test base here](https://github.com/AntaresSimulatorTeam/Antares_Simulator/tree/develop/src/tests/resources/modeler).
