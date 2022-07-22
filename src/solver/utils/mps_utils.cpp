#include <antares/study.h>
#include <antares/emergency.h>

#include "../simulation/simulation.h"

#include "mps_utils.h"

using namespace Antares;
using namespace Antares::Data;
using namespace Yuni;

#ifdef _MSC_VER
#define SNPRINTF sprintf_s
#else
#define SNPRINTF snprintf
#endif

constexpr size_t OPT_APPEL_SOLVEUR_BUFFER_SIZE = 256;

static void printHeader(Clob& Flot, int NombreDeVariables, int NombreDeContraintes)
{
  Flot.appendFormat("* Number of variables:   %d\n", NombreDeVariables);
  Flot.appendFormat("* Number of constraints: %d\n", NombreDeContraintes);
  Flot.appendFormat("NAME          Pb Solve\n");
}

static void printColumnsObjective(Clob& Flot,
                                  int NombreDeVariables,
                                  const int* NumeroDeContrainte,
                                  const double* CoefficientsDeLaMatriceDesContraintes,
                                  const int* Cdeb,
                                  const int* Csui,
                                  const double* CoutLineaire)
{
    char buffer[OPT_APPEL_SOLVEUR_BUFFER_SIZE];
    int il;

    Flot.appendFormat("COLUMNS\n");
    for (int Var = 0; Var < NombreDeVariables; Var++)
    {
        if (CoutLineaire && CoutLineaire[Var] != 0.0)
        {
            SNPRINTF(buffer, OPT_APPEL_SOLVEUR_BUFFER_SIZE, "%-.10lf", CoutLineaire[Var]);
            Flot.appendFormat("    C%07d  OBJECTIF  %s\n", Var, buffer);
        }

        il = Cdeb[Var];
        while (il >= 0)
        {
            SNPRINTF(buffer,
                     OPT_APPEL_SOLVEUR_BUFFER_SIZE,
                     "%-.10lf",
                     CoefficientsDeLaMatriceDesContraintes[il]);
            Flot.appendFormat("    C%07d  R%07d  %s\n", Var, NumeroDeContrainte[il], buffer);
            il = Csui[il];
        }
    }
}

static void printBounds(Clob& Flot,
                        int NombreDeVariables,
                        const int* TypeDeBorneDeLaVariable,
                        const double* Xmin,
                        const double* Xmax)
{
    char buffer[OPT_APPEL_SOLVEUR_BUFFER_SIZE];

    Flot.appendFormat("BOUNDS\n");

    for (int Var = 0; Var < NombreDeVariables; Var++)
    {
        if (TypeDeBorneDeLaVariable[Var] == VARIABLE_FIXE)
        {
            SNPRINTF(buffer, OPT_APPEL_SOLVEUR_BUFFER_SIZE, "%-.9lf", Xmin[Var]);
            Flot.appendFormat(" FX BNDVALUE  C%07d  %s\n", Var, buffer);
            continue;
        }

        if (TypeDeBorneDeLaVariable[Var] == VARIABLE_BORNEE_DES_DEUX_COTES)
        {
            if (Xmin[Var] != 0.0)
            {
                SNPRINTF(buffer, OPT_APPEL_SOLVEUR_BUFFER_SIZE, "%-.9lf", Xmin[Var]);
                Flot.appendFormat(" LO BNDVALUE  C%07d  %s\n", Var, buffer);
            }

            SNPRINTF(buffer, OPT_APPEL_SOLVEUR_BUFFER_SIZE, "%-.9lf", Xmax[Var]);
            Flot.appendFormat(" UP BNDVALUE  C%07d  %s\n", Var, buffer);
        }

        if (TypeDeBorneDeLaVariable[Var] == VARIABLE_BORNEE_INFERIEUREMENT)
        {
            if (Xmin[Var] != 0.0)
            {
                SNPRINTF(buffer, OPT_APPEL_SOLVEUR_BUFFER_SIZE, "%-.9lf", Xmin[Var]);
                Flot.appendFormat(" LO BNDVALUE  C%07d  %s\n", Var, buffer);
            }
        }

        if (TypeDeBorneDeLaVariable[Var] == VARIABLE_BORNEE_SUPERIEUREMENT)
        {
            Flot.appendFormat(" MI BNDVALUE  C%07d\n", Var);
            if (Xmax[Var] != 0.0)
            {
                SNPRINTF(buffer, OPT_APPEL_SOLVEUR_BUFFER_SIZE, "%-.9lf", Xmax[Var]);
                Flot.appendFormat(" UP BNDVALUE  C%07d  %s\n", Var, buffer);
            }
        }

        if (TypeDeBorneDeLaVariable[Var] == VARIABLE_NON_BORNEE)
        {
            Flot.appendFormat(" FR BNDVALUE  C%07d\n", Var);
        }
    }
}

static void printRHS(Clob& Flot, int NombreDeContraintes, const double* SecondMembre)
{
    char buffer[OPT_APPEL_SOLVEUR_BUFFER_SIZE];

    Flot.appendFormat("RHS\n");
    for (int Cnt = 0; Cnt < NombreDeContraintes; Cnt++)
    {
        if (SecondMembre[Cnt] != 0.0)
        {
            SNPRINTF(buffer, OPT_APPEL_SOLVEUR_BUFFER_SIZE, "%-.9lf", SecondMembre[Cnt]);
            Flot.appendFormat("    RHSVAL    R%07d  %s\n", Cnt, buffer);
        }
    }
}

void OPT_dump_spx_fixed_part(const PROBLEME_SIMPLEXE* Pb, uint numSpace)
{
    Clob Flot;
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
        
    auto filename = study->createFileIntoOutputWithExtension("problem-fixed-part", "mps", numSpace);

    printHeader(Flot, Pb->NombreDeVariables, Pb->NombreDeContraintes);

    Flot.appendFormat("ROWS\n");
    Flot.appendFormat(" N  OBJECTIF\n");

    for (Cnt = 0; Cnt < Pb->NombreDeContraintes; Cnt++)
    {
        if (Pb->Sens[Cnt] == '=')
        {
            Flot.appendFormat(" E  R%07d\n", Cnt);
        }
        else if (Pb->Sens[Cnt] == '<')
        {
            Flot.appendFormat(" L  R%07d\n", Cnt);
        }
        else if (Pb->Sens[Cnt] == '>')
        {
            Flot.appendFormat(" G  R%07d\n", Cnt);
        }
        else
        {
            Flot.appendFormat(
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

    Flot.appendFormat("ENDATA\n");

    // [FO] TODO
    // auto archive = study->pZipArchive;
    // archive->addData(filename,
    //                  Flot.c_str(),
    //                  Flot.size());

    // archive->close();
    // archive->open(libzippp::ZipArchive::Write);

    free(Cdeb);
    free(NumeroDeContrainte);
    free(Csui);
}

void OPT_dump_spx_variable_part(const PROBLEME_SIMPLEXE* Pb, uint numSpace)
{
    Clob Flot;
    int Var;
    

    char buffer[OPT_APPEL_SOLVEUR_BUFFER_SIZE];

    auto study = Data::Study::Current::Get();
    auto filename = study->createFileIntoOutputWithExtension("problem-variable-part", "mps", numSpace);


    printHeader(Flot, Pb->NombreDeVariables, Pb->NombreDeContraintes);

    Flot.appendFormat("COLUMNS\n");
    for (Var = 0; Var < Pb->NombreDeVariables; Var++)
    {
        if (Pb->CoutLineaire[Var] != 0.0)
        {
            SNPRINTF(buffer, OPT_APPEL_SOLVEUR_BUFFER_SIZE, "%-.10lf", Pb->CoutLineaire[Var]);
            Flot.appendFormat("    C%07d  OBJECTIF  %s\n", Var, buffer);
        }
    }

    printRHS(Flot, Pb->NombreDeContraintes, Pb->SecondMembre);

    printBounds(Flot, Pb->NombreDeVariables, Pb->TypeDeVariable, Pb->Xmin, Pb->Xmax);

    Flot.appendFormat("ENDATA\n");

    // [FO] TODO
    // auto archive = study->pZipArchive;
    // archive->addData(filename,
    //                  Flot.c_str(),
    //                  Flot.size());

    // archive->close();
    // archive->open(libzippp::ZipArchive::Write);
}

void OPT_EcrireJeuDeDonneesLineaireAuFormatMPS(void* Prob, uint numSpace)
{
    Clob Flot;
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
    auto filename = study->createFileIntoOutputWithExtension("problem", "mps", numSpace);

    printHeader(Flot, NombreDeVariables, NombreDeContraintes);

    Flot.appendFormat("ROWS\n");
    Flot.appendFormat(" N  OBJECTIF\n");

    for (Cnt = 0; Cnt < NombreDeContraintes; Cnt++)
    {
        if (Sens[Cnt] == '=')
        {
            Flot.appendFormat(" E  R%07d\n", Cnt);
        }
        else if (Sens[Cnt] == '<')
        {
            Flot.appendFormat(" L  R%07d\n", Cnt);
        }
        else if (Sens[Cnt] == '>')
        {
            Flot.appendFormat(" G  R%07d\n", Cnt);
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

    Flot.appendFormat("ENDATA\n");

    // [FO] TODO
    // auto archive = study->pZipArchive;
    // archive->addData(filename,
    //                  Flot.c_str(),
    //                  Flot.size());

    // archive->close();
    // archive->open(libzippp::ZipArchive::Write);

    free(Cdeb);
    free(NumeroDeContrainte);
    free(Csui);
}
