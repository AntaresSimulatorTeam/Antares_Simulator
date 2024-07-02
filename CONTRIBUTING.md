# How to contribute to Antares Simulator

## Reporting a bug
-  First, please make sure that the bug has not been already reported under [Issues](https://github.com/AntaresSimulatorTeam/Antares_Simulator/issues).

- If not, [open a new one](https://github.com/AntaresSimulatorTeam/Antares_Simulator/issues/new). You must provide  :
  - a description of the bug and its unexpected behavior
  - the expected behavior
  - a small antares study to reproduce the unexpected behavior
  - the version of Antares Simulator and OS used 


## Development
Antares Simulator team will be pleased to have developers join our project.

You can find all the steps needed to build & install Antares Simulator in 
the [documentation website](https://antares-simulator.readthedocs.io/)
or [its sources](docs/developer-guide/0-Introduction.md).

### Branch names
Currently, CI is run only for specific branch names:
- `feature/*`
- `features/*`
- `fix/*`
- `release/*`
- `issue-*`
- `doc/*`

If you create a branch with a different name, no CI will be run, but you will receive an email 
indicating that your branch name is incorrect.

In order to avoid pushing with invalid branch name, a git hook is provided for pre-commit check.
This hook is available in the `.githooks` directory.

By default, git use hooks in `.git/hooks` directory which is not under version control. You can
define a new hooks directory with this command in Antares Simulator root directory :
```
git config core.hooksPath .githooks
```

### Code formatting
We're using [clang-format](https://clang.llvm.org/docs/ClangFormat.html) to format code. Rules are defined in [.clang-format](src/.clang-format) file.

### Pull Requests

A pull request name must be self-explanatory: this will be the default commit title when merging.

Please provide a description in the head comment of the PR. This description will be the details of the merge commit.
The description should be short but proportional to the length or complexity of the PR. Try to explain the motivation
of the PR (why) and the method employed (how).

When a pull request is opened, please set it to draft if it is still being worked on or not ready for review.  

If your Pull Request changes a part of the code that is [documented](https://antares-simulator.readthedocs.io/), 
please update the documentation also, in the ["docs"](docs) directory.  