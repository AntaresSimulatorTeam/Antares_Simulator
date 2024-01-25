
import os
from utils.find_output import  find_dated_output_folder
from pathlib import Path
class CustomBenchmark:
    def __init__(self, study_path: Path, dest: Path) -> None:
        self.study_path = study_path
        output_path =  find_dated_output_folder(study_path)
        self.custom_benchmark_file = output_path / "custom_benchmark.txt"
        self.dest = dest
    
    def copy(self) : 
        content = ""
        with open(self.custom_benchmark_file, 'r') as f:
            content = f.read()
        
        with open(self.dest, "+a") as dest_file:
            if  os.stat(self.dest).st_size != 0:
                dest_file.write(",\n")
            dest_file.write(content)
              