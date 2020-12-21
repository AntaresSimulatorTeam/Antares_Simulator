# Code used to generate test functions

SHORT_TESTS=[
    '001 One node - passive',
    '002 Thermal fleet - Base',
    '003 Thermal fleet - Must-run',
    '004 Thermal fleet - Partial must-run',
    '005 Thermal fleet - Minimum stable power and min up down times',
    '006 Thermal fleet - Extra costs',
    '007 Thermal fleet - Fast unit commitment',
    '008 Thermal fleet - Accurate unit commitment',
    '009 TS generation - Thermal power',
    '010 TS generation - Wind speed',
    '011 TS generation - Wind power - small scale',
    '012 TS Generation - Wind power - large scale',
    '013 TS Generation - Solar power',
    '014 TS generation - Load',
    '015 TS generation - Hydro power',
    '016 Probabilistic vs deterministic - 1',
    '018 Probabilistic vs deterministic - 3',
    '020 Single mesh - DC law',
    '021 Four areas - DC law',
    '022 Negative marginal price',
    '023 Anti-pricewise flows',
    '024 Hurdle costs - 1',
    '025 Hurdle costs - 2',
    '026 Day ahead reserve - 1',
    '027 Day ahead reserve - 2',
    '028 Pumped storage plant - 1',
    '029 Pumped storage plant - 2',
    '030 Pumped storage plant - 3',
    '031 Wind Analysis',
    '033 Mixed Expansion - Storage',
    '034 Mixed Expansion - Smart grid model 1',
    '036 Multistage study -1-Isolated systems',
    '037 Multistage study-2-Copperplate',
    '040 Multistage study-5-Derated',
    '044 PSP strategies-1-No PSP',
    '045 PSP strategies-2-Det pumping',
    '046 PSP strategies-3-Opt daily',
    '047 PSP strategies-4-Opt weekly',
    '053 System Map Editor - 1',
    '054 System Map Editor -2',
    '055 System Map Editor - 3',
    '056 System Map Editor - 4',
    '057 Four areas - Grid outages  01',
    '058 Four areas - Grid outages  02',
    '059 Four areas - Grid outages  03',
    '060 Four areas - Grid outages  04',
    '061 Four areas - Grid outages  05',
    '062 Grid-Topology changes on contingencies 01',
    '063 Grid-Topology changes on contingencies 02',
    '064 Probabilistic exchange capacity',
    '065 Pumped storage plant -explicit model-01',
    '066 Pumped storage plant -explicit model-02',
    '067 Pumped storage plant -explicit model-03',
    '068 Hydro Reservoir Model -enhanced-01',
    '069 Hydro Reservoir Model -enhanced-02',
    '070 Hydro Reservoir Model -enhanced-03',
    '071 Hydro Reservoir Model -enhanced-04',
    '072 Hydro Reservoir Model -enhanced-05',
    '073 Hydro Reservoir Model -enhanced-06',
    '074 KCG on Four areas -01',
    '075 KCG on Four areas -02']

MEDIUM_TESTS=[
    '000 Free Data Sample',
    '017 Probabilistic vs deterministic - 2',
    '032 Exploited Wind Fields',
    '035 Mixed Expansion - Smart grid model 2',
    '038 Multistage study-3-PTDF',
    '039 Multistage study-4-Kirchhoff',
    '041 Multistage study-6-NTC',
    '042 Multistage study-7-PTDF',
    '043 Multistage study-8-Kirchhoff',
    '048 Benchmark CPU RAM HDD - 1',
    '076 KCG on regional dataset 01',
    '077 KCG on regional dataset 02',
    '078 KCG on regional dataset 03']

LONG_TESTS=[
    '049 Benchmark CPU RAM HDD - 2',
    '050 Export MPS File',
    '051 Benchmark CPU RAM HDD - 3',
    '052 Benchmark CPU RAM HDD - 4',
    '079 Zero  Power Balance - Type 1',
    '080 Zero  Power Balance - Type 2',
    '081 Zero  Power Balance - Type 3',
    '082 Zero  Power Balance - Type 4',
    '083 Zero  Energy Balance - Daily',
    '084 Zero  Energy Balance - Weekly',
    '085 Zero  Energy Balance - Annual']


def format_function_name(name) -> str:
    name=name.replace(' ', '_')
    name=name.replace('-', '_')
    for i in range(3):
        name=name.replace('__','_')
    return name.lower()

for tab, name in [(SHORT_TESTS, 'short'), (MEDIUM_TESTS, 'medium'), (LONG_TESTS, 'long')]:
    for i in range(len(tab)):
        print(f"@pytest.mark.{name}")
        print(f"def test_{format_function_name(tab[i])}(use_ortools, ortools_solver, solver_path):")
        print(f"    study_path = ALL_STUDIES_PATH / \"{name}-tests\" / \"{tab[i]}\"")
        print(f"    run_study(solver_path, study_path, use_ortools, ortools_solver)")
        print(f"    check_integrity_first_opt(study_path)")
        print(f"    check_integrity_second_opt(study_path)\n")

