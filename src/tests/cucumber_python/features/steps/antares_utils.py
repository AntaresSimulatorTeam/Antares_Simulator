from behave import *
import subprocess
from pathlib import Path

def run_simulation(context):
    solver_full_path = str(Path(context.solver_path).resolve())

    command = [solver_full_path, "-i", str(context.study_path)]
    if context.use_ortools:
        command.append('--use-ortools')
        command.append('--ortools-solver=' + context.ortools_solver)
    if context.named_mps_problems:
        command.append('--named-mps-problems')
    if context.parallel:
        command.append('--force-parallel=4')

    process = subprocess.Popen(command, stdout=subprocess.PIPE, stderr=subprocess.DEVNULL)
    out, err = process.communicate()
    
    for line in out.splitlines():
        if b'Output folder : ' in line:
            context.output_path = line.split(b'Output folder : ')[1].decode('ascii')
            break
            
    context.return_code = process.returncode

    if not context.raise_exception_on_failure:
        return
    
def read_output_file_and_get_value_for(context, output_file, key):
    file = open(context.output_path + "/" + output_file, 'r')
    for line in file.readlines():
        if key in line:
            return line.split(key + " : ")[1]
    raise LookupError(f"Key {key} not found in output file {output_file}")