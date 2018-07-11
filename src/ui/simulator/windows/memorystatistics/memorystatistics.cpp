/*
** Copyright 2007-2018 RTE
** Authors: Antares_Simulator Team
**
** This file is part of Antares_Simulator.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** There are special exceptions to the terms and conditions of the
** license as they are applied to this software. View the full text of
** the exceptions in file COPYING.txt in the directory of this software
** distribution
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Antares_Simulator. If not, see <http://www.gnu.org/licenses/>.
**
** SPDX-License-Identifier: licenceRef-GPL3_WITH_RTE-Exceptions
*/

#include <yuni/yuni.h>
#include <yuni/core/system/memory.h>
#include <yuni/thread/thread.h>
#include "memorystatistics.h"

#include <antares/study.h>
#include <antares/study/memory-usage.h>
#include <antares/memory/memory.h>
#include "../../application/study.h"
#include "../../toolbox/create.h"
#include "../../toolbox/resources.h"
#include "../../toolbox/system/diskfreespace.hxx"
#include <ui/common/component/panel.h>

#include <wx/sizer.h>
#include <wx/button.h>
#include <wx/statline.h>
#include <wx/stattext.h>
#include <wx/timer.h>
#include <wx/wupdlock.h>

#include <yuni/core/system/cpu.h>

using namespace Yuni;




namespace Antares
{
namespace Private
{
namespace Window
{


	class MemoryStatisticsData final
	{
	public:
		MemoryStatisticsData() :
			stTxtTotal(nullptr),
			stTxtDiskFree(nullptr),
			stTxtSimu(nullptr),
			stTxtSimuDisk(nullptr),
			stTxtSimuSwap(nullptr),
			stTxtSimuSwapDisk(nullptr),
			stTxtSimuPar(nullptr),
			stTxtSimuParDisk(nullptr),
			stTxtDataDisplay(nullptr),
			stTxtMemoryCache(nullptr),
			stTxtMemoryCacheUsage(nullptr),
			stTxtMemoryCacheCapacity(nullptr),
			timer(nullptr)
		{}

		~MemoryStatisticsData();


	public:
		wxStaticText* stTxtTotal;
		wxStaticText* stTxtDiskFree;
		wxStaticText* stTxtSimu;
		wxStaticText* stTxtSimuDisk;
		wxStaticText* stTxtSimuSwap;
		wxStaticText* stTxtSimuSwapDisk;
		wxStaticText* stTxtSimuPar;
		wxStaticText* stTxtSimuParDisk;
		wxStaticText* stTxtDataDisplay;
		wxStaticText* stTxtMemoryCache;
		wxStaticText* stTxtMemoryCacheUsage;
		wxStaticText* stTxtMemoryCacheCapacity;
		wxTimer* timer;
		Yuni::Thread::IThread::Ptr thread;

	}; // class MemoryStatisticsData



	MemoryStatisticsData::~MemoryStatisticsData()
	{
		if (!(!thread))
			thread->gracefulStop();

		if (timer)
		{
			timer->Stop();
			delete timer;
		}
		if (!(!thread))
		{
			thread->stop(10000);
			thread = nullptr;
		}
	}



} // namespace Window
} // namespace Private
} // namespace Antares




namespace Antares
{
namespace Window
{

	namespace // anonymous
	{

		class ResourcesInfoTimer final : public wxTimer
		{
		public:
			ResourcesInfoTimer(MemoryStatistics& form) :
				wxTimer(),
				pForm(form)
			{}
			virtual ~ResourcesInfoTimer()
			{}

			void Notify() final
			{
				pForm.refreshInformation();
			}

		private:
			MemoryStatistics& pForm;
		};


		static inline Yuni::uint64 NormalizeAmountOfMemory(Yuni::uint64 v)
		{
			v *= 10u;
			v /= 1024u * 1024u;
			v /= 10u;
			return v;
		}

	} // anonymous namespace




	MemoryStatistics::MemoryStatistics(wxWindow* parent) :
		wxDialog(parent, wxID_ANY, wxT("Resources monitor"), wxDefaultPosition, wxDefaultSize),
		pDisplayLogsOnce(false)
	{
		// Allocating internal data
		pData = new Antares::Private::Window::MemoryStatisticsData();

		// Informations about the study
		auto study = Data::Study::Current::Get();

		wxColour defaultBgColor = GetBackgroundColour();
		SetBackgroundColour(wxColour(255, 255, 255));

		if (!(!study))
		{
			pData->thread = study->createThreadToEstimateInputMemoryUsage();
			pData->thread->start();
		}

		auto* sizer = new wxBoxSizer(wxVERTICAL);
		sizer->AddSpacer(10);


		auto* gridSizer = new wxFlexGridSizer(3, 4, 10);
		gridSizer->AddGrowableCol(1, 1);


		// Space
		gridSizer->AddSpacer(2);
		{
			auto* st = Component::CreateLabel(this, wxT("Memory"), true);
			gridSizer->Add(st, 0, wxRIGHT | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
		}
		{
			auto* st = Component::CreateLabel(this, wxT("    Disk"), true);
			gridSizer->Add(st, 0, wxRIGHT | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
		}


		// Space
		{
			auto* st = new wxStaticText(this, wxID_ANY,
				wxT("Interface  "), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT);
			wxFont f = st->GetFont();
			f.SetWeight(wxFONTWEIGHT_BOLD);
			st->SetFont(f);
			gridSizer->Add(st, 0, wxRIGHT | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
			gridSizer->AddSpacer(5);
			gridSizer->AddSpacer(5);
		}

		// Data display
		auto* stTxtDataDisplayTtl = new wxStaticText(this, wxID_ANY,
			wxT("Currently used for data display : "), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT);
		gridSizer->Add(stTxtDataDisplayTtl, 0, wxRIGHT | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);

		pData->stTxtDataDisplay = new wxStaticText(this, wxID_ANY,
			wxT(""), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT);
		gridSizer->Add(pData->stTxtDataDisplay, 0, wxRIGHT | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);

		pData->stTxtMemoryCache = new wxStaticText(this, wxID_ANY,
			wxT(""), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT);
		gridSizer->Add(pData->stTxtMemoryCache, 0, wxRIGHT | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);


		// Memory cache - capacity
		auto* stTxtMemoryCacheCapaTtl = new wxStaticText(this, wxID_ANY,
			wxT("Used by swap files : "), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT);
		gridSizer->Add(stTxtMemoryCacheCapaTtl, 0, wxRIGHT | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);

		pData->stTxtMemoryCacheUsage = new wxStaticText(this, wxID_ANY,
			wxT("N/A"), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT);
		gridSizer->Add(pData->stTxtMemoryCacheUsage, 0, wxRIGHT | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);

		pData->stTxtMemoryCacheCapacity = new wxStaticText(this, wxID_ANY,
			wxT(""), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT);
		gridSizer->Add(pData->stTxtMemoryCacheCapacity, 0, wxRIGHT | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);


		// Space
		{
			auto* st = new wxStaticText(this, wxID_ANY,
				wxT("System  "), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT);
			wxFont f = st->GetFont();
			f.SetWeight(wxFONTWEIGHT_BOLD);
			st->SetFont(f);
			gridSizer->Add(st, 0, wxRIGHT | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
			gridSizer->AddSpacer(5);
			gridSizer->AddSpacer(5);
		}


		// Total available memory
		auto* stTxtTotalTtl = new wxStaticText(this, wxID_ANY,
			wxT("Currently available on the system : "), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT);
		gridSizer->Add(stTxtTotalTtl, 0, wxRIGHT | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);

		pData->stTxtTotal = new wxStaticText(this, wxID_ANY,
			wxT("   Updating..."), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT);
		gridSizer->Add(pData->stTxtTotal, 0, wxRIGHT | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);

		pData->stTxtDiskFree = new wxStaticText(this, wxID_ANY,
			wxT("   Updating..."), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT);
		gridSizer->Add(pData->stTxtDiskFree, 0, wxRIGHT | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);



		// Space
		{
			gridSizer->AddSpacer(10);
			gridSizer->AddSpacer(10);
			gridSizer->AddSpacer(10);

			auto* st = new wxStaticText(this, wxID_ANY,
				wxT("Simulation  "), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT);
			wxFont f = st->GetFont();
			f.SetWeight(wxFONTWEIGHT_BOLD);
			st->SetFont(f);
			gridSizer->Add(st, 0, wxRIGHT | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
			gridSizer->AddSpacer(5);
			gridSizer->AddSpacer(5);
		}



		// Required for a simulation (default mode)
		auto* stTxtSimuTtl = new wxStaticText(this, wxID_ANY,
			wxT("Default : "), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT);
		gridSizer->Add(stTxtSimuTtl, 0, wxRIGHT | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);

		pData->stTxtSimu = new wxStaticText(this, wxID_ANY,
			wxT("   Updating..."), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT);
		gridSizer->Add(pData->stTxtSimu, 0, wxRIGHT | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);

		pData->stTxtSimuDisk = new wxStaticText(this, wxID_ANY,
			wxT("    Updating..."), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT);
		gridSizer->Add(pData->stTxtSimuDisk, 0, wxRIGHT | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);



		// Required for a simulation (with swap support)
		wxStaticText* stTxtSimuSwapTtl = new wxStaticText(this, wxID_ANY,
			wxT("Swap : "), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT);
		gridSizer->Add(stTxtSimuSwapTtl, 0, wxRIGHT | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);

		pData->stTxtSimuSwap = new wxStaticText(this, wxID_ANY,
			wxT("   Updating..."), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT);
		gridSizer->Add(pData->stTxtSimuSwap, 0, wxRIGHT | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);

		pData->stTxtSimuSwapDisk = new wxStaticText(this, wxID_ANY,
			wxT("   Updating..."), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT);
		gridSizer->Add(pData->stTxtSimuSwapDisk, 0, wxRIGHT | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);



		// Parallel mode
		auto* stTxtSimuParTtl = new wxStaticText(this, wxID_ANY,
			wxT("Parallel : "), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT);
		gridSizer->Add(stTxtSimuParTtl, 0, wxRIGHT | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);

		pData->stTxtSimuPar = new wxStaticText(this, wxID_ANY,
			wxT("   Updating..."), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT);
		gridSizer->Add(pData->stTxtSimuPar, 0, wxRIGHT | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);

		pData->stTxtSimuParDisk = new wxStaticText(this, wxID_ANY,
			wxT("    Updating..."), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT);
		gridSizer->Add(pData->stTxtSimuParDisk, 0, wxRIGHT | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);


		// Machine number of cores
		{
			gridSizer->AddSpacer(10);
			gridSizer->AddSpacer(10);
			gridSizer->AddSpacer(10);

			auto* stTxtMachineNbCoresTtl = new wxStaticText(this, wxID_ANY,
				wxT("CPU cores :"), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT);
			wxFont f = stTxtMachineNbCoresTtl->GetFont();
			f.SetWeight(wxFONTWEIGHT_BOLD);
			stTxtMachineNbCoresTtl->SetFont(f);
			gridSizer->Add(stTxtMachineNbCoresTtl, 0, wxRIGHT | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);

			wxString s;
			s << Yuni::System::CPU::Count();
			wxStaticText* stTxtMachineNbCores = new wxStaticText(this, wxID_ANY, wxT(""), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT);
			stTxtMachineNbCores->SetLabel(s);
			gridSizer->Add(stTxtMachineNbCores, 0, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL);
		}



		auto* hz = new wxBoxSizer(wxHORIZONTAL);
		hz->Add(Resources::StaticBitmapLoadFromFile(this, wxID_ANY, "images/64x64/cpu.png"), 0, wxALL|wxALIGN_TOP|wxALIGN_CENTER_HORIZONTAL);
		hz->Add(gridSizer, 1, wxALL|wxEXPAND);
		sizer->Add(hz, 1, wxALL|wxEXPAND, 20);


		auto* panel = new Component::Panel(this);
		auto* sizerBar = new wxBoxSizer(wxHORIZONTAL);
		sizerBar->AddStretchSpacer();
		panel->SetSizer(sizerBar);
		panel->SetBackgroundColour(defaultBgColor);

		// Button Close
		{
			auto* btnClose = Component::CreateButton(panel, wxT("   Close   "), this, &MemoryStatistics::onClose);
			sizerBar->Add(btnClose, 0, wxFIXED_MINSIZE|wxALIGN_CENTRE_VERTICAL|wxALL, 8);
			sizerBar->Add(15, 5);
			btnClose->SetDefault();
			btnClose->SetFocus();
		}

		sizer->AddSpacer(15);
		sizer->Add(new wxStaticLine(this), 0, wxALL|wxEXPAND);
		sizer->Add(panel, 0, wxALL|wxEXPAND);

		sizer->Layout();
		SetSizer(sizer);
		sizer->Fit(this);
		Centre(wxBOTH);

		refreshInformation();

		pData->timer = new ResourcesInfoTimer(*this);
		pData->timer->Start(100);
	}


	MemoryStatistics::~MemoryStatistics()
	{
		delete pData;

		// To avoid corrupt vtable in some rare cases / misuses
		// (when children try to access to this object for example),
		// we should destroy all children as soon as possible.
		auto* sizer = GetSizer();
		if (sizer)
			sizer->Clear(true);
	}



	void MemoryStatistics::refreshInformation()
	{
		assert(pData);
		wxString s;

		// Prevent refreshing
		wxWindowUpdateLocker updater(this);

		if (pData->timer)
			pData->timer->Stop();


		// Memory available on the system
		{
			const uint64 memFree = System::Memory::Available() / (1024 * 1024);
			s.clear();
			s << memFree << wxT(" Mo");
			pData->stTxtTotal->SetLabel(s);
		}

		// The study
		auto study = Data::Study::Current::Get();

		// Disk free
		{
			# ifdef YUNI_OS_WINDOWS
			if (!(!study) and not study->folder.empty())
			{
				s.clear();
				CString<5, false> driveletter;
				driveletter += study->folder[0];
				s << wxStringFromUTF8(driveletter) << wxT(": ");

				uint64 diskFree = DiskFreeSpace(study->folder);
				if (diskFree == (uint64) -1)
					s << wxT("N/A");
				else
					s << (diskFree / (1024 * 1024)) << wxT(" Mo");
				pData->stTxtDiskFree->SetLabel(s);
			}
			else
			{
				uint64 diskFree = DiskFreeSpace("C:\\");
				s.clear();
				s << wxT("C: ") << (diskFree / (1024 * 1024)) << wxT(" Mo");
				pData->stTxtDiskFree->SetLabel(s);
			}
			# else
			// TODO linux implementation is missing
			(void) DiskFreeSpace(nullptr); // remove compilation warning
			pData->stTxtDiskFree->SetLabel(wxT("N/A"));
			# endif
		}

		auto& allocator = Antares::memory;

		// Memory cache
		{
			uint64 memCache = allocator.memoryUsageInSwap() / (1024 * 1024);
			s.clear();
			s << memCache << wxT(" Mo");
			pData->stTxtMemoryCache->SetLabel(s);
		}

		// Memory cache
		{
			uint64 memCache = allocator.memoryUsage() / (1024 * 1024);
			s.clear();
			s << NormalizeAmountOfMemory(memCache) << wxT(" Mo");
			pData->stTxtMemoryCacheUsage->SetLabel(s);
		}

		// Memory cache
		s.clear();
		s << (allocator.memoryCapacity() / (1024 * 1024)) << wxT(" Mo");
		pData->stTxtMemoryCacheCapacity->SetLabel(s);


		if (!(!study))
		{
			const bool updating = (not pData->timer or pData->thread->started());

			if (not updating)
			{
				// Memory required for a simulation
				{
					// For default mode, parallel mode is disabled.
					study->maxNbYearsInParallel = 1;
					
					Data::StudyMemoryUsage m(*study);
					m.estimate();
					s.clear();

					if (!pDisplayLogsOnce)
					{
						logs.info() << "Memory usage: estimate: input: "
							<< (m.requiredMemoryForInput / (1024 * 1024))  << "Mo, output: "
							<< (m.requiredMemoryForOutput / (1024 * 1024)) << "Mo,   disk: "
							<< (m.requiredDiskSpace / (1024 * 1024)) << "Mo";
					}

					s << wxT("~ ") << NormalizeAmountOfMemory(m.requiredMemory) << wxT(" Mo");
					pData->stTxtSimu->SetLabel(s);
					s.clear();
					if (m.requiredDiskSpace)
						s << wxT("~ ") << (m.requiredDiskSpace / (1024 * 1024)) << wxT(" Mo");
					else
						s << wxT("0 Mo");
					pData->stTxtSimuDisk->SetLabel(s);
				}

				{
					Data::StudyMemoryUsage m(*study);
					m.swappingSupport = true;
					m.estimate();
					s.clear();

					if (!pDisplayLogsOnce)
					{
						logs.info() << "Memory usage: estimate: input: "
							<< (m.requiredMemoryForInput / (1024 * 1024))  << "Mo, output: "
							<< (m.requiredMemoryForOutput / (1024 * 1024)) << "Mo,   disk: "
							<< (m.requiredDiskSpace / (1024 * 1024)) << "Mo (with swap support)";
					}

					s << wxT("~ ") << NormalizeAmountOfMemory(m.requiredMemory) << wxT(" Mo");
					pData->stTxtSimuSwap->SetLabel(s);
					s.clear();
					if (m.requiredDiskSpace)
						s << wxT("~ ") << (m.requiredDiskSpace / (1024 * 1024)) << wxT(" Mo");
					else
						s << wxT("0 Mo");
					pData->stTxtSimuSwapDisk->SetLabel(s);
				}

				{
					// Parallel mode
					study->maxNbYearsInParallel = study->maxNbYearsInParallel_save;

					Data::StudyMemoryUsage m(*study);
					m.estimate();
					s.clear();

					if (!pDisplayLogsOnce)
					{
						logs.info() << "Memory usage: estimate: input: "
							<< (m.requiredMemoryForInput / (1024 * 1024))  << "Mo, output: "
							<< (m.requiredMemoryForOutput / (1024 * 1024)) << "Mo,   disk: "
							<< (m.requiredDiskSpace / (1024 * 1024)) << "Mo";
					}

					s << wxT("~ ") << NormalizeAmountOfMemory(m.requiredMemory) << wxT(" Mo");
					pData->stTxtSimuPar->SetLabel(s);
					s.clear();
					if (m.requiredDiskSpace)
						s << wxT("~ ") << (m.requiredDiskSpace / (1024 * 1024)) << wxT(" Mo");
					else
						s << wxT("0 Mo");
					pData->stTxtSimuParDisk->SetLabel(s);
				}


				pDisplayLogsOnce = true;
			}
			else
			{
				pData->stTxtSimu->SetLabel(wxT("updating"));
				pData->stTxtSimuDisk->SetLabel(wxT("updating"));
				pData->stTxtSimuSwap->SetLabel(wxT("updating"));
				pData->stTxtSimuSwapDisk->SetLabel(wxT("updating"));
				pData->stTxtSimuPar->SetLabel(wxT("updating"));
				pData->stTxtSimuParDisk->SetLabel(wxT("updating"));
			}

			// Memory consummed by the study
			s.clear();
			if (updating)
				s << wxT("  ~ ");
			s << (1 + study->memoryUsage() / (1024 * 1024)) << wxT(" Mo");
			pData->stTxtDataDisplay->SetLabel(s);
		}
		else
		{
			pData->stTxtDataDisplay->SetLabel(wxT("N/A"));

			pData->stTxtSimu->SetLabel(wxT("N/A"));
			pData->stTxtSimuDisk->SetLabel(wxT("N/A"));
			pData->stTxtSimuSwap->SetLabel(wxT("N/A"));
			pData->stTxtSimuSwapDisk->SetLabel(wxT("N/A"));
			pData->stTxtSimuPar->SetLabel(wxT("N/A"));
			pData->stTxtSimuParDisk->SetLabel(wxT("N/A"));
		}

		GetSizer()->Layout();
		if (pData->timer)
			pData->timer->Start(3000);
	}


	void MemoryStatistics::onClose(void*)
	{
		if (!(!pData->thread))
			pData->thread->gracefulStop();
		// ASync close
		Dispatcher::GUI::Close(this);
	}





} // namespace Window
} // namespace Antares
