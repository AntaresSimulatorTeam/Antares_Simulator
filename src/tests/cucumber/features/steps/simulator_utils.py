#  Copyright 2007-2024, RTE (https://www.rte-france.com)
#  See AUTHORS.txt
#  SPDX-License-Identifier: MPL-2.0
#  This file is part of Antares-Simulator,
#  Adequacy and Performance assessment for interconnected energy networks.
#
#  Antares_Simulator is free software: you can redistribute it and/or modify
#  it under the terms of the Mozilla Public Licence 2.0 as published by
#  the Mozilla Foundation, either version 2 of the License, or
#  (at your option) any later version.
#
#  Antares_Simulator is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
#  Mozilla Public Licence 2.0 for more details.
#
#  You should have received a copy of the Mozilla Public Licence 2.0
#  along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.

# Methods to run Antares simulator

import subprocess
import glob
import yaml
from pathlib import Path
from study_input_handler import study_input_handler
from output_utils import parse_output_folder_from_logs


def get_solver_path():
    with open("conf.yaml") as file:
        content = yaml.full_load(file)
    return content.get("antares-solver")


SOLVER_PATH = get_solver_path()  # we only need to run this once


def run_simulation(context):
    activate_simu_outputs(context)  # TODO : remove this and update studies instead
    command = build_antares_solver_command(context)
    print(f"Running command: {command}")
    process = subprocess.Popen(command, stdout=subprocess.PIPE, stderr=subprocess.DEVNULL)
    out, err = process.communicate()
    context.output_path = parse_output_folder_from_logs(out)
    context.return_code = process.returncode
    context.annual_system_cost = None
    context.hourly_values = None


def activate_simu_outputs(context):
    sih = study_input_handler(Path(context.study_path))
    sih.set_value(variable="synthesis", value="true", file_nick_name="general")
    sih.set_value(variable="year-by-year", value="true", file_nick_name="general")


def build_antares_solver_command(context):
    command = [SOLVER_PATH, "-i", f'"{context.study_path}"']
    if context.use_ortools:
        command.append('--use-ortools')
        command.append('--ortools-solver=' + context.ortools_solver)
    if context.named_mps_problems:
        command.append('--named-mps-problems')
    if context.parallel:
        command.append('--force-parallel=4')
    return command
