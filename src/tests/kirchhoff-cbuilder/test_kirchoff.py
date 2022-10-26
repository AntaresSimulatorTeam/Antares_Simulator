import subprocess
import sys

def run_and_compare(path, ref_path):
    res = subprocess.run([sys.argv[1], path])
    if res.returncode != 0:
        print("Exec of this study failed: " + path)
        exit(1)

    out_path = path + "input/bindingconstraints/"
    res = subprocess.run(["diff", "-bur", out_path, ref_path])
    if res.returncode != 0:
        print("Wrong results for study: " + path)
        exit(1)


subprocess.run(["ls",".."])
run_and_compare("/home/payetvin/Projects/Antares_Simulator/src/tests/resources/Antares_Simulator_Tests/medium-tests/039 Multistage study-4-Kirchhoff", "39")
