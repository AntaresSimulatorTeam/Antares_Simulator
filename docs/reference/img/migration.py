import numpy as np

NB_DAYS=365
NB_HOURS=8760
NB_HOURS_PER_DAY=24

class Legacy:
    GEN_POWER=0
    GEN_ENERGY=1
    PUMP_POWER=2
    PUMP_ENERGY=3
    def __init__(self):
        self.dailyAll = np.zeros((NB_DAYS, 4))

class New:
    def __init__(self, NbScenarios=1):
        self.dailyPumpEnergy = np.zeros(NB_DAYS)
        self.dailyGenEnergy = np.zeros(NB_DAYS)
        self.hourlyGenPower = np.zeros((NB_HOURS, NbScenarios))
        self.hourlyPumpPower = np.zeros((NB_HOURS, NbScenarios))

def Migration(legacy):
    new = New()
    new.dailyGenEnergy = legacy.dailyAll[:, Legacy.GEN_ENERGY]
    new.dailyPumpEnergy = legacy.dailyAll[:, Legacy.PUMP_ENERGY]
    for day in range(NB_DAYS):
        for hour in range(NB_HOURS_PER_DAY):
            new.hourlyGenPower[NB_HOURS_PER_DAY * day + hour] = (legacy.dailyAll[day, Legacy.GEN_ENERGY] / NB_HOURS_PER_DAY) * legacy.dailyAll[day, Legacy.GEN_POWER]
            new.hourlyPumpPower[NB_HOURS_PER_DAY * day + hour] = (legacy.dailyAll[day, Legacy.PUMP_ENERGY] / NB_HOURS_PER_DAY) * legacy.dailyAll[day, Legacy.PUMP_POWER]
    return new

