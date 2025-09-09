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
using the new [modelling language](../modeler/05-model.md), and inserting them into existing studies, 
by connecting them to "areas". This has the effect of adding generation or consumption to an existing area, thus 
modifying its [balance constraint](05-model.md#balance-between-load-and-generation).  

This is implemented in Antares thanks to the [ports](../modeler/05-model.md#ports-and-connections) concept.  
In order to connect new components to existing areas, you must follow two steps:

  - In the system definition, connect a component port to an existing area
  - In the port definition, define which field of the port gets to connect to areas

#### Defining the connections
To connect a component to an area, you must define a connection in the **area-connections** section of the 
[system file](../modeler/02-inputs.md#system-file). 
This section has been specifically created for this use case. Every element in this section represents a connection 
between a component defined in the same system file, and an existing area defined in the antares-solver study.  

Example:
~~~yaml
area-connections:
 - component: generator1
   port: injection
   area: area1
 - component: generator2
   port: injection
   area: area1
 - component: consumer1
   port: consumption
   area: area2
~~~

- **component1**: the IDs of the component to connect to the area, as defined in the [components section](../modeler/02-inputs.md#components)
- **port**: the ID of the component's port to connect to the area (as defined by the model of the component). This port 
  must be of a type that defines an area-connection injection field (see [next paragraph](#selecting-the-area-connection-port-fields))
- **area**: the ID of the area to connect the component to, as defined in the [antares-solver input files](02-inputs.md).


#### Selecting the area-connection port fields
Adding terms to the area's balance constraint only requires one port field definition. Thus, when connecting a port to 
an area, we must specify which field gets to be used for the connection.  
This is done in the [model library](../modeler/02-inputs.md#model-libraries), in the [port type definition](../modeler/02-inputs.md#port-types).  

Example:
~~~yaml
port-type:
   id: ac_link
   fields:
      - id: flow
      - id: angle
   area-connection:
      - injection-field: flow
~~~

**area-connection** is the name of the optional section to use. It is mandatory if you want to use such a port type to 
connect modeler components to solver areas.  

  - **injection-field**: the field to use when adding the contribution of this port bearer to a connected area, were the 
    component to be connected to an area

#### Optimization model
The linear expression defined by the connected component's port field definition is simply added to the **left-hand side** 
of the area's [balance constraint](05-model.md#balance-between-load-and-generation).  

???+ warning
    
    The current convention of this constraint is:  
      - Generation contributions to the balance should be **positive**  
      - Load contributions to the balance should be **negative**  
    Take this into account when defining the connection port value.

## Limitations

- In legacy mode, each MC year is optimized separately. Thus, hybrid studies cannot contain scenario-independent 
  variables. If you try to use such a variable in hybrid mode, the solver will fail.

## Troubleshooting

### Your model does not behave as expected
Check that your model respects the internal optimization model's injection [convention](#optimization-model).