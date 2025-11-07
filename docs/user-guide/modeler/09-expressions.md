# Expressions

Expressions are human-readable arithmetic expressions.
They can contain a sign (in this case, they are equations) or not (simply representing a value or a function).
They allow a large flexibility in defining objects of optimization.

Expressions can only be found in [models](../02-inputs.md#Models) (defined in libraries yaml files).

More specifically, they can be found in the following contexts inside a model : 
- **constraints**
- **binding-constraints**
- **objective-contributions**
- **port-field-definitions**
- in fields **lower-bound** and **upper-bound** of **variables**.
- **extra-output**


Many elements can be found in an expression. In the following sections, we give a list of these elements, but have in mind that all these elements cannot be used in any kind of expression.
It depends on the context of the expression inside the model.
For instance, if the expression is used to define the **lower-bound** of a **variable**, it cannot contain a reference to any **variable**.
For more precision see [Cases where elements can be used in expressions]

Here is the list of elements possibly composing an expression :

## Arithmetic operators in expressions

The following operators are allowed between two elements :

- **+**: addition
- **-**: subtraction
- __*__: multiplication (muliplying 2 variables is always forbidden)
- **/**: division (dividing 2 variables is always forbidden)
- **=**: equality, only allowed for constraint definitions
- **<=**: less or equal to, only allowed for constraint definitions
- **>=**: greater or equal to, only allowed for constraint definitions

## Scalars in expressions

You can use simple floating-point scalars anywhere. The character `.` represents the floating point.

**Example:**

~~~yaml
expression: 3 * 67.43 - 5 / 3.14
~~~

## References to elements defined elsewhere

Expressions can contain references to elements defined elsewhere, either in the same model (as the expression), or outside the model (via ports).
These elements can be the following (we assume they are already defined elsewhere) :

### Parameters in expressions

You can use a parameter by using its ID. Note that if the parameter is time-dependent (resp. scenario-dependent), then
it can be used only for variables or constraints that are time-dependent (resp. scenario-dependent), and that its values
will be implicitly unfolded during the interpretation of the expression.  
**Example:**

~~~yaml
expression: 3 * parameter_1 + 6.345 / parameter_2
~~~

### Variables in expressions

You can use a variable by using its ID. Note that if the variable is time-dependent (resp. scenario-dependent), then
it can be used only for constraints that are time-dependent (resp. scenario-dependent), and that its values
will be implicitly unfolded during the interpretation of the expression.
  
**Example:**

~~~yaml
expression: 3 * parameter_1 * variable_a + variable_b + 56.4 <= variable_4 * 439
~~~

Also note that all expressions must be linear with respect to variables.  
**Examples of prohibited expressions:**

~~~yaml
(X) expression: variable_a * variable_b

(X) expression: 3 / variable_a
~~~

### Ports in expressions

You can use a port field in the expression, using its ID composed by: **port_ID.field_ID**. Note that if the
port is time-dependent (resp. scenario-dependent), which is deduced from the variables defining it, then
it can be used only for constraints that are time-dependent (resp. scenario-dependent), and that its values
will be implicitly unfolded during the interpretation of the expression. Unless, of course, you use time (resp.
scenario)
aggregators to aggregate it into a time-constant (resp. scenario-constant) value. 
 
**Example:**

~~~yaml
expression: 45.4 * port_3.field_6 + 5.4 * variable_6 - 9
~~~

Note that, like with variables, all expressions must be linear with respect to ports.

## Other operators in expressions

In expressions, apart from arithmetic operators, other operator can be found.

### Time operators

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

**Examples:**

~~~yaml
expression: a[t] + b[t + 5] * c[t - 3 - 65 * parameter_1] - sum(a)

expression: sum(4 .. 87, c) - sum(t - 3 * parameter_15 + 5 .. t, d)
~~~

### Scenario operators

For scenario-dependent parameters, variables, and port fields, you can use this operator:

- **expec(X)** aggregator: where X is the scenario-dependent operand, this operator computes its expected value (i.e.
  its scenario-wise average).

### Ports' sum_connections operator

You can aggregate incoming ports using the following operator :

**sum_connections(port.field)** : where "port" is the port ID and "field" is the field ID, this operator computes the
sum of values of this port field, on all incoming connections from other models.  
Note that the resulting sum can be time-dependent and/or scenario-dependent, depending on the port definition.

**Examples:**

~~~yaml
expression: sum_connections(dc_port.flow) = 0
~~~

### Dual operators

In some cases, we need to access dual results of variables / constraints of the linear problem.
For both case, there are a different unary operator : 
- dual result of a variable of id **my_var** is accessed by **-reduced_cost(myVar)**
- dual result of a constraint of if **my_constraint** is accessed by **dual(myConstraint)**

Note that dual results can only used within an expression whose context is an extra output. 

**Exemple** : 

```yaml
models:
  - id: myModel
    variables:
    - id: myVar
      upper_bound: 1
    constraints:
    - id: myConstraint
      expression: x <= 1
    extra-outputs:
    - id: marginal_price_variable
      expression: -reduced_cost(myVar)
    - id: marginal_price_constraint
      expression: dual(myConstraint)
```

### Power operator

This unary operator **^** is used within an expression whose context is an extra output.
It can apply only to a reference an existing variable.

**Example :** 

```yaml
models:
  - id: myModel
    variables:
    - id: myVar
    parameters:
    - id: myParam
    extra-outputs:
    - id: myOutput
      expression: myVar^(2 + myParam)
```

### Max operator

This binary operator **max(u, v)** is used inside an expression whose context is either **constraints** or **extra-output**.
- in the context of a constraint, the 2 operands must be parameters or literals
- in the context of an output, there are no restriction on operands.

**Example :**

```yaml
models:
  - id: myModel
    variables:
    - id: x
    parameters:
    - id: a
    - id: b
    constraints:
    - id: myConstraint
      expression: x < max(a,b)
    extra-outputs:
    - id: myOutput
      expression: max(x, a*b)
```


## Cases where elements can be used in expressions

### Operators

**Caution** : multiplying (*) or dividing (/) 2 (references to) variables is always forbidden.
We only multiply/divide scalars, parameters, or vaiables to scalars/parameters. 

---

|Context                      | [+-]  | [*/] | [<>=] |  Time | sum_connections |
|-----------------------------|-------|------|-------|-------|-----------------|
|constraints                  |  x    |   x  |  x    |   x   |  x              |
|binding-constraints          |  x    |   x  |  x    |   x   |  x              |
|objective-contributions      |  o    |   x  |  o    |   o   |  o              |
|port-field-definitions       |  x    |   x  |  o    |   o   |  o              |
|variable bounds              |  x    |   x  |  o    |   x   |  o              |
|extra-output                 |  ?    |   ?  |  ?    |   ?   |  ?              |

---

|Context                      | Scenario | Dual | Power | Max | sum |
|-----------------------------|----------|------|-------|-----|-----|
|constraints                  |    ?     |   o  |   ?   |  ?  |  ?  |
|binding-constraints          |    ?     |   o  |   ?   |  ?  |  ?  |
|objective-contributions      |    ?     |   o  |   ?   |  ?  |  ?  |
|port-field-definitions       |    ?     |   o  |   ?   |  ?  |  ?  |
|variable bounds              |    ?     |   o  |   ?   |  ?  |  ?  |
|extra-output                 |    ?     |   o  |   ?   |  ?  |  ?  |


### References to elements defined elsewhere

|Context                      | variable  | parameter| Port |
|-----------------------------|-----------|----------|------|
|constraints                  |  x        |   x      |  x   |
|binding-constraints          |  x        |   x      |  x   |
|objective-contributions      |  o        |   x      |  o   |
|port-field-definitions       |  x        |   x      |  o   |
|variable bounds              |  0        |   x      |  o   |
|extra-output                 |  ?        |   ?      |  ?   |


 