# How to contribute to Antares Simulator

## Reporting bug
-  Make sure that the bug was not already reported by searching on GitHub under [Issues](https://github.com/AntaresSimulatorTeam/Antares_Simulator/issues).

- If you're unable to find an open issue addressing the problem, [open a new one](https://github.com/AntaresSimulatorTeam/Antares_Simulator/issues/new). You must provide  :
  - description of bug and unexpected behavior
  - expected behavior
  - small antares study to reproduce unexpected behavior
  - Antares Simulator version and OS used 


## Developpement
Antares Simulator team will be pleased to have some developpers join our project.

You can find all steps needed for compilation and installation of Antares Simulator in [INSTALL.md](INSTALL.md).

### Branch names
Currently CI is runned only for specific branch names :
- `feature/*`
- `features/*`
- `fix/*`
- `release/*`

If you create a branch with a different name no CI will be runned but you should receive an email indicating that your branch name is incorrect.

In order to avoid pushing with invalid branch name, a git hooks is provided for pre-commit check. This hooks is available in `.githooks` directory.

By default git use hooks in `.git/hooks` directory which is not under version control. You can define a new hooks directory with this command in Antares Simulator root directory :
```
git config core.hooksPath .githooks
```

### Code formatting
We're using [clang-format](https://clang.llvm.org/docs/ClangFormat.html) to format code. Rules are defined in [.clang-format](src/.clang-format) file.
