import shutil

from os import remove
from os.path import isdir, isfile, basename
from utils.benchmark_utils import CustomBenchmark

class results_remover:
    def __init__(self, study_path,do_benchmark, custom_benchmark_json):
        self.study_path = study_path
        self.do_benchmark = do_benchmark
        self.custom_benchmark_json = custom_benchmark_json

    def run(self):
        if(self.do_benchmark):
            self.copy_benchmarks()
        self.remove_logs()
        self.clean_results()

    def remove_logs(self):
        logs_folder = self.study_path / 'logs'
        if isdir(logs_folder):
            shutil.rmtree(logs_folder)

    def clean_results(self):
        output_folder = self.study_path / 'output'
        for element in output_folder.iterdir():
            if isdir(element) and basename(element) != 'reference':
                shutil.rmtree(element)
            if isfile(element):
                remove(element)

    def copy_benchmarks(self):
        CustomBenchmark(self.study_path, self.custom_benchmark_json).copy()

