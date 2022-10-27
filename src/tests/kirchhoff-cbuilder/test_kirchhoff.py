import subprocess
import sys

nb_test = 3

files = [[0 for x in range(2)] for y in range(nb_test)]
files[0][0] = "../resources/Antares_Simulator_Tests/medium-tests/039 Multistage study-4-Kirchhoff"
files[0][1] = "reference/39"
files[1][0] = "../resources/Antares_Simulator_Tests/medium-tests/043 Multistage study-8-Kirchhoff"
files[1][1] = "reference/43"
files[2][0] = "../resources/Antares_Simulator_Tests/long-tests/079 Zero  Power Balance - Type 1"
files[2][1] = "reference/79"


def run_and_compare(path, ref_path):
    res = subprocess.run([sys.argv[1], path])
    if res.returncode != 0:
        print("\nExec of this study failed: " + path)
        exit(1)

    out_path = path + "/input/bindingconstraints/"
    res = subprocess.run(["diff", "-bur", out_path, ref_path])
    if res.returncode != 0:
        print("\nWrong results for study: " + path)
        exit(1)

for i in range(nb_test):
    run_and_compare(files[i][0], files[i][1])

print()
for i in range(nb_test):
    print("Test OK: " + files[i][0])

print("\nAll tests succeeded, exiting now")
