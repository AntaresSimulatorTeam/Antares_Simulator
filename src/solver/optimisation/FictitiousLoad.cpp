
#include "FictitiousLoad.h"
#include "opt_rename_problem.h"

void FictitiousLoad::add(int pdt, int pays, std::shared_ptr<FictitiousLoadData> data)
{
    /** can be done without this --- keep it for now**/
    data->NumeroDeContraintePourEviterLesChargesFictives[pays] = builder->data->nombreDeContraintes;

    /******/

    // TODO improve this
    {
        ConstraintNamer namer(builder->data->NomDesContraintes, builder->data->NamedProblems);

        namer.UpdateTimeStep(builder->data->weekInTheYear * 168 + pdt);
        namer.UpdateArea(builder->data->NomsDesPays[pays]);
        namer.FictiveLoads(builder->data->nombreDeContraintes);
    }

       builder->updateHourWithinWeek(pdt);
       new_exportPaliers(data->PaliersThermiquesDuPays, builder);
       auto coeff = data->DefaillanceNegativeUtiliserHydro[pays] ? -1 : 0;
       builder->include(NewVariable::HydProd(pays), coeff)
         .include(NewVariable::NegativeUnsuppliedEnergy(pays), 1.0);

       builder->lessThan();
       builder->build();
}
