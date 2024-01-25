
import os
from utils.find_output import  find_dated_output_folder
from pathlib import Path
class CustomBenchmark:
    def __init__(self, study_path: Path, dest: Path) -> None:
        self.study_path = study_path
        output_path =  find_dated_output_folder(study_path)
        self.custom_benchmark_file = output_path / "custom_benchmark.txt"
        self.dest = Path(dest)
    
    def copy(self) : 
        content = ""
        with open(self.custom_benchmark_file, 'r') as f:
            content = f.read()
        
        empty_dest = False
        if not self.dest.parent.exists():
            self.dest.parent.mkdir()
            empty_dest = True

        with open(self.dest, "+a") as dest_file:
            if not empty_dest:
                dest_file.write(",\n")
            dest_file.write(content)
              