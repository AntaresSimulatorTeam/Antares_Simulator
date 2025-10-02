# Release Notes: v9.3.0 to develop head

This document provides a comprehensive overview of all changes between version 9.3.0 and the current develop branch head.

**Total commits**: 99
**Latest commit**: 81cad35 - Use boost::iostreams::mapped_file_source to improve CSV loading performance [ANT-3915] (#3089)

## New features

* **Modeler: Implement eval and time index visitors for portfield nodes** (#3110)
* **Optional overflow for short-term storage objects** [ANT-3695] (#3036)
* **ANT-3617 - modeler 5.1** (#2933)
* **Modeler 5.3: Add extra outputs to simulation table** (#3053)
* **Modeler 5.4: Evaluate comparisons** [ANT-3620] (#3086)
* **5.2: Export ports in simulation table** (#2982)
* **[5.3 part 1/2] Add extra-outputs to models & ID unicity checks** (#2988)
* **Evaluation context provider from Component** [ANT-3750] (#3043)

## Removed features

* **[ANT-3638] Remove H cost from Ov cost** (#3035)
* **Remove special value "default" for scenario-group field** (#2977)

## Improvements

* **Use boost::iostreams::mapped_file_source to improve CSV loading performance** [ANT-3915] (#3089)
* **Save hydro gen and pump in legacy GUI** [ANT-3140] (#3084)
* **Logs time used to build modeler problem** [ANT-3759] (#3041)
* **Display correct problem size in logs** [ANT-3716] (#3026)
* **Short term storage : simplify results data structure** [ANT-3175] (#2991)
* **Load only needed solvers** [ANT-3543] (#2976)
* **Improve behave's error message for simulation tables** (#3061)
* **Remix by shaving peaks : adapt for multi storage** [ANT-3175] (#2975)
* **Reduce number of copy for Xpansion interface** (#3062)
* Simplify ShortTermStorageCumulation.cpp (#3010)
* Simplify logging handling in node visitor (#2996)
* Hydro remix : more simplifications (#2959)
* Various simplifications in modelConverter.cpp (#2981)
* Linear expression : some cleaning (#3042)
* Remove unused nodes and visitors (component substitutions) (#3037)
* Update XPRESS documentation (#2990)
* Warning if simplex-range = day [ANT-2695] (#3055)

## Bugfixes

* **ANT-3697 : fix contraint on weekly hydro generation amount** [ANT-3697] (#3040)
* **Catch modeler loading error** [ANT-3837] (#3063)
* **Modeler - fix various problems with time & scenarios** [ANT-3712] (#2972)
* Fix link error sonar (#3103)
* Fix naming in hydro remix unit tests (#3098)
* Fix clang Wunused-local-typedef (#3069)
* Fix ignoring return value of function declared with 'nodiscard' (#3027)
* Fix: moving a temporary prevent copy elision (#3029)
* Fix unqualified call to 'std::move' (#3031)
* Fix pragma diagnostic pop could not pop, no matching push (#3030)
* Fix warnings (#3022)
* Fix array default initialization (#3006)
* Fix broken link for Sonar badges (#2994)
* Fix logic for map parsing in yaml-cpp (#2992)
* Fix behave tests for CentOS (#3039)
* Fix boost.test log format (#2967)
* Several warning fixes (#3017)

## Modeler

* Add debug logs for modeler (#2971)
* Add modeler doc precisions (#3000)
* 4.3 : Clearer error message when out of bound ts number (#2979)
* Flatten `SumNode` to avoid deep recursion (#3094)
* Refactor unique ID check (#2997)
* Replace binary minus with -= in ReadLinearConstraintVisitor (#2987)
* [ANT-3606] Uppercase for group scenarios (#2970)

## CI

* Bump SonarSource/sonarqube-scan-action from 5.3.1 to 6.0.0 (#3083)
* Bump SonarSource/sonarqube-scan-action from 5.3.0 to 5.3.1 in /.github/workflows in the github_actions group across 1 directory (#3047)
* Bump actions/checkout from 4 to 5 (#2973)
* Bump actions/setup-python from 5 to 6 (#3051)
* Bump install-build-wrapper@v5.3.1 to fix vulnerability in 5.3.0 (#3019)
* windows-vcpkg workflow - skip CI if commit message contains [skip ci] (#3046)
* Use gcc-11 for ubuntu & sonarcloud jobs (#3095)
* pull-request-size bot comments XXL PRs (#3079)

## Build

* Disable warning in antlr (#3070)
* Ignore warning in tests (#3028)
* Reintroduce cmake options (#3024)
* Clean some warnings (#3009)
* Use windows-2022 as runner.os (#3012)

## Documentation

* Document `allow-overflow` in migration guide (#3008)
* Document guidelines for the study format (#2989)
* [Doc] Fix starting index for year and time-series (#2978)

## Code quality

* Code comments on districts [ANT-3408] (#3096)
* Add `final` keyword for final classes (#3078)
* Add missing virtual Public destructor (#3073)
* Missing virtual destructor (#3034)
* Missing override on method (#3072)
* Avoid copies for `OptimisationsSimulationTable::buffers()` (#3077)
* Factorise constrainte and variable name in constant data (#3066)
* Field initialization order (#3071)
* Declare nested namespace in one line (#3056)
* Explicitly delete implicitly delete operator/constructor (#3033)
* Throw exception properly (#3016)
* Clean `Benchmarking::DurationCollector` (#3025)
* "mc-" + int does not append to string (#3015)
* [ANT-3680] Use vector instead of map in linear expressions (#2986)
* Remove duplication in "variable de sorties" (#2980)
* Remove duplication involving `std::transform` (#2993)
* Remove \n in ortools_tag (#3080)
* Remove dependency on chocolatey / fossies.org (#2984)
* Remove useless dependency to `Boost::unit_test_framework` library (#2968)
* private field 'pJobCount' is not used (#3032)

## Technical cleaning

* Remove unused methods `getOptimalValue` and `getOptimalValues` (#3101)
* Remove unused parameter (#3075)
* Remove MD5 (#3007)
* Remove hash (#3020)
* Remove dead code in file name manipulation (#3023)
* Remove dead code in yuni (#3018)
* Remove yuni variant (#3005)
* Split function, remove unused arguments for quadratic optimization (#3081)

## Tests

* Multi storage remix : more tests [ANT-3175] (#3067)
* Bump Antares_Simulator_Tests_NR / SimTest (#3059)
* Add disabled test case for SumNode conversion with more than two operands (#3045)
* Change None to NaN in cucumber step (#3091)

## For developers

* Update copyright 2025 (#3054)

---

## Summary by category

| Category | Count |
|----------|-------|
| New features | 8 |
| Removed features | 2 |
| Improvements | 18 |
| Bugfixes | 17 |
| Modeler | 7 |
| CI | 8 |
| Build | 5 |
| Documentation | 3 |
| Code quality | 19 |
| Technical cleaning | 8 |
| Tests | 4 |
| For developers | 1 |
| **Total** | **100** |

## Key Highlights

### Performance Improvements
- Improved CSV loading performance using boost::iostreams::mapped_file_source
- Optimized linear expressions using vectors instead of maps
- Reduced number of copies for Xpansion interface
- Simplified hydro remix algorithms

### Modeler Enhancements
- Multiple new modeler versions (5.1, 5.3, 5.4) with enhanced features
- Added extra outputs to simulation tables
- Improved evaluation of comparisons
- Better error handling and logging

### Code Quality & Maintenance
- Removed deprecated Yuni components (variant, hash, MD5)
- Added `final` keyword for final classes
- Fixed multiple memory-related issues and warnings
- Improved exception handling

### CI/CD Updates
- Updated multiple GitHub Actions to latest versions
- Improved build infrastructure with gcc-11 support
- Enhanced PR size bot for better code review

---
*Generated from git log between v9.3.0 and develop head*
*Date: 2025*
