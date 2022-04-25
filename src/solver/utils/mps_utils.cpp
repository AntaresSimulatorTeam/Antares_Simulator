#include <antares/study.h>
#include <antares/emergency.h>

#include "../simulation/simulation.h"

#include "mps_utils.h"
#include "named_problem.h"

using namespace Antares;
using namespace Antares::Data;

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

static void printRows(FILE* Flot,
                      int NombreDeContraintes,
                      const char* Sens,
                      const std::vector<std::string>* NomDesContraintes)
{
    char buffer[OPT_APPEL_SOLVEUR_BUFFER_SIZE];

    fprintf(Flot, "ROWS\n");
    fprintf(Flot, " N  OBJECTIF\n");

    for (int Cnt = 0; Cnt < NombreDeContraintes; Cnt++)
    {
        if (NomDesContraintes != nullptr)
        {
            SNPRINTF(
              buffer, OPT_APPEL_SOLVEUR_BUFFER_SIZE, "%s", (*NomDesContraintes)[Cnt].c_str());
        }
        else
        {
            SNPRINTF(buffer, OPT_APPEL_SOLVEUR_BUFFER_SIZE, "R%07d", Cnt);
        }

        if (Sens[Cnt] == '=')
        {
            fprintf(Flot, " E  %s\n", buffer);
        }
        else if (Sens[Cnt] == '<')
        {
            fprintf(Flot, " L  %s\n", buffer);
        }
        else if (Sens[Cnt] == '>')
        {
            fprintf(Flot, " G  %s\n", buffer);
        }
        else
        {
            logs.error() << "OPT_EcrireJeuDeDonneesMPS : Wrong direction for constraint no. "
                         << Sens[Cnt];
            AntaresSolverEmergencyShutdown();
        }
    }
}

void OPT_dump_spx_fixed_part(const Antares::Optimization::PROBLEME_SIMPLEXE_NOMME& Pb,
                             uint numSpace,
                             bool /* includeNames */)
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

    for (ilMax = -1, Cnt = 0; Cnt < Pb.NombreDeContraintes; Cnt++)
    {
        if ((Pb.IndicesDebutDeLigne[Cnt] + Pb.NombreDeTermesDesLignes[Cnt] - 1) > ilMax)
        {
            ilMax = Pb.IndicesDebutDeLigne[Cnt] + Pb.NombreDeTermesDesLignes[Cnt] - 1;
        }
    }

    ilMax += Pb.NombreDeContraintes;

    if (ilMax < 0)
    {
        logs.fatal() << "Invalid size detected";
        return;
    }

    Cder = (int*)malloc(Pb.NombreDeVariables * sizeof(int));
    Cdeb = (int*)malloc(Pb.NombreDeVariables * sizeof(int));
    NumeroDeContrainte = (int*)malloc(ilMax * sizeof(int));
    Csui = (int*)malloc(ilMax * sizeof(int));

    if (Cder == nullptr || Cdeb == nullptr || NumeroDeContrainte == nullptr || Csui == nullptr)
    {
        logs.fatal() << "Not enough memory";
        AntaresSolverEmergencyShutdown(2);
    }

    for (Var = 0; Var < Pb.NombreDeVariables; Var++)
        Cdeb[Var] = -1;

    for (Cnt = 0; Cnt < Pb.NombreDeContraintes; Cnt++)
    {
        il = Pb.IndicesDebutDeLigne[Cnt];
        ilMax = il + Pb.NombreDeTermesDesLignes[Cnt];
        while (il < ilMax)
        {
            Var = Pb.IndicesColonnes[il];
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
    Flot = study->createFileIntoOutputWithExtension("problem-fixed-part", "mps", numSpace);

    if (!Flot)
        AntaresSolverEmergencyShutdown(2);

    printHeader(Flot, Pb.NombreDeVariables, Pb.NombreDeContraintes);

    printRows(Flot, Pb.NombreDeContraintes, Pb.Sens, &Pb.NomDesContraintes);

    printColumnsObjective(Flot,
                          Pb.NombreDeVariables,
                          NumeroDeContrainte,
                          Pb.CoefficientsDeLaMatriceDesContraintes,
                          Cdeb,
                          Csui,
                          nullptr);

    fprintf(Flot, "ENDATA\n");

    free(Cdeb);
    free(NumeroDeContrainte);
    free(Csui);

    fclose(Flot);
}

void OPT_dump_spx_variable_part(const Antares::Optimization::PROBLEME_SIMPLEXE_NOMME& Pb,
                                uint numSpace,
                                bool /*includeNames*/)
{
    FILE* Flot;
    int Var;

    char buffer[OPT_APPEL_SOLVEUR_BUFFER_SIZE];

    auto study = Data::Study::Current::Get();
    Flot = study->createFileIntoOutputWithExtension("problem-variable-part", "mps", numSpace);

    if (!Flot)
        AntaresSolverEmergencyShutdown(2);

    printHeader(Flot, Pb.NombreDeVariables, Pb.NombreDeContraintes);

    fprintf(Flot, "COLUMNS\n");
    for (Var = 0; Var < Pb.NombreDeVariables; Var++)
    {
        if (Pb.CoutLineaire[Var] != 0.0)
        {
            SNPRINTF(buffer, OPT_APPEL_SOLVEUR_BUFFER_SIZE, "%-.10lf", Pb.CoutLineaire[Var]);
            fprintf(Flot, "    C%07d  OBJECTIF  %s\n", Var, buffer);
        }
    }

    printRHS(Flot, Pb.NombreDeContraintes, Pb.SecondMembre);

    printBounds(Flot, Pb.NombreDeVariables, Pb.TypeDeVariable, Pb.Xmin, Pb.Xmax);

    fprintf(Flot, "ENDATA\n");

    fclose(Flot);
}

void OPT_EcrireJeuDeDonneesLineaireAuFormatMPS(void* Prob, uint numSpace, bool /*includeNames*/)
{
    using PSN = Antares::Optimization::PROBLEME_SIMPLEXE_NOMME;
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
    PSN* Probleme;

    int NombreDeVariables;
    double* CoutLineaire;
    int NombreDeContraintes;
    int* IndicesDebutDeLigne;
    int* NombreDeTermesDesLignes;
    int* IndicesColonnes;
    int ExistenceDUneSolution;
    double* X;

    Probleme = static_cast<PSN*>(Prob);

    ExistenceDUneSolution = Probleme->ExistenceDUneSolution;
    if (ExistenceDUneSolution == OUI_SPX)
        ExistenceDUneSolution = OUI_ANTARES;

    NombreDeVariables = Probleme->NombreDeVariables;
    X = Probleme->X;
    CoutLineaire = Probleme->CoutLineaire;
    NombreDeContraintes = Probleme->NombreDeContraintes;
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
    Flot = study->createFileIntoOutputWithExtension("problem", "mps", numSpace);

    if (!Flot)
        AntaresSolverEmergencyShutdown(2);

    printHeader(Flot, NombreDeVariables, NombreDeContraintes);

    printRows(Flot, NombreDeContraintes, Probleme->Sens, &Probleme->NomDesContraintes);

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
