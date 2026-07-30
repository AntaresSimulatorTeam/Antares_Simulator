# Optimization model

_**This section is under construction**_

The new Antares dynamic modeler is all about giving the user the power to define the mathematical model of every
physical element in the system. Elements of this model are described in this section.

## Models

A model defines the behavior of an element in the simulated system. Several elements can have the same behavior, and
therefore the same model.
For example, a "FlexibleLoad" model can define the behavior of a flexible demand.

The basic attributes of a model are:

- a list of parameters: these are the input data required by the model.  
  For example: the nominal power of the groups in a thermal cluster, the value of a load, etc.  
  A parameter can be time-step-dependent or not, and scenario-dependent or not.
- a list of potentially bounded variables: these are the quantities whose values the simulation will have to define.
  For example: the power produced by a thermal cluster, or the level of a stock.
- a list of constraints: these are equations that link parameters and model variables.
  For example, for a battery, we might have an equation of the following type:  
  **level[t] - level[t-1] - efficiency * injection + withdrawal = inflows**
- a contribution to system cost, defined on the basis of model parameters and variables.  
  For example, for a thermal cluster, the contribution might look like this:  
  **time_sum(cost * generation)**
- a list of [ports](#ports-and-connections)

## Components
A “component” is a model instance: it is an object in the simulated system whose behavior is defined by a model as 
described above.  
It is therefore defined by:
- a unique identifier in the study
- a model that defines its behavior
- a set of values for the parameters of the model

## Ports and connections
So far, we haven't defined any possibilities for interaction between models, or between components of a study. However, 
we will need to connect these objects to each other: connect a power demand, a production cluster, or a link to a node, typically.

- a [model](#models) defines a list of its ports, which enable components implementing this model to be connected to other components
- a port, in a model, is defined by a name and its “type”.
- a port type defines the quantities that are exposed: typically a power flow, but also, for example, a gas flow, a voltage phase, etc.
  A port type can expose several quantities; these quantities are called “fields”.
- In a system, two components may be linked together by a connection between two ports of identical type (and only in this way).
  
The introduction of the port concept allows the user to:
1. expose the internal quantities of a model in a standard way to the outside: for example, the object to which a 
   generator is connected doesn't need to know the name of the variable internal to the power flow model, it just needs 
   to know its name on the port type common to all the models.
2. assemble several quantities on the same interface: for example, an electrical power flow and the voltage phase, 
   for an AC electrical network.


Each model can:
1. define, with an [expression](#expressions), the value of a field exposed on one of its ports.
2. use, in any [expression](#expressions), the value of a field of one of its ports, potentially defined by another model.

In order to obtain a mathematically solvable problems, in a system, each connection between two ports must have a 
single definition for each field.

## Expressions
The [models](#models) allow certain elements to be defined by the user, using “free” mathematical expressions:
- constraints
- variable bounds
- contribution to system cost
- definition of the value of a field exposed on a port

Expressions can be used to reference:
- model parameters
- model variables
- model port fields

They also enable a number of antares-specific operations using:
- temporal operators:
  - shift: for example, to reference the value of a parameter or variable at the previous time step
  - evaluation: reference the value of a parameter or variable at a specific time step, e.g. time step 0
- time aggregations: for example summing a variable on an interval of time
