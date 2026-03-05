# Hybrid studies


Antares-Solver allows conducting "hybrid" (solver x [modeler](../modeler/01-overview-modeler.md)) studies.  
This enables using [antares-solver studies](02-inputs.md), in which the simulated system is enriched with components 
that are described in the [antares-modeler format](../modeler/05-model.md).

## Input structure
To define a hybrid study, define a [solver study](02-inputs.md), and simply add the [modeler files and directories](../modeler/02-inputs.md) 
to the input directory.    
The parameter.yml file from modeler studies is not needed (if it exists, it will be ignored). The [solver parameters](04-parameters.md) 
are used, since hybrid studies are conducted using [antares-solver](10-command-line.md).  
You can find simple examples by checking out the studies used in our [test base](https://github.com/AntaresSimulatorTeam/Antares_Simulator/blob/develop/src/tests/cucumber/features/solver-features/hybrid_studies.feature).  

## Output files
The hybrid simulation outputs the [same files as a legacy study](03-outputs.md). But these files only contain the 
optimization results of the system components that were created by the legacy study.  

The results of the modeler's components optimization is exported under the simulation table format, described in 
the [modeler outputs page](../modeler/03-outputs.md), in the same output folder as the legacy outputs.  
One simulation table for each optimization step (called `simulation_table--optim-nb-X`) will be generated.

> _**Note:**_ 
> Because every MC year is divided into multiple simplex ranges (blocks), time-independent outputs will appear once for 
> every block in the simulation table.

## Features
Hybrid studies offer the following features.

### Connecting modeler components to legacy areas
One of the most useful use cases is the added ability to define new types of system components with custom behavior, 
using the new [modelling language](../modeler/05-model.md), and inserting these system components into existing studies, 
by connecting them to "areas". 
Such connections allow : 
- adding generation or consumption to an existing area, thus modifying its [balance constraint](05-model.md#balance-between-load-and-generation).
- bounding unsupplied energy or spillage with linear expressions coming from modeler components.

This is implemented in Antares thanks to the [ports](../modeler/05-model.md#ports-and-connections) concept.  
In order to connect new components to existing areas, you must follow two steps:

  - In the system definition, connect a component port to an existing area
  - In the port definition, define which field of the port gets to connect to areas

#### Defining the connections
To connect a component to an area, you must define a connection in the **area-connections** section of the 
[system file](../modeler/02-inputs.md#system-file). 
This section has been specifically created for this use case. Every element in this section represents a connection 
between a component (defined in the same system file) and an existing area defined in the antares-solver study.  

Example:
~~~yaml
area-connections:
 - component: generator1
   port: injection-port
   area: area1
 - component: generator2
   port: injection-port
   area: area1
 - component: consumer1
   port: consumption-port
   area: area2
~~~

- **component**: the IDs of the component to connect to the area, as defined in the [components section](../modeler/02-inputs.md#components)
- **port**: the ID of the component's port to connect to the area (as defined by the model of the component). This port 
  must be of a type that defines an area-connection injection field (see [next paragraph](#selecting-the-area-connection-port-fields))
- **area**: the ID of the area to connect the component to, as defined in the [antares-solver input files](02-inputs.md).


#### Selecting the area-connection port fields
Adding terms to the area's balance constraint only requires one port field definition. Thus, when connecting a port to 
an area, we must specify which field gets to be used for the connection.  
This is done in the [model library](../modeler/02-inputs.md#model-libraries), in the [port type definition](../modeler/02-inputs.md#port-types).  

Example:
~~~yaml
port-types:
   - id: port-to-area
     fields:
        - id: field_to_balance
        - id: angle
	    - id: to-area-bound
        - id: from-area-bound
     area-connection:
        injection-to-balance: field_to_balance
	    spillage-bound: to-area-bound
        unsupplied-energy-bound: from-area-bound
~~~

**area-connection** is the name of the optional section to use. It is mandatory if you want to use such a port type to 
connect modeler components to solver areas.
Here we describe the role of each field contained in **area-connection**. The value for each of them
must be an existing port name in the same port type.
All components connected to this port and area (via an **area-connections** system section entry, 
see previous paragraph [Defining the connections](#defining-the-connections)) will have their corresponding linear 
expression contributing to a constraint of the linear problem.
The nature of this contribution depends on the field : 

  - **injection-to-balance**: the linear expression is injected in the balance constraint of the area.
	This is done with respect to a convention (see next [Optimization model](#optimization-model)).
	
  - **spillage-bound**: the linear expression is added to the sum of all variables or linear expressions already used 
    to bound the spillage in the constraint called "fictitious load".
	
  - **unsupplied-energy-bound** : the linear expression is added to any linear expression already used to bound the unsupplied energy.

These fields are independent : you don't have to define the 3 of them at the same time, you can define only one (as long as its value is an existing port in the same port type).

These fields must be present in the **area-connection** section of a port type, even if they are not defined (= corresponding value is empty). 
 
#### Adding a linear expression in optimization model
When you connect a component to an area via a port (containing an **area-connection** section), you must respect conventions on the GEMS side.

In examples below, for simplicity, we make productions and loads expressions some paramaters with a constant value.
  
- **Connecting to balance constraint**

If you need to involve a production (defined in a component), make it positive and don't prefix it with a - sign.
Typically, you would defined it like this : 
~~~yaml
  # library.yml
  port-types:
    - id: port-to-area
      fields:
        - id: field_to_balance
      area-connection:
        injection-to-balance: field_to_balance
	    spillage-bound:
        unsupplied-energy-bound:

  models:
    - id: my-production
      parameters:
        - id: flat_production # Here is positive production
      ports:
        - id: balance_port
          type: port-to-area
      port-field-definitions:
        - port: balance_port
          field: field_to_balance
          definition: flat_production
~~~

Reversely, if you need to involve a load in the balance constaint, make it negative :
~~~yaml
  models:
    - id: my-load
      parameters:
        - id: flat_load # Here is positive load
      ports:
        - id: port-to-area
          type: field_to_balance
      port-field-definitions:
        - port: port-to-area
          field: field_to_balance
          definition: -flat_load
~~~

- **Connecting to fictitious load constraint**

This kind of connection is specifically made to connect a production from a GEMS component, because it's about limiting spillage optimization variable.
The convention is the same as the connection to balance constraint : make the production positive and don't prefix it with a - sign.
~~~yaml
  # library.yml
  port-types:
    - id: port-to-area
      fields:
        - id: to-area-bound
      area-connection:
        injection-to-balance:
        spillage-bound: to-area-bound
        unsupplied-energy-bound:

  models:
    - id: my-production
      parameters:
        - id: flat_production # Here is positive production
      ports:
        - id: spillage_port
          type: port-to-area
      port-field-definitions:
        - port: spillage_port
          field: to-area-bound
          definition: flat_production
~~~

- **Connectiong to bounding unsupplied energy constraint**

This kind of connection is specifically made to connect a loads from a GEMS component, because it's about limiting the unsupplied energy optimization variable.
The convention is to make the loads positive and don't prefix it with a - sign.
~~~yaml
  # library.yml
  port-types:
    - id: port-to-area
      fields:
        - id: from-area-bound
      area-connection:
        injection-to-balance:
        spillage-bound:
        unsupplied-energy-bound: from-area-bound

  models:
    - id: my-load
      parameters:
        - id: flat_load # Here is positive load
      ports:
        - id: unsup_energy_port
          type: port-to-area
      port-field-definitions:
        - port: unsup_energy_port
          field: from-area-bound
          definition: flat_production
~~~


For more details why we adpot these conventions, please read [this article](https://github.com/AntaresSimulatorTeam/Antares_Simulator/blob/develop/docs/Architecture_Decision_Records/from-GEMS-to-legacy-linear-preblem.md)

## Limitations

- In legacy mode, each MC year is optimized separately. Thus, hybrid studies cannot contain scenario-independent
  variables unless using the Benders decomposition resolution mode. If you try to use such a variable in hybrid mode
  with
  the default sequential-subproblems mode, the solver will fail. Use `resolution-mode: benders-decomposition` in
  the `optim-config.yml` file for investment studies with scenario-independent variables.

## Troubleshooting

### Your model does not behave as expected
Check that your model respects the internal optimization model's injection [convention](#optimization-model).