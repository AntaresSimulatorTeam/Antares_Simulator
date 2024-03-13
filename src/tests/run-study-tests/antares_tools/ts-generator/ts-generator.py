import pytest
import subprocess
import sys

from pathlib import Path
from utils.find_output import find_dated_output_folder
from utils.find_reference import find_reference_folder
from parse_studies.look_for_studies import look_for_studies

def get_ts_files(path):
    ts_files = list(path.glob('**/*.txt'))
    assert ts_files
    return ts_files

def get_clusters_to_gen(path):
    clusterToGenFile = open(path / "clustersToGen.txt", 'r')
    line = clusterToGenFile.readline().rstrip() # remove new line char
    clusterToGenFile.close()
    return line

def run_and_compare(exe, path):
    clusterToGenFile = open(path / "clustersToGen.txt", 'r')
    line = clusterToGenFile.readline().rstrip() # remove new line char
    clusterToGenFile.close()

    res = subprocess.run([exe, "--thermal=" + line, path])
    assert (res.returncode == 0), "The exec failed for study: " + str(path)

    ref_path = Path(find_reference_folder(path)) / "ts-generator"
    out_path = Path(find_dated_output_folder(path)) / "ts-generator"

    ref_ts_files = get_ts_files(ref_path)
    ts_files = get_ts_files(out_path)

    # get pairs of same file to compare
    # we need to have the correct cluster and area pair, that's why we check ts.parent
    list_of_pairs = [(ts_ref, ts) for ts_ref in ref_ts_files for ts in ts_files if ts_ref.name == ts.name and ts_ref.parent == ts.parent]
    for pair in list_of_pairs:
        print(pair[0], file=sys.stderr)
        print(pair[1], file=sys.stderr)
        ref_content = open(pair[0]).read()
        output_content = open(pair[1]).read()
        check(ref_content == output_content, f"Difference between files {pair[0]} and {pair[1]}")



# main: get studies and iterate on them
ROOT_FOLDER = Path('../resources/batches').resolve()
study_paths = look_for_studies(ROOT_FOLDER)

@pytest.mark.tsgenerator
def test_ts_generator(tsgenerator_path):
    for study in study_paths:
        run_and_compare(tsgenerator_path, study)
