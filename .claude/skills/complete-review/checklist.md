# Antares review checklist

Lens used by the "Maintainer checklist" pass of the complete-review skill. Derived from recurring
review feedback on this repository (C++20, Boost.Test, CMake/vcpkg, Python tests).

### Correctness (always HIGH)
- Removed resets/initialisations causing accumulation across years (`+=` on stale data).
- Unchecked preconditions: empty `allCapacityReservations`, null `timeSumNode->from()/to()`, negative index
  (e.g. `layerStorage = -1`) passed to `cost`/`x`, expression not evaluating to a vector (literal port value).
- Integer overflow at accepted extremes: loop `for (int i = from; i <= to; ++i)` with `to == INT_MAX`,
  `to - from + 1` in `int`, `a + b` after a range check. Widen to `long long` or reject the endpoint.
- Bounds that may become inconsistent (`Xmax < Xmin`): suggest `std::max`/clamp.
- Numerical tricks leaking into outputs (e.g. noise added to linear costs used when computing results).
- Skipped years due to playlists; expression evaluated at the wrong time step.
- `assert` for runtime checks (gone in release): use a real check.
- Ignored return values, swallowed exceptions, errors without a log. "Ignoring exceptions seems dangerous."
- Throw and let the caller decide, rather than warn-and-continue. Absent optional fields must not warn.
- Const-correctness: modifying through a const ref, forced copies (`for (auto m : models)` -> `auto&`).

### Tests
- Tests that only check "no exception" instead of numerical values.
- Weakened tests (error content no longer checked) are HIGH.
- Duplicated tests: make them parametrised (`BOOST_DATA_TEST_CASE`) or share a fixture.
- Shared mutable studies between tests; large `Study` on the stack.
- Use existing helpers (`NullResultWriter`, `InMemoryWriter`) instead of ad hoc ones.
- Test names that don't match what is tested (e.g. "ConcurrentAccess" under a mutex).
- Prefer `BOOST_REQUIRE_EQUAL` for sizes before indexing. Use raw string literals for inline YAML.
- Code only used by tests: consider removing it.

### Design and API
- One source of truth: call `BuildThermalClusterComponentId` etc. rather than hard-coding strings.
- Enums (`enum class`) instead of strings/bools/ints (`outputSelection`, `ReportType`, `MiscGenIndex`).
- Factory methods instead of `impl_ != nullptr` checks everywhere; virtual `WriteTable`/`Close`
  instead of `if (impl_->parquetWriter)`.
- A struct instead of 3 parallel `map`s keyed identically.
- Make required things mandatory (reference instead of nullable pointer; no `std::optional` where an empty vector works).
- Delete dangerous default constructors (e.g. a `ThreadPool` that starts no threads).
- Avoid visitors that behave differently depending on a nullable member.
- Duplicated logic: refactor with a helper or a `bool` parameter. Generic code goes to its own function.
- One function instead of `has_x` and `has_no_x`. Inline one-line wrappers.
- New CLI behaviour belongs in command-line flags only. Nothing added to `generaldata.ini`
  unless explicitly intended.
- Dead code that becomes unreachable after the change must be removed.

### Performance / idiom (C++20 project)
- Index-based `std::vector` instead of `map<string,...>` when an area/cluster index exists.
- `find` once and reuse the iterator, not `contains` + `at`/`[]`.
- No lookup repeated at every `add`: do it once. Skip iterating empty maps with an `if`.
- Static `std::array` for fixed-size dimensions; `getFlag`/`setFlag` as with other variables.
- `std::ranges::any_of`, structured bindings, `std::false_type`, `std::optional` comparison.
- `constexpr std::string` is not allowed in C++20. `constexpr` data members with non-literal types, same.
- Avoid ternaries in complex logic: split into smaller functions, `if/else`.
- Never add zero coefficients to constraints.
- Prefer existing accessors (`Component::getModel`) over double linear searches.

### Hygiene
- Declarations only used by the `.cpp` do not belong in the `.h` (anonymous namespace, `decoders.cpp`).
- Includes: move to the header that needs them; add missing ones (`<type_traits>`, `<ranges>`).
- Redundant `= default` constructors/destructors, redundant `if` before a loop, redundant aliases (`using X = std::string`).
- Use `override` / `final` consistently. Use `[[nodiscard]]` on `bool load()`-style functions.
- Don't pass `.c_str()` where a `std::string` is accepted.
- Const-ref params; no needless `std::optional`; no unneeded mutexes in single-threaded sections.
- Error messages: no malformed output on empty fields (`"in file  : ..."`), log with context (file name, `e.what()`).
- Naming: misleading names (`checkKeysIfMap`), unclear `index`, comments that state the obvious.
- A comment where the logic is subtle (e.g. which time step an expression is evaluated at).

### Docs, CI and build
- Docs must match the code: every key the code accepts, none it doesn't (check `global-parameters`,
  symmetries for thermal/STS, not only hydro). Vocabulary: cluster = thermal/renewable, not short-term storage.
- vcpkg: declare a dependency for each Boost component used (e.g. `boost-unordered`).
- CI: question unnecessary `apt-get`/prebuilt downloads. Use `-DBUILD_TESTING=$RUN_TESTS`-style
  variables. Revert accidental changes ("revert this, my bad").
- Python tests: if indices change, update all dependent assertions. 2-letter aliases are fine (`np`).
- Changes touching results: ask for a non-regression study in the tests repo.
