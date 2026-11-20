# ADR: Use jsoncons to validate GEMS input files against JSON Schema

**Status**: Proposed  
**Date**: 2026-11-20  
**Related PRs**: #4059 (library metadata), GEMS #284 (JSON Schemas)

## Context

The Simulator reads GEMS inputs (model libraries and `system.yml`) with hand-written code:
a whitelist of accepted fields per node (`YmlUtils::checkFields`) followed by manual decoding.
Several other Antares projects (GEMS, GemsPy, AntaREST, ...) validate the same files with their own
rules, and the rules have drifted apart.

GEMS PR #284 publishes JSON Schemas (draft **2020-12**, authored in YAML) as the single source of
truth for what is mandatory, optional and forbidden. The Simulator should validate against these
schemas instead of maintaining its own field lists.

This requires a JSON Schema validator in C++. Constraints:

- **Reputable**: maintained, widely packaged, not a hobby experiment.
- **Licence compatible with MPL-2.0**: permissive, no copyleft or network clause.
- **Draft 2020-12**: the schemas use `$defs`, `$ref` next to sibling keywords and cross-file
  `$ref`s. Older drafts ignore sibling keywords of `$ref` or do not know `$defs`.
- **Available through vcpkg**, which is how every other dependency is obtained.
- **Independent of the input format**: the YAML files are converted to JSON in memory before
  validation (see below), so the validator does not need to read YAML.

## Decision

Use **jsoncons** (`jsoncons_ext/jsonschema`, vcpkg port `jsoncons`, CMake target
`jsoncons::jsoncons`, licence BSL-1.0).

Surrounding design, so that the dependency stays replaceable:

- The library is used only inside `src/io/inputs/yml-schema`. It is a `PRIVATE` dependency: no
  jsoncons type appears in a public header. The public API is
  `YmlSchema::validate(const YAML::Node&, SchemaKind, context)`, which throws `InputError`.
- yaml-cpp stays the YAML reader. A small converter (`YamlToJson`) builds the JSON value, types
  plain scalars as in YAML 1.2, rejects duplicated keys, and keeps line and column for diagnostics.
- The schemas are a local copy of the GEMS files, embedded in the binary at configure time.
  Deviations from upstream are marked `x-simulator-deviation` and listed in
  `src/io/inputs/yml-schema/schemas/README.md`.

## Alternatives considered

Figures were read from GitHub on 2026-11-20.

| Library | Licence | Draft | Stars | Verdict |
|---|---|---|---|---|
| **jsoncons** | BSL-1.0 | 2020-12, 2019-09, 7, 4 | 859 | **Chosen** |
| pboettch/json-schema-validator (nlohmann) | MIT | 7 | 619 | Rejected: draft 7 only, and requires adding nlohmann/json |
| valijson | BSD-2-Clause | 7 (partial newer support) | 426 | Rejected: would force the schemas down to draft 7 |
| RapidJSON schema module | MIT | 4 | 15,157 | Rejected: draft 4 cannot express our schemas |
| Sourcemeta Blaze | AGPL / commercial | 2020-12 | 133 | Rejected: licence incompatible with MPL-2.0 |
| Hand-written validation (status quo) | - | - | - | Rejected: duplicates the rules the schemas already define and keeps projects out of sync |

Other languages (Python `jsonschema`, Ajv, Rust `jsonschema`, Go, Java) are not usable in the
Simulator binary. `check-jsonschema` (Python CLI, reads YAML natively) remains an option for a CI
check outside the binary.

## Consequences

### Positive

- One definition of the input format shared with the other projects. A rule changes in the schema,
  not in several parsers.
- Precise errors: every finding reports line, column and a path named by entry `id`.
- Unknown-field handling no longer depends on hand-maintained whitelists in the parsers.
- Draft 2020-12 is fully supported, so the upstream schemas are used unchanged apart from the
  documented deviations.
- Permissive licence (BSL-1.0, the Boost licence), compatible with MPL-2.0.

### Negative

- New third-party dependency (header-only, so no extra binary to link), listed in `vcpkg.json` and
  `Config.cmake.in`.
- **Concentration risk**: jsoncons is essentially one maintainer's project (30 contributors in
  total), although it has been maintained for many years and released v1.10.0 in October 2026.
  Mitigated by keeping it private to `yml-schema`, so replacing it only touches one library.
- Error messages change: tests asserting the old messages ("Missing field", "Unrecognised field")
  were rewritten.
- The Simulator now accepts and ignores some keys that the schema allows but it does not use
  (for example `library.dependencies`, constraint bounds, `integer-strategy`).
- The local schema copy can drift from upstream until a pinned release is available.
- Schemas validate structure only: expression syntax, cross-references and dimensions are still
  checked by the decoders and converters.

## Verification still to do

- GitHub does not detect the licence of jsoncons (it reports `NOASSERTION`, since it does not
  recognise BSL-1.0): confirm the `LICENSE` file of the pinned vcpkg version before release.
- Build and test on Windows and the other CI compilers.
- Run the modeler cucumber features, which assert on error messages.

## Future considerations

- Replace the local schema copy with a pinned upstream release and propose the deviations upstream.
- Add a CI check of the study files against the pinned schemas with `check-jsonschema`.
- Migrate `optim-config` once its schema is no longer provisional.
- If jsoncons becomes unmaintained, the encapsulation in `yml-schema` allows switching to another
  2020-12 validator without touching the parsers.
