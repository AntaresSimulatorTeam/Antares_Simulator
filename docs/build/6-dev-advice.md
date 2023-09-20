# Developer advice

## Ignore submodules to make git operations faster
Antares_Simulator is quite a large project, with a few large submodules. In file .git/config, you can add this line to all [submodule] sections

```
ignore = all
```

This way git won't waste time computing diff on these when checking out, diffing commits, etc. git operations should be a lot faster. Keep in mind that your submodules won't be updated.

## Disable the UI build to make builds faster
The UI takes up a good chunk of compilation time. It is enabled by default, byt you can disable it by providing CMake with this option at configure time

```
cmake -S src [...] -DBUILD_UI=OFF
```

## Use Ninja to speed up target generation by CMake
At configure time, you may specify Ninja for generation instead of traditional Make. This will speed up the update step after you made small changes to the code.

```
cmake -S src [...] -G Ninja
```

Note that you may need to install Ninja first (package `ninja-build` on Ubuntu).

## Adding the solver build path to your PATH
If you use `antares-x.y-solver` often, you may find it convenient to add it's location to your PATH. In your .bashrc, add (with slight variations)

```
export PATH="$PATH:path/to/Antares_Simulator/build/solver
```

Then you can open a terminal and execute antares-x.y-solver by just typing

```
antares-x.y-solver
```
