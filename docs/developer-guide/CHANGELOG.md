---
toc_depth: 2
---

# Antares Changelog


### 10.0.0-rc1

#### Major changes

* Remove GUI (legacy UI)

#### New features

* Hybrid studies with bounds from modeler [ANT-4033] (#3355)
* Thermal capacity Investment with integer variables [ANT-4342] (#3369)
* Improve handling for MIP variables in hybrid studies (#3422)

#### Improvements

* Fix MILP problems in hybrid studies [ANT-4551] (#3407)
* Refactor options.cpp, add tests (#3340)
* Hybrid studies: turn YAML area connections into a dictionary (#3381)
* Index thermal clusters only once [ANT-4522] (#3425)
* Fix: digest [ANT-4630] (#3455)
* Fix dynamic district aggregation (#3429)
* Improve handling for modeler error (#3393)
* Add warning in antares-api if weeks are dependent (#3398)
* Performance scaling tests for expression sum (#3424)
* Fix unit tests binding constraints (#3453)
* Feature/tiny move (#3408)
* Remove unnecessary code and cleanup:
  - Remove GUI code (#3411, #3417, #3438, #3439, #3440, #3441, #3442, #3443, #3444)
  - Remove JIT::usedFromGUI (#3419)
  - Remove loadOnlyNeeded (#3440)
  - Remove unused core/preprocessor/vaargs.h (#3420)
  - Remove ts-generator binary (#3334)
  - Remove code only used by GUI (#3417)
  - Remove functions related to rename (#3444)
  - Remove unused code from class Study (#3442)
  - Optimize variable name building (avoid concat) (#3423)

#### Bugfixes

* Fix: install optim-model-filler (#3394)
* Fix Benders decomposition (#3395)
* Fix: remove unnecessary global time step adjustment to avoid wrong time labels (#3377)
* Fix: windows runtime libraries (#3435)

#### Tests

* Cucumber tests: new checks for area connection (#3443)
* Fix Cucumber test_launcher_8.feature (#3426)


#### Documentation

* Fix: execution_info.ini doc format in readthedoc (#3405)
* Update documentation for ticket ANT-4033 (#3445)
* Fix itemization in port field (#3454)

#### CI/Chore

* chore(ci): update cache action to AntaresSimulatorTeam/cache to avoid deprecated action (#3390)
* CI: Bump actions/upload-artifact from 6 to 7 (#3447)
* chore: update sonar.projectVersion & simtest.json (#3412)
* Allow generating results on release (#3427)

#### For developers

* Refactor AGENTS.md using progressive disclosure pattern (#3396)
* Docker solution for clang-format (#3389)
* MPS Generator ADR (#3410)
* Update simtest (#3415)
* Fix compilation warnings (#3401)


### 9.3.7

#### Feature

* Problem Generator: new tool to write problems without running the simulation (#3314,#3399,#3165)

#### Improvements

* Add a warning in the problem-generator when weeks are dependent, and cleanup (#3398)
* Improve performance for sum operations in expressions

#### Bugfixes

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
* Use devtoolset-11 on CentOS7 to fix build (#2282)
* Keep using node js 16 on centos CI (#2248)
* Remove actions dependencies using node js 16 (#2215)
* Update deprecated actions (#2381)
* Add short test to coverage analysis (#2280) (#2267)
* Check formatting as part of the CI (workflow only) (#2198)
* Always run clang-format on PR (#2230)
* Fix OL8 workflow (#2632)
* Fix migration guide (#2633)
* Parallel tests run twice on ubuntu CI (#2587)
* centos build (#2509) (#2510)

#### Build

* vcpkg (linux, sirius) (#2078) (#2090) (#2145)
* Remove src/antares-deps (#2182)
* Fix or-tools integration (#2402)
* Better dependencies with cmake, antares matrix (#2369)
* Fix build for clang (#2654)
* Place all unit test executables in the same build subdirectory (#2658)
* Improve vcpkg configuration (#2630)
* Remove recently introduced build warnings (#2602)(#2569)
* Update vcpkg@2023.07.21 -> vcpkg@2024.12.16, Boost@1.81.0 -> Boost@1.86.0 (#2553)
* Fix yaml-cpp target to avoid cmake warnings (#2564)
* Use sirius-solver@antares-integration-v1.6 (#2533)

#### Doc

* CHANGELOG improvements (#2287) (#2229) (#2125)
* Fix PDF generation for useguide (#2134)
* Add contribution guidelines (#2380)
* update AUTHORS.txt (#2312)
* Fix links in README (#2310)
* Document clang-format (#2243)
* Add help button to website (#2368)
* Remove doc related to the GUI (#2238)
* Update doc for command invokation : --ortools-solver -> --solver (#2507)
* Migration doc : move hydro level from v8.7.0 to v9.2.0 (#2482)

#### Code quality

* Using filesystem path instead of Yuni [ANT-1999] (#2435) (#2454) (#2123) (#2066)
* Compilation warnings (#2237) (#2199) (#2183) (#2144) (#2119) (#2340)
* Separation of loading and validation [ANT-1213] (#2173) (#2175) (#2177) (#2179)
* Remove manually allocated memory  (#2254) (#2270) (#2273) (#2363)
* Mark overriden functions as such in hydroLevelsData (#2389)
* Districts : simplifying the code (#2279)
* Remove duplication in simulation run (#2274)
* Fix various compilation warnings (#2346)
* Remove useless ;, macro definition (#2348)
* Use const char** for argv, remove function prepareArgs (#2338)
* Move TS number print (#2228)
* Link TS generation : splitting into multiple files (#2171)
* Infeasibility more cleaning (#2231)
* Harmonize time constants (#2203)
* Replace NULL -> nullptr in .cpp (#2209)
* Resize data collections in PROBLEME_A_RESOUDRE (#2189)
* Remove TS links from solver (#2155)
* Simplify functions prepareClustersInMustRunMode (#2168)
* Add a few const ref qualifiers to improve code readibility (#2459)
* Parallel years cleaning : few removal of "numspace" (#2128)
* Capture explicit var for lambdas (#2170)
* Fix circular dependencies on CMake targets (#2140)
* Fix Variable::Join (#2116)
* Throw exception instead of pointer to exception (#2600)
* Clean up container.h / container.hxx (#2601)
* Remove a few new/delete in src/solver/variable (#2595)
* Remove manual allocations for set of areas, simplify code (#2588)
* Rename encoders.hxx -> decoders.hxx (#2591)
* Remove new/delete for AreaUI (#2589)
* Remove new/delete from analyzer (#2590)
* Fix NonMovable class (#2555)
* High-level API: provide output path instead of returning it [ANT-2561] (#2548)
* Add a warning if sts group property is empty [ANT-2026] (#2534)
* Add interface for Thermal costs, fix wrong values in GUI (#2526)
* Fix function ThermalCluster::getMarketBidCost [ANT-2527] (#2525)
* Rename thread number -> numSpace for consistency (#2515)
* Use thread-safe version of localtime (#2503)

#### Technical cleaning

* Remove last global variable (#2410)
* Local matching removal : remove unused thread number / numSpace (#2404)
* Remove Antares::Memory::Array (#2187)
* Remove function memoryUsage and affiliates (#2456)
* Remove multiple definitions for Antares::Statistics::HasWrittenToDisk (#2136)
* Headers files : remove useless inclusions in OPT (#2411)
* Remove unused code & #defines (#2412)
* Remove unused CoutDeDefaillanceEnReserve (#2392)
* Remove unused adqPatchParams argument, class member (#2390)
* Remove unused headers in src/solver (yuni, cstdint, sim.h) (#2371)
* Remove unused CMake option BUILD MINIZIP (#2210)
* Remove useless forward declaration (#2268)
* Remove Antares_Simulator_Tests submodule (#2606)
* Remove unused enum class (#2584)

#### Tests

* Unfeasible tests (#2611)
* Add unit tests on class Antares::Solver::Variable::IntermediateValues (#2593)
* Add tests for TSData getters & setters (#2677)
* Improve unit tests for cost computation (non-constant modulation) [ANT-2527] (#2681)
* Unit test for the RHS of cumulative constraints (#2603)
* cleanup cucumber test outputs (#2663)
* Checkout test nr to main (#2586)
* Add unit test -- short-term storage cumulation constraints (#2563)
* Add and use add_boost_test CMake helper function (#2545) (#2552)
* Unit test for reverse calculation (#2483)

#### For developers

* Support for Ubuntu 22, Ubuntu 20 dropped
* Update ortools to 9.11-rte1.3
* Various improvement to code quality
* Sonarcloud coverage (#2652)(#2649)(#2647)(#2640)(#2641)(#2645)(#2639)
* Write raw optimization results [ANT-2302] (#2565)

## Branch 9.1.x

### 9.1.0 (06/2024)

#### New features

* Scenarized & hourly values for hydro pumping and hydro generation. Previously this data was not scenarized and daily.
* STS groups are now "dynamic" : group names are no longer fixed by code, user is free to define these groups.
* Add optimization options from command line in OR-Tools / XPRESS (#1837)

#### Improvements

* Rationalize consistency checks on the number of columns (#2073)
* Documentation reorganization and improvement (#2024) (#2023) (#2022)
* Add doc for thermal heuristic (#2048)

#### Bugfixes

* Change the formula used in thermal clusters profits (#2097) [ANT-1719]
* Bug on GUI: double-click on study.antares doesn't work anymore (#2047) [ANT-1634]
* Fix build error related to Boost Test < 1.67 on OL8 (#2094)

#### For developers

* Update format-code.sh (#2027)
* Update clang-format options and apply them (#2067)
* FileTreeStudyLoader [ANT-1213] (#2058)
* Yaml parser and Antlr4 integration (#1985)
* Fix oracle 8 compilation related to boost test linking (#2101)
* Fix enum deprecation warning for FileLevel (#2104)
* Improve new release workflow (#2088), make "target branch" field mandatory (#2095)
* Exclude src/ui from sonarQ analysis (#2087)
* Graphviz presets (#2065)
* Unit tests for parameters and generaldata.ini, remove limit on number of MC years (#2041)
* Move Run X Mode in dedicated files (#2054)
* Fix duplicate build for series/series.cpp (#2046)
* Improve dependency requirements (#2009)
* Time Series generation refactor (#2045)
* Create & forward declare BasisStatus struct, reduce build time (#2044)

#### Code quality

* Simplify code for timer (#2032)
* Refactor Application (#2056)
* Remove some Yuni Strings in solver (#2061)
* Cleanup link variables includes (#2030)
* Remove Study::Ptr variant for RetrieveListFromStudy and cbuilder (#2051) (#2052)
* Add more explicit example on how to set solver parameters from command line (#2100)
* Fix a few warnings emitted by clang-19 (#2099)
* Remove Fixture Test Suite, use fixture test case (#2055)
* Use std::atomic instead of std::mutex (#2082)
* Use std::clamp for sc-builder's hydro levels (#2074)

#### Removed

* Remove study cleaner tool (#2059)
* Remove export target (#2053)
* Remove unique_ptr passed by ref (#2086)
* Remove dependency of Antares::logs to antares-core (#2083)
* Remove unnecessary ".inc.hxx" files (#2031)
* Remove deps-build Actions (#2043)
* Remove unused logs.hxx (#2026)

## Branch 9.0.x

### 9.0.0

#### License

* Use licence MPL 2.0 instead of GPL3_WITH_RTE-Exceptions (#1812)

#### Improvements

* Include overflow variable in HydroPower constraint (#1903)
* Add total time logging at the end of the simulation (#1908)
* Add STS level constraint to suspect list for infeasible problem analysis (#1891)

#### For developers

* Use precompiled OR-Tools for Oracle Linux 8 CI (#1893)
* Change version behavior to allow more flexibility (#1898)

#### Code quality

* Use std::shared_ptr instead of indices for active binding constraints in results (#1887)
* Fix a few compilation warnings (#1880)
* Scratchpad numspace (#1749)

#### Tests

* Fix invalid index causing segfault in `test-study` test (#1902)

## Branch 8.8.x (end of support 12/2025)

### 8.8.18 (08/2025)

#### Bugfixes

* Fix segfault when exporting binding constraints results (#2955)

### 8.8.17 (06/2025)

#### Bugfixes

* Bump OR-Tools v9.13-rte1.0 [ANT-3350]
* Remove limit on CPU cores when requested threads == nproc (#2833)
* Create criterion files in `Expansion` mode (#2822) [ANT-3137]
* Fix monthly hydro heuristic infeasibilities [ANT-3236]

### 8.8.16 (05/2025)

#### Bugfixes

* Ceiling error with xpress and accurate [ANT-3167] #2785
* Output variable : near price cap [ANT-3038] (#2793)
* Fix monthly generating capacity [ANT-3095] (#2806)

### 8.8.15 (04/2025)

#### Bugfixes

* Change the formula for the hydro daily max generation [ANT-3095] (#2764)
* Allow empty series files for STS [ANT-3053] (#2731)
* Fix undefined behavior in BCs [ANT-2994] (#2708)

#### For developers

* Fix OR-Tools URL for FetchContent (used when no OR-Tools install is found in the prefix path)

### 8.8.14 (03/2025)

#### Improvements

* Increase precision for `LOLD[CSR]` and `MRG PRICE[CSR]` [ANT-2443] (#2613)

#### Bugfix

* Segfault related to **scenariobuilder.dat** [ANT-2890] (#2675)
* Fix function `ThermalCluster::getMarketBidCost` [ANT-2527] (#2605)

#### Legacy GUI bugs

* Binding constraints TS loading in 8.8 [ANT-2746] (#2635)
* Correct cluster count, load constraints even for disabled clusters [ANT-2754] (#2659)

### 8.8.13 (01/2025)

#### Bugfix

* Backport bug on annual aggregation [ANT-2630] (#2585)

### 8.8.12 (01/2025)

#### New features

* Add variable MRG PRICE CSR [ANT-2562] (#2531)
* Use VCPKG for dependencies (#2549)
* Activate long tests on branch release/8.8.x (#2538)

#### Bugfix

* Fix segfault related to district for 8.8 [ANT-2452] (#2519)
* Fix uninitialized CSR variables [ANT-2496] (#2518)
* Fix segfault caused by invalid index [ANT-2582] (#2544)

### 8.8.11 (11/2024)

#### New features

* Adding parameters for the used optimization solver [ANT-2280] (#2466)
* Adequacy patch CSR : add out variables after DTG netting step [ANT-2295] (#2472)

#### Bugfix

* Legacy GUI : Restore and fix grid statistics [ANT-2381] (#2478)
* Don't apply reverse spinning if the cluster is no force gen [ANT-2293] (#2468)
* Use "trigerred" criterion for mrg price [ANT-2294] (#2453)

### 8.8.10 (09/2024)

#### Bugfix (adequacy patch)

* Force enable-first-step=false [ANT-2218] (#2419)
* Adequacy patch CSR - revamp output variables [ANT-1932] (#2421)
* Place CSR after hydro remix [ANT-2070] (#2407)

#### Bugfix (other)

* Use OR-Tools v9.11-rte1.1 [ANT-2069] (#2418)

### 8.8.9 (09/2024)

* Revert "Fix bug hydro heuristic with mingen (ANT-1825) (#2258)"

### 8.8.8 (09/2024)

#### Bugfix

* Timeseries generation stored in input (#2180)
* Fix bug hydro heuristic with mingen (ANT-1825) (#2258)

### 8.8.7 (07/2024)

#### Improvements

- Add OR-Tools solver option for batchrun tool (#1981)

#### Bugfix

- Adequacy Patch regression [ANT-1845] #2235

### 8.8.6 (07/2024)

#### Bugfix

- Fix missing synthesis results for links (#2115)

#### Dependencies

- Update vcpkg (fix Boost)

### 8.8.5 (05/2024)

#### Bugfix

- [UI] Fix opening a study from the file browser
- Fix crash occurring when duplicate thermal clusters are present in a study (same name)
- Fix formula for "PROFIT BY PLANT"

### 8.8.4 (03/2024)

#### Bugfix

* Adequacy patch CSR - fix DTG MRG (#1982)
* Fix ts numbers for no gen clusters (#1969)
* Remove unitcount limit for time series generation (#1960)

### 8.8.3 (02/2024)

#### Bugfix

* Fix an issue where depending on the platform the output archive could contain several entries of the same area and
  interco files

### 8.8.2

#### Bugfix

* Fix segfault caused by uninitialized `cluster.series.timeseriesNumbers` (#1876). This bug was introduced in v8.8.1 by
  #1752
* Bump OR-Tools from 9.5 to 9.8 (fix crash with XPRESS) (#1873)

### 8.8.1 (01/2024)

/!\ This version has known bugs, please use 8.8.2 instead.

#### Bugfix

* Simplify TS numbers drawings, fix bug related to refresh & local thermal generation (#1752)

#### Improvements

* Take into account breaking change in OR-Tools's API, enable SCIP & GLPK solvers, bump OR-Tools (#1825). This should
  improve performances with FICO XPRESS
* Fail if OR-Tools solver is not found (#1851)
* Normalize simulation mode Expansion, fix logs (#1771)
* Add possibility to release without running any tests (#1852)

#### Code quality

* Use `std::unordered_map` for tmpDataByArea_ (hydro ventilation) (#1855)
* Remove `mutable` keyword from `PROBLEME_HEBDO` (#1846)
* Remove `Study::gotFatalError`, throw exceptions instead (#1806)
* Renewable containers (#1809)
* Use modern style `for` loops, avoid int cast (#1847)
* Remove `YUNI_STATIC_ASSERT` (#1863)

#### Testing

* Add tests on short-term storage/thermal cluster/renewable cluster removal (#1841)

#### Doc

* Add precision about `enabled` field in ST storage (#1850)
* Use dedicated URL path for Doxygen, instead of root (#1865)
* Fix HTML generation for readthedocs (#1867)
* Add dark mode switch for Doxygen online documentation (#1792)

### 8.8.0 (12/2023)
--------------------

#### New features

* New "cash-flow" variable for ST storage (#1633)
* Experimental optimization with discrete variables (MILP unit-commitment mode #670)
* Add `enabled` property for ST storage objects, fix bug related to saving ST objects (#1807)
* Solver logs can be enabled either by the command-line option (--solver-logs) or in the generaldata.ini

#### Improvements

* Add shortcut -s for names MPS problems in CLI options (#1613)
* Use 50% as a default value for ST-storage property initiallevel (#1632)
* Add warning logs for non-existent output variable (#1638)
* Add area name in error message for rare error (#1695)
* Fix wrong year number in logs upon failed year (#1672)
* Always check mingen against maxPower, regardless of reservoirManagement (#1656)
* New log msg when solver not found in or-tools (#1687)

#### For developers

* Fix annoying error log about correlation matrices in tests (#1573)

#### Bugfixes (reported by users)

* Fix output variable PROFIT for thermal clusters (#1767)
* Bug on renewable cluster (wrong group) (#1631)

#### Bugfixes (reported internally)

* Fix oracle-linux8 binaries missing compression feature (#1741)
* Named MPS - fix duplicated "ranged" binding constraints (#1569)
* Fix save for short term storage objects (#1807)
* Prevent segfault during simulation, check bounds of scenario builder (#1567)
* Fix number of links in deprecated output file digest.txt (#1646)
* Fix unfeasible problem analyzer (#1527)
* [Windows only] Increase file size limit when reading file (#1674)
* Fix segfault encountered when importing logs (#1702)
* Fixes swallowed exceptions in computation thread (#1685)
* Fix writer causing a segfault with OR-Tools (#1584)

#### Documentation

* Create Doxygen documentation (#1650)
* Update README.md (#1654)
* Add advice for developers (#1639)
* Document the usage of XPRESS (#1596)

#### GUI

* Fix regression on cluster renaming, add unit tests (#1699)

#### Dependencies

* Use minizip-ng 4.0.1 (from 3.0.7) (#1696)
* Bump vcpkg to latest tag (2023.07.21) (#1532)
* Remove dead code yuni-docmake (#1544)
* Remove fixed-size ints from Yuni (#1622, #1629)

#### Code cleaning / quality

* Architecture Decision Record for Study breakdown (#1600)
* Remove dependency to UI, use RAII to handle resources (#1678)
* De-templatize `HydroManagement::prepareNetDemand` (#1679)
* Use `std::unique_ptr::operator->` rather than `std::unique_ptr::get` (#1676)
* Renamed `timeSeries` into `timeSeriesEnum` (#1675)
* Remove RenouvelableParPalier from global variable (#1659)
* Remove code duplication in src/libs/antares/study/parameters.cpp (#1667)
* Remove manual alloc and update code standards in hydro/daily2 (#1651)
* Remove `StudyRuntimeInfos::parameters` (#1665)
* Introduce & use DataSeriesCommon::getAvailablePowerYearly (#1660)
* Add getValue for thermal clusters, remove `State::timeSeriesIndex` (#1644)
* Move `resultWriter` out of class `Study` (#1649)
* Remove malloc in src/solver/hydro/ (#1626)
* Refactor constraint building (#1607)
* Remove most uses of `IResultWriter::Ptr`, use ref instead (#1642)
* Refactor hydro pmin checks (#1381)
* Clean up parameters.cpp (#1623)
* Remove unused `BindingConstraintsRepository::eachActive` (#1625)
* Provide reference to `Benchmarking::IDurationCollector`, not raw pointer (#1621)
* Use `int32_t` and `uint32_t` in `PROBLEME_HEBDO` (#1577, #1543)
* Reduce use of study in helper functions (#1610)
* Move thermal noises to class Simulation (#1594)
* Upgrade src/solver/hydro to C++ (#1581)
* Remove duplicate header (#1595)
* Remove unused variable (#1585)
* Remove dynamic alloc of PROBLEME_HEBDO (#1554)
* Remove memory usage (#1578)
* Use reference instead of raw pointers (#1579)
* Re-order initialization to avoid compilation warnings (#1599)
* Remove some global variables (#1545)
* Remove `goto` in `OPT_AppelDuSimplexe` (#1533)
* Remove useless Area's data member : `enabled` (#1556)
* Clarify string -> enum conversion for thermal cluster groups (#1553)
* Remove unmanaged dynamic allocation for `ProblemeAResoudre` (#1549)
* Remove `shared_ptr` for `OptPeriodStringGenerator` (#1528)
* Final removal of study singleton from the solver (#1536)
* Remove use of current study singleton in emergency exit (#1519)
* Remove unused function `getFilenameWithExtension` (#1537)
* Remove tmpnam from tests (#1506)

#### Build

* ACR CMake (#1551)
* Add "Antares::action" CMake library, build only if BUILD_UI=ON (#1637)
* Fix conflicting library name (#1590)
* Cleanup date dependency (#1565)
* CMake Memory and Sys (#1558)
* Split benchmarking (#1582)
* Remove liblib* CMake targets (#1609)
* Split study in CMakefile (#1583)
* CMake usage improvements (#1548)
* Fix build for Oracle Linux 8 (#1542)

#### Misc

* Schedule deps compile instead of develop merge (#1530)

## Branch 8.7.x

### 8.7.3 (02/2024)

#### Bugfix

* Use OR-Tools v9.8-rte1.0 (performance improvements with OR-Tools + XPRESS)

### 8.7.2 (11/2023)

#### Bugfix

* Named MPS - fix duplicated "ranged" binding constraints (#1569)

### 8.7.1 (11/2023)

#### Bugfix

* Fix output variable PROFIT for thermal clusters (#1767)

### 8.7.0 (08/2023)

#### New Features

* Binding constraint RHS scenarization (#1219)
* Implement --mps-export command-line option (#1404)
* Name constraints & variables in MPS files using --named-mps-problems command-lin e option (#1294)
* Thermal price definition (contributed by RTE-i, #1272)

#### Improvements

* Write full command-line instead of solver location in logs (#1518)

#### Packages

* Give Oracle Linux 8 assets a proper name instead of "unknown" (#1438)
* Don't build tools (study-updater, etc.) by default (#1442)

#### Bugfixes

* Fix error when writing files over 80Mb into a zip (#1488)
* Fix memory leaks (#1468)
* Fix segfault, add !skipped to enabled constraints (#1441)
* Backport [v8.4.3](#v843-082023) changes

#### GUI

* Thermal price definition (RTEi's -> CR20) - UI (#1485)

#### Docs

* Fix possible values for ST storage (#1455)

#### Tests

* Add named MPS tests  (#1408)
* Enforce better unit test isolation (#1486)
* Add tests for Windows CI when job is scheduled (#1483)
* Tests for CR20: thermal price definition (#1364), improvements (#1422)

#### For developers

* Remove platform-specific headers (#1523)
* Remove one `goto` instruction in OPT (#1522)
* Remove study singleton in application signal handlers (#1513)
* Remove barely-used `ThermalCluster::productionCost` temporary (#1517)
* Bindings constraints scenario : handling last remarks (#1466)
* Replace remaining vector of pointers by plain vector (#1505)
* Split ts numbers data classes in different files (#1398)
* Remove more usage of study singleton (#1509)
* Group of constraints (#1388)
* Remove unused headers in optimization module (#1495)
* Refactor ProblemeHebdo allocation (#1489)
* Remove use of study singleton in optimization module (#1492)
* Remove Study::Get from SIM, pass as function argument instead (#1494)
* Objects in CMakeFiles (#1465)
* Variables with a Ref or Ref_SV suffix (#1475, #1481, #1497, #1498, #1500, #1501, #1502)
* Use std::vector instead of MemAlloc/MemFree, clean up (#1471)
* Use ~class() = default (#1473)
* Memory allocation for ProblemeAResoudre, use std::vector (#1432)
* Improve clarity for constraints that should be used (#1452)
* Automatically include yuni include directory in dependents (#1525)
* Clarify logs & code in case of "failed year" (#1450)
* Remove two static functions (#1440)
* Avoid copies using std::move (#1445)
* Memory allocation, use std::vector (#1402)
* Fix critical code smells (#1412)
* Named MPS: factorize variable & constraint namers (#1409)
* Array, logs jit and correlation in makefile (#1410)

## Branch 8.6.x (end of support 06/2025)

### 8.6.8 (07/2024)

#### Bugfix

- [UI] Remove propery storagecycle for short term storage added when saving a study (#2037)

#### Dependencies

- Update vcpkg (fix Boost)

### 8.6.7 (05/2024)

#### Bugfixes

* Fix formula use in output var Profit by
  plant [ANT-1719] (https://github.com/AntaresSimulatorTeam/Antares_Simulator/pull/2097)

### 8.6.6 (03/2024)

#### Bugfixes

* Adequacy patch CSR - fix DTG MRG (#1982)
* Fix ts numbers for no gen clusters (#1969)
* Remove unitcount limit for time series generation (#1960)

### 8.6.5 (02/2024)

#### Bugfix

* Use OR-Tools v9.8-rte1.0 (performance improvements with OR-Tools + XPRESS)

### 8.6.4 (11/2023)

#### Bugfixes

* Fix Oracle Linux minizip build + actually run zip unit tests (#1744)
* Fix output variable PROFIT for thermal clusters (#1767)

### 8.6.3 (10/2023)

#### Bugfixes

* Increase file size limit from 80Mo to 80Go when reading file. Fix issue on Windows

### 8.6.2 (08/2023)

#### Bugfixes

* Backport [v8.4.3](#v843-082023) changes

### 8.6.1 (06/2023)
--------------------

#### Bugfixes

* Fix major bug related to short-term storage & MRG. PRICE (#1377)

### 8.6.0 (06/2023)
--------------------

#### New features

* Short-term storage (#1163).
* Add pollutant emissions (#1184, #1222)
* Minimal generation for hydraulic (#1273, RTE/RTE-i/Redstork)
* Make LMR optional for adequacy patch (#1247)

#### Improvements

* Use ISO8601 for date format in the logs (#1303)
* Publish installers for Oracle Linux 8 (#1341)
* Remove doc from UI/package, publish it as a separate PDF (#1233)
* Tune XPRESS resolution (#1158), this should improve performance for OR-Tools+XPRESS
* Performance metrics (produce a JSON file) (#1306)

#### Bugfixes

* Fix a bug in adequacy study mode (#1314)
* Fix memory errors detected by valgrind (#1302)
* Fix empty ROR & STORAGE in output using the TS-Generator (#1293)

#### Code quality

* Simplify if/else (#1309)
* Max number of columns in an output file (#1159)
* Fix a few compilation warnings (int -> uint) (#1301)
* Remove parameters.hxx, fix Parameters::memoryUsage (#1299)
* Remove OUI_ANTARES NON_ANTARES (#1188)
* Use CMakeLists.txt instead of *.cmake (#1198)
* Clean up src/solver/optimisation (#1161)
* Adequacy patch : refactor parameters management (#1154)
* Add a few const qualifiers (#1178)
* INI files load encapsulation (#1057)
* Remove group{Min,Max}Count, annuityInvestment in thermal clusters (#1350)

#### For developers

* Bumped OR-Tools 9.2 -> 9.5. CMake 3.18+ is required for build if building OR-Tools, and XPRESS 9.0 for execution (
  previously 8.13).

## Branch 8.5.x

### 8.5.1 (08/2023)
--------------------

#### Changes

* Backport [v8.4.3](#v843-082023) changes

### 8.5.0 (02/2022)
--------------------

#### New features

* Curtailment Sharing Rule for Adequacy Patch #1062, including a scaling coefficient in hurdle costs #1155. This feature
  was contributed by RTE-i with support from RTE, ELIA and APG.

#### Bugfix

* Hydraulic patch #697
* Fix link path error in Kirchhoff constraint builder #1157

#### For developers

* Fix build on Ubuntu 22.04 #1160
* Cleaning #1142, 1146, #1149

#### Examples & documentation

* Update docs to include CSR #1156
* Fix examples studies (invalid v8.3.0 -> v8.5.0) #1136

## Branch 8.4.x

### 8.4.3 (08/2023)
--------------------

#### Features

* Increase file size limit from 80Mo to 80Go when reading file.

#### Bugfix

* Fix "unhandled error" with big studies in zip mode. MPS files were too big and hit file size hard limit

### 8.4.2 (01/2022)
--------------------

#### Improvements

* Generate a solver-only asset for every release #976, #1080
* Use MPS writer from solvers, instead of copy-pasted functions #1023

#### GUI

* Allow more than 100 thermal clusters #1011
* Remove status bar count. The sum was sometimes wrong #1029
* Remove unused "District marginal prices" button from Advanced Parameters #1067

#### Bugfix

* Fix overwritten MPS files when optimization/simplex-range=day #1041
* Restore warm start for OR-Tools+XPRESS #1079
* Fix output overwrite when using zip output by adding a -2, -3, etc. suffix #1044
* Fix crash when generaldata.ini is empty, using default values #892

#### Testing

* Introduce missing MPS comparison tests #1035

#### For developers

* Fix empty CMAKE_BUILD_TYPE #1028
* Fix memory leaks #669

#### Code cleaning

* Clean up Yuni #1055
* Remove default implementation for hourEnd (see variable.hxx) #1020
* Remove free functions for areas' mem allocation #922
* Remove dead code associated to debug #1048
* Remove unused `ContrainteDeReserveJMoins1ParZone`, `NumeroDeVariableDefaillanceEnReserve` #1047
* Extract code related to spilled energy bounds #1049
* Simplify code for adq patch (Local matching) #1054
* Refactor naming for {MPS, criterion, etc.} files and exported structures (see parameter
  optimization/include-exportstructure) #1030
* Refactor: move current year and week from study to weekly optimization problem #1032
* Pass optimization number as an argument #1040

#### New Contributors

* @JasonMarechal25 made their first contribution in #1055

**Full Changelog**: https://github.com/AntaresSimulatorTeam/Antares_Simulator/compare/v8.4.1...v8.4.2

### 8.4.1 (12/2022)
--------------------

#### Bugfix

* Fix formula for profit calculation #1022

### 8.4.0 (12/2022)
--------------------

#### Features

* Add option & command-line argument to write results into a single zip archive #794
* Add option to set link capacity to null/infinity for physical links only #792
* Speed up simulations by extracting simplex basis in XPRESS/OR-Tools #957
* Move Kirchhoff constraint generator to an executable #909
* Do not export BC MARG PRICE variable by default by #928
* Enable MPS write with OR-Tools+Sirius

#### GUI

* Make sure that RC/beta are displayed in version numbers #739

#### Bugfix

* Remove error message printed on -h/--help in antares-solver #895
* Use average for BC MARG PRICE (daily & weekly) #940
* Fix crash when exporting MPS with OR-Tools #923
* Remove obsolete option `include-split-exported-mps` #956
* Fix crash when many binding constraints are present in a study #924
* Fix BC MARG COST values in parallel by @flomnes in #1015
* Use min gen modulation to compute variable "profit by plant" by @guilpier-code in #1016
* Round renewable power generation by @flomnes in #985
* Make logs more expressive when checking link values by @flomnes in #1005
* Add condition to avoid null pointer by @payetvin in #989

#### For developers

* Case insensitive option for build type by @payetvin in #986
* [DEV] Refactored ortools_utils by @payetvin in #978
* [CI] Integrate ortools v9.2-rte2.0 by @payetvin in #1007
* [FIX] Cleaned day ahead reserve management by @payetvin in #1010
* [CentOS 7] Remove deps from docker image by @flomnes in #994
* Cleaning, renaming (#896, #897, #898, #935, #937, #944, #945, #950, #954, #959 , #966)
* Refactor average results #946
* CI: upgrade deprecated Github actions #938
* CI: activate automatic gcov report on SonarCloud #967
* Use GIT_SHALLOW for OR-Tools's FetchContent #904
* Remove antares-solver swap variant & librairies, reduce build duration #906

#### Documentation

* [DOC] OR-Tools build #880
* Document how to run JSON tests #902
* Document option --list-solvers #770

#### New Contributors

* @kathvargasr made their first contribution in #967

**Full Changelog**: https://github.com/AntaresSimulatorTeam/Antares_Simulator/compare/v8.3.2..v8.4.0

## Older branches

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
  output files so as to include only variables of interest

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

- Output: when simulation results are trimmed so as not to produce
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

