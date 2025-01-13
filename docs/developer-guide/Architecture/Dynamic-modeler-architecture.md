# Dynamic modeler architecture

## Models
(for details about these concepts, see [this page](../../user-guide/solver/dynamic-modeler/05-model.md))

```plantuml
@startuml

class Model {
+ string id
+ Expression objective
+ map<string, Parameter> parameters
+ map<string, Variable> variables
+ map<string, Constraint> constraints
+ map<string, Port> ports
}
Model "1" *-- "0:N" Parameter
Model "1" *-- "0:N" Variable
Model "1" *-- "0:N" Constraint
Model "1" *-- "0:N" Port
Model --> Expression

class Parameter {
+ string id
+ ValueType type
+ bool timeDependent
+ bool scenarioDependent
}
Parameter "N" *-- "1" ValueType

enum ValueType {
FLOAT
INTEGER
BOOL
}

class Variable {
+ string id
+ ValueType type
+ Expression lowerBound
+ Expression upperBound
}
Variable "N" *-- "1" ValueType
Variable --> Expression

class Constraint {
+ string id
+ Expression expression
}
Constraint --> Expression

class Port {
+ string id
+ PortType type
}
Port "N" *-- "1" PortType

class PortType {
+ id
+ vector<PortField> fields
}
PortType "1" *-- "1:N" PortField

class PortField {
+ string id
}

class Expression {
+ string textualRepresentation
+ Node nodeRepresentation
}

class ModelLibrary {
+ string id
+ map<string, Model> models
}
ModelLibrary "1" *-- "1:N" Model

class ModelLibraryRepository {
+ map<string, ModelLibrary> modelLibraries
}
ModelLibraryRepository "1" *-- "0:N" ModelLibrary

@enduml
```

## Components
(for details about these concepts, see [this page](../../user-guide/solver/dynamic-modeler/05-model.md))

```plantuml
@startuml

class Model {
+ string id
+ Expression objective
+ map<string, Parameter> parameters
+ map<string, Variable> variables
+ map<string, Constraint> constraints
+ map<string, Port> ports
}

class Component {
+ string id
+ Model model
+ string scenarioGroup
+ map<string, Expression> parameterValues
}
Component "0:N" *-- "1" Model
Component --> Expression

class Expression {
+ string textualRepresentation
+ Node nodeRepresentation
}

class PortConnection {
+ string component1Id
+ string port1Id
+ string component2Id
+ string port2Id
}

class System {
+ map<string, Component> components
+ vector<PortConnection> portConnections
}
System "1" *-- "1:N" Component
System "1" *-- "0:N" PortConnection

@enduml
```