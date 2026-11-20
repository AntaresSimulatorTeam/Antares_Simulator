# GEMS input schemas (local copy)

JSON Schemas (draft 2020-12, written in YAML) used by the Simulator to validate GEMS input files
before they are decoded. They are embedded in the binary at configure time (see `../CMakeLists.txt`),
so nothing is read from disk or the network at run time.

| File | Validates | Used by |
|---|---|---|
| `model-library.schema.yml` | model libraries (`library:` files) | `YmlModel::Parser` |
| `system.schema.yml` | `system.yml` | `YmlSystem::Parser` |
| `common.schema.yml` | shared `$defs` (referenced by both) | - |

`optim-config` and `system-for-view` are deliberately **not** copied: they are marked provisional
upstream, and `yml-optim-config` still uses the hand-written field check in `yml-utils`.

## Source

Copied from the GEMS repository, pull request
[AntaresSimulatorTeam/GEMS#284](https://github.com/AntaresSimulatorTeam/GEMS/pull/284), branch
`feat/json-schemas`, commit `4498d940d28878067ce5cc586bc818feb8f665f9`, directory `schemas/`.
That PR was still a draft when this copy was taken; the `$id`s point at a tag
(`schemas-v0.1.0`) that did not exist yet. They are only identifiers here: `$ref`s are resolved
against the embedded files by file name.

The goal is for GEMS to be the single source of truth. **This copy is temporary**: once the schemas
are released upstream, replace it with a pinned release and delete the deviations below by getting
them accepted upstream (or by changing the Simulator).

## Deviations from the upstream copy

Every deviation is marked in place with an `x-simulator-deviation` key. They exist so that the
Simulator's behaviour does not change when validation moves to the schema. Each one is a candidate
to propose upstream or to remove.

| Where | Deviation | Upstream ID | Why |
|---|---|---|---|
| `model-library` | `variable-type` also accepts `boolean` | L02 | The Simulator spells the third type `boolean`; 5 test libraries use it. GEMS, GemsPy and AntaREST use `binary`. The decoder treats `binary` as an alias of `boolean`. |
| `model-library` | `location` accepted on variables, constraints and objective contributions (new `Location` and `Objective` defs) | L04 | Simulator-only key. |
| `model-library` | every collection also accepts `null` | L12 | The Simulator reads a null collection as absent. |
| `model-library` | `thermal-capacity-connection.capacity-field` accepts `null` | - | The Simulator accepts an empty value. |
| `model-library` | `area-connection`: the "at least one non-null value" rule (`anyOf`) is removed | L07 | The Simulator accepts an all-null connection. |
| `common` | `Identifier` has no `minLength: 1` | L08 | The Simulator accepts an empty id. |
| `common` | property `value` accepts numbers and booleans | S12 | The Simulator reads any scalar as a string. |
| `system` | component `model` is a plain string instead of `<library>.<model>` | - | The format is checked, with a dedicated message, when the system is converted. |
| `system` | the number-or-series-id rule on a parameter `value` is removed | S05 | The Simulator reads every value as a string and rejects an inconsistent one when it converts. |
| `system` | parameter-level `scenario-group` is **not** accepted | S06 | Accepted upstream (as in GemsPy), but the GEMS doc only documents it on components. The Simulator would silently ignore it. |

Accepted upstream and now also accepted (and ignored) by the Simulator: `library.version`,
`library.taxonomy`, model `taxonomy-category` and `properties` (L01); `library.dependencies` (L05);
constraint bounds (L06); component `integer-strategy` (S07); the hybrid sections
(`area-connections`, `thermal-capacity-connections`) and `system.description` (S01).

## How YAML is read

- Plain scalars are typed as in the YAML 1.2 core schema: `true`/`false`, `null`/`~`/empty,
  integers, floats. `yes`, `on`, `no` and `off` are **strings**. Quoted scalars are always strings,
  so `id: 123` is rejected where a string is expected but `id: "123"` is accepted.
- Duplicated mapping keys are errors (a JSON object cannot hold them). The first occurrence is the
  one validated.
- Errors carry the line and column of the offending node and a path through the document that
  names entries by `id` when they have one (`library/models/m/variables/v/variable-type`).

## Scope

Validation checks structure only: required and unknown keys, types, enums. The expression syntax,
references between libraries, ports and components, and data-series dimensions are still checked by
the decoders and converters. The decoders still read values from the YAML tree by hand once it has
been validated.

## Updating

1. Take the new files from GEMS (same three names) and update the commit above.
2. Re-apply the deviations, or drop the ones upstream has adopted.
3. Run `TestYmlModel` and the modeler tests; real study files are the best conformance check.
