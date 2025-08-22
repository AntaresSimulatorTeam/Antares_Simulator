# Conventions regarding the study format
Here are some guidelines for the Antares_Simulator format. This document is meant to serve as a reference when applying changes to the study format.

## Input files
All input files are placed under the **input** directory of a study. The **input** directory contains thematic sub-directories (**thermal**, **wind**, **solar**, etc.). If you add functionality that is not related to the existing functionalities, consider adding a new sub-directory in **input**. Otherwise, use the existing sub-directories.

## File names
File names containing non-latin characters are not supported, and may lead to errors (Windows uses UTF-16 encoding for local paths).

## Floating point numbers representation
- All floating point numbers are represented with a floating point decimal separator (for example "4.4" is valid , "4,4" is invalid)
- Negative numbers start with "-" e.g "-5"
- Scientific notation is accepted e.g "1e-4"
- The above can be combined e.g "-1.3e-4"

## File encoding
The default encoding for all files is ASCII, although UTF-8 is also supported when necessary (non-latin characters). To be clear, while file paths may not contain non-latin characters, their content can contain UTF-8 characters.

## Timeseries
In cases where a time and/or scenario-dependent parameter is needed, we use "time series", which are stored in TAB-separated text files. The newline character "\n" is also used between entries.

### Number types
It is possible to add timeseries of integer values, and floating-point values (see above for their representation).

### Sizing
Timeseries can have 1..N columns and any number of rows. A common choice for the number of rows is `HOURS_PER_YEAR = 8760` (annual timeseries of hourly values).

If multiple columns are present, then all of them must contain the same number of rows.

### Empty files
An empty file is implicitly considered to contain one column of zeros, with the appropriate number of rows. More specifically, if we expect N rows, an empty file is considered to be a single column of zeros with N rows

```
0.0
...
... (N times)
...
0.0
```

## Parameters
For parameters we use both INI and YAML. For newer additions, YAML should be preferred over INI.

### INI
Example
```ini
[section1]
key1 = value1
key2 = value2

[section1]
key1 = value1
key2 = value2
```
Within a section a key must be unique. While it is not the case in the current format, this restriction should be enforced for newer additions to the format.

Empty lines are ignored.

### YAML

```yaml
system:
  id: fr
  components:
  - id: gas
    model: standard.thermal
```
We sometimes use IDs to refer to some existing objects. In this case, the ID must be unique at the local scope

Valid

```yaml
parameters:
- id: reservoir_capacity
  value: 1200
- id: injection_nominal_capacity
  value: 300
```

Invalid (duplicate ID)

```yaml
parameters:
- id: reservoir_capacity
  value: 1200
- id: reservoir_capacity
  value: 300
```
In this case `parameters.id` would be ambiguous, which is why we enforce uniqueness.
