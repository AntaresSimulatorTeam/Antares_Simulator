# Copyright 2007-2026, RTE (https://www.rte-france.com)
# SPDX-License-Identifier: MPL-2.0

"""Steps that modify the GEMS part (input/system.yml) of a hybrid study."""

from pathlib import Path

import yaml
from behave import given


def _system_file(context) -> Path:
    # 'the solver study path is a copy of "..."' sets context.tmp_workdir.
    assert hasattr(context, "tmp_workdir"), (
        "this step modifies input/system.yml; load the study with "
        '\'Given the solver study path is a copy of "..."\' so the change stays confined to '
        "a temporary copy"
    )
    return Path(context.study_path) / "input" / "system.yml"


def _find_component(system: dict, component_id: str) -> dict:
    for component in system["system"]["components"]:
        if component["id"] == component_id:
            return component
    raise AssertionError(f"No component '{component_id}' in system.yml")


def _parse_integer_strategy(strategy: str) -> dict:
    """'exact', 'relaxed', 'heuristic fast' or 'heuristic accurate'.

    'heuristic' alone (no heuristic-id) is accepted to test the input checks."""
    words = strategy.split()
    assert 1 <= len(words) <= 2, f"Unexpected integer strategy '{strategy}'"
    strategy_node = {"id": words[0]}
    if len(words) == 2:
        strategy_node["heuristic-id"] = words[1]
    return strategy_node


def _update_component(context, component_id: str, update) -> None:
    system_file = _system_file(context)
    system = yaml.safe_load(system_file.read_text(encoding="utf-8"))
    update(_find_component(system, component_id))
    system_file.write_text(yaml.safe_dump(system, sort_keys=False), encoding="utf-8")


@given('in system.yml, component "{component_id}" has integer strategy "{strategy}"')
def set_component_integer_strategy(context, component_id, strategy):
    _update_component(
        context,
        component_id,
        lambda component: component.update({"integer-strategy": _parse_integer_strategy(strategy)}),
    )


@given('in system.yml, component "{component_id}" uses model "{model}" with integer strategy "{strategy}"')
def set_component_model_and_integer_strategy(context, component_id, model, strategy):
    def update(component):
        component["model"] = model
        component["integer-strategy"] = _parse_integer_strategy(strategy)

    _update_component(context, component_id, update)
