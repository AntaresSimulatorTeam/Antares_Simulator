# Usage with FICO® Xpress Optimization

## Introduction
The FICO Xpress optimizer is a commercial optimization solver for linear programming (LP), mixed integer linear programming (MILP), convex quadratic programming (QP), convex quadratically constrained quadratic programming (QCQP), second-order cone programming (SOCP) and their mixed integer counterparts.

Antares Solver only uses LP, with plans to use MILP at some point in the future. Various benchmarks have revealed that Xpress allows faster simulations than Sirius on large studies.

## Using Xpress in the command-line
```
antares-x.y-solver --use-ortools --ortools-solver xpress [options] <study>
```

## Setup
The `XPRESSDIR` env variable must point to the Xpress install directory. The author strongly suggest placing this in your .bashrc if you use Bash
```
export XPRESSDIR=path/to/xpress/install/dir
```

An Xpress install directory looks like this. Actually, only these files are required, the rest are unnecessary
```
xpress814 <= XPRESSDIR
└── lib
    ├── libxprs.so -> libxprs.so.40
    ├── libxprs.so.40 <= The Xpress shared library
    └── xpauth.xpr <= A valid licence
```

## Checking that Xpress works
After setting up Xpress, the following command line should display a line if Xpress is installed and a valid licence is found.
```
antares-x.y-solver --list-solvers | grep xpress
```

## Compatibility
Antares Simulator 8.x has been shown to work with Xpress 8.13.0. Trying to use other versions may work, but can lead to crashes.

## Known issues
### Performance issues with Xpress 9.x
The following Xpress versions suffer a performance regression that makes them unsuitable for use with Antares Simulator
- 9.0.x
- 9.1.x
- 9.2.x

As of Q3 2023, it is best to stick to Xpress 8.13.0 for best performance

### Community license is very restrictive
FICO offers a Community License for testing purposes. It should be noted however that it is extremely restrictive. You may experience issues when trying to run Antares Solver with this license, even on small test-cases.
