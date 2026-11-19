import configparser
from pathlib import Path


def load_executables(config_file=Path(__file__).with_name("executables.ini")):
    config = configparser.ConfigParser(interpolation=None)
    try:
        with config_file.open() as file:
            config.read_file(file)
    except FileNotFoundError as error:
        raise FileNotFoundError(
            f"{config_file} is missing; configure CMake with BUILD_TESTING=ON first"
        ) from error
    except configparser.Error as error:
        raise ValueError(f"{config_file}: invalid executable configuration: {error}") from error

    executables = {}
    for name in ("antares-solver", "antares-problem-generator"):
        if not config.has_option("executables", name) or not config["executables"][name].strip():
            raise ValueError(f"{config_file}: missing executable path for {name}")
        path = Path(config["executables"][name])
        if not path.is_absolute():
            raise ValueError(f"{config_file}: executable path for {name} must be absolute")
        executables[name] = str(path)
    return executables
