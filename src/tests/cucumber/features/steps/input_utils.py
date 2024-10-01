from study_input_handler import study_input_handler
from pathlib import Path

def get_thermal_cluster_unitcount(study_path, area, cluster) -> int:
    sih = study_input_handler(Path(study_path))
    return int(sih.get_input(f"thermal/clusters/{area.lower()}/list.ini", cluster, "unitcount"))

def get_thermal_cluster_nominal_capacity(study_path, area, cluster) -> float:
    sih = study_input_handler(Path(study_path))
    return float(sih.get_input(f"thermal/clusters/{area.lower()}/list.ini", cluster, "nominalcapacity"))