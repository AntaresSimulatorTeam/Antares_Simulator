---
toc_depth: 2
---

# Antares Changelog

### 9.3.12

#### Bugfixes

* Fix writing of simulation output files larger than 4 GB
* Fix reservoir level adjustment when hydro initialization does not start in January
* Fix units in districts aggregation

#### Dependencies

* Update vcpkg to 2025.06.13: some dependency assets were not available anymore. Versions details in [dependency updates](Dependencies/updates.md)

#### CI/CD

* Automatically cache vcpkg assets on release
* Use an asset cache mirror (`X_VCPKG_ASSET_SOURCES`) in build workflows and docker image

### 9.3.11

#### Bugfixes

* Fix memory leak in thermal (#3619)
* Reduce memory usage of minmax values (#3626)
* Properly populate digest with dynamic group variables and districts (#3643)
* Fix value of inter-monthly-breakdown not being read
* Fix remix hydro sts levels computation [ANT-4888] (#3557)

### 9.3.10

#### Bugfixes

* Fix bounds on fictitious loads and max unsupplied in adequacy patch [ANT-4906] (#3575)


### 9.3.9

#### Features

* feat: Write longer durations in execution_info.ini (#3476)

#### Bugfixes

* Fix: remove dynamique variables from digest, fix errors in digest (#3535)
* Districts: fix offest with disabled set | simplify [ANT-4830] (#3533)
* chore: update gitignore
* Fix unit tests binding constraints (#3453)
* Fix: windows runtime libraries (#3435)
* Fix: several issues on CentOS7 (#3561)

#### CI/CD

* feat: Speed up SonarCloud job, increase cache hit rate to 100% (#3518)
* chore(deps): Up build.os@readthedocs.yml (#3531)
* chore(deps): bump docker/build-push-action from 6 to 7 (#3472)
* chore(deps): bump docker/login-action from 3 to 4 (#3473)
* ci: Bump actions/upload-artifact from 6 to 7 (#3447)

#### For developers

* Add tools/antares-version.py (#3534)
* feat: allow generating results on release (#3427)

### 9.3.8

#### Features

* Add sts prod to fictitious load [ANT-3739] (#3486)
* Add constraint fictitious load to adq patch [ANT-4365] (#3487)

#### Bugfixes

* Fix truncated digest [ANT-4630] (#3455)
* Fix dynamic district aggregation (#3429)

### 9.3.7

#### Feature

* Problem Generator: new tool to write problems without running the simulation

#### Improvements

* Add a warning in the problem-generator when weeks are dependent, and cleanup (#3398)

#### Bugfixes

* Improve performance for sum operations in expressions
* Fix: remove unnecessary global time step adjustment to avoid wrong time labels (#3377)

### 9.3.6

#### New features

* feat: resolution mode [ANT-4380] (#3339)
* Dynamic aggregation [ANT-2739] (#3342)
* Investment with integer variables for hybrid studies [ANT-4046] (#3372)
* [100% optional] Use customized mps generator in the simulator (#3346)
* [100%] print mps in api mode (#3314)
* Modeler: operators ceil and floor [ANT-4295] (#3316)
* feat(api): check week independency (#3387)

#### Improvements

* Remove usage of raw pointers around MPSolver, use std::shared_ptr (#3348)
* Refactor AverageData year management (#3332)
* Simplify, fix and slightly extend output variable classes (#3337)
* Use a custom MPS writer in antares-modeler [ANT-4324] (#3302)
* Change PMax computation for STS in remix hydro (#3363)
* Remove ts-generator binary (#3334)
* Shorten header/copyright notice (#3353)
* Residual consumption [ANT-2444] (#3330)

#### Bugfixes

* fix: hourly PMax not clamped up to PMax Hydro Écrêtée (#3361)
* fix: Offset overwritten instead of accumulated [ANT-4467] (#3385)
* Fix naming for daily binding constraints (#3362)
* Fix missing elements from digest.txt [ANT-4446] (#3388)
* fix test "single_problem_thermal_first_week_nominal_case" (#3380)
* fix: API save study meta data (#3356)
* APIv2 - Fix wrong size for TS numbers (#3391)
* Fix TODO & add tests in hydro allocation (#3343)
* test: Handle new study directory "invalid-studies" (#3246)

#### Tests

* Add tests on `SurveyResults` (#3338)
* constraints builder tests: adding tests + improvements (#3305)
* Add unit tests for `IntermediateValues::computeStatisticsForTheCurrentYear` (#3344)
* Add tests on time aggregation + formatting to a survey report (#3345)

#### Documentation

* Add documentation on missing parameters (#3347)
* Add doc for execution_info.ini durations (#3359)
* Add doc for resolution mode (#3371)
* Add AGENTS.md for coding agents (#3368)

#### CI/Chore

* update simtest version (#3360)
* chore(simtest): update version to v9.3.6b (#3386)
* [oracle 8] update python version to get ortools (#3379)

### 9.3.5

#### Improvements

* New option Adequacy patch debug files [ANT-4034] (#3297)
* Add time infos to API [ANT-4261] (#3315)
* Add more details for steps durations [ANT-4263] (#3319)
* APIv2 - Fix initial hydro levels (#3326)

#### CI

* Bump actions/cache from 4 to 5 (#3293)
* Bump actions/upload-artifact from 5 to 6 (#3294)

### 9.3.4

#### Improvements

* Various fixes for the "single problem API" (#3313, #3311, #3312, #3309)
* Bump SonarSource/sonarqube-scan-action from 6.0.0 to 7.0.0 (#3295)

### 9.3.3

#### New features

* Add support for Gurobi-specific solver parameters (#3264)

#### Improvements

* Add error handling to antlr parsing [ANT-4032] (#3254)
* Small improvements for slack analysis (#3255)
* setObjectiveOffset API [ANT-4168] (#3251)

#### Bugfixes

* Additional constraints : bad unexpected input management [ANT-4297]
* Add week number to additional constraint name (#3300)

#### Modeler

* Modeler 6.5: valid location [ANT-4045] (#3258)
* Consistency in Node Composition for Modeler (#3242)
* Modeler scenarized constant parameter [ANT-3800] (#3275)
* Time dependent objective is not supported (#3257)

#### CI

* Fix: sonar hit ccache properly (#3196)

#### Build

* Add a few missing install directives (#3266)

#### Doc

* Fix on readthedoc rendering (#3245)
* FunctionNode doc (#3270)

#### Code quality

* refactor: TimeIndex -> TimeScenarioVariability (#3261)
* Expose studyPath instead of study (#3267)
* Add SingleProblemGetter::getProblemIds() for problem IDs (#3268)
* Single problem getter - Truncate variable & constraint vectors if needed (#3272)
* fix: build std::type\_index is not constexpr (#3277)
* Single problem getter - Week indexing starts at 1 (#3271)
* Single problem API (#3280) (#3279) (#3286)
* Fix Mdeb's trailing zeroes [ANT-2334] (#3303)

#### Technical cleaning

Modeler : restore models as const [ANT-4051] (#3273)
Remove useless if/else for links loop-flow (#3262)
Remove unused LpsFromAntares data members (#3289)

#### Tests

Storage remix tests : trial for improvement [ANT-4022] (#3244)
Expressions : tests sum operator (#3260)
collectRemixDebugInfo (#3278)

### 9.3.2

#### Important notice

* Changing the way some warning or errors are handled during loading
    - Warnings that were previouly deemed fatal are now proper errors
    - It is still possible to bypass those specific errors with the command line option `--force`
    - Some of those warnings are kept as warning and now non-blocking. Mainly those related deprecated options or values

#### New features

* Modeler 5.5: Add dual and reduced\_cost operators [ANT-3621] (#3177)
* Modeler 5.6, 5.7: max, min and pow operators [ANT-3642] (#3212)
* Modeler 6.2: Variables in investment problems [ANT-3885] [ANT-3913] (#3166)
* Modeler 6.3: Constraints in investment problems [ANT-3892] (#3220)
* Add debug files to hydro remix [ANT-4028] (#3206)

#### Improvements

* Ascending compatibility [ANT-3989](#3161)
* Add a deprecated message to the GUI [ANT-4042] (#3238)(#3240)
* Fix: Reduce log spam and improve information (#3232)
* Interprétation des valeurs d'usage [ANT-3894] (#3132)
* Add thread number in logs [ANT-2174] (#3138)

#### Bugfixes

* Infaisabilité avec l'hydro avec min gen et sans reservoir management [ANT-3748] (#3181)
* Fix segfault caused by expression[t] [ANT-4049] (#3201)
* Fix LolpCsr floor, LoldCsr spatial aggregate value (#3169)(#3168)(#3151)

#### Modeler

* Add ExportBehavior for structure files (#3210)
* 6.1: Several objectives [ANT-3887] (#3090)
* 6.2 Load E2E [ANT-3885] (#3185)
* 6.2a parse optim config (#3159)
* 6.2a : Data structures for optimization configuration [ANT-3885] (#3092)
* No simulation table when no-output is given [ANT-3883] (#3137)
* Improve study model (#3093)

#### CI

* Cache improvements (#3128)(#3154)(#3179)(#3219)(#3140)
* Fix continuous delivery title (#3183)
* Use boolean type instead of string type for "Run all tests" (#3175)
* Update CD title and tag (#3163)

#### Build

* testing cmake unity build (#3191)
* On Windows : avoiding a compilation collision between antlr4 and Yuni (#3178)
* Remove compile option unsuported on Clang (#3068)
* Remove study dependency in exception (#3164)

#### Doc

* Doc on expressions : move and complete expressions [ANT-4117] (#3217)
* Update doc for objective contributions (#3158)
* Update XPRESS doc regarding MILP problems (#3160)

#### Code quality

* Factorize lolp and lold traits (#3145)
* Deduplicate code in lolp vcards (#3144)(#3143)
* Simplify chained templates for variables (#3142)(#3131)(#3129)(#3127)
* Getter for single problem [ANT-2334] (#3165)
* Move regenerateTimeSeries to separate header/source (#3182)
* Refactor enums project utilitaries (#3186)
* Simplify condition in FinalLevelValidator::skippingFinalLevelUse (#3170)
* Extract random number handling from templated class ISimulation (technical preriquisite) [ANT-2334] (#3156)
* Move yearRandomNumbers to random.h/random.cpp (#3162)
* Fix memory leak in class SpatialAggregate (#3139)
* Fixes after review for #3093 (#3124)
* Fix a few code smells following #3093 (#3123)

#### Technical cleaning

* Remove unused dummy\_data (#3221)
* Remove unused OptimComponent::index (#3218)
* Wrap getDisplayName and getIcon inside BUILD\_UI (#3216)
* Clean stuff in and around optimEntityContainer (#3209)
* Remove #ifdef \_\_cplusplus, \_\_CPLUSPLUS, unused #define (#3171)
* Remove std::enable\_shared\_from\_this from ThermalCluster (#3167)
* Remove most ORTOOLS_* functions, use std::unique\_ptr to handle MPSolver instance (#3155)
* Remove unused local variable (#3141)
* Remove unused "Hashable" class (#3115)

#### Tests

* Add 2 hydro remix tests [ANT-4022] (#3213)
* handle invalid-studies (#3250)
* update expectations 015 Hydro power (#3249)
* Add behave tests for 6.2 [ANT-3885] (#3237)
* Ignore basis\_status in simulation table comparison (#3233)(#3236)
* Fix values for NPCAP after switching to HIGHS (#3234)
* Add information when simulation time exceed expectations (#3228)
* Improve log when simulation tables don't match (#3230)
* Add unit testing on benders decomposition (structure.txt) (#3231)
* Tests for modeler 5.5 (#3198)
* NPCAP test cucumber [ANT-3760] (#3133)
* Input data for modeler test with investment (#3113)
* Publish log-capture util to test\_utils\_unit [ANT-3989] (#3173)
* Make parameters test in-memory using loadFromINI [ANT-3989] (#3172)
* Fix studiesToBenchmark.json (#3153)
* Restore "linear expression" tests (#3149)
* Restore "testScenarioGroupRepo" test (#3147)
* Fix test\_readLinearExpressionVisitor unit test, fix missing DivisionNode, refactor fixture (#3120)
* Add modeler test 10_4 for comparison operator (#3126)

### 9.3.1

#### New features

* Modeler: Implement eval and time index visitors for portfield nodes (#3110)
* Optional overflow for short-term storage objects [ANT-3695] (#3036)
* ANT-3617 - modeler 5.1 (#2933)
* Modeler 5.3: Add extra outputs to simulation table (#3053)
* Modeler 5.4: Evaluate comparisons [ANT-3620] (#3086)
* 5.2: Export ports in simulation table (#2982)
* [5.3 part 1/2] Add extra-outputs to models & ID unicity checks (#2988)
* Evaluation context provider from Component [ANT-3750] (#3043)

#### Removed features

* [ANT-3638] Remove H COST from OV COST (#3035)
* Remove special value "default" for scenario-group field (#2977)

#### Improvements

* Use boost::iostreams::mapped_file_source to improve CSV loading performance [ANT-3915] (#3089)
* Logs time used to build modeler problem [ANT-3759] (#3041)
* Display correct problem size in logs [ANT-3716] (#3026)
* Short term storage : simplify results data structure [ANT-3175] (#2991)
* Load only needed solvers [ANT-3543] (#2976)
* Improve behave's error message for simulation tables (#3061)
* Remix by shaving peaks : adapt for multi storage [ANT-3175] (#2975)
* Reduce number of copy for Xpansion interface (#3062)
* Simplify ShortTermStorageCumulation.cpp (#3010)
* Simplify logging handling in node visitor (#2996)
* Hydro remix : more simplifications (#2959)
* Various simplifications in modelConverter.cpp (#2981)
* Linear expression : some cleaning (#3042)
* Remove unused nodes and visitors (component substitutions) (#3037)
* Update XPRESS documentation (#2990)
* Warning if simplex-range = day [ANT-2695] (#3055)

#### Bugfixes

* Fix inconsistent LOLD for districts [ANT-3408] (#3117)
* Save hydro gen and pump in legacy GUI [ANT-3140] (#3084)
* ANT-3697 : fix contraint on weekly hydro generation amount [ANT-3697] (#3040)
* Catch modeler loading error [ANT-3837] (#3063)
* Modeler - fix various problems with time & scenarios [ANT-3712] (#2972)
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

#### Modeler

* Add debug logs for modeler (#2971)
* Add modeler doc precisions (#3000)
* 4.3 : Clearer error message when out of bound ts number (#2979)
* Flatten `SumNode` to avoid deep recursion (#3094)
* Refactor unique ID check (#2997)
* Replace binary minus with -= in ReadLinearConstraintVisitor (#2987)
* [ANT-3606] Uppercase for group scenarios (#2970)

#### CI

* Bump SonarSource/sonarqube-scan-action from 5.3.1 to 6.0.0 (#3083)
* Bump SonarSource/sonarqube-scan-action from 5.3.0 to 5.3.1 in /.github/workflows in the github_actions group across 1
  directory (#3047)
* Bump actions/checkout from 4 to 5 (#2973)
* Bump actions/setup-python from 5 to 6 (#3051)
* Bump install-build-wrapper@v5.3.1 to fix vulnerability in 5.3.0 (#3019)
* windows-vcpkg workflow - skip CI if commit message contains [skip ci] (#3046)
* Use gcc-11 for ubuntu & sonarcloud jobs (#3095)
* pull-request-size bot comments XXL PRs (#3079)

#### Build

* Disable warning in antlr (#3070)
* Ignore warning in tests (#3028)
* Reintroduce cmake options (#3024)
* Clean some warnings (#3009)
* Use windows-2022 as runner.os (#3012)

#### Doc

* Document `allow-overflow` in migration guide (#3008)
* Document guidelines for the study format (#2989)
* [Doc] Fix starting index for year and time-series (#2978)

#### Code quality

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

#### Technical cleaning

* Remove unused methods `getOptimalValue` and `getOptimalValues` (#3101)
* Remove unused parameter (#3075)
* Remove MD5 (#3007)
* Remove hash (#3020)
* Remove dead code in file name manipulation (#3023)
* Remove dead code in yuni (#3018)
* Remove yuni variant (#3005)
* Split function, remove unused arguments for quadratic optimization (#3081)

#### Tests

* Multi storage remix : more tests [ANT-3175] (#3067)
* Bump Antares_Simulator_Tests_NR / SimTest (#3059)
* Add disabled test case for SumNode conversion with more than two operands (#3045)
* Change None to NaN in cucumber step (#3091)

#### For developers

* Update copyright 2025 (#3054)

### 9.3.0

#### New features

* Scenarize short-term storage additional constraints [ANT-3037] (#2813)
* Scenarize inflows for short-term storage [ANT-3037] (#2772)
* Dynamic groups for thermal and renewable clusters [ANT-1672] (#2579)
* Near price cap hours variable [ANT-3038] (#2810)

#### Removed features

* Remove timeseries refresh [ANT-1335] (#2451)
* Forbid study path with non ascii chars [ANT-3635] (#2934)

#### Improvements

* Remove year batches, launch jobs continually [ANT-2139] (#2718)
* Random TS numbers for STS inflows & additional constraints [ANT-3415] (#2882)
* or-tools : update to v9.13 [ANT-3348] (#2857)
* Add STS additional constraints to infeasible problem analysis [ANT-3386] (#2869)
* Add Gurobi to list of queried solvers [ANT-3381] (#2865)
* Force the export for criterion files in Expansion mode [ANT-3137] (#2821)
* [ANT-2140] MustRun clusters taken into account in BC (#2690)
* Add fixed variables support to OR-Tools quadratic wrapper (#2789)
* New cmd line options for solver (#2765)
* Add checks for duplicates on study objects [ANT-2127] (#2733)
* Allow empty series files for STS [ANT-3053] (#2731)
* Fix final level computation in monthly hydro heuristic (#2943)
* Move STS additional constraint data folder [ANT-3666] (#2944)

#### Bugfixes

* Infeasibility caused by hydro (attempt 2) [ANT-3122] (#2874)
* Re-implement MPObjective::Value (#2892)
* Bypass JIT only for STS [ANT-3425] (#2887)
* Hydro monthly heuristic : add variable overflow to problem [ANT-3236] (#2837)
* Fix missing ortools dll in windows zip asset (#2840)
* [ANT-3264] Default author is username (#2817)
* Ceiling error with xpress and accurate [ANT-3167] (#2785)
* Fix fixed variable value in quadratic optimization (#2790)
* Monthly turbine error [ANT-3095] (#2759)
* [ANT-3122] Part 1/2 : Set primal tolerance to 1e-6 in ortools_utils (#2776)
* Check constraints width if the sizes are only ones (#2771)
* Change control for sts injection/withdrawal efficiency [ANT-3100] (#2749)
* Fix unfeasible problems with <0 hydro infows [ANT-3043] (#2732)
* Fix undefined behavior in BCs [ANT-2994] (#2708)
* Fix segfault when exporting binding constraints (#2954)
* Clear basis at start of each MC year to avoid sequential/parallel discrepancies (#2941)

#### Modeler

* 2.9: TimeIndex and TimeShift operators [ANT-2874] (#2678)
* 2.9: TimeSum and AllTimeSum operators [ANT-2932][ANT-2933] (#2703)
* 3.3: Hydrid studies without connections [ANT-2898] (#2699)
* 3.5: Hybrid area connections from yaml [ANT-2901] (#2787)
* 3.6: Connect components to areas in linear optimization problem [ANT-2902] (#2792)
* 4.3 Scenario [ANT-2034][ANT-3603] (#2824)(#2863)
* 4.7: Import connections and ports [ANT-2875] (#2662)
* 4.8: Establishing Component Connections [ANT-2876] (#2724)
* Connections : build expressions from connections and ports (#2739)
* Checking modeler variable bounds (#2844)
* [ANT-3359] Scenario-group is optional in component and default to "default" (#2855)
* [ANT-3092] Valeur par defaut des bornes des variables (#2814)
* Don't write modeler files if there's no solutions (#2760)
* Work on port rules (#2746)
* Use indices for getVariable & getConstraint, don't reconstruct name (#2729)
* Improve modeler API & main class (#2669)
* Performance optimizations in TimeDependentLinearExpression (#2962)

#### CI

* Use correct keys for ccache, update ccache for windows, always save ccache (#2884)(#2864)
* Remove end to end tests on TS generator binary (#2843)
* Package OR-Tools shared libs [ANT-3227] (#2803)
* Fix windows CI (#2788)
* [ANT-2997] use official actions (#2761)
* Add continuous-delivery for windows & simplify it (#2755)
* Fix Oracle continuous-delivery (#2758)
* Fix Sonarcloud jobs failing silently (#2710)
* Create a permanent release from develop (#2688)
* Remove usage of surge.sh (#2960)
* Update sonar scanner (#2951)
* Activate tests for 9.3 studies in CI (#2950)
* Generate & upload HTML report for unit tests (#2930)
* Badges: use status from branch develop instead of latest from any branch (#2939)

#### Build

* [ANT-2034] Various warnings or linter fixes (#2861)
* Fix compilation warning (implicit double -> float conversion) (#2862)
* Remove some warnings (#2774)
* Fix dependency issue for clients (#2757)
* Bump vcpkg to 2025.02.14 (#2741)
* Remove compilation warnings for GCC (#2740)
* Add helper CMake targets to generate C++ headers & source from ANTLR grammar files [ANT-3429] (#2924)
* CMake presets (#2928)
* Remove warnings on gcc (#2927)
* Factorise antlr4 lib linking resolution (#2929)

#### Doc

* Migration guide for dynamic cluster groups and sts inflows (#2913)(#2915)(#2916)(#2918)
* Document STS additional constraints @ v9.2.0 (#2847)(#2867)
* Add doc for scenariobuilder.dat, add short-term storage feature (#2808)
* Add documentation for hybrid area connections (#2754)
* FIx typos in doc (#2707)
* Add doc for behave.ini (#2748)
* Add doc for overflow spilled cost difference (#2713)
* Add documentation on the ANTLR4 code generation process (#2745)
* Fix broken link in mkdocs.yml (#2743)
* updating output changelog (#2689)
* correct python command to install dependencies (#2695)
* Improve doc for STS additional constraints (#2940)
* Add documentation for include-export-solutions for migration guide (#2937)

#### Code quality

* Use range-based for loops (#2897)
* Save sts inflows in legacy GUI [ANT-3425] (#2886)
* Use std::vector<double> instead of double\*, add const (#2878)
* Simplify linear optimization code (#2809)
* Fix prefix for short-term storage in scenariobuilder parser (#2811)
* Rename lp-solver -> linear-solver, move some options in the "Optimization" section in the help page (#2781)
* Fix double free and leak (#2767)
* Fix memory leak in hydro (#2736)
* Fix leak using matrix_to_buffer_dumper_factory::get_dumper (#2737)
* Use std::identity from header <functional> (#2727)
* Use std::call_once to avoid race conditions in ThermalCluster::getCostProvider (#2725)
* Small refactor around PROBLEME_SIMPLEXE_NOMME -> MPSolver conversion (#2723)
* Fix memory leak, double free (#2700)
* Use MPSolver* instead of void\*, remove static_casts (#2704)
* Pass strings by const reference when reading scenariobuilder.dat (#2683)
* Remove dynamic allocations, use local variables instead (#2694)
* Hydro remix algo : simplification (#2922)(#2956)
* Remove a few unused headers (#2935)
* Performance for problem building : avoid maps copy (#2926)

#### Technical cleaning

* Remove yuni limit on max threads (#2832)
* [ANT-3409] Store short term time series number (#2881)
* Remove unused members from spatial-aggregate.h (#2848)
* Remove SparseVector, remove data()[position] (#2816)
* Remove verbose log (#2756)
* Remove macro ndebug (#2720)
* Remove unused Parameters member data (#2712)

#### Tests

* [In-memory studies] Add & use helpers for short-term storage (#2910)
* Remove useless std::shared_ptr around SimulationHandler (#2911)
* Improve helper class ThermalClusterConfig (#2909)
* [ANT-2034] InMemory modeler test (#2827)
* New modeler : make test 7.1 pass (#2786)
* Add modeler tests 7.2, 7.2, 7.4 (#2800)
* Add modeler features 8.1, 8.2 and 8.3 [ANT-3535] (#2902)
* Add cucumber tests for v9.3.0 STS scenarization features [ANT-3037] (#2873)
* Unit test synthesis aggregation (EXP/MIN/MAX/STD) (#2828)
* Add some unit tests for hydro (#2831)
* Fix behave comparison tolerance (#2826)
* Add behave short tests 9 to 20 (#2807)
* Add a few unit tests for "math utils" functions (#2795)
* [behave] Update solver steps (#2753)
* Migrate some tests to cucumber (#2682)
* Add unit tests for BindingConstraintGroup::numberOfTimeseries (#2711)
* Fix ortools unit tests (#2702)
* Fix non-passing tests (#2696)
* Minimal system under test [ANT-2034] (#2830)

#### For developers

* Add 2 future parameters in generaldata.ini [ANT-3504] (#2907)
* Use antlr to process st additional constraints [ANT-3091] (#2777)
* Use OR-Tools/MathOpt for quadratic problem resolution [ANT-2546] (#2574)
* Don't rebuild optimization problem at every simplex resolution (#2722)
* Enable extra linear solvers: PDLP & HiGHS (#2693)
* Rebuild MPSolver between solves & transmit basis when using sirius [ANT-1287] (#2868)
* fix infeasible problem analyzer, remove "safe resolution mode" [ANT-3507](#2898)

## Branch 9.2.x

### 9.2.3

#### Bugfixes

* Fix segfault when exporting binding constraints results (#2954)

### 9.2.2

#### New features

> [!CAUTION] Breaking change

* [ANT-3666] Move STS additional constraint data folder (#2945)

### 9.2.1

#### New features

* Add new near price cap hours variable (NPCAP HOURS) [ANT-3038] (#2815)

#### Modeler features/improvements

* 3.3: Hydrid studies without connections [ANT-2898] (#2699)
* Do not create modeler solution files if study has no modeler components (#2849)

#### Numerical issues fixes

* Set primal tolerance to 1e-6 in ortools_utils [ANT-3122] (#2850)
* Fix unfeasible problems with <0 hydro infows [ANT-3043] (#2732)
* Ceiling error with xpress and accurate [ANT-3167] (#2785)
* Hydro monthly heuristic : add a new overflow optimization variable for v9.2.x [ANT-3236] (#2845)

#### Short-term storage fixes/improvements

* Use ANTLR4 to handle STS additional constraints, allowing the parsing of larger "hours" field for additional
  constraints [ANT-3091] (#2777)
* Fix check for STS injection/withdrawal efficiency [ANT-3100] (#2749)
* Allow empty series files for STS [ANT-3053] (#2731)

#### Other bugfix

* The use of OR-Tools v9.13-rte1.0 allows fixing multiple minor bugs with XPRESS [ANT-3348]
    - non-reproducibility
    - unsafe "locale" modification
    - performance issues
* Fix memory leak in hydro (#2736)
* Don't rebuild optimization problem at every simplex resolution (performance issues with XPRESS) [ANT-3044] (#2722)
* Fix missing DLL / .so by packaging OR-Tools shared libs [ANT-3227] (#2803)
* Remove limit on max threads when requested threads == nproc (#2834)
* Use `std::call_once` to avoid race conditions in `ThermalCluster::getCostProvider` (#2725)

#### Other improvements

* Use OR-Tools/MathOpt for quadratic problem resolution [ANT-2546] (#2574)
* Force the export for criterion files in Expansion mode [ANT-3137] (#2823)
* Check for duplicates on study objects (thermal, renewable clusters, short-term storages) [ANT-2127] (#2733)

### 9.2.0

#### New features

* Short term storage: withdrawal efficiency [ANT-1862] (#2223)
* Short term storage: penalty on level, injection, withdrawal [ANT-1854] (#2302)
* Short term storage: additional constraints on injection, withdrawal, netting [ANT-1855] (#2550, #2546)
* Short term storage: penalty for storage control, injection and withdrawal flow gradient [ANT-2300] (#2491)
* Make it possible to specify the final hydro reservoir level [ANT-1084] (#1521)
* Major changes on the hydro behavior. Use overflow from the weekly optimization problem, don't recompute levels as a
  post-processing. These changes improve the handling of min-gen constraints for hydro reservoirs. [ANT-1825]

#### Removed features

* Remove hydro hotstart (#2131)
* Remove adequacy patch lmr [ANT-1933] (#2341)
* Possibility to disable OR-Tools. All problems are now solved through OR-Tools (#2450)

#### Improvements

* Changed the formula for the number of cores [details](../user-guide/solver/optional-features/multi-threading.md)
* Expose API [ANT-1158] (#1993)
* Adequacy patch CSR - revamp output variables [ANT-1932] (#2306)
* batchrun forwards options to antares-solver [ANT-2314] (#2463)
* Infeasibility analyzer [ANT-1825] (#2232) (#2227)
* Collect hydro validation errors (#2204)
* Perform hydro checks prior to the simulation [ANT-1720] (#2132)
* New output variable : partial min gen for thermal clusters [ANT-2425] (#2608)
* Improve logging & loading of STS elements introduced in v9.2.0 [ANT-2730] (#2648)
* Accurate remix hydro [ANT-1825] (#2599)
* Increase precision for LOLD[CSR] and MRG PRICE[CSR] [ANT-2443] (#2618)
* Add variable MRG PRICE CSR [ANT-2562] (#2559)
* Add optional additionnal constraints to short-term storage objects [ANT-1855] (#2546)
* Adding constraints on injection-witdrawal cumulation with constant RHS [ANT-1885] (#2535)
* Remove direct use of Sirius solver (ANT-1152) (#2450)
* Allow the passing of optimization options through the API (#2502)
* Place CSR after hydro remix [ANT-2070] (#2438)
* Remove computation for number of non-zero terms in the constraint matrix [ANT-2258] (#2496)

#### Bugfixes

* Adequacy Patch regression [ANT-1845] (#2235)
* BC marginal cost : remove "return"s that break the static chain (#2121)
* Fix condition for disabling "store in input" (#2180)
* Fix/rhs hydro power constraint (#2034)
* Fix Windows Debug version crash (#2322)
* Reset adequacy patch enabled (#2420)
* Fix reset order (#2304)
* Restore correct behavior when options -h/--list-solvers are provided (#2138)
* Fix missing synthesis results for links (#2115)
* Check scenario year for Binding constraints [ANT-2890] (#2674)
* [GUI] Fix binding constraint TS & weights loading [ANT-2746] [ANT-2754] (#2636)
* Fix the upgrade from v8.8 to 9.2 of hydro power matrices [ANT-2742] (#2625)
* Fix segfault on BP studies (#2598)
* Fix milp bug (#2489)
* GUI bug: wrong indexes for thermal clusters in scenario builder [ANT-2580] (#2576)
* Output results : fix annual aggregation (#2580)
* Fix segfault caused by invalid index [ANT-2582] (#2543)
* Rebase the 8.8.x branch on top of develop to take multiple fixes into account (#2512)
* Default solver in GUI (#2524)
* Fix launch studies with empty solver (#2523)
* Fix cost for ST storage (#2522)
* Fix segfault when there are more districts than areas [ANT-2452] (#2516)
* Fix variable bounds for 1st week in the year (#2517)
* Fix timestamp = 0 in file info.antares-output (ANT-2494) (#2508)
* Fix build after the removal of bool OptimizationOptions::ortoolsUsed (#2505)

#### Modeler

* 1.1: Modeler API [ANT-1876] (#2286) (#2391)
* 1.1c: Scenarize problem filler (#2445)
* 2.1: Lib for modeling objects (#2383)
* 2.2: System model [ANT-2208] (#2500)
* 2.4: Expression visitors : first implementation (#2290)
* 2.4a: Replace AddNode with SumNode (#2396)
* 2.4c: portfield substitution (#2406)
* 2.4c: PortFieldSum and substitution [ANT-2005] (#2415)
* 2.4e: visualize ast with graphviz [ANT-2036] (#2399) (#2426) (#2429)
* 2.5a: Read models and system yaml files [ANT-2306] (#2540) (#2539)
* 2.5b: build linear problem from components (#2558)
* 2.7: Introducing time dependency behaviour (#2572)
* 4.1: Data (#2577)
* 4.4: expression parsing [ANT-2313] (#2471)
* 4.5 Parse yaml [ANT-2206] (#2433) (#2431) (#Output results : fix annual aggregation (#2580)2447)
* 4.5 Full exemple of parsing (#2448)
* 4.6: system import [ANT-2207] (#2530)
* Add SumNode "wide" test (#2403)
* Add iterators on ASTs, allowing for loops (#2387)
* Use variable dict (#2670)(#2655)
* Support time dependency [ANT-2749] (#2622)
* Import modeler data-series [ANT-2033](#2621)
* Simplify ComponentFiller::addVariables (#2615)
* add of unit tests for time dependant expression [ANT-2608] (#2597)
* Refactor modeler code (#2616)
* Move libObjectModel code (#2498)
* Documentation (#2624)(#2473)
* Tests (#2626)(#2617)

#### CI

* SonarCloud job, improvements, bugfixes (#2315) (#2281) (#2246)
* Run all tests even if one of them fails (#2265)
* install gh from rpm (#2216)
* Centos mirror list (#2372)
* Add tests in CI for version 9.2 (#2241)
* Bump C++11 to C++17

### 8.3.3 (12/2022)
--------------------

#### Bugfix

- Round renewable production (#985)

### 8.3.2 (09/2022)
--------------------

#### Output aggregation change

- Daily, weekly, monthtly & annual values for link variable "MARG. COST" (EUR/MWh) was previously obtained by a sum. It
  now computed by average (#881).

#### New features

- Add new BC marginal price output variable (#801)
- Add Antares logo in solver logs (#861)
- Add XPRESS-compatible assets for Ubuntu. Windows coming soon.

#### GUI

- In the "Links" panel, replace "Flat" view by "By area" view (#755)

#### Bug fixes

- Fix segfault when more than 9 renewable clusters are present in an area (#869)
- Fix segfault related to the digest occurring when many thermal clusters are present (#852)
- Statistics: use std::mutex / std::atomic<> to prevent concurrent writes (#838)
- Allow more than 100 thermal clusters (#886)
- Fix warning in hydro.ini loading function (#883)
- MPS for the 1st optimization were erased by the one related to the 2nd optimization. Fixed (#863).

#### For developers

- Simplify day ahead reserve condition (#777)
- Remove unused "shedding strategy" (#788)
- Refactor Layers by adding a LayerData class (#866)
- Output variables: remove useless methods for renewable clusters (#869)
- Restore notifications from Sonar (#828)
- Restore tests in the CI (Windows only) (#850)
- Add OR-Tools+XPRESS to Ubuntu CI and merge files for centos CI (#859)
- Simplify Parameters::fixRefreshIntervals, clean headers (#832)
- Simplify tuneSolverSpecificOptions (#829)

#### Documentation

- Input and output format due to addition of BC marginal prices (#836)
- Fix e-mail address and website (#834)

### 8.3.1 (08/2022)
--------------------

#### New features

- Add execution-info.ini output file, containing execution durations and study info #740 #803 #816
- OR-Tools: set solver-specific options for XPRESS #796

#### Bug fixes

- Fix missing renewable columns in districts (sets of areas) #802

#### GUI

- Fix wrong number of cores in the "Run a simulation" window #793

#### For developers

- Bump C++11 to C++17

### 8.3.0 (07/2022)
--------------------

#### New features

- Adequacy patch - share the unsupplied energy according to the "local matching rule". This feature was contributed by
  RTE-i with support from RTE, ELIA and APG #657
- Add output variable "profit by cluster". This variable represents the difference between proportional costs and
  marginal costs in the area. It provides a partial answer to the question "what is the economic profit associated to a
  thermal cluster ?", excluding non-proportional (€/h) and startup costs (€/startup). #686
- Allow +/- infinity in binding constraint RHS, allowing the user to enable BCs only for some timesteps #631(*)
- Add option to enable the splitting of exported MPS files. This feature is intended to be used by Antares Xpansion.
- Add --list-solvers command-line argument, to list linear solvers available through OR-Tools
- Measure the execution duration for every computation step, store the durations in a dedicated file #546

(*) May not work with the Sirius solver. Consider using other solvers through OR-Tools

#### Bug fixes

- Fix segfault occuring when inter/intramodal correlation is enabled and TS width are inconsistent #694
- Fix logging of performed MC years when running jobs in parallel #680
- Fix a crash occuring in studies where an area contains 100+ thermal clusters (#753)
- Load & check prepro data only if needed (#754)
- Fix possible loss of data if adq-patch is disabled (#738)
- UI For property update for adq-patch mode, allow batch edit (#747)
- UI - Fix GUI freeze when using multiple map layers (#721)

#### For developers

- Display the git commit-id in the logs for debugging & diagnosis purposes #698
- Code cleaning in hydro heuristic #671
- Use antares-deps 2.0.2, which now excludes OR-Tools #684
- Fetch OR-Tools directly, allowing more flexibility on the flavor (XPRESS/Sirius/etc.) #684
- Code cleaning #663 #665 #666 #687 #725 #667 #668 #730

#### Misc. improvements

- Generate 2 assets in CentOS 7 : one that includes XPRESS, one that does not #689
- Upgrade examples 8.1 -> 8.3 (#733)

### 8.2.3 (11/2022)
--------------------

#### Bug fixes

- Round renewable production (#985)

### 8.2.2 (04/2022)
--------------------

#### Bug fixes

- Fix solver crash on parsing command-line parameters #624

#### GUI

- Fix crash occuring when switching to the links panel #658

### 8.2.1 (03/2022)
--------------------

#### Bug fixes

- Fix scenario builder data loss when renaming area #610
- Write 1 in the ts-numbers when series.width == 1 #609
- Add noise to the cost vector in the allocation problems to enforce uniqueness #622
- Linux only : fix segfault occurring when an INI file does not exist #606

#### GUI

- Place "Dataset > Resize columns to..." in first position #607
- Allow that all NTC be 0 in one direction #595
- Fix occasional crash when opening the links panel #594
- Linux only : fix frozen window occurring after a "Save as..." #586
- Linux only : fix impossibility to save a study #600
- Developer version only : fix assertion errors in the GUI #584

### 8.2.0 (03/2022)
--------------------

#### New features

- Multiple timeseries for link capacities (NTC). It is now possible to establish different scenarios for the capacity of
  a link. Users can now take partial or total outages for links into account #520
- Infeasible problem analyzer. When the underlying problem has no solution, list the most suspicious constraints in a
  report. This should help users identify faulty binding constraints #431
- Add a hydro-debug switch that allows the printing of some useful debug data in heuristic mode #254

#### GUI

- Add a "view results" button in the dialog that appears when a simulation has been completed #511
- Help menu : add an "online documentation" item #509
- Improve UI for new thermal parameter "tsGenBehavior" #534
- Improve cell styles when loop-flow is enabled for a link #571

#### Bug fixes

- Prevent an area from having a link to itself #531
- Fix crash when the study folder does not exist #521
- Fix crash when failing to load a study #502

#### For developers

- Remove calls to exit() #505. Provide consistent return values for antares-solver by fixing a segfault related to the
  log object #522
- Remove calls to setjmp, goto's big brother #527
- Large refactor of antares-solver's main function, hoping to make error management easier to understand #521
- Use std::shared_ptr instead of Yuni::SmartPtr in most cases #529
- Simplify and clean some parts of the code #537, #538, #540

### 8.1.1 (01/2022)
--------------------

#### Bug fixes

- Fix segfault occurring randomly when thermal clusters are disabled (#472)
- Fix hydro level discontinuities (#491). Very rarely, hydro reservoirs would inexplicably be filled from 0% to 100% in
  1h, this violating the modelling constraints.
- Execution times when the Sirius solver is used in conjunction with OR-Tools are now similar as with Sirius alone. This
  is a result of [this fix](https://github.com/AntaresSimulatorTeam/or-tools/pull/1), related to "hot-start".

#### GUI

- Speed up scrolling (#395)
- Warn the user about disabled renewable clusters only when relevant (#386)

#### Packages

Include antares-analyzer into .zip and .tar.gz archives (#470). This is especially useful if you use the portable
version of Antares.

#### For developers

In the CI workflow, CentOS 7 now uses devtoolset-9 (previously 7), with in particular gcc 9. This allows for C++17
features to be used without hassle.


### 8.1.0 (09/2021)
--------------------

#### New features

- Allow up to 9 RES groups (off-shore wind, on-shore wind, rooftop solar, PV solar, etc.) as opposed to wind and solar
  previously. This allows the user to distinguish between more renewable energy sources. When creating a new study,
  renewable generation modelling is set to "clusters" by default. This change does not affect opening an existing study.
  Note that TS generation is not available for these new RES groups.
- Add 3 thermal groups, named other, other 2, other 3 and other 4.

#### Bug fixes

- When a binding constraint is marked as skipped in the GUI, disable it in the solver #366

#### GUI

- Keep selection on thermal/renewable cluster when its group changes #360
- Dialogs "Thematic trimming" and "User playlist" are now resizable

#### For developers

- Add non-regression tests on each release
- Fix vcpkg on Github Actions
- Add build cache for Github Actions to speed up the build (Linux only)

### 8.0.3 (05/2021)
--------------------

#### Bug fixes

- Fix calculation of average for variable "congestion probability"
- Fix NODU when unit number is not an integer i.e has decimals
- GUI: allow decimal nominal capacity for thermal clusters
- GUI: Linux: use xdg-open to open pdf files instead of gnome-open

#### For developers

- Remove code related to licence management
- Remove openssl and libcurl dependencies
- Remove dead code

### 8.0.2 (04/2021)
--------------------

#### Bug fixes

- Fix GUI freeze when area color is changed but user don't validate new color
- Correction of MC year weight use for PSP and MISC NDG

### 8.0.1 (03/2021)
--------------------

#### Features

- Add "Continue Offline" button at startup if antares metric server is unreachable

#### Bug fixes

- Error with hydro start when using scenario playlist and stochastic TS refresh span
- Files needed for antares-xpansion not exported when using scenario playlist with first year disabled
- Correction of crash if user define a stochastic TS refresh span of 0 : minimum value is now 0
- Correction of MC years playlist weight write when sum of weight was equal to number oy years (no MC years playlist
  export in .ini)

#### For developers

- Add a GitHub action to check if branch name will launch CI
- Add shared dll in windows .zip archive

### 8.0.0 (03/2021)
--------------------

#### Features

- OR-Tools integration :
    - add command line option in antares-solver to define OR-Tools use and OR-Tools solver (option --use-ortools and
      --ortools-solver='solver')
    - add GUI option in run simulation to define antares-solver launch with OR-Tools option

- Add advanced hydro allocation feature. The default and existing behavior is to accomodate the guide curves, the new
  behavior is to maximize generation, even if it means that the reservoir level goes beyond the guide curves.

- Add indication on how to disable anonymous metrics

- antares-xpansion :
    - add option `include-exportstructure` in `generaldata.ini` to export .txt files needed for antares-xpansion

- Scenario builder and hydraulic level :<br/>
  Adding an hydraulic starting level tab in the scenario builder.<br/>
  For each MC year and area, a starting level can be defined, that is a 0-100 value.<br/>
  When the scenario builder is enabled, these levels get priority upon hot-start mode.

- Binding constraints (BC) and thermal clusters :<br/>
  If a must-run or disabled cluster is involved in a binding constraint :
    - the cluster is marked as "N/A" in the BC formula (GUI > Binding constraint > Summary)
    - the cluster is marked as must-run/disabled in the Weights or Offsets tabs.

  If a BC involves only zero weighted clusters/links or must-run/disabled clusters, the BC is :
    - marked with a red bullet in the Summary tab
    - marked as Skipped in the Weights and Offsets tabs

- MC Scenario Playlist :
  Add possibility to define a weight for each MC years in the synthetis results.<br/>
  See : GUI > Configure > MC scenario playlist.<br/>
  By default, a MC year's weight is 1, but can be set by user to more or less.<br/>
  After simulation, the MC year have a contribution to averages or standard deviations in synthesis results
  depending on the weight it was given.

#### Bug fixes

- Selecting an area and then, from the inspector, trying to select a thermal cluster or a link of this area in the
  dependencies
  section causes a crash. The inspector's cluster/link selection was removed.
- Scenario builder :
    - It makes no sense for the user to access the scenario builder Configure menu item whereas the Building mode
      parameter is set
      to Automatic or Derated. In the previous cases, the Configute menu item is disabled.
    - If a disabled thermal cluster is given a time series number in a non active rule of the scenario builder, a
      warning should not be
      triggered. If the disabled cluster is given a number for many MC years in the active rule, a single summary
      warning should be raised,
      not a warning per year.

#### For developers

- External dependencies :
    - use of new repository [antares-deps](https://github.com/AntaresSimulatorTeam/antares-deps) for external
      dependencies compilation

- Fix several compilation warnings
- Remove unused `COUT_TRANSPORT` constant
- Add code formatting with clang-format
- Remove PNE dead code

- docker image :
    - create of dockerfile for docker image creation

- continuous integration :
    - use docker images in CI
    - use of antares-deps release artifact in CI
    - push of docker image to dockerHub
      in [antaresrte/rte-antares repository](https://hub.docker.com/repository/docker/antaresrte/rte-antares)
    - add Centos7 support

- Unit tests :
    - Adding an end-to-end test in memory (see simple-study.cpp) :<br/>
      This test calls high level functions to build a simple study and runs it.<br/>
      It then checks if some elements of results match associated expected values.<br/>
      During this process, file system is not involved : everything takes place in RAM
    - Adding pytest scripts to check reference output values
    - Add pytest scripts related to unfeasible problems

### 7.2.0 (06/2020)
--------------------

#### Features

- Simulation dashboard: A new  "Geographic Trimming" option
  is now available in the "Configure" menu. This option makes
  it possible to filter the simulation's output content so as
  to include only results regarding Areas and Links of interest

- Optimization:  a new parameter "Unfeasible Problems Behavior"
  is available in the "advanced preferences" section of the
  "Configure" menu, with four possible values:
  (Error Dry, Error Verbose, Warning Dry, Warning Verbose)
  The first two options make the simulation stop right after
  encountering the first mathematically unfeasible problem, if any
  The last two options make the simulation skip all unfeasible
  problems, if any
  "Verbose" options print faulty problems in the “mps” format
  "Dry" options only report the time frame (MC year, week) for which
  an unfeasible problem was detected

- Compilation and cmake tree :
  Updates were made for more modern CMake use.
  Git submodules (extern dependencies : curl, openssl, wxwidget) are no more in use.
  These external dependencies can be retrieved :
    - either from a library manager : vcpkg for Windows, classic package
      repositories for Linux. With this way to proceed, an installation of external
      dependencies is required once for all.
    - or thanks to an automatic download : at Antares' cmake configure step,
      all needed downloads, compilation and installation are done.

- Unit tests :
  unit tests around class Matrix are now available.
  They can be compiled (on demand) during Antares' cmake build step
  and run either with ctest or in the classic way.
  Boost.Test is required and can be priorily retrieved and installed in the
  same way as the other external dependencies.

- Continuous integration : yaml files for github actions allow the run of
  all build chain and unit tests on several environment (Windows and Ubuntu).
  The 2 ways of getting external dependencies are also tested.

- Documentation: updated reference guide

- Usage metrics: added reference key for this version

#### Bug fixes

- GUI of the "Thematic trimming" option: Window size is naturally readjusted
  to improve readability by upgrading wxwidgets (3.1.3 and above).

- Auxiliary "Batchrun" tool: two options previously missing in the
  command line syntax have been introduced and now make it possible
  to launch a sequence of simulations to run in parallel

### 7.1.0 (12/2019)
--------------------

#### Features

- Simulation Dashboard: A new option "Thematic Trimming"
  is available in the "Output Profile" Section. This option
  now makes it possible to define precisely the content of
  output files so as to include variables of interest

- Optimization: a new parameter "Hydro Pricing mode" is
  available in the "advanced parameters" section, with two
  possible values (fast, accurate):
  In mode "fast", water value is, in the course of optimization,
  taken to be constant throughout the (daily or weekly)
  optimization period, and equal to that found for the exact
  day and level at which the optimization begins. Water values
  are reassessed afterwards, for each hour, on the basis of
  relevant time and level.
  In mode "accurate", the variations of water value along with
  the reservoir level are taken into account in the course of
  the (weekly) optimization. Reference (level-dependent) values
  are those attached to the end of the week. Water values
  are reassessed afterwards, for each hour, on the basis of
  relevant time and level.

- Documentation: updated reference guide

- Documentation: updated optimization problem formulation
  (modelling of hydro pricing options)

- Usage metrics: added reference key for this version

#### Bug fixes

- Output file "mc-all/grid/digest.txt": replaced "NaN" values
  by zeroes, where appropriate

- Output file "mc-all/grid/digest.txt": replaced "0" values
  by N/A, where appropriate (especially, hydro reservoir-related
  variables, when the "reservoir management" area attribute is set
  to "No")

- Output GUI: fixed a display bug regarding missing items in the
  "links" panel, in the case where simulation parameters are set
  so as not to produce synthetic results

- Links GUI: improved integrity control regarding hurdle costs.
  Negative values are allowed in either direct or indirect
  orientation, provided that the sum of both is non-negative

- General GUI: removed redundant items and renamed option menu
  "Geographic District" as "Regional District" to avoid confusion
  with new "Trimming" options

- Output: When simulation results are trimmed so as not to produce
  any data for given Areas or Links, avoid creation of empty folders
  named after said Areas or Links

### 7.0.1 (04/2019)
--------------------

#### Features

- Time-series analysis: in "detrended mode", extended perimeter
  to raw data including periods with no meaningful signal
  (e.g. solar production at night)
- Hydro-storage modelling: added ability to optimize pumping along
  with generation in mode "use heuristic target without leeway"

### 7.0.0 (12/2018)
--------------------

#### Features

- GPL release: updated companion files (README,...)
- GPL release: updated project Icons
- GPL release: insertion of license headers
- GPL release: translation of comments
- GPL release: removal of license control
- GPL release: code restructuring to separate Antares and Sirius
- Examples library: upgraded and added 16 new examples
- Documentation: updated reference guide
- Documentation: updated map editor guide
- Documentation: updated optimization problem formulation
- Documentation: updated examples library

### 7.0.0-rc (12/2018)
--------------------

#### Features

- Improved code for linux compilation with gcc 7

#### Bugs

- Fixed various issues in GUI
- Fixed RHS of constraints generated by the KCG when
  min and max values of PST settings are strictly equal
  and constraints are generated for the whole year

### 6.5.1 (11/2018)
----------------

#### Bugs

- Fixed index in hydro heuristic engine
- Hydro GUI: added scrollbars for correct display on laptops
- Output: improved presentation of results for incomplete calendar-based weeks
- Kirchhoff's constraint generator: fixed several GUI issues
- Districts GUI: improved syntax control

### 6.5.0 (11/2018)
----------------

#### Features

- Implementation of Kirchhoff's laws (DC approximation),
  modeling of phase-shifters and representation of passive
  loop flows (to account for on highly reduced gris): a
  dedicated Kirchhoff's constraints generator is now available
  It makes use of both classical input data (impedances)
  and new input data. Its results are specific binding
  constraints whose names begin by @UTO-, storable in the
  INPUT folder after user's validation ("save")

  New or modified input data for link L (8760 hourly values):
  Impedances (moved from col.3 to col.5)(Ohms at ref. voltage U)
  Loop flow (passive) (MW)
  Min Tap of phase-shifter  (MW*Ohms/U2 along any AC cycle including L)
  Max Tap of phase-shifter  (MW*Ohms/U2 along any AC cycle including L)
  New link parameters (one value)
  Asset type (AC,DC,Gas,Virtual,Other) : KCG deals only with AC links
  "account for loop flow" toggle
  "tune PST"        toggle
  KCG generating directives:
  Working map to use for generation
  Calendar to use for constraints activation (relaxation outside)
  Status of passive loop flow in constraints RHS (included or not)
  Status or PST settings in constraints RHS      (included or not)
  Auto-check of nodal loop flow balance (activated or not)
  Definition of the "infinite" to use for constraints relaxation
  KCG results:
  For AC Links involved in the generation process: The KCG sets the
  values of the two input data toggles related to loop flows and
  PST settings, in accordance with the current generation directives

        Identification of an optimal (minimum-weight) cycle basis for the
        formulation of constraints

        Generation of all relevant constraints (equality, inequalities, with
        or without relaxation)

- Reservoir-type hydro and other energy storage facilities:
  interface, input and output data structure, functionalities,
  have been completely redesigned. As a consequence, a number
  of new items (variables & parameters) are introduced in both
  input and output, while a few input variables are redefined
  or deprecated:

  Deprecated hydro variables:
  Pmax hydro "min", Pmax hydro "max"
  Redefined hydro variables and parameters:
  Hydro-storage time-series :    redefined at the daily scale
  Bounds for Reservoir levels:   redefined at the daily scale
  Res.level initialization date: redefined at the monthly scale
  New hydro variables and parameters:
  Input : max daily hydro generating energy
  max daily hydro pumping energy and power
  monthly-to-daily inflow breakdown pattern
  water value (time, level)
  modulation of max generating power (level)
  modulation of max pumping power    (level)
  pumping efficiency
  +many "storage management options" parameters
  Output: Reservoir level    (H.LEV)
  Water value        (H.VAL)
  Pumping power        (H.PUMP)
  Natural Inflow        (H.INFL)
  Forced Overflow        (H.OVFL)
  Cost of Gen+Pumping (H.COST)
  Optimization preferences:
  "Hot/Cold start" (year N may start or not at the final N-1 level)

- GUI: Districts may now be defined from within the interface
  (notepad tab connected to the Inspector's clipboard)

- Time-series generation (solar, wind, load) : increased speed
  when "high accuracy" option is selected, in the special case
  where all diffusion processes produce "Normal" variables

- Example library: upgraded to 6.5 (without extension)

#### Bug fixes

- Time-series generation: the storage (Input folder)
  of time-series generated for thermal clusters either in the
  "disabled" or "must-run" state did not work properly

- Time-series analysis: when short- and long-term levels
  defined for auto-correlation assessment are identical, the
  analyzer now performs a pure exponential fitting

- Time-series analysis: monthly time-series containing no
  non-zero value are no longer rejected by the analyzer

- Output: the link-variable "MARG.COST" was rounded to an integer
  value (changed to 2 decimal accuracy)

### 6.1.3 (06/2018)
----------------

#### Features

- Output: added a new file at the root of simulation results,
  displaying a short summary of the overall system economic
  performance throughout all Monte-Carlo years

- Log file: added new info messages on the size of optimization
  problems

- Updater (standalone): added new options and improved
  help messages

- Expansion mode: presolve stage replaced by hot start

#### Bug fixes

- Simulation: In the "accurate" Unit Commitment mode, the
  optimization preference "thermal Clusters Min Up/Down Time"
  can now be turned to "ignore"

- Simulation: removed remaining debug traces

- Simulation: zero-reset on interconnection marginal costs
  was sometimes missing in optimization final stage

- Example library : upgraded to 6.1 and extended

### 6.1.2 (11/2017)
----------------

#### Features

- Solver, Simplexe package: Improvement of the Scaling stage
  (Matrix, right hand side, costs)

### 6.1.1 (11/2017)
----------------

#### Features

- Solver: Light changes in Presolve stage

### 6.1.0 (09/2017)
----------------

#### Features

- GUI and simulation: "binding constraints" objects may now involve
  not only flows on interconnections but also power generated from
  thermal clusters. Alike flows, generation from thermal clusters may
  be handled either on an hourly, daily or weekly basis and may be
  associated with arbitrary offsets (time-lags expressed in hours).

### 6.0.6 (07/2017)
----------------

#### Features

- GUI: Binding constraint parameters tables (weights and offsets) are trimmed
  line-wise so as to fit exactly with the content of the selected working map

- Solver: strenghtening of the final admissibility check step in the "accurate"
  commitment mode

### 6.0.5 (07/2017)
----------------

#### Bug fixes

- Solver: Pruning of redundant messages in simulations launched from command line

- Solver: Removal of misprints in command line help messages

- Files:  Fixed issues (detected as of 6.0.1) regarding storage of thermal time-series files

- Study Cleaner: Unwarranted removal of the graphic multi-map lay-out could occur when
  cleaning datasets  (detected as of 6.0.0)

### 6.0.4 (06/2017)
----------------

#### Bug fixes

- GUI: The "variable per variable" view of the output files allows
  to display the power generated by each thermal cluster

- Simulation: Negative "ROW Balance" is properly included in
  unsupplied energy allowances

### 6.0.3 (06/2017)
----------------

#### Features

- GUI: The number of system maps that could be stored in a given study
  was limited to 19. This number is now unbounded.

#### Bug fixes

- GUI: The list of thermal clusters displayed for a given Area in the
  current map was sometimes wrongly initialized (Area considered
  selected though not explicitly clicked on yet)

- GUI: The order in which binding constraint terms are shown in the
  "summary" Window could depend on the execution platform used

- GUI: The Antares study icon could not be properly copied in some
  circumstances

### 6.0.2 (06/2017)
----------------

#### Features

- Optimization : To help discriminate between equivalent economic
  solutions, random noises on hydro hourly prices are more regularly
  spread out (absolute values) in the interval (5 e-4 ,1 e-3)Euros/MWh

#### Bug fixes

- Simulation : The identification of the Monte-Carlo year numbers
  in which the smallest/greatest values of random variables are
  reached could be ambiguous when identical results are found for
  two years ore more.

### 6.0.1 (05/2017)
----------------

#### Features

- Thermal Time-series generation: Data regarding all thermal clusters
  are generated and stored in the same way, regardless of their activity
  status (unabled/disabled). This makes easier to check data consistency

- Simulation: Upper bounds for spilled power and unsupplied power are
  actually set to their maximum theoretical value(i.e. if economic
  conditions make it justified: spill all power or shed all demand)
  So far, spillage of power that could be absorbed by the local demand
  was not allowed

- Simulation: a silent "Expansion" mode has been added to the regular
  modes "Economy/Adequacy/Draft". The three differences with the
  "Economy" mode are:
  a) In "accurate" unit commitment, integrity constraints are relaxed
  in the core optimization problem.
  b) Day-ahead reserve is no more subtracted from the initial demand
  to get back to "standard" conditions
  c) The values of all optimal criteria are printed in ad hoc files
  The use of this mode should be restricted to well-designed scripted
  automatic simulation sequences taking into account the simplifications
  listed above

### 6.0.0 (04/2017)
----------------

#### Features

- GUI: A new interface makes it possible to define several views (maps) of
  the Power System modelled in an Antares study. These maps are meant to give
  the user the ability to set different layouts in which each Antares Area
  or Link can be either shown or remain hidden. Accordingly, all input and
  output data windows can now adapt the information displayed so as to match
  exactly the content of any given map. Copy/Paste functions have been
  extended so as to work between different maps of different studies opened
  in multiple Antares sessions

- Simulation: Introduction of a flexible multi-threaded mode for the processing
  of heavy problems: Antares "Monte-Carlo years" can be be distributed on a
  number of CPU cores freely set by the user. This parameter appears as a new
  tunable item of the  "advanced parameters" list attached to any Antares Study.
  Five values are available in the [1, N] interval, N being the number of CPU
  cores of the machine (virtual or physical) Antares is run on

- License control through the internet: a new system has been developed for
  accommodating situations where users wish to operate Antares on a large
  fleet of machines among which a limited set of commercial license tokens
  can float freely

- Data organizer: Antares studies often include a great number of files of
  all sizes, which may take long to process when multiple copies are needed.
  Likewise, the management of the HDD space required for regular storage of
  all of the studies involved in a complex study workflow may turn out to be
  a demanding and heavy task. To save both time and hardware resources, the
  Antares Data Organizer, now provided as a companion tool to the Antares
  Simulator, brings the ability to schedule basic data management tasks
  such as study archiving/expansion (use of a specific compressed format),
  copy to backup folders, registering of studies and archives in catalogues.

### 5.0.9-SE (04/2017)
----------------

#### Bug fixes

- Random noises on thermal clusters costs now include the zero-cost
  "must-run" clusters (as a consequence, noises assumptions do not vary
  with the cluster status)

- Fixing an initialization issue that could sporadically affect the
  minimum number of committed thermal units (+1 or -1 deviation,
  "accurate" mode only)

### 5.0.7-SE (04/2017)
----------------

#### Features

- License control : management of SSL certificates encrypted through SHA-256 algorithm

### 5.0.7 (12/2016)
----------------

#### Bug fixes

- Fixing a packaging error

### 5.0.6 (12/2016)
----------------

#### Bug fixes

- Results processing: For full "must-run" thermal clusters, the NODU variable
  could be wrongly assessed in the "accurate" unit commitment simulation mode

- GUI: when the scenario builder feature is active, saving right after deleting
  a thermal cluster could result in a partial dataset corruption (references to
  the deleted object were kept alive in the scenario builder context)

#### Features

- Unsupplied energy control: if the actual economic optimization requires it, load
  shedding is now allowed to occur in areas where the available thermal generation
  is higher than the local demand (e.g. if local VOLL < local thermal costs)

- Linear solver, hot starting of weekly problems: in the "fast" unit commitment
  mode, optimal bases are flushed at the beginning of each Monte-Carlo year. This
  comes as a pre-requirement for the next versions of Antares, which will be
  fully multi-threaded

- Simulation results: code segments processing all variables attached to spatial
  aggregates, and the variable representing the number of running thermal units
  on the first hour of the year, were re-written to be compatible with the next
  versions of Antares, which will be fully multi-threaded

### 5.0.5 (08/2016)
----------------

#### Bug fixes

- No-Load Heat costs and Start-up costs: in the "fast" unit commitment options,
  the result was slightly below the actual optimal possible cost for some
  datasets (i.e. datasets in which the thermal cluster coming last in alphabetic
  order had a minimum stable power equal to zero).

- Spilled energy control: the three parameters defining how energy in excess should
  be split between the different possible sources when there is a choice to make
  can work properly again (feature inhibited in previous 5.0.x versions)

#### Features

- License control throughout the internet: all combinations of UTF8 characters can
  now be used within proxy ids and passwords

- Economic optimization: in an area where the amount of available thermal power
  exceeds that of load, the fact that the demand should necessarily be served
  is locally expressed as a constraint of the optimization problem  (LOLE=0)

### 5.0.4 (05/2016)
----------------

#### Bug fixes

- Errors occured on loading the "min gen modulation" time-series of thermal clusters

#### Features

- Better estimate of the number of thermal units dispatched in "fast" unit commitment mode
- Nodal Marginal Prices and Marginal yield on interconnections are now available in
  "accurate" unit commitment mode
- Binding constraints including offset parameters: unbounded positive or
  negative values can be used for all classes of constraints (hourly, daily, weekly)

### 5.0.3 (05/2016)
----------------

#### Bug fixes

- Crashes occured when the "full must-run status" parameter was set on
  "true" for thermal clusters

### 5.0.2 (04/2016)
----------------

#### Bug fixes

- Removed debug information that should not be displayed in release mode

#### Features

- The optimization criterion used to assess the hydro energies to generate throughout
  each month incorporates heavier penalization terms for the 12 deviations from the
  theoretical monthly targets (formerly, only the largest deviation was penalized).

### 5.0.1 (04/2016)
----------------

#### Bug fixes

- Adequacy mode: fixed a memory allocation bug that forced the post-simulation
  output files processing to be interrupted

- In the previous version, additional logs were added. That could lower the simulation
  performances in some cases. This problem is now solved.

### 5.0.0 (03/2016)
----------------

#### Bug fixes

- GUI, system map:  copy /paste of binding constraints could alter the activity status or
  the names of the duplicated binding constraints in some instances

- GUI, system map:  some conflicts in copy/paste actions were not always properly raised
  (e.g. attempt to copy three nodes and paste them on two other nodes)

- Thermal clusters: Improved checking of time-series generation parameters (improper use of a
  nominal capacity modulation factor lower than the minimum stable power is no longer possible)

- Thermal clusters: Improved checking of ready-made time-series. If the user-chosen time-series
  are not consistent with the parameters set in the GUI, warnings are issued in log files

- Output , LOLD variable:  in some instances, the values assessed in "economic" simulation mode and in
  "adequacy" simulation mode could slightly differ because of sporadic rounding side-effects.
  rounding convention is now set uniformly to : 0 < X < 0.5 -> (X=0)

- Output, MISC.NDG and PSP variable:  values were not properly edited for the specific category
  "geographic districts, "year-by-year results"

- Output, OV. COST, OP. COST, NP. COST variables: values were not properly edited for the last
  hour of the last day of the simulation

- Output, File comparison functions: calendar marks were not properly displayed in some views

- Output, File comparison functions: "Max" operator has been added

#### Features

- Optimization: introduction of a new unit-commitment mode based on a MILP approach slower but more
  accurate than the former one. An option lets the user choose which mode should be used (fast/accurate)

- Optimization: in "accurate" unit-commitment mode, incorporation of thermal start-up costs and
  no-load heat costs within the global objective function to minimize. In "fast" unit-commitment
  mode, start-up costs and no-load heat costs are minimized independently from the main objective

- Optimization: in both unit-commitment modes, improvement of the inter-weekly start-up strategies
  (seamless reformulation of the optimization results obtained beforehand)

- Thermal clusters: definition of separate minimum up/down durations to be used for unit-commitment

- Thermal clusters: definition of a minimum amount of power (hourly time-series) to be generated
  by the units of the cluster, regardless of economic considerations (partial must-run commitment)

- Thermal clusters: start-up cost can now be set from -5000000 to 5000000 (was from -50000 to 50000)

- Binding constraints: introduction of new "offset" parameters which make it possible to define
  constraints whose terms can refer to different times (e.g. 2 X(t) - 1.5 Y(t-4) + 3 Z(t+1) <10)

- Benchmarking: so as to allow transparent comparisons with other software, the user may demand
  that all optimization problems solved by Antares be printed in a standardized "mps" format
  along with the values of the optimized criterion.

- GUI, System map : new button available in the tool bar for centring the map on a (x,y) location

- GUI, System map : new button available in the tool bar for map trimming around used space

- Output: In synthetic Monte-Carlo results,year-by-year results and cluster-by-cluster results,
  Addition of a field "Number of dispatched units" (NODU)

### 4.5.4 (03/2015)
----------------

#### Bug fixes

- License checking: internet proxys for which no login and/or password have been
  defined can now be used

- Upgrade to 4.5 format of datasets edited in 4.4 format or lower, in which the "scenario builder"
  feature was activated: the conversion to 4.5 format could fail sometimes.

### 4.5.3 (02/2015)
----------------

#### Features

- Start-up and fixed thermal costs: the interpretation of the unit-commitment strategy
  (starting-up and shutting-down times of each thermal unit) includes the explicit
  minimization of the total sum of start-up costs and fixed costs (in previous versions,
  units were called on as late as possible and called off as soon as possible)


- Various improvements in the linear solver yielding some speed increase in hard cases


- Control of license validity through the internet (setting up of a dedicated server)

#### Bug fixes

- Scenario builder: indices not subject to random draws could be mixed up in areas
  including both "must-run" units and "regular" units (bug circumscribed to the thermal
  time-series section)

- Spillage management, when numerous binding constraints are active: an excessive leeway
  could be observed regarding the level of hydro power allowed to be curtailed

### 4.5.2 (06/2014)
----------------

#### Bug fixes

- In the previous version, the average values of interconnection-related variables were multiplied by two
  and this error was propagated to the standard deviation of the same variables

### 4.5.1 (06/2014)
----------------

#### Features

- Start-up and fixed thermal costs: the contribution of each thermal cluster to the operating
  cost is now explicitly displayed in the results (field : "non proportional cost")


- Load time-series : negative values are now authorized

#### Bug fixes

- Creation of a thermal cluster : the default value of the NPOMAX parameter is set to 100


- Hydro energy spatial allocation matrix : values are displayed more clearly in the GUI


- Copy/paste of nodes : the field "spread on unsupplied energy cost" was not pasted

### 4.5.0 (04/2014)
----------------

#### Features

- Simplex solver: acceleration regarding the control of the admissibility of the solution
  in the dual stages. This brings a significant improvement of the calculation time for
  large problems in which the relative scale of system costs is very wide


- Identical upper and lower bounds have been set for the absolute values of all
  non-zero system costs ( max = 5 10^4 Euros/MWh ; min = 5 10^-3 Euros/MWh)

#### Bug fixes

- Hydro Time-series generation : the GUI did not react properly when forbidden
  values (negative) were seized for energy expectation and/or standard deviation


- Unit commitment of thermal plants: the time of the first activation of a plant
  within a week was not fully optimized

### 4.4.1 (05/2013)
----------------

#### Bug fixes

- Creation of a new binding constraint: the operation needed to be confirmed twice
  (double click on "create button") and the study had to be "saved as" and reloaded before
  proceeding further.

- Time-series analyzer : due to round-off errors, spatial correlation of 100 %
  (perfectly identical sets of time-series in different locations) could sometimes
  be casted to 99%. Exact 100% correlations are now properly displayed.

### 4.4.0 (04/2013)
----------------

#### Features

- Antares licenses can be either static or floating. Floating tokens are managed and
  distributed by the Flexnet product, version 11.9.

- Thermal plants time-series generator : availability parameters (outage rates and duration)
  corresponding to a Mean Time Between Failure (MTBF) < 1 day are now allowed. Though unusual,
  such sets of parameters may prove useful when it comes to modelling specific situations

- Thermal plants time-series generator : it is possible to model the duration of each kind
  of outages as 365-day random arrays instead of 365-day constant arrays. Two parameters
  are available for the description of the probability distribution function of each component.
  A first parameter allows to set the variable law to either "uniform" or "geometric".
  A second parameter allows to set the ratio of the variable standard deviation to
  its expectation to a particular value

- Thermal plants time-series generator : The planned outage process is now committed to meet a
  set of constraints defined by two 365-day arrays (PO Min Nb, PO Max Nb). For every day of
  each Monte-Carlo year, the actual number of overhauls is kept within the [Min,Max] interval,
  the exact value being determined by regular random draws based on outage rates and durations

- As a consequence of the introduction of these new features, Monte-Carlo time-series
  of available thermal power generated with Antares 4.4 may differ from those generated with
  previous versions. Though differences may be observed draw by draw, the statistical
  properties of the generated time-series are strictly preserved when datasets are identical.

- Hydro storage optimization : when the maximum available power of a given day is not high
  enough to allow the full use of the daily hydro storage energy credit, the energy in excess
  is levelled on the other days of the month with a flatter pattern.

#### Bug fixes

- On creation of a new link, the transmission capacity status parameter is set
  to `Use transmission capacities` instead of `Set to null`.

### 4.3.7 (02/2013)
----------------

#### Features

- Performance improvements for graphical display of large tables

#### Bug fixes

- The binding constraint data might not be written properly in some cases
  when the constraint was renamed.

### 4.3.6 (12/2012)
----------------

#### Bug fixes

- Windows only: fixed potential crash which could happen when exiting
  a simulation in adequacy mode with import of generated time-series

- Windows only: improved free disk space assessment, which now takes into
  consideration user- and folder-related quotas

### 4.3.5 (10/2012)
----------------

#### Features

- The calendar field "year" is now available in the simulation main screen
  (allows not only simulations from JAN to DEC but also from OCT to SEP, etc.)

- The attribute "Leap year" is now available in the simulation main screen

- The attribute "Week" is now available in the main simulation screen
  (weekly results may be defined not only from MON to SUN but also from SAT to FRI,etc.)

- Time-series screens: a new function is available for hourly and daily time-series
  (shift rows until #date#)

- Linear solver: new version slightly more accurate than the previous one.
  Note that when a daily or weekly optimization has multiple equally optimal solutions,
  the ultimate choice may differ from that of the previous version

#### Bug fixes

- Reference numbers of the time-series used in the course of a simulation:
  When the simulation is based on a user-defined scenario (building mode: custom)
  and when a printout of the reference numbers of the time-series used in the simulation
  is asked for (MC scenarios: true), the numbers printed for thermal clusters running
  under the "must-run" status were wrong

- Interconnection results, marginal costs:
  For a congested interconnection whose transmission capacities are not symmetric,
  and in presence of hurdle costs, a zero could sometimes be delivered instead of
  the actually expected value

- Districts: when the Monte-Carlo synthesis edition is skipped, the results regarding
  districts were not accessible via the output viewer.

### 4.2.6 (07/2012)
----------------

#### Features

- The field "MAX MRG" (last of the nodal results) is now available in the output files

- The Monte-Carlo synthesis edition can be skipped when year-by-year results are asked for

#### Bug fixes

- Binding constraints: in the filter available for the weight matrix, removal of
  redundant options

- Copy/Paste nodes on the general map: "print status" parameters can now be copied like
  any other data

- Upgrade of studies in 3.8 format: negative hurdle costs were not correctly transposed

- Thermal plants time-series generator: outages lasting N days, starting on day D, were
  considered as outages lasting N days starting on D+1 (corrected by removal of the
  one-day shift)

- Advanced parameters, option "shave peaks" used along with the "weekly" simplex range:
  the maximum intra-daily hydro storage limit on power could occasionally be overcome during
  the unsupplied energy levelling process (corrected by a slight lessening of the authorized
  levelling)

### 4.1.0 (06/2012)
----------------

#### Features

- Hydro storage energy management : each nodal policy of use can be tuned so as to
  accommodate simultaneously the net load of several nodes

- Hydro storage energy modelling : monthly time-series of inflows and reference trajectories
  for reservoir levels can be used instead of monthly time-series of generated energies.

- Load shedding strategies : when unsupplied energy is unavoidable, a choice is now possible
  between two policies : minimize the duration of sheddings or "shave" the load curve.

- When multiple mathematically equivalent solutions exist a the first order for the
  economic optimization problem, a choice can be made at the second order between three
  ramping strategies

### 3.8.0 (12/2011)
----------------

#### Features

- The simulation mode `Adequacy` is renamed `Draft`.

- A new simulation mode `Adequacy` is available. In this mode, all thermal plants are
  considered as must-run zero-cost units.

- New possibilities are given regarding the filtering of simulation results (selection
  of nodes, of interconnections, etc.)

- Automatic spatial aggregation of results is possible through the use of the new
  "district" object (a district is a sort of macro-node gathering several regions)

- Nodal costs of unsupplied energy and of spilled energy : a small additive stochastic
  noise around the reference values can be introduced to help discriminate between
  theoretically equivalent solutions

### 3.7.4 (08/2011)
----------------

#### Features

- New version of the dual simplex engine (speed is about twice that of 3.6 version)

- Economic optimizations now encompass a full week (168 hours) span. Traditional
  day-long optimizations can still be carried out (ad hoc "preference" parameter)

- Binding constraints can be defined at the weekly scale in addition to the
  daily and hourly scales

- Several other "optimization preferences" are made available to allow the quick examination
  of variants used in sensitivity analyses

- A new graphic interface is available for the consultation of all simulation results
  (except those obtained in draft mode)

- Extraction of data regarding any given variable from the whole Monte-Carlo year-by-year
  set of results is now possible

- New variables are introduced in the economic output files : the overall available dispatchable
  thermal generation (AVL DTG) and the thermal margin (DTG MRG = AVL DTG - dispatched power)

### 3.6.4 (04/2011)
----------------

#### Features

- The "scenario builder" is now available. With this builder it is possible to define
  precisely the simulation context (for any given year, random numbers drawn for each
  kind of time-series can be replaced by user-defined numbers). This feature allows
  simulations to be carried out in a versatile "What If" mode.

### 3.5.3 (03/2011)
----------------

#### Features

- Addition of the fuel category "lignite" to the regular options available
  for the description of thermal plants.

- Improvement of the presentation of the 365-day arrays "market bid modulation"
  and "marginal cost modulation".

- Automatic processing of the inter-monthly & inter-regional hydro correlation hydro
  energy matrix to meet the feasibility constraints (the matrix has to be positive
  semi-definite). User should check in the simulation log file that no warning such as :
  "info : hydro correlation not positive semi-definite : shrink by factor x " appears.

### 3.4.4 (02/2011)
----------------

#### Features

- The names of nodes, thermal clusters and binding constraints can be extended to
  128 characters. Authorized characters are : `a-z, A-Z,0-9,-,_, space`

### 3.4.3 (10/2010)
----------------

#### Features

- Two calculations modes are now available (in the "run" window):

  "regular": the software tries to hold all simulation data in RAM
  this mode is faster than the second one when datasets are small but
  can get dramatically slow when RAM limits are close

  "swap" : a dedicated memory management module loads in RAM amounts
  of data as small as possible. This mode should be prefered to the
  other when datasets are large.

  Note that in "regular" mode, the maximum amount of data loaded is
  limited by the OS to 2 Go on 32-bit machines, regardless of the
  memory installed. The integrality of installed memory can be used
  on 64-bit machines.

- A new module (time-series analyzer) is available to help set the
  parameters of the stochastic time-series generators for wind power,
  solar power and load. The analyzer determines, on sets of historical
  8760-hour time-series the relevant parameters for different kinds of
  random laws (uniform, normal,Weibull, Beta, Gamma), along with a
  description of the auto-correlation dynamic (two parameters)
  and a full spatial correlation matrix

### 3.3.2 (07/2010)
----------------

#### Features

- Improvement of the wind power time-series generator (faster calculations)

- Introduction of new stochastic time-series generators for
  solar power and load

- Introduction of an explicit modelling of wind-to-power curves.
  As a consequence, wind power time-series can now be generated
  either through a direct approach (by analysis of historical
  time-series of power) or through an indirect (more physical)
  approach, based on the analysis of historical time-series of
  wind speed

- Introduction of a new 8760-hour power array for each node,
  representing the day-ahead reserve that should be made available
  (either on-site or at distance) to face last-minute incidents
  and/or forecasts errors.

- Introduction of so-called hurdles costs on interconnection.

### 3.1.0 (01/2010)
----------------

#### Features

- Breakdown of monthly hydro storage energy credits in daily credits:
  The pilot curve is now the net load (i.e. load - all must-run generation)
  instead of the gross load

- New functionalities available for datasets management (stucy cleaner,
  Log file wiewer)

- New info is given for simulation context (available & required amounts
  of RAM & HDD space)

### From V1 to V2 (all versions)
----------------------------

- Refer to project development archives (TRAC thread)

