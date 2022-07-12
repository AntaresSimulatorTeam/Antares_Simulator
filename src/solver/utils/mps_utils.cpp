#include <antares/study.h>
#include <antares/emergency.h>

#include "../simulation/simulation.h"

#include "mps_utils.h"

using namespace Antares;
using namespace Antares::Data;

#ifdef _MSC_VER
#define SNPRINTF sprintf_s
#else
#define SNPRINTF snprintf
#endif

constexpr size_t OPT_APPEL_SOLVEUR_BUFFER_SIZE = 256;

static inline void check(int ret, const char* func, size_t line)
{
    if (ret < 0)
    {
        logs.error() << "MPS Writer - write failed in function " << func << " at line " << line;
        AntaresSolverEmergencyShutdown(2);
    }
}

static void printHeader(FILE* Flot, int NombreDeVariables, int NombreDeContraintes)
{
    check(fprintf(Flot, "* Number of variables:   %d\n", NombreDeVariables), __func__, __LINE__);
    check(fprintf(Flot, "* Number of constraints: %d\n", NombreDeContraintes), __func__, __LINE__);
    check(fprintf(Flot, "NAME          Pb Solve\n"), __func__, __LINE__);
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

    check(fprintf(Flot, "COLUMNS\n"), __func__, __LINE__);
    for (int Var = 0; Var < NombreDeVariables; Var++)
    {
        if (CoutLineaire && CoutLineaire[Var] != 0.0)
        {
            SNPRINTF(buffer, OPT_APPEL_SOLVEUR_BUFFER_SIZE, "%-.10lf", CoutLineaire[Var]);
            check(fprintf(Flot, "    C%07d  OBJECTIF  %s\n", Var, buffer), __func__, __LINE__);
        }

        il = Cdeb[Var];
        while (il >= 0)
        {
            SNPRINTF(buffer,
                     OPT_APPEL_SOLVEUR_BUFFER_SIZE,
                     "%-.10lf",
                     CoefficientsDeLaMatriceDesContraintes[il]);
            check(fprintf(Flot, "    C%07d  R%07d  %s\n", Var, NumeroDeContrainte[il], buffer),
                  __func__,
                  __LINE__);
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

    check(fprintf(Flot, "BOUNDS\n"), __func__, __LINE__);

    for (int Var = 0; Var < NombreDeVariables; Var++)
    {
        if (TypeDeBorneDeLaVariable[Var] == VARIABLE_FIXE)
        {
            SNPRINTF(buffer, OPT_APPEL_SOLVEUR_BUFFER_SIZE, "%-.9lf", Xmin[Var]);

            check(fprintf(Flot, " FX BNDVALUE  C%07d  %s\n", Var, buffer), __func__, __LINE__);
            continue;
        }

        if (TypeDeBorneDeLaVariable[Var] == VARIABLE_BORNEE_DES_DEUX_COTES)
        {
            if (Xmin[Var] != 0.0)
            {
                SNPRINTF(buffer, OPT_APPEL_SOLVEUR_BUFFER_SIZE, "%-.9lf", Xmin[Var]);
                check(fprintf(Flot, " LO BNDVALUE  C%07d  %s\n", Var, buffer), __func__, __LINE__);
            }

            SNPRINTF(buffer, OPT_APPEL_SOLVEUR_BUFFER_SIZE, "%-.9lf", Xmax[Var]);
            check(fprintf(Flot, " UP BNDVALUE  C%07d  %s\n", Var, buffer), __func__, __LINE__);
        }

        if (TypeDeBorneDeLaVariable[Var] == VARIABLE_BORNEE_INFERIEUREMENT)
        {
            if (Xmin[Var] != 0.0)
            {
                SNPRINTF(buffer, OPT_APPEL_SOLVEUR_BUFFER_SIZE, "%-.9lf", Xmin[Var]);
                check(fprintf(Flot, " LO BNDVALUE  C%07d  %s\n", Var, buffer), __func__, __LINE__);
            }
        }

        if (TypeDeBorneDeLaVariable[Var] == VARIABLE_BORNEE_SUPERIEUREMENT)
        {
            check(fprintf(Flot, " MI BNDVALUE  C%07d\n", Var), __func__, __LINE__);
            if (Xmax[Var] != 0.0)
            {
                SNPRINTF(buffer, OPT_APPEL_SOLVEUR_BUFFER_SIZE, "%-.9lf", Xmax[Var]);
                check(fprintf(Flot, " UP BNDVALUE  C%07d  %s\n", Var, buffer), __func__, __LINE__);
            }
        }

        if (TypeDeBorneDeLaVariable[Var] == VARIABLE_NON_BORNEE)
        {
            check(fprintf(Flot, " FR BNDVALUE  C%07d\n", Var), __func__, __LINE__);
        }
    }
}

static void printRHS(FILE* Flot, int NombreDeContraintes, const double* SecondMembre)
{
    char buffer[OPT_APPEL_SOLVEUR_BUFFER_SIZE];

    check(fprintf(Flot, "RHS\n"), __func__, __LINE__);
    for (int Cnt = 0; Cnt < NombreDeContraintes; Cnt++)
    {
        if (SecondMembre[Cnt] != 0.0)
        {
            SNPRINTF(buffer, OPT_APPEL_SOLVEUR_BUFFER_SIZE, "%-.9lf", SecondMembre[Cnt]);
            check(fprintf(Flot, "    RHSVAL    R%07d  %s\n", Cnt, buffer), __func__, __LINE__);
        }
    }
}

void OPT_dump_spx_fixed_part(const PROBLEME_SIMPLEXE* Pb, uint numSpace)
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
    Flot = study->createFileIntoOutputWithExtension("problem-fixed-part", "mps", numSpace);

    if (!Flot)
        AntaresSolverEmergencyShutdown(2);

    printHeader(Flot, Pb->NombreDeVariables, Pb->NombreDeContraintes);

    check(fprintf(Flot, "ROWS\n"), __func__, __LINE__);
    check(fprintf(Flot, " N  OBJECTIF\n"), __func__, __LINE__);

    for (Cnt = 0; Cnt < Pb->NombreDeContraintes; Cnt++)
    {
        if (Pb->Sens[Cnt] == '=')
        {
            check(fprintf(Flot, " E  R%07d\n", Cnt), __func__, __LINE__);
        }
        else if (Pb->Sens[Cnt] == '<')
        {
            check(fprintf(Flot, " L  R%07d\n", Cnt), __func__, __LINE__);
        }
        else if (Pb->Sens[Cnt] == '>')
        {
            check(fprintf(Flot, " G  R%07d\n", Cnt), __func__, __LINE__);
        }
        else
        {
            logs.error() << __func__ << " : Wrong direction for constraint no. " << Cnt;
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

    check(fprintf(Flot, "ENDATA\n"), __func__, __LINE__);

    free(Cdeb);
    free(NumeroDeContrainte);
    free(Csui);

    fclose(Flot);
}

void OPT_dump_spx_variable_part(const PROBLEME_SIMPLEXE* Pb, uint numSpace)
{
    FILE* Flot;
    int Var;

    char buffer[OPT_APPEL_SOLVEUR_BUFFER_SIZE];

    auto study = Data::Study::Current::Get();
    Flot = study->createFileIntoOutputWithExtension("problem-variable-part", "mps", numSpace);

    if (!Flot)
        AntaresSolverEmergencyShutdown(2);

    printHeader(Flot, Pb->NombreDeVariables, Pb->NombreDeContraintes);

    check(fprintf(Flot, "COLUMNS\n"), __func__, __LINE__);
    for (Var = 0; Var < Pb->NombreDeVariables; Var++)
    {
        if (Pb->CoutLineaire[Var] != 0.0)
        {
            SNPRINTF(buffer, OPT_APPEL_SOLVEUR_BUFFER_SIZE, "%-.10lf", Pb->CoutLineaire[Var]);
            check(fprintf(Flot, "    C%07d  OBJECTIF  %s\n", Var, buffer), __func__, __LINE__);
        }
    }

    printRHS(Flot, Pb->NombreDeContraintes, Pb->SecondMembre);

    printBounds(Flot, Pb->NombreDeVariables, Pb->TypeDeVariable, Pb->Xmin, Pb->Xmax);

    check(fprintf(Flot, "ENDATA\n"), __func__, __LINE__);

    fclose(Flot);
}

void OPT_EcrireJeuDeDonneesLineaireAuFormatMPS(void* Prob, uint numSpace)
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
    Flot = study->createFileIntoOutputWithExtension("problem", "mps", numSpace);

    if (!Flot)
        AntaresSolverEmergencyShutdown(2);

    printHeader(Flot, NombreDeVariables, NombreDeContraintes);

    check(fprintf(Flot, "ROWS\n"), __func__, __LINE__);
    check(fprintf(Flot, " N  OBJECTIF\n"), __func__, __LINE__);

    for (Cnt = 0; Cnt < NombreDeContraintes; Cnt++)
    {
        if (Sens[Cnt] == '=')
        {
            check(fprintf(Flot, " E  R%07d\n", Cnt), __func__, __LINE__);
        }
        else if (Sens[Cnt] == '<')
        {
            check(fprintf(Flot, " L  R%07d\n", Cnt), __func__, __LINE__);
        }
        else if (Sens[Cnt] == '>')
        {
            check(fprintf(Flot, " G  R%07d\n", Cnt), __func__, __LINE__);
        }
        else
        {
            logs.error() << __func__ << " : Wrong direction for constraint no. " << Cnt;
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

    check(fprintf(Flot, "ENDATA\n"), __func__, __LINE__);

    free(Cdeb);
    free(NumeroDeContrainte);
    free(Csui);

    fclose(Flot);
}
