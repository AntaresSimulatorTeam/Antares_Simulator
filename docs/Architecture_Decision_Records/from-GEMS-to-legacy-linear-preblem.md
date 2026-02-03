# Adding expressions from GEMS into legacy LP constraints

Every time we want to add an expressions from GEMS into an existing legacy LP constaint, this expression 
is evaluated into a time-dependent linear expression.
A time-dependent linear expression is a vector of linear expressions (one for each time step).
A linear expression is comprises a linear combination of variables and a unique constant term.
In a linear expression, each variable is associated to a constant coefficient.

So when adding an expression from GEMS into a LP constraint, we merely add coefficients or constants to the constraint.

In the following, writting constraints often implies gathering constant-in-time terms in the RHS, 
and variable terms to the LHS.

## Balance constraint

In Antares legacy modeler code, balance constraint is actually written as follows :
 **-P = -(L-Fatal)**, with **P** being productions, **L** being loads and **L-Fatal** being the residual load.
An equivalent form is : **L - P = Fatal**.
Now, let's break P into Pv (production variable in time) and Pc (production constant in time) :
**L - Pv = Fatal + Pc**
As a consequence, variable productions from GEMS are added negatively, and constant productions are added positively. 


## Fictitious load constraint

This constraint can be written as follows : 

**Spillage - Thermal - Hydro < Fatal**.  

As previously, adding time-variable productions from GEMS is done negatively, and adding time-constant production is done positively.

## Bounding unsupplied enegy constraint

This constraint can be written as follows :

**unsupE - Lv < Lc + Fatal**, **Lv** being variables loads, **Lc** being constant loads.


adding time-variable loads from GEMS is done negatively, and adding time-constant loads is done positively.
