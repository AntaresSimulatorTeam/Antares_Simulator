#include <antares/study.h>
#include <antares/emergency.h>

#include "../simulation/simulation.h"

#include "mps_utils.h"


#ifdef _MSC_VER
#define SNPRINTF sprintf_s
#else
#define SNPRINTF snprintf
#endif

constexpr size_t OPT_APPEL_SOLVEUR_BUFFER_SIZE = 256;

static void printHeader(FILE* Flot, int NombreDeVariables, int NombreDeContraintes)
{
    fprintf(Flot, "* Number of variables:   %d\n", NombreDeVariables);
    fprintf(Flot, "* Number of constraints: %d\n", NombreDeContraintes);
    fprintf(Flot, "NAME          Pb Solve\n");
}

static void printColumnsObjective(FILE* Flot,
                                  int NombreDeVariables,
                                  const int* NumeroDeContrainte,
                                  const double* CoefficientsDeLaMatriceDesContraintes,
                                  const int* Cdeb,
                                  const int* Csui,
                                  const double* CoutLineaire)
{
    char buffer[OPT_APPEL_SOLVEUR_BUFFER_SIZE];
    int il;

    fprintf(Flot, "COLUMNS\n");
    for (int Var = 0; Var < NombreDeVariables; Var++)
    {
        if (CoutLineaire && CoutLineaire[Var] != 0.0)
        {
            SNPRINTF(buffer, OPT_APPEL_SOLVEUR_BUFFER_SIZE, "%-.10lf", CoutLineaire[Var]);
            fprintf(Flot, "    C%07d  OBJECTIF  %s\n", Var, buffer);
        }

        il = Cdeb[Var];
        while (il >= 0)
        {
            SNPRINTF(buffer,
                     OPT_APPEL_SOLVEUR_BUFFER_SIZE,
                     "%-.10lf",
                     CoefficientsDeLaMatriceDesContraintes[il]);
            fprintf(Flot, "    C%07d  R%07d  %s\n", Var, NumeroDeContrainte[il], buffer);
            il = Csui[il];
        }
    }
}

static void printBounds(FILE* Flot,
                        int NombreDeVariables,
                        const int* TypeDeBorneDeLaVariable,
                        const double* Xmin,
                        const double* Xmax)
{
    char buffer[OPT_APPEL_SOLVEUR_BUFFER_SIZE];

    fprintf(Flot, "BOUNDS\n");

    for (int Var = 0; Var < NombreDeVariables; Var++)
    {
        if (TypeDeBorneDeLaVariable[Var] == VARIABLE_FIXE)
        {
            SNPRINTF(buffer, OPT_APPEL_SOLVEUR_BUFFER_SIZE, "%-.9lf", Xmin[Var]);

            fprintf(Flot, " FX BNDVALUE  C%07d  %s\n", Var, buffer);
            continue;
        }

        if (TypeDeBorneDeLaVariable[Var] == VARIABLE_BORNEE_DES_DEUX_COTES)
        {
            if (Xmin[Var] != 0.0)
            {
                SNPRINTF(buffer, OPT_APPEL_SOLVEUR_BUFFER_SIZE, "%-.9lf", Xmin[Var]);
                fprintf(Flot, " LO BNDVALUE  C%07d  %s\n", Var, buffer);
            }

            SNPRINTF(buffer, OPT_APPEL_SOLVEUR_BUFFER_SIZE, "%-.9lf", Xmax[Var]);
            fprintf(Flot, " UP BNDVALUE  C%07d  %s\n", Var, buffer);
        }

        if (TypeDeBorneDeLaVariable[Var] == VARIABLE_BORNEE_INFERIEUREMENT)
        {
            if (Xmin[Var] != 0.0)
            {
                SNPRINTF(buffer, OPT_APPEL_SOLVEUR_BUFFER_SIZE, "%-.9lf", Xmin[Var]);
                fprintf(Flot, " LO BNDVALUE  C%07d  %s\n", Var, buffer);
            }
        }

        if (TypeDeBorneDeLaVariable[Var] == VARIABLE_BORNEE_SUPERIEUREMENT)
        {
            fprintf(Flot, " MI BNDVALUE  C%07d\n", Var);
            if (Xmax[Var] != 0.0)
            {
                SNPRINTF(buffer, OPT_APPEL_SOLVEUR_BUFFER_SIZE, "%-.9lf", Xmax[Var]);
                fprintf(Flot, " UP BNDVALUE  C%07d  %s\n", Var, buffer);
            }
        }

        if (TypeDeBorneDeLaVariable[Var] == VARIABLE_NON_BORNEE)
        {
            fprintf(Flot, " FR BNDVALUE  C%07d\n", Var);
        }
    }
}

static void printRHS(FILE* Flot, int NombreDeContraintes, const double* SecondMembre)
{
    char buffer[OPT_APPEL_SOLVEUR_BUFFER_SIZE];

    fprintf(Flot, "RHS\n");
    for (int Cnt = 0; Cnt < NombreDeContraintes; Cnt++)
    {
        if (SecondMembre[Cnt] != 0.0)
        {
            SNPRINTF(buffer, OPT_APPEL_SOLVEUR_BUFFER_SIZE, "%-.9lf", SecondMembre[Cnt]);
            fprintf(Flot, "    RHSVAL    R%07d  %s\n", Cnt, buffer);
        }
    }
}

void OPT_dump_spx_fixed_part(const PROBLEME_SIMPLEXE* Pb, int currentOptimNumber, uint numSpace)
{
    FILE* Flot;
    int Cnt;
    int Var;
    int il;
    int ilk;
    int ilMax;
    int* Cder;
    int* Cdeb;
    int* NumeroDeContrainte;
    int* Csui;

    for (ilMax = -1, Cnt = 0; Cnt < Pb->NombreDeContraintes; Cnt++)
    {
        if ((Pb->IndicesDebutDeLigne[Cnt] + Pb->NombreDeTermesDesLignes[Cnt] - 1) > ilMax)
        {
            ilMax = Pb->IndicesDebutDeLigne[Cnt] + Pb->NombreDeTermesDesLignes[Cnt] - 1;
        }
    }

    ilMax += Pb->NombreDeContraintes;

    if (ilMax < 0)
    {
        logs.fatal() << "Invalid size detected";
        return;
    }

    Cder = (int*)malloc(Pb->NombreDeVariables * sizeof(int));
    Cdeb = (int*)malloc(Pb->NombreDeVariables * sizeof(int));
    NumeroDeContrainte = (int*)malloc(ilMax * sizeof(int));
    Csui = (int*)malloc(ilMax * sizeof(int));

    if (Cder == nullptr || Cdeb == nullptr || NumeroDeContrainte == nullptr || Csui == nullptr)
    {
        logs.fatal() << "Not enough memory";
        AntaresSolverEmergencyShutdown(2);
    }

    for (Var = 0; Var < Pb->NombreDeVariables; Var++)
        Cdeb[Var] = -1;

    for (Cnt = 0; Cnt < Pb->NombreDeContraintes; Cnt++)
    {
        il = Pb->IndicesDebutDeLigne[Cnt];
        ilMax = il + Pb->NombreDeTermesDesLignes[Cnt];
        while (il < ilMax)
        {
            Var = Pb->IndicesColonnes[il];
            if (Cdeb[Var] < 0)
            {
                Cdeb[Var] = il;
                NumeroDeContrainte[il] = Cnt;
                Csui[il] = -1;
                Cder[Var] = il;
            }
            else
            {
                ilk = Cder[Var];
                Csui[ilk] = il;
                NumeroDeContrainte[il] = Cnt;
                Csui[il] = -1;
                Cder[Var] = il;
            }

            il++;
        }
    }

    free(Cder);

    auto study = Data::Study::Current::Get();
    Flot = study->createFileIntoOutputWithExtension("problem-fixed-part", "mps", numSpace, currentOptimNumber);

    if (!Flot)
        AntaresSolverEmergencyShutdown(2);

    printHeader(Flot, Pb->NombreDeVariables, Pb->NombreDeContraintes);

    fprintf(Flot, "ROWS\n");
    fprintf(Flot, " N  OBJECTIF\n");

    for (Cnt = 0; Cnt < Pb->NombreDeContraintes; Cnt++)
    {
        if (Pb->Sens[Cnt] == '=')
        {
            fprintf(Flot, " E  R%07d\n", Cnt);
        }
        else if (Pb->Sens[Cnt] == '<')
        {
            fprintf(Flot, " L  R%07d\n", Cnt);
        }
        else if (Pb->Sens[Cnt] == '>')
        {
            fprintf(Flot, " G  R%07d\n", Cnt);
        }
        else
        {
            fprintf(Flot,
                    "Writing fixed part of MPS data : le sens de la contrainte %c ne fait pas "
                    "partie des sens reconnus\n",
                    Pb->Sens[Cnt]);
            AntaresSolverEmergencyShutdown();
        }
    }

    printColumnsObjective(Flot,
                          Pb->NombreDeVariables,
                          NumeroDeContrainte,
                          Pb->CoefficientsDeLaMatriceDesContraintes,
                          Cdeb,
                          Csui,
                          nullptr);

    fprintf(Flot, "ENDATA\n");

    free(Cdeb);
    free(NumeroDeContrainte);
    free(Csui);

    fclose(Flot);
}

void OPT_dump_spx_variable_part(const PROBLEME_SIMPLEXE* Pb, int currentOptimNumber, uint numSpace)
{
    FILE* Flot;
    int Var;

    char buffer[OPT_APPEL_SOLVEUR_BUFFER_SIZE];

    auto study = Data::Study::Current::Get();
    Flot = study->createFileIntoOutputWithExtension("problem-variable-part", "mps", numSpace, currentOptimNumber);

    if (!Flot)
        AntaresSolverEmergencyShutdown(2);

    printHeader(Flot, Pb->NombreDeVariables, Pb->NombreDeContraintes);

    fprintf(Flot, "COLUMNS\n");
    for (Var = 0; Var < Pb->NombreDeVariables; Var++)
    {
        if (Pb->CoutLineaire[Var] != 0.0)
        {
            SNPRINTF(buffer, OPT_APPEL_SOLVEUR_BUFFER_SIZE, "%-.10lf", Pb->CoutLineaire[Var]);
            fprintf(Flot, "    C%07d  OBJECTIF  %s\n", Var, buffer);
        }
    }

    printRHS(Flot, Pb->NombreDeContraintes, Pb->SecondMembre);

    printBounds(Flot, Pb->NombreDeVariables, Pb->TypeDeVariable, Pb->Xmin, Pb->Xmax);

    fprintf(Flot, "ENDATA\n");

    fclose(Flot);
}

void OPT_EcrireJeuDeDonneesLineaireAuFormatMPS(void* Prob, int currentOptimNumber, uint numSpace)
{
    FILE* Flot;
    int Cnt;
    int Var;
    int il;
    int ilk;
    int ilMax;
    int* Cder;
    int* Cdeb;
    int* NumeroDeContrainte;
    int* Csui;
    double CoutOpt;
    PROBLEME_SIMPLEXE* Probleme;

    int NombreDeVariables;
    double* CoutLineaire;
    int NombreDeContraintes;
    char* Sens;
    int* IndicesDebutDeLigne;
    int* NombreDeTermesDesLignes;
    int* IndicesColonnes;
    int ExistenceDUneSolution;
    double* X;

    Probleme = (PROBLEME_SIMPLEXE*)Prob;

    ExistenceDUneSolution = Probleme->ExistenceDUneSolution;
    if (ExistenceDUneSolution == OUI_SPX)
        ExistenceDUneSolution = OUI_ANTARES;

    NombreDeVariables = Probleme->NombreDeVariables;
    X = Probleme->X;
    CoutLineaire = Probleme->CoutLineaire;
    NombreDeContraintes = Probleme->NombreDeContraintes;
    Sens = Probleme->Sens;
    IndicesDebutDeLigne = Probleme->IndicesDebutDeLigne;
    NombreDeTermesDesLignes = Probleme->NombreDeTermesDesLignes;
    IndicesColonnes = Probleme->IndicesColonnes;

    if (ExistenceDUneSolution == OUI_ANTARES)
    {
        CoutOpt = 0;
        for (Var = 0; Var < NombreDeVariables; Var++)
            CoutOpt += CoutLineaire[Var] * X[Var];
    }

    for (ilMax = -1, Cnt = 0; Cnt < NombreDeContraintes; Cnt++)
    {
        if ((IndicesDebutDeLigne[Cnt] + NombreDeTermesDesLignes[Cnt] - 1) > ilMax)
        {
            ilMax = IndicesDebutDeLigne[Cnt] + NombreDeTermesDesLignes[Cnt] - 1;
        }
    }

    ilMax += NombreDeContraintes;

    Cder = (int*)malloc(NombreDeVariables * sizeof(int));
    Cdeb = (int*)malloc(NombreDeVariables * sizeof(int));
    NumeroDeContrainte = (int*)malloc(ilMax * sizeof(int));
    Csui = (int*)malloc(ilMax * sizeof(int));

    if (Cder == nullptr || Cdeb == nullptr || NumeroDeContrainte == nullptr || Csui == nullptr)
    {
        logs.fatal() << "Not enough memory";
        AntaresSolverEmergencyShutdown();
    }

    for (Var = 0; Var < NombreDeVariables; Var++)
        Cdeb[Var] = -1;

    for (Cnt = 0; Cnt < NombreDeContraintes; Cnt++)
    {
        il = IndicesDebutDeLigne[Cnt];
        ilMax = il + NombreDeTermesDesLignes[Cnt];
        while (il < ilMax)
        {
            Var = IndicesColonnes[il];
            if (Cdeb[Var] < 0)
            {
                Cdeb[Var] = il;
                NumeroDeContrainte[il] = Cnt;
                Csui[il] = -1;
                Cder[Var] = il;
            }
            else
            {
                ilk = Cder[Var];
                Csui[ilk] = il;
                NumeroDeContrainte[il] = Cnt;
                Csui[il] = -1;
                Cder[Var] = il;
            }

            il++;
        }
    }

    free(Cder);

    auto study = Data::Study::Current::Get();
    Flot = study->createFileIntoOutputWithExtension("problem", "mps", numSpace, currentOptimNumber);

    if (!Flot)
        AntaresSolverEmergencyShutdown(2);

    printHeader(Flot, NombreDeVariables, NombreDeContraintes);

    fprintf(Flot, "ROWS\n");
    fprintf(Flot, " N  OBJECTIF\n");

    for (Cnt = 0; Cnt < NombreDeContraintes; Cnt++)
    {
        if (Sens[Cnt] == '=')
        {
            fprintf(Flot, " E  R%07d\n", Cnt);
        }
        else if (Sens[Cnt] == '<')
        {
            fprintf(Flot, " L  R%07d\n", Cnt);
        }
        else if (Sens[Cnt] == '>')
        {
            fprintf(Flot, " G  R%07d\n", Cnt);
        }
        else
        {
            logs.error() << "OPT_EcrireJeuDeDonneesMPS : Wrong direction for constraint no. "
                         << Sens[Cnt];
            AntaresSolverEmergencyShutdown();
        }
    }

    printColumnsObjective(Flot,
                          Probleme->NombreDeVariables,
                          NumeroDeContrainte,
                          Probleme->CoefficientsDeLaMatriceDesContraintes,
                          Cdeb,
                          Csui,
                          Probleme->CoutLineaire);

    printRHS(Flot, Probleme->NombreDeContraintes, Probleme->SecondMembre);

    printBounds(
      Flot, Probleme->NombreDeVariables, Probleme->TypeDeVariable, Probleme->Xmin, Probleme->Xmax);

    fprintf(Flot, "ENDATA\n");

    free(Cdeb);
    free(NumeroDeContrainte);
    free(Csui);

    fclose(Flot);
}



// --------------------
// Full mps writing
// --------------------
fullMPSwriter::fullMPSwriter(
        PROBLEME_SIMPLEXE_NOMME* named_splx_problem, 
        int currentOptimNumber, 
        uint thread_number) :
    named_splx_problem_(named_splx_problem),
    current_optim_number_(currentOptimNumber),
    thread_number_(thread_number)
{}
void fullMPSwriter::runIfNeeded()
{
    OPT_EcrireJeuDeDonneesLineaireAuFormatMPS((void*)named_splx_problem_, current_optim_number_, thread_number_);
}

// ---------------------------------
// Full mps writing by or-tools
// ---------------------------------
fullOrToolsMPSwriter::fullOrToolsMPSwriter(
        MPSolver* solver, 
        int currentOptimNumber, 
        uint thread_number) :
    solver_(solver), 
    current_optim_number_(currentOptimNumber),
    thread_number_(thread_number)
{}
void fullOrToolsMPSwriter::runIfNeeded()
{
    // Make or-tools print the MPS files leads to a crash ! 
    ORTOOLS_EcrireJeuDeDonneesLineaireAuFormatMPS(solver_, thread_number_, current_optim_number_);
}


// ---------------------------------
// mps written under split form
// ---------------------------------
splitMPSwriter::splitMPSwriter(
        PROBLEME_SIMPLEXE_NOMME* named_splx_problem,
        int currentOptimNumber,
        uint thread_nb, 
        bool simu_1st_week) :
    named_splx_problem_(named_splx_problem),
    current_optim_number_(currentOptimNumber),
    thread_nb_(thread_nb), 
    simu_1st_week_(simu_1st_week)
{}

void splitMPSwriter::runIfNeeded()
{
    if (simu_1st_week_)
        OPT_dump_spx_fixed_part(named_splx_problem_, current_optim_number_, thread_nb_);

    OPT_dump_spx_variable_part(named_splx_problem_, current_optim_number_, thread_nb_);
}

mpsWriterFactory::mpsWriterFactory(
        PROBLEME_HEBDO* ProblemeHebdo,
        int NumIntervalle,
        PROBLEME_SIMPLEXE_NOMME* named_splx_problem,
        bool ortoolsUsed,
        MPSolver* solver,
        uint thread_number) :
    pb_hebdo_(ProblemeHebdo),
    num_intervalle_(NumIntervalle),
    named_splx_problem_(named_splx_problem),
    ortools_used_(ortoolsUsed),
    solver_(solver),
    thread_number_(thread_number)
{
    current_optim_number_ = pb_hebdo_->numeroOptimisation[num_intervalle_];
    export_mps_ = pb_hebdo_->ExportMPS;
    export_mps_on_error_ = pb_hebdo_->exportMPSOnError;
    split_mps_ = pb_hebdo_->SplitExportedMPS;
    is_first_week_of_year_ = pb_hebdo_->firstWeekOfSimulation;
}

bool mpsWriterFactory::doWeExportMPS()
{
    if (export_mps_ == Data::mpsExportStatus::EXPORT_BOTH_OPTIMS)
        return true;
    if (static_cast<int>(export_mps_) == current_optim_number_)
        return true;
    return false;
}

std::unique_ptr<I_MPS_writer> mpsWriterFactory::create()
{
    if (doWeExportMPS() && split_mps_)
    {
        return std::make_unique<splitMPSwriter>(named_splx_problem_, current_optim_number_, thread_number_, is_first_week_of_year_);
    }
    if (doWeExportMPS() && not split_mps_)
    {
        return createFullmpsWriter();
    }

    return std::make_unique<nullMPSwriter>();
}

std::unique_ptr<I_MPS_writer> mpsWriterFactory::createOnOptimizationError()
{
    if (export_mps_on_error_ && not doWeExportMPS())
    {
        return createFullmpsWriter();
    }

    return std::make_unique<nullMPSwriter>();
}

std::unique_ptr<I_MPS_writer> mpsWriterFactory::createFullmpsWriter()
{
    if (ortools_used_)
    {
        return std::make_unique<fullOrToolsMPSwriter>(solver_, current_optim_number_, thread_number_);
    }
    else
    {
        return std::make_unique<fullMPSwriter>(named_splx_problem_, current_optim_number_, thread_number_);
    }
}