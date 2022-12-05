# Executable Kirchhoff's Constraint Generator

The kirchhoff constraint generator is a standalone tool used to automatically create kirchhoff constraints. For more details about this process read:

[7-kirchhoffs_constraint_generator.md](https://github.com/AntaresSimulatorTeam/Antares_Simulator/blob/develop/docs/reference-guide/07-kirchhoffs_constraint_generator.md)

The binary is located in Antares_Simulator/bin/

---

## Usage

**./antares-8.3-kirchhoff-constraints-builder** [study_path] [option_file_path]

## Options

**study_path**: the path of the study, mandatory

**option_file_path**: Optional. The path of the **constraintbuilder.ini** file. It's normally located in *study_path/settings/*. If not provided the constraint builder will take the default values, which are defined at the end of cbuilder.h. 

Those values were used to fill the GUI default values, and then the GUI created the file. That's why it's optional since only the default values are guaranteed to work.

## Results

New constraints generated this way will be stored in *input/bindingconstraints/* with the name uto_0001.txt, with incrementing numbers. The file *bindingconstraints.ini* located in the same directory will be updated accordingly.

## Study components needed

The study only needs to include the following components to generate kirchhoff constraints:

- Areas (contains links)
- Binding Constraints
- version number