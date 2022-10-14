#pragma once

extern "C"
{
#include "spx_definition_arguments.h"
#include "spx_fonctions.h"

#include "srs_api.h"
}

#include "named_problem.h"
#include "ortools_utils.h"

using namespace Antares;
using namespace Antares::Data;
using namespace Antares::Optimization;
using namespace operations_research;

void OPT_dump_spx_fixed_part(const PROBLEME_SIMPLEXE* Pb, uint numSpace);
void OPT_dump_spx_variable_part(const PROBLEME_SIMPLEXE* Pb, uint numSpace);
void OPT_EcrireJeuDeDonneesLineaireAuFormatMPS(void*, uint, uint);

// ======================
// MPS files writing
// ======================

class I_MPS_writer
{
public:
	I_MPS_writer() = default;
	virtual void runIfNeeded() = 0;
};

class fullMPSwriter final : public I_MPS_writer
{
public:
	fullMPSwriter(PROBLEME_SIMPLEXE_NOMME* named_splx_problem, int currentOptimNumber, uint thread_number);
	void runIfNeeded();
private:
	PROBLEME_SIMPLEXE_NOMME* named_splx_problem_ = nullptr;
	int current_optim_number_;
	uint thread_number_;
};

class fullOrToolsMPSwriter : public I_MPS_writer
{
public:
	fullOrToolsMPSwriter(MPSolver* solver, int currentOptimNumber, uint thread_number);
	void runIfNeeded();
private:
	MPSolver* solver_ = nullptr;
	int current_optim_number_;
	uint thread_number_;
};

class splitMPSwriter : public I_MPS_writer
{
public:
	splitMPSwriter(	PROBLEME_SIMPLEXE_NOMME* named_splx_problem,
					int currentOptimNumber,
					uint thread_nb,
					bool simu_1st_week);

	void runIfNeeded();
private:
	PROBLEME_SIMPLEXE_NOMME* named_splx_problem_;
	int current_optim_number_;
	uint thread_nb_;
	bool simu_1st_week_;
};

class nullMPSwriter : public I_MPS_writer
{
public:
	void runIfNeeded() 
	{ 
		// Does nothing
	}
};

class mpsWriterFactory
{
public:
	mpsWriterFactory(
		PROBLEME_HEBDO* ProblemeHebdo,
		int NumIntervalle,
		PROBLEME_SIMPLEXE_NOMME* named_splx_problem,
		bool ortoolsUsed,
		MPSolver* solver,
		uint thread_number);

	std::unique_ptr<I_MPS_writer> create();
	std::unique_ptr<I_MPS_writer> createOnOptimizationError();

private:
	// Member functions...
	std::unique_ptr<I_MPS_writer> createFullmpsWriter();
	bool doWeExportMPS();

	// Member data...
	PROBLEME_HEBDO* pb_hebdo_ = nullptr;
	int num_intervalle_;
	PROBLEME_SIMPLEXE_NOMME* named_splx_problem_ = nullptr;
	bool ortools_used_;
	MPSolver* solver_ = nullptr;
	uint thread_number_;
	int current_optim_number_;
	Data::mpsExportStatus export_mps_;
	bool export_mps_on_error_;
	bool split_mps_;
	bool is_first_week_of_year_;
};
