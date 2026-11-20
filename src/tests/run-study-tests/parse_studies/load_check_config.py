import json
from pathlib import Path

from jsonschema import Draft7Validator


SCHEMA_FILE = Path(__file__).with_name("json_schema.json")


def load_check_config(study_path):
    config_file = study_path / "check-config.json"
    try:
        with config_file.open() as file:
            entries = json.load(file)
    except json.JSONDecodeError as error:
        raise ValueError(f"{config_file}: invalid JSON: {error}") from error

    with SCHEMA_FILE.open() as file:
        schema = json.load(file)

    errors = sorted(Draft7Validator(schema).iter_errors(entries), key=str)
    if errors:
        details = "; ".join(
            f"{list(error.absolute_path)}: {error.message}" for error in errors
        )
        raise ValueError(f"{config_file}: invalid test configuration: {details}")

    names = [entry["name"] for entry in entries]
    if len(set(names)) != len(names):
        raise ValueError(f"{config_file}: test names must be unique")
    return entries
