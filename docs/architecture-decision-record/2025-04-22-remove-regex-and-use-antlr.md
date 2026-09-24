````markdown name=adr-2025-04-22-remove-regex-and-use-antlr.md
# ADR: Replace Regex-Based Parsing with Antlr for HoursField

Date: 2025-04-22  
Author: a-zakir  

## Context

Previously, the system used regular expressions to parse the `hours` field in additional constraints. While functional, this approach had limitations:

- **Error-Prone**: Regular expressions are difficult to maintain and debug, especially as requirements evolve.
- **Limited Flexibility**: Regex-based parsing is tightly coupled to specific formats, making it harder to extend.
- **Poor Error Handling**: Regex provides minimal feedback on parsing errors, which can lead to vague error messages.

To address these issues, we decided to replace the regex-based parsing with Antlr, a powerful parser generator that supports robust grammar definitions and error handling.

## Decision

We transitioned from regex-based parsing to Antlr. Key changes include:

1. **Grammar Definition**:
   - Introduced an Antlr grammar (`HoursField.g4`) to define the syntax for the `hours` field.
   - The grammar supports grouping of hours (e.g., `[1,2,3]`) and ensures strict validation of input formats.

2. **Code Generation**:
   - Used Antlr to generate lexer, parser, and visitor classes for the `HoursField`.

3. **Integration**:
   - Replaced the existing `loadHours` function with a combination of:
     - `makeGroupsOfHoursFromString`: Parses the input string using Antlr.
     - `toConstraints`: Converts the parsed groups into the required constraint format.

4. **Enhanced Error Handling**:
   - Added a custom error listener to capture and log detailed error messages when parsing fails.

## Consequences

### Positive
- **Maintainability**: The grammar is easier to update and debug compared to regex.
- **Extensibility**: Antlr provides a structured framework for adding new syntax or rules.
- **Improved Error Feedback**: Parsing errors are now more descriptive, aiding faster debugging.

### Negative
- **Build Complexity**: The introduction of Antlr adds a new dependency and increases the complexity of the build process.
- **Learning Curve**: Team members need to familiarize themselves with Antlr.

## Status

**Accepted**

## Alternatives Considered

1. **Enhanced Regex**:
   - Pros: No additional dependencies.
   - Cons: Would not address maintainability or error feedback issues.

2. **Custom Parser**:
   - Pros: Tailored to specific needs.
   - Cons: Time-intensive to implement and less robust than Antlr.

## Future Considerations

- Evaluate the performance of the Antlr-based parser with large configurations.
- Explore extending the grammar to handle additional constraint types.
````
