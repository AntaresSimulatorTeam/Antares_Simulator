# Guidelines for the Study Format

This document provides reference guidelines for the **Antares\_Simulator** study format and should be consulted when making modifications to the format.

## Input Files

All input files are stored in the **input** directory of a study. This directory is organized into thematic subdirectories (e.g., **thermal**, **wind**, **solar**, etc.).

* If you introduce functionality unrelated to existing categories, create a new subdirectory under **input**.
* Otherwise, place new files in the appropriate existing subdirectory.

## File Names

File names must use only Latin characters. Non-Latin characters are not supported and may cause errors (especially on Windows, which uses UTF-16 encoding for local paths).

## Floating-Point Number Format

* Use a decimal point as the separator (e.g., `4.4` is valid, `4,4` is not).
* Negative numbers must start with a minus sign (e.g., `-5`).
* Scientific notation is allowed (e.g., `1e-4`).
* These formats can be combined (e.g., `-1.3e-4`).

## File Encoding

* ASCII is the default encoding for all files.
* UTF-8 can be used when necessary (e.g., for non-Latin characters in file contents).
* Note: file paths cannot contain non-Latin characters, but file content can use UTF-8.

## Timeseries

Time- and/or scenario-dependent parameters are represented as **timeseries**, stored in TAB-separated text files. Each entry is separated by a newline (`\n`).

### Number Types

Timeseries can contain integer or floating-point values (see **Floating-Point Number Format** above).

### Dimensions

* Timeseries can have 1 to N columns and any number of rows.
* A typical choice is `HOURS_PER_YEAR = 8760` (hourly values for a full year).
* If multiple columns exist, all columns must have the same number of rows.

### Empty Files

An empty file is treated as a single column of zeros with the expected number of rows:

```
0.0
...
... (repeated N times)
...
0.0
```

## Parameters

Parameters can be defined using **INI** or **YAML**. For new additions, **YAML** is preferred.

### INI

Example:

```ini
[section1]
key1 = value1
key2 = value2

[section2]
key1 = value1
key2 = value2
```

* Keys must be unique within a section. This rule should be enforced for new additions.
* Empty lines are ignored.

### YAML

Example:

```yaml
system:
  id: fr
  components:
    - id: gas
      model: standard.thermal
```

* IDs can be used to reference objects and must be unique within the local scope.

Valid example:

```yaml
parameters:
  - id: reservoir_capacity
    value: 1200
  - id: injection_nominal_capacity
    value: 300
```

Invalid example (duplicate ID):

```yaml
parameters:
  - id: reservoir_capacity
    value: 1200
  - id: reservoir_capacity
    value: 300
```

Duplicate IDs are ambiguous and must be avoided.
