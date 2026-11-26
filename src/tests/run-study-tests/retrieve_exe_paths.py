import configparser
from configparser import ConfigParser
from pathlib import Path
from typing import Any


def load_executables(config_file=Path(__file__).with_name("executables.ini")):
    config = read_config_file(config_file)
    return extract_exe_paths(config, config_file)


def extract_exe_paths(config: ConfigParser, config_file: Path) -> dict[Any, Any]:
    executables = {}
    for exe_name in ("antares-solver", "antares-problem-generator"):
        if not config.has_option("executables", exe_name) or not config["executables"][exe_name].strip():
            raise ValueError(f"{config_file}: missing executable path for {exe_name}")
        path = Path(config["executables"][exe_name])
        if not path.is_absolute():
            raise ValueError(f"{config_file}: executable path for {exe_name} must be absolute")
        executables[exe_name] = str(path)
    return executables


def read_config_file(config_file: Path) -> ConfigParser:
    config = ConfigParser(interpolation=None)
    try:
        with config_file.open() as file:
            config.read_file(file)
    except FileNotFoundError as error:
        raise FileNotFoundError(
            f"{config_file} is missing; configure CMake with BUILD_TESTING=ON first"
        ) from error
    except configparser.Error as error:
        raise ValueError(f"{config_file}: invalid executable configuration: {error}") from error
    return config
