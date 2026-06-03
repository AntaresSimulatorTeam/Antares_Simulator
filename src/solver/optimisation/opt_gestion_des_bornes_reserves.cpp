// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/optimisation/opt_gestion_des_bornes_reserves.h"

#include "antares/solver/optimisation/opt_structure_probleme_a_resoudre.h"
#include "antares/solver/simulation/sim_structure_probleme_economique.h"
#include "antares/solver/simulation/simulation.h"

struct ReserveVariablesBoundsSetter
{
    PROBLEME_HEBDO* problemeHebdo;
    const std::unique_ptr<PROBLEME_ANTARES_A_RESOUDRE>& ProblemeAResoudre;
    std::vector<double>& Xmin;
    std::vector<double>& Xmax;
    std::vector<double*>& AdresseOuPlacerLaValeurDesVariablesOptimisees;
    int pdtJour = 0, pdtHebdo = 0, pays = 0;

    ReserveVariablesBoundsSetter(PROBLEME_HEBDO* hebdo):
        problemeHebdo(hebdo),
        ProblemeAResoudre(hebdo->ProblemeAResoudre),
        Xmin(ProblemeAResoudre->Xmin),
        Xmax(ProblemeAResoudre->Xmax),
        AdresseOuPlacerLaValeurDesVariablesOptimisees(
          ProblemeAResoudre->AdresseOuPlacerLaValeurDesVariablesOptimisees)
    {
    }

    void setPdtJour(int pdt)
    {
        pdtJour = pdt;
    }

    void setPdtHebdo(int pdt)
    {
        pdtHebdo = pdt;
    }

    void setPays(int p)
    {
        pays = p;
    }

    // Set variables bounds for a reserve
    void setReserveBounds(int reserveIdInArea, int reserveId)
    {
        const auto& indices = varIndices();
        auto& res = problemeHebdo->ResultatsHoraires[pays].Reserves.value()[pdtHebdo];

        setVarBounds(indices.internalUnsatisfied[reserveId],
                     &res.ValeursHorairesInternalUnsatisfied[reserveIdInArea]);

        setVarBounds(indices.internalExcess[reserveId],
                     &res.ValeursHorairesInternalExcessReserve[reserveIdInArea]);
    }

    // Set variables bounds for a Thermal cluster participation to a reserve up or down
    void setThermalReserveParticipationBounds(int clusterParticipationIdInArea,
                                              int clusterParticipationId,
                                              ReserveType type)
    {
        const auto& indices = varIndices();
        auto& prod = problemeHebdo->ResultatsHoraires[pays].ProductionThermique[pdtHebdo];

        setVarBounds(indices.runningThermalClusterParticipation[clusterParticipationId],
                     &prod.ParticipationReservesDuPalierOn.value()[clusterParticipationIdInArea]);

        if (type == ReserveType::UP)
        {
            setVarBounds(
              indices.offThermalClusterParticipation[clusterParticipationId],
              &prod.ParticipationReservesDuPalierOff.value()[clusterParticipationIdInArea]);
        }

        setVarBounds(indices.thermalClusterParticipation[clusterParticipationId],
                     &prod.ParticipationReservesDuPalier.value()[clusterParticipationIdInArea]);
    }

    // Set variables bounds for a ShortTerm cluster participation to a reserve up or down

    void setSTStorageReserveParticipationBounds(int clusterParticipationIdInArea,
                                                int clusterParticipationId,
                                                ReserveType type)
    {
        const auto& indices = varIndices();
        auto& st = problemeHebdo->ResultatsHoraires[pays]
                     .ShortTermStorageReserves.value()[clusterParticipationIdInArea];

        setVarBounds(indices.STStorageReleaseClusterParticipation[clusterParticipationId], nullptr);
        setVarBounds(indices.STStorageStoreClusterParticipation[clusterParticipationId], nullptr);

        int dirVar = indices.STStorageClusterParticipation[type][clusterParticipationId];

        setVarBounds(dirVar, &st.reserveParticipationOfCluster.value()[pdtHebdo]);
    }

    // Set variables bounds for a Hydro participation to a reserve up
    void setHydroReserveParticipationBounds(int clusterParticipationIdInArea,
                                            int clusterParticipationId,
                                            ReserveType type)

    {
        const auto& indices = varIndices();
        auto& hydroUsage = problemeHebdo->ResultatsHoraires[pays].HydroUsage[pdtHebdo];

        setVarBounds(indices.HydroReleaseParticipation[clusterParticipationId], nullptr);

        setVarBounds(indices.HydroStoreParticipation[clusterParticipationId], nullptr);

        setVarBounds(
          indices.HydroParticipation[type][clusterParticipationId],
          &hydroUsage.reserveParticipationOfCluster.value()[clusterParticipationIdInArea]);
    }

private:
    void setVarBounds(int var, double* adresseDuResultat)
    {
        Xmin[var] = 0;
        Xmax[var] = LINFINI_ANTARES;
        AdresseOuPlacerLaValeurDesVariablesOptimisees[var] = adresseDuResultat;
    }

    inline const CORRESPONDANCES_DES_VARIABLES::ReservesIndices& varIndices() const noexcept
    {
        return problemeHebdo->CorrespondanceVarNativesVarOptim[pdtJour].reservesIndices.value();
    }
};

void SetAllReservesBoundsForArea(ReserveVariablesBoundsSetter& setter,
                                 const std::vector<CAPACITY_RESERVATION>& areaCapacityReservations)
{
    for (const auto& areaReserve: areaCapacityReservations)
    {
        setter.setReserveBounds(areaReserve.areaReserveIndex, areaReserve.globalReserveIndex);

        for (const auto& [clusterId, thermal]: areaReserve.AllThermalReservesParticipation)
        {
            setter.setThermalReserveParticipationBounds(thermal.areaIndexClusterParticipation,
                                                        thermal.globalIndexClusterParticipation,
                                                        areaReserve.type);
        }

        for (const auto& [clusterId, stStorage]: areaReserve.AllSTStorageReservesParticipation)
        {
            setter.setSTStorageReserveParticipationBounds(stStorage.areaIndexClusterParticipation,
                                                          stStorage.globalIndexClusterParticipation,
                                                          areaReserve.type);
        }

        for (const auto& hydro: areaReserve.AllHydroReservesParticipation)
        {
            setter.setHydroReserveParticipationBounds(hydro.areaIndexClusterParticipation,
                                                      hydro.globalIndexClusterParticipation,
                                                      areaReserve.type);
        }
    }
}

void OPT_InitialiserLesBornesDesVariablesDuProblemeLineaireReserves(
  PROBLEME_HEBDO* problemeHebdo,
  const int PremierPdtDeLIntervalle,
  const int DernierPdtDeLIntervalle)
{
    ReserveVariablesBoundsSetter setter(problemeHebdo);

    for (int pdtHebdo = PremierPdtDeLIntervalle, pdtJour = 0; pdtHebdo < DernierPdtDeLIntervalle;
         ++pdtHebdo, ++pdtJour)
    {
        setter.setPdtJour(pdtJour);
        setter.setPdtHebdo(pdtHebdo);

        for (uint32_t pays = 0; pays < problemeHebdo->NombreDePays; ++pays)
        {
            setter.setPays(pays);

            const auto& areaReserves = problemeHebdo->allReserves.value()[pays];

            SetAllReservesBoundsForArea(setter, areaReserves.areaCapacityReservations);
        }
    }
}
