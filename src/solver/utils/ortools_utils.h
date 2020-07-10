#pragma once

#include "ortools/linear_solver/linear_solver.h"
extern "C"
{
# include "ortools_wrapper.h"
}

extern bool withOrtool;

size_t current_memory_usage(std::string const & message);

std::string getRunName(std::string const & prefix, size_t numSpace, int numInterval, int numOptim);

void ORTOOLS_EcrireJeuDeDonneesLineaireAuFormatMPS(operations_research::MPSolver * solver, size_t numSpace, int const n);