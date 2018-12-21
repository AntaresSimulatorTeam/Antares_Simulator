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

#include <antares/antares.h>
#include "analyzer.h"
#include <yuni/io/directory.h>
#include <yuni/io/directory/info.h>

#include <wx/sizer.h>
#include <wx/checkbox.h>
#include <wx/timer.h>
#include <wx/app.h>
#include <wx/dialog.h>
#include <wx/frame.h>
#include <wx/statline.h>
#include <wx/textctrl.h>
#include <wx/dirdlg.h>
#include <wx/generic/dirdlgg.h>
#include <wx/stattext.h>
#include <wx/button.h>

#include <ui/common/component/panel.h>
#include "../../toolbox/create.h"
#include "../../toolbox/components/wizardheader.h"
#include "../../toolbox/components/datagrid/component.h"
#include "../../toolbox/components/notebook/notebook.h"
#include "../../toolbox/components/button.h"
#include "../../application/main/main.h"
#include "../../application/menus.h"
#include "../message.h"
#include <antares/logs.h>
#include <antares/memory/memory.h>
#include <antares/inifile.h>
#include "../../application/study.h"
#include <antares/config.h>
#include <antares/io/statistics.h>
#include "../../../../internet/limits.h"


using namespace Yuni;


# define SEP Yuni::IO::Separator


namespace Antares
{
namespace Window
{
	BEGIN_EVENT_TABLE(AnalyzerWizard, wxDialog)
		EVT_MENU(  mnIDUpdateInfo,          AnalyzerWizard::evtUpdateInfo)
		EVT_MENU(  mnIDUpdateFileMapping,   AnalyzerWizard::evtUpdateFileMapping)
		EVT_MENU(  mnIDGridUpdate,          AnalyzerWizard::evtGridUpdate)
		EVT_MENU(  mnIDCanRunAnalyzer,      AnalyzerWizard::evtCanRunAnalyzer)
	END_EVENT_TABLE()



	namespace // anonymous
	{

		static wxString gLastFolderForTSAnalyzer;



		class FileSearchProvider final : public Antares::Component::Spotlight::IProvider
		{
		public:
			//! The spotlight component (alias)
			typedef Antares::Component::Spotlight Spotlight;

		public:
			//! \name Constructor & Destructor
			//@{
			/*!
			** \brief Default constructor
			*/
			FileSearchProvider()
			{
			}
			//! Destructor
			virtual ~FileSearchProvider()
			{
			}
			//@}

			/*!
			** \brief Perform a new search
			*/
			virtual void search(Spotlight::IItem::Vector& out, const Spotlight::SearchToken::Vector& tokens, const Yuni::String& text = "") override
			{
				if (tokens.empty())
				{
					foreach (auto& filename, pFiles)
					{
						auto* item = new Spotlight::IItem();
						item->caption(filename);
						item->addTag("F", 210, 210, 255);
						out.push_back(item);
					}
				}
				else
				{
					foreach (auto& filename, pFiles)
					{
						foreach (auto& tokenname, tokens)
						{
							if (filename.icontains(tokenname->text))
							{
								auto* item = new Spotlight::IItem();
								item->caption(filename);
								item->addTag("F", 210, 210, 255);
								out.push_back(item);
								break;
							}
						}
					}
				}
			}

			/*!
			** \brief An item has been selected
			*/
			virtual bool onSelect(Spotlight::IItem::Ptr&) override
			{
				return false;
			}


			void onFileSearchAdd(const String& filename)
			{
				pFiles.push_back(filename);
			}

			void onFileSearchClear()
			{
				pFiles.clear();
			}

		private:
			//! List of files
			String::Vector pFiles;

		}; // class Layer



		class AnalyzeSourceFolder final : public Yuni::Thread::IThread
		{
		public:
			explicit AnalyzeSourceFolder(AnalyzerWizard& form) :
				pForm(form),
				pMapping(nullptr)
			{}

			explicit AnalyzeSourceFolder(AnalyzerWizard& form, const wxString& f) :
				pForm(form),
				pMapping(nullptr)
			{
				wxStringToString(f, pFolder);
			}

			virtual ~AnalyzeSourceFolder()
			{
				stop();
				delete pMapping;
			}

			void folder(const wxString& f)
			{
				pDataMutex.lock();
				wxStringToString(f, pFolder);
				pDataMutex.unlock();
			}

			void folder(const AnyString& path)
			{
				pDataMutex.lock();
				pFolder = path;
				pDataMutex.unlock();
			}

		protected:
			virtual bool onExecute() override
			{
				String msg;
				msg.reserve(128);
				pForm.info("Fetching informations...");

				pDataMutex.lock();
				IO::Directory::Info info(pFolder);
				logs.info() << "[analyzer] Analyzing " << pFolder;
				pDataMutex.unlock();

				if (not pMapping)
					pMapping = new AnalyzerWizard::FileMapping();
				else
					pMapping->clear();

				uint total = 0;
				uint found = 0;
				String ext;
				String entry;

				auto end = info.recursive_file_end();
				for (auto i = info.recursive_file_begin(); i != end; ++i)
				{
					// empty files are useless
					if (i.size() == 0)
						continue;

					ext.clear();
					IO::ExtractExtension(ext, *i);
					ext.toLower();
					if (ext == ".txt" or ext == ".csv")
					{
						++found;
						entry = *i;
						entry.toLower();
						(*(pMapping))[entry] = i.filename();

						// a few checks to reduce flickering
						if (found and ((found < 40 and not (found % 5)) or (found > 40 and !(found % 100))))
						{
							msg.clear();
							msg << "Fetching informations...   " << found << " items found";
							pForm.info(msg);
						}
					}

					++total;

					if (not (total & 3) and shouldAbort())
					{
						pForm.info("Aborting...");
						return true;
					}
				}

				pForm.info("Updating...");
				pForm.fileMapping(pMapping);
				pMapping = nullptr;

				return true;
			}

			virtual void onStop() override
			{
				pForm.info(nullptr);
			}

			virtual void onKill() override
			{
				pForm.info(nullptr);
			}

			virtual void onPause() override
			{
				pForm.info(nullptr);
			}

		private:
			AnalyzerWizard& pForm;
			Mutex pDataMutex;
			String pFolder;
			AnalyzerWizard::FileMapping* pMapping;

		}; // class AnalyzeSourceFolder




		class AnalyzeRelationship final :
			public Yuni::Thread::IThread,
			public Yuni::IEventObserver<AnalyzeRelationship>
		{
		public:
			AnalyzeRelationship(AnalyzerWizard& form, const AnalyzerWizard::FileMappingPtr& mapping,
				AnalyzerWizard::RecordVector& record)
				:pForm(form),
				pMapping(mapping),
				pRecord(record),
				pUseCustomTS(false),
				pMaxTS(0)
			{
				pRecord.mutex.lock();
				pRecordCount = (uint) pRecord.array.size();
				pRecord.mutex.unlock();
			}

			virtual ~AnalyzeRelationship()
			{
				destroyBoundEvents();
				stop();
			}

			void updateMaxTimeseries(bool useCustomTS, uint maxTS)
			{
				pRecord.mutex.lock();
				pUseCustomTS = useCustomTS;
				pMaxTS = maxTS;
				pRecord.canRunAnalyzer = false;
				pRecord.mutex.unlock();
			}

		protected:
			virtual bool onExecute() override
			{
				// alias
				typedef Component::Datagrid::Renderer::Analyzer::Areas::Record Record;

				auto& mapping = *pMapping;
				Record record;
				String s;

				pRecord.mutex.lock();
				if (pRecord.canRunAnalyzer)
				{
					pRecord.canRunAnalyzer = false;
					pForm.enableAnalyzer(false);
				}
				pRecord.mutex.unlock();

				bool errorFound = false;
				bool gotAtLeastOneReady = false;
				Matrix<>::BufferType buffer;
				Matrix<sint32> m;
				uint size[2] = {(uint) -1, (uint) -1};

				for (uint i = 0; i != pRecordCount; ++i)
				{
					// avoid locking as much as possible to not freeze the interface
					{
						pRecord.mutex.lock();
						Record& r = pRecord.array[i];

						if (r.status != Record::stWaiting)
						{
							if (r.enabled)
							{
								if (r.status == Record::stError)
								{
									errorFound = true;
								}
								else
								{
									if (r.status == Record::stReady)
									{
										if (size[0] == (uint) -1 or size[1] == (uint) -1)
										{
											size[0] = r.mWidth;
											size[1] = r.mHeight;
										}
										if (!widthIsValid(r.mWidth, size[0]) or size[1] != r.mHeight)
										{
											r.status = Record::stError;
											errorFound = true;
										}
										else
											gotAtLeastOneReady = true;
									}
								}
							}

							pRecord.mutex.unlock();
							continue;
						}

						record = r;
						pRecord.mutex.unlock();
					}

					s.clear() << "Checking " << record.fileToSearch << "...";
					pForm.info(s);

					auto it = mapping.find(record.fileToSearch);
					if (it == mapping.end() or !record.fileToSearch)
					{
						pRecord.mutex.lock();
						pRecord.array[i].status = Record::stNotFound;
						pRecord.mutex.unlock();
					}
					else
					{
						pRecord.mutex.lock();
						Record& r = pRecord.array[i];
						if (r.mWidth == (uint) -1 or r.mHeight == (uint) -1)
						{
							pRecord.mutex.unlock();

							// Checking for the matrix size
							if (m.loadFromCSVFile(it->second, 1, 8760, Matrix<>::optImmediate, &buffer))
							{
								pRecord.mutex.lock();

								r.mWidth = m.width;
								r.mHeight = m.height;
							}
							else
							{
								pRecord.mutex.lock();
								r.mWidth = 0;
								r.mHeight = 0;
							}
						}
						if (!r.mWidth or !r.mHeight)
						{
							r.status = Record::stError;
							if (r.enabled)
								errorFound = true;
						}
						else
						{
							if (size[0] == (uint) -1 or size[1] == (uint) -1)
							{
								size[0] = r.mWidth;
								size[1] = r.mHeight;
							}
							if (!widthIsValid(r.mWidth, size[0]) or size[1] != r.mHeight)
							{
								r.status = Record::stError;
								if (r.enabled)
									errorFound = true;
							}
							else
							{
								r.status = Record::stReady;
								if (r.enabled)
									gotAtLeastOneReady = true;
							}
						}
						pRecord.mutex.unlock();
					}

					// Update the grid and the label info
					pForm.refreshGrid();

					if (shouldAbort())
						return true;
				}

				pForm.info(nullptr);

				pRecord.mutex.lock();
				pRecord.canRunAnalyzer = (gotAtLeastOneReady and !errorFound and size[0] > 0 and size[0] < 2000);

				if (not pRecord.canRunAnalyzer)
				{
					if (errorFound)
						pForm.info("Impossible to run the analyzer. An error has been found.");
					else
					{
						if (!gotAtLeastOneReady)
							pForm.info("Impossible to run the analyzer. No timeseries to analyze");
						else
						{
							if (size[0] >= 2000)
								pForm.info("Impossible to run the analyzer. Too many timeseries per area");
						}
					}
				}
				else
				{
					pForm.info(String() << "Ready to run the analyzer on " << size[0] << 'x'
						<< pRecordCount << " timeseries");
				}

				pForm.enableAnalyzer(pRecord.canRunAnalyzer);
				pRecord.mutex.unlock();
				return true;
			}


			virtual void onKill() override
			{
				pForm.info(nullptr);
			}

			bool widthIsValid(uint value, uint previous) const
			{
				return (pUseCustomTS) ? (value >= pMaxTS) : (value >= previous);
			}

		private:
			AnalyzerWizard& pForm;
			const AnalyzerWizard::FileMappingPtr pMapping;
			AnalyzerWizard::RecordVector& pRecord;
			uint pRecordCount;
			bool pUseCustomTS;
			uint pMaxTS;

		}; // class AnalyzeSourceFolder



		class AnalyzeTimer final : public wxTimer
		{
		public:
			AnalyzeTimer(AnalyzerWizard& form) :
				wxTimer(), pForm(form)
			{}
			virtual ~AnalyzeTimer()
			{}

			void Notify() override
			{
				pForm.recheckEachArea();
			}

		private:
			AnalyzerWizard& pForm;
		};


		class ProceedTimer final : public wxTimer
		{
		public:
			ProceedTimer(AnalyzerWizard& form) :
				wxTimer(), pForm(form)
			{}
			virtual ~ProceedTimer()
			{}

			void Notify() override
			{
				String file;
				file << Antares::memory.cacheFolder()
					<< SEP << "antares-" << Antares::memory.processID()
					<< ".antares-analyzer";

				if (pForm.saveToFile(file))
				{
					pForm.analyzerInfoFile(file);
					pForm.Close();
				}
				else
				{
					logs.error() << "Impossible to create a temporary file which would contain the required informations for the analyzer";
					pForm.recheckEachArea();
				}
			}

		private:
			AnalyzerWizard& pForm;
		};



	} // anonymous namespace









	void AnalyzerWizard::ResetLastFolderToCurrentStudyUser()
	{
		// nothing to do if there is no study
		if (not Data::Study::Current::Valid())
		{
			gLastFolderForTSAnalyzer.clear();
			return;
		}

		// the current study
		auto& study = *Data::Study::Current::Get();
		if (study.folder.empty())
		{
			// The study must be saved for using the analyzer
			gLastFolderForTSAnalyzer.clear();
		}
		else
		{
			// the new value is <study folder> + /user
			String newvalue;
			newvalue << study.folder << SEP << "user";
			if (!IO::Directory::Exists(newvalue))
			{
				// If the user folder does not exist, fallback to the study folder
				gLastFolderForTSAnalyzer = wxStringFromUTF8(study.folder);
			}
			else
				gLastFolderForTSAnalyzer = wxStringFromUTF8(newvalue);
		}
	}



	AnalyzerWizard::AnalyzerWizard(wxFrame* parent) :
		wxDialog(parent, wxID_ANY, wxT("Timeseries Analyzer"), wxDefaultPosition, wxSize(750, 450),
			wxCLOSE_BOX|wxCAPTION|wxCLIP_CHILDREN),
		pRefreshTimer(nullptr),
		pProceedTimer(nullptr),
		pAnalyzeSource(nullptr),
		pCheckRelationship(nullptr),
		pFileSearch(nullptr),
		pTSSelected(Data::timeSeriesCount), // invalid in our case
		pUpdating(false)
	{
		pRefreshTimer = new AnalyzeTimer(*this);

		wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

		// Header
		mainSizer->Add(Toolbox::Components::WizardHeader::Create(this, wxT("Timeseries Analyzer"),
			"images/32x32/new.png",
			wxT("")), 0, wxALL|wxEXPAND | wxFIXED_MINSIZE);

		Component::Notebook* n = new Component::Notebook(this, Component::Notebook::orTop);
		n->theme(Component::Notebook::themeLight);
		mainSizer->Add(n, 1, wxEXPAND|wxALL);

		auto* panelTS = new Component::Panel(n);
		auto* mainSizerTS = new wxBoxSizer(wxVERTICAL);
		mainSizerTS->AddSpacer(20);
		panelTS->SetSizer(mainSizerTS);

		auto* panelSettings = new Component::Panel(n);
		wxBoxSizer* mainSizerSettings = new wxBoxSizer(wxVERTICAL);
		panelSettings->SetSizer(mainSizerSettings);

		// Pages
		Component::Notebook::Page* pageTS = n->add(panelTS, wxT(" Timeseries & Mapping areas "));
		/*Component::Notebook::Page* pageSettings =*/
		n->add(panelSettings, wxT(" Global settings "));

		// Where to find timeseries
		{
			wxBoxSizer* hz = new wxBoxSizer(wxHORIZONTAL);
			hz->AddSpacer(20);

			wxBoxSizer* s = new wxBoxSizer(wxVERTICAL);

			wxStaticText* whereToFindTS = new wxStaticText(panelTS, wxID_ANY, wxT("Timeseries folder"));
			{
				wxFont f = whereToFindTS->GetFont();
				f.SetWeight(wxFONTWEIGHT_BOLD);
				whereToFindTS->SetFont(f);
			}
			s->Add(whereToFindTS, 0, wxEXPAND|wxALL);
			s->AddSpacer(4);

			wxSizer* sz = new wxBoxSizer(wxHORIZONTAL);

			// Browse
			sz->AddSpacer(2);

			auto* btn = new Component::Button(panelTS, wxT("Browse"));
			btn->image("images/16x16/folder-search.png");
			btn->dropDown(true);
			btn->onClick(this, &AnalyzerWizard::onBrowse);
			btn->onPopupMenu(this, &AnalyzerWizard::onBrowseMenu);
			sz->Add(btn, 0, wxEXPAND|wxALL);
			sz->AddSpacer(3);

			pPath = new wxTextCtrl(panelTS, wxID_ANY,
				gLastFolderForTSAnalyzer,
				wxDefaultPosition, wxSize(-1, btn->GetSize().GetHeight()), wxTE_READONLY);

			sz->Add(pPath, 1, wxEXPAND | wxALL);

			s->Add(sz, 1, wxEXPAND | wxALL);

			wxStaticText* doc = Component::CreateLabel(panelTS,
				wxT(" CSV or TSV format (*.txt, *.csv)"), false, true, -1);
			s->AddSpacer(3);
			s->Add(doc, 0, wxEXPAND|wxALL);

			s->AddSpacer(15);

			{
				auto* btn = new Component::Button(panelTS, wxT("(unknown)"));
				pTSSelector = btn;
				btn->image("images/16x16/layer.png");
				btn->menu(true);
				btn->onPopupMenu(this, &AnalyzerWizard::onSelectTimeseries);

				auto* vz = new wxBoxSizer(wxVERTICAL);
				vz->Add(Component::CreateLabel(panelTS, wxT("Target timeseries"), true), 0, wxALL|wxEXPAND);
				vz->AddSpacer(4);
				auto* localhz = new wxBoxSizer(wxHORIZONTAL);
				localhz->Add(btn, 0, wxALL|wxEXPAND);
				localhz->AddStretchSpacer();
				vz->Add(localhz, 0, wxLEFT|wxTOP|wxEXPAND);
				vz->AddStretchSpacer();
				hz->Add(vz, 0, wxALL|wxEXPAND);

				hz->AddSpacer(20);
			}

			hz->Add(s, 1, wxEXPAND | wxALL);
			hz->AddSpacer(20);
			mainSizerTS->Add(hz, 0, wxEXPAND | wxALL);
			mainSizerTS->AddSpacer(8);
		}

		// Mapping Areas & Files
		{
			auto* hz = new wxBoxSizer(wxHORIZONTAL);
			auto* split = new wxBoxSizer(wxHORIZONTAL);
			hz->AddSpacer(13);

			// Mapping areas
			// \_ renderer
			//typedef Component::Datagrid::Renderer::Analyzer::Areas RendererType;
			pRenderer = new RendererType();
			pRenderer->study = Data::Study::Current::Get();
			pRenderer->initializeFromStudy();
			onUpdateTimeseriesType.connect(pRenderer, &RendererType::reloadDistributionLawsFromStudy);
			// \_ grid
			pGrid = new Component::Datagrid::Component(panelTS, pRenderer, wxEmptyString, false, true, true);
			pGrid->markTheStudyAsModified(false);
			pGrid->Enable(false);
			{
				auto* s = new wxBoxSizer(wxVERTICAL);
				auto* v = new wxBoxSizer(wxHORIZONTAL);
				v->AddSpacer(12);
				auto* title = Component::CreateLabel(panelTS, wxT("Mapping areas"), true);
				v->Add(title, 0, wxEXPAND|wxALL);
				s->Add(v, 0, wxEXPAND|wxALL);
				s->AddSpacer(6);
				s->Add(pGrid, 1, wxEXPAND|wxALL);
				split->Add(s, 7, wxEXPAND|wxALL);
			}
			
			// Vertical separator
			split->AddSpacer(3);
			Component::AddVerticalSeparator(panelTS, split, 1, 1);
			split->AddSpacer(3);

			// File search
			{
				auto* files = new wxBoxSizer(wxVERTICAL);

				auto* title = Component::CreateLabel(panelTS, wxT(" Files in timeseries folder"), true);
				files->Add(title, 0, wxEXPAND|wxALL);
				files->AddSpacer(6);

				pFileSearch = new Component::Spotlight(panelTS, 0);
				auto* provider = new FileSearchProvider();
				onFileSearchAdd.connect(provider, &FileSearchProvider::onFileSearchAdd);
				onFileSearchClear.connect(provider, &FileSearchProvider::onFileSearchClear);
				pFileSearch->provider(provider);
				files->Add(pFileSearch, 1, wxALL|wxEXPAND);

				split->AddSpacer(5);
				split->Add(files, 2, wxEXPAND|wxALL);
			}

			auto* s = new wxBoxSizer(wxVERTICAL);
			s->Add(split, 1, wxEXPAND | wxALL);
			s->AddSpacer(8);

			hz->Add(s, 1, wxEXPAND | wxALL);
			hz->AddSpacer(20);
			mainSizerTS->Add(hz, 1, wxEXPAND | wxALL);
		}

		// Where to find timeseries
		{
			wxBoxSizer* s = new wxBoxSizer(wxVERTICAL);

			{
				auto* tmpF = Component::CreateLabel(panelSettings, wxT("Temporary folder"), true);
				s->Add(tmpF, 0, wxEXPAND|wxALL);
				s->AddSpacer(5);
			}

			pTmpUseDefault = new wxRadioButton(panelSettings, mnIDTmpUseDefault, wxT(" Use the default temporary folder"));
			pTmpUseStudyUserDir = new wxRadioButton(panelSettings, mnIDTmpUseStudyUserDir, wxT(" Use the study' user folder"));
			pTmpUseCustom = new wxRadioButton(panelSettings, mnIDTmpUseStudyUserDir, wxT(" Use a custom folder"));

			pTmpUseDefault->Connect(pTmpUseDefault->GetId(), wxEVT_COMMAND_RADIOBUTTON_SELECTED,
				wxCommandEventHandler(AnalyzerWizard::evtTemporaryPathSelectorChanged), nullptr, this);
			pTmpUseStudyUserDir->Connect(pTmpUseStudyUserDir->GetId(), wxEVT_COMMAND_RADIOBUTTON_SELECTED,
				wxCommandEventHandler(AnalyzerWizard::evtTemporaryPathSelectorChanged), nullptr, this);
			pTmpUseCustom->Connect(pTmpUseCustom->GetId(), wxEVT_COMMAND_RADIOBUTTON_SELECTED,
				wxCommandEventHandler(AnalyzerWizard::evtTemporaryPathSelectorChanged), nullptr, this);

			pTmpUseDefault->Enable(false);
			pTmpUseStudyUserDir->Enable(false);
			pTmpUseStudyUserDir->SetValue(true);
			pTmpUseCustom->Enable(false);

			s->Add(pTmpUseDefault,0, wxEXPAND|wxALL);
			if (System::windows)
				s->AddSpacer(3);

			s->Add(pTmpUseStudyUserDir, 0, wxEXPAND|wxALL);
			if (System::windows)
				s->AddSpacer(3);

			s->Add(pTmpUseCustom, 0, wxEXPAND|wxALL);
			if (System::windows)
				s->AddSpacer(3);


			pPathTemp = new wxTextCtrl(panelSettings, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY);
			pPathTemp->SetValue(wxStringFromUTF8(Antares::memory.cacheFolder()));
			pPathTemp->Enable(false);
			pBtnBrowseTemp = Component::CreateButton(panelSettings, wxT("Browse"), this, &AnalyzerWizard::onBrowseTemp);
			pBtnBrowseTemp->Enable(false);
			wxSizer* sz = new wxBoxSizer(wxHORIZONTAL);
			sz->Add(20, 5);
			sz->Add(pBtnBrowseTemp, 0, wxEXPAND|wxALL);
			sz->AddSpacer(4);

			sz->Add(pPathTemp, 1, wxEXPAND | wxALL);
			sz->AddSpacer(6);

			if (System::windows)
				s->AddSpacer(3);
			s->Add(sz, 1, wxEXPAND | wxALL);

			s->AddSpacer(8);

			pAutoClean = new wxCheckBox(panelSettings, mnIDAutoClean, wxT(" Clean the temporary folder"));
			pAutoClean->Enable(false);
			pAutoClean->SetValue(true);
			s->Add(pAutoClean, 0, wxEXPAND|wxALL);

			pLblCleanInfo = Component::CreateLabel(panelSettings,
				wxT("(All files created during the process will be removed)"), false, false, -1);
			wxBoxSizer* cleanInfoSizer = new wxBoxSizer(wxHORIZONTAL);
			cleanInfoSizer->Add(22, 5);
			cleanInfoSizer->Add(pLblCleanInfo, 0, wxEXPAND|wxALL);
			if (System::windows)
				s->AddSpacer(2);
			s->Add(cleanInfoSizer, 0, wxEXPAND|wxALL);

			s->AddSpacer(30);

			{
				wxStaticText* tmpF = new wxStaticText(panelSettings, wxID_ANY, wxT("Analyzer Settings"));
				wxFont f = tmpF->GetFont();
				f.SetWeight(wxFONTWEIGHT_BOLD);
				tmpF->SetFont(f);
				s->Add(tmpF, 0, wxEXPAND|wxALL);
				s->AddSpacer(5);
			}

			{
				wxFlexGridSizer* flex = new wxFlexGridSizer(3, 1, 0);

				wxStaticText* lblShortTerm = new wxStaticText(panelSettings, wxID_ANY, wxT("Short-term auto-correlation adjustment : "));
				wxStaticText* lblMediumTerm  = new wxStaticText(panelSettings, wxID_ANY, wxT("Medium-term auto-correlation adjustment : "));
				wxStaticText* lblTrim = new wxStaticText(panelSettings, wxID_ANY, wxT("Trimming threshold : "));

				wxStaticText* p1 = new wxStaticText(panelSettings, wxID_ANY, wxT(" %"));
				wxStaticText* p2 = new wxStaticText(panelSettings, wxID_ANY, wxT(" %"));
				wxStaticText* p3 = new wxStaticText(panelSettings, wxID_ANY, wxT(" %"));

				pEdShortTermAutoCorr = new wxTextCtrl(panelSettings, wxID_ANY, wxT("90"), wxDefaultPosition, wxSize(50, -1));
				pEdMediumTermAutoCorr  = new wxTextCtrl(panelSettings, wxID_ANY, wxT("50"), wxDefaultPosition, wxSize(50, -1));
				pEdTrim = new wxTextCtrl(panelSettings, wxID_ANY, wxT("5"), wxDefaultPosition,  wxSize(50, -1));

				pEdShortTermAutoCorr->SetMaxLength(5);
				pEdMediumTermAutoCorr->SetMaxLength(5);
				pEdTrim->SetMaxLength(5);

				flex->Add(lblShortTerm, 0, wxRIGHT | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
				flex->Add(pEdShortTermAutoCorr, 0, wxALL|wxEXPAND);
				flex->Add(p1, 0, wxLEFT | wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL);

				flex->Add(lblMediumTerm,  0, wxRIGHT | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
				flex->Add(pEdMediumTermAutoCorr, 0, wxALL|wxEXPAND);
				flex->Add(p2, 0, wxLEFT | wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL);

				flex->Add(lblTrim, 0, wxRIGHT | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
				flex->Add(pEdTrim, 0, wxALL|wxEXPAND);
				flex->Add(p3, 0, wxLEFT | wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL);

				s->Add(flex, 0, wxALL|wxEXPAND);
			}

			{
				s->AddSpacer(30);
				wxStaticText* tmpF = new wxStaticText(panelSettings, wxID_ANY, wxT("Input data"));
				wxFont f = tmpF->GetFont();
				f.SetWeight(wxFONTWEIGHT_BOLD);
				tmpF->SetFont(f);
				s->Add(tmpF, 0, wxEXPAND|wxALL);
				s->AddSpacer(5);
			}

			{
				wxFlexGridSizer* flex = new wxFlexGridSizer(3, 1, 0);

				wxStaticText* lblNB    = new wxStaticText(panelSettings, wxID_ANY, wxT("Timeseries per area :    "));
				wxStaticText* lblCeil  = new wxStaticText(panelSettings, wxID_ANY, wxT("Upper bound :    "));
				wxStaticText* lblFloor = new wxStaticText(panelSettings, wxID_ANY, wxT("Lower bound :    "));

				pCbMaxTimeseries = new wxCheckBox(panelSettings, wxID_ANY, wxT(" custom value  "));
				pCbUpperBound    = new wxCheckBox(panelSettings, wxID_ANY, wxT(" enabled  "));
				pCbLowerBound    = new wxCheckBox(panelSettings, wxID_ANY, wxT(" enabled  "));

				pEdMaxTimeseries = new wxTextCtrl(panelSettings, wxID_ANY, wxT("100"), wxDefaultPosition, wxSize(50, -1));
				pEdUpperBound    = new wxTextCtrl(panelSettings, wxID_ANY, wxT("100.0"), wxDefaultPosition, wxSize(50, -1));
				pEdLowerBound    = new wxTextCtrl(panelSettings, wxID_ANY, wxT("0.0"), wxDefaultPosition,  wxSize(50, -1));

				pEdMaxTimeseries->SetMaxLength(5);
				pEdUpperBound->SetMaxLength(5);
				pEdLowerBound->SetMaxLength(5);

				flex->Add(lblNB, 0, wxRIGHT | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
				flex->Add(pCbMaxTimeseries, 0, wxLEFT | wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL);
				flex->Add(pEdMaxTimeseries, 0, wxALL|wxEXPAND);

				flex->Add(lblCeil,  0, wxRIGHT | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
				flex->Add(pCbUpperBound, 0, wxLEFT | wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL);
				flex->Add(pEdUpperBound, 0, wxALL|wxEXPAND);

				flex->Add(lblFloor, 0, wxRIGHT | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
				flex->Add(pCbLowerBound, 0, wxLEFT | wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL);
				flex->Add(pEdLowerBound, 0, wxALL|wxEXPAND);

				s->Add(flex, 0, wxALL|wxEXPAND);

				pCbMaxTimeseries->Connect(pCbMaxTimeseries->GetId(), wxEVT_COMMAND_CHECKBOX_CLICKED,
					wxCommandEventHandler(AnalyzerWizard::evtLimitsChanged), nullptr, this);
				pCbUpperBound->Connect(pCbUpperBound->GetId(), wxEVT_COMMAND_CHECKBOX_CLICKED,
					wxCommandEventHandler(AnalyzerWizard::evtLimitsChanged), nullptr, this);
				pCbLowerBound->Connect(pCbLowerBound->GetId(), wxEVT_COMMAND_CHECKBOX_CLICKED,
					wxCommandEventHandler(AnalyzerWizard::evtLimitsChanged), nullptr, this);

				pEdMaxTimeseries->Connect(pEdMaxTimeseries->GetId(), wxEVT_COMMAND_TEXT_UPDATED,
					wxCommandEventHandler(AnalyzerWizard::evtLimitsChanged), nullptr, this);
			}

			mainSizerSettings->Add(s, 0, wxEXPAND | wxALL, 30);
		}

		// Buttons
		{
			wxBoxSizer* pnlBtns = new wxBoxSizer(wxHORIZONTAL);
			pnlBtns->Add(20, 5);

			pLblInfo = Component::CreateLabel(this, wxEmptyString);
			pnlBtns->Add(pLblInfo, 0, wxLEFT | wxALIGN_CENTER_VERTICAL);
			pnlBtns->AddStretchSpacer();

			wxWindow* pBtnCancel = Component::CreateButton(this, wxT(" Cancel "), this, &AnalyzerWizard::onCancel);
			pnlBtns->Add(pBtnCancel, 0, wxALL|wxEXPAND);
			pnlBtns->AddSpacer(5);
			pBtnRun = Component::CreateButton(this, wxT(" Run the analyzer "), this, &AnalyzerWizard::onProceed);
			pBtnRun->Enable(false);
			pnlBtns->Add(pBtnRun, 0, wxALL|wxEXPAND);
			pnlBtns->Add(25, 5);

			// Button Panel
			mainSizer->AddSpacer(2);
			mainSizer->Add(new wxStaticLine(this, wxID_ANY), 0, wxALL|wxEXPAND);
			mainSizer->AddSpacer(8);
			mainSizer->Add(pnlBtns, 0, wxALL|wxEXPAND);
			mainSizer->AddSpacer(8);
		}

		// Update the informations about the temp folder
		updateInfoForTempFolder();

		enableAll(false);

		pageTS->select();
		// Set the sizer
		SetSizer(mainSizer);

		pGrid->forceRefresh();

		GetSizer()->Fit(this);
		SetSize(900, 600);
		Centre(wxBOTH);

		Yuni::Bind<void (int, int)> b1;
		b1.bind(this, &AnalyzerWizard::beforeUpdate);
		pRenderer->onBeforeUpdate(b1);

		Yuni::Bind<void (int, int)> b2;
		b2.bind(this, &AnalyzerWizard::afterUpdate);
		pRenderer->onAfterUpdate(b2);


		// Automatic analyzis of the last folder
		Yuni::Bind<void ()> callback;
		callback.bind(this, &AnalyzerWizard::browseDataFolder, gLastFolderForTSAnalyzer);
		Dispatcher::GUI::Post(callback, 100 /*ms*/);
	}


	AnalyzerWizard::~AnalyzerWizard()
	{
		if (pProceedTimer)
		{
			delete pProceedTimer;
			pProceedTimer = nullptr;
		}
		if (pRefreshTimer)
		{
			delete pRefreshTimer;
			pRefreshTimer = nullptr;
		}

		delete pAnalyzeSource;
		delete pCheckRelationship;
	}


	void AnalyzerWizard::onCancel(void*)
	{
		// Try to stop the threads as soon as before, even before destroying the
		// window itself
		if (pAnalyzeSource)
			pAnalyzeSource->gracefulStop();
		if (pCheckRelationship)
			pCheckRelationship->gracefulStop();
		// Disabling all components
		enableAll(false);
		// Closing the window
		Dispatcher::GUI::Close(this);
	}


	void AnalyzerWizard::onProceed(void*)
	{
		// Check for restrictions
		if (not Data::Study::Current::Valid())
			return;

		if (pTSSelected == Data::timeSeriesCount) // invalid in our case
		{
			pTSSelector->pushed(true); // visual notification
			pTSSelector->image("images/16x16/error.png");
			pTSSelector->Refresh();

			Window::Message message(this, wxT("Run the analyzer"),
				wxT("Target timeseries"),
				wxString() << wxT("The target timeseries is unknown. ")
					<< wxT("Please select 'load', 'wind' or 'solar' before running the analysis"),
				"images/misc/error.png");
			message.add(Window::Message::btnCancel, true);
			message.showModal();

			pTSSelector->pushed(false);
			pTSSelector->image("images/16x16/layer.png");
			return;
		}

		// Reset internal IO statistics
		Statistics::Reset();

		if (License::Limits::areaCount) // checking for license restrictions
		{
			auto& study = *Data::Study::Current::Get();
			bool isTrial = (study.areas.size() > License::Limits::areaCount);
			if (not isTrial and License::Limits::thermalClusterCount)
			{
				study.areas.each([&] (const Data::Area& area)
				{
					if (area.thermal.list.size() + area.thermal.mustrunList.size() > License::Limits::thermalClusterCount)
						isTrial = true;
				});
			}

			if (isTrial)
			{
				wxString text;
				if (License::Limits::thermalClusterCount)
				{
					text << wxT("Analyzer limited to ") << License::Limits::areaCount
						<< wxT(" areas and ") << License::Limits::thermalClusterCount
						<< wxT(" thermal clusters / area.");
				}
				else
				{
					text << wxT("Analyzer limited to ") << License::Limits::areaCount << wxT(" areas");
				}

				Window::Message message(this, wxT("Analyzer"),
					wxT("LICENSE RESTRICTION"), text, "images/misc/warning.png");
				message.add(Window::Message::btnCancel, true);
				message.showModal();
				return;
			}
		}

		Enable(false);
		if (StudyHasBeenModified())
		{
			Window::Message message(this, wxT("Analyzer"),
				wxT("Analyzer"),
				wxT("The study has been modified. All changes must be written to the disk\nbefore using the analyzer.")
				);
			message.add(Window::Message::btnSaveChanges);
			message.add(Window::Message::btnCancel, true);
			if (message.showModal() == Window::Message::btnCancel)
			{
				Enable(false);
				return;
			}

			const SaveResult r = ::Antares::SaveStudy();
			if (!(r == svsDiscard or r == svsSaved))
			{
				Enable(false);
				return;
			}
		}

		// Block all matrices
		// It will be unlocked later
		OnStudyBeginUpdate();

		// Try to stop the threads as soon as before, even before destroying the
		// window itself
		if (pAnalyzeSource)
			pAnalyzeSource->gracefulStop();
		if (pCheckRelationship)
			pCheckRelationship->gracefulStop();
		// Disabling all components
		enableAll(false);

		if (!pProceedTimer)
			pProceedTimer = new ProceedTimer(*this);
		pProceedTimer->Start(200, true);
	}


	void AnalyzerWizard::onBrowse(void*)
	{
		if (pUpdating)
			return;
		wxString path;
		{
			wxDirDialog dlg(this, wxT("Data folder"), pPath->GetValue(),
				wxDD_DEFAULT_STYLE | wxDD_DIR_MUST_EXIST, wxDefaultPosition, wxSize(500, 600));
			if (wxID_OK != dlg.ShowModal())
				return;
			path = dlg.GetPath();
		}

		// Prevention of unwanted behaviour
		if (path == wxT("/") ||
			(path.size() == 3 and path[1] == L':' and (path[2] == L'\\' or path[2] == L'/')))
		{
			Window::Message message(this, wxT("Folder selection"),
				wxT("Confirmation"),
				wxString() << wxT("Scanning the folder \"") << path << wxT("\" ")
					<< wxT("may cause high cpu usage and may take some time to complete.")
					<< wxT("\n\nDo you really want to continue ?\n\n"),
				"images/misc/warning.png");
			message.add(Window::Message::btnContinue);
			message.add(Window::Message::btnCancel, true);
			if (message.showModal() != Window::Message::btnContinue)
				return;
		}
		browseDataFolder(path);
	}


	void AnalyzerWizard::browseDataFolder(const wxString& newpath)
	{
		if (pUpdating)
			return;
		pUpdating = true;
		pRefreshTimer->Stop();
		if (pCheckRelationship)
		{
			delete pCheckRelationship;
			pCheckRelationship = nullptr;
		}

		if (not newpath.empty()) // may be empty
		{
			gLastFolderForTSAnalyzer = newpath;
			pPath->SetValue(newpath);
		}

		enableAll(false);
		pRenderer->resetStatusWaiting();
		pGrid->forceRefresh();

		delete pAnalyzeSource;
		// Reducing memory consumption
		pFileMapping = nullptr;

		pRefreshTimer->Stop();
		pAnalyzeSource = new AnalyzeSourceFolder(*this, pPath->GetValue());
		pAnalyzeSource->start();
	}


	void AnalyzerWizard::onBrowseTemp(void*)
	{
		wxDirDialog dlg(this, wxT("Temporary folder"), pPathTemp->GetValue(),
			wxDD_DEFAULT_STYLE | wxDD_DIR_MUST_EXIST);
		if (wxID_OK == dlg.ShowModal())
			pPathTemp->SetValue(dlg.GetPath());
	}


	void AnalyzerWizard::evtUpdateInfo(wxCommandEvent& evt)
	{
		pLblInfo->SetLabel(evt.GetString());
		pLblInfo->Refresh();
	}


	void AnalyzerWizard::enableAll(bool v)
	{
		pGrid->Enable(v);
		pAutoClean->Enable(v);
		pBtnRun->Enable(false); // always false here
		pLblCleanInfo->Enable(v);

		pTmpUseDefault->Enable(v);
		pTmpUseStudyUserDir->Enable(v);
		pTmpUseCustom->Enable(v);
		pPathTemp->Enable(v and pTmpUseCustom->GetValue());
		pBtnBrowseTemp->Enable(v and pTmpUseCustom->GetValue());

		pCbMaxTimeseries->Enable(v);
		pCbUpperBound->Enable(v);
		pCbLowerBound->Enable(v);

		pEdShortTermAutoCorr->Enable(v);
		pEdMediumTermAutoCorr->Enable(v);
		pEdTrim->Enable(v);
		pEdMaxTimeseries->Enable(v and pCbMaxTimeseries->GetValue());
		pEdLowerBound->Enable(v and pCbLowerBound->GetValue());
		pEdUpperBound->Enable(v and pCbUpperBound->GetValue());
	}


	void AnalyzerWizard::evtTemporaryPathSelectorChanged(wxCommandEvent& evt)
	{
		updateInfoForTempFolder();
		evt.Skip();
	}


	void AnalyzerWizard::updateInfoForTempFolder()
	{
		// Accessibility
		pPathTemp->Enable(pTmpUseCustom->IsEnabled() and pTmpUseCustom->GetValue());
		pBtnBrowseTemp->Enable(pPathTemp->IsEnabled());

		if (pTmpUseDefault->GetValue())
		{
			const wxString& text = wxStringFromUTF8(Antares::memory.cacheFolder());
			pPathTemp->SetValue(text);
		}
		else
		{
			if (pTmpUseStudyUserDir->GetValue())
			{
				String s;
				s << Data::Study::Current::Get()->folder << SEP << "user" << SEP << "analyzer";
				pPathTemp->SetValue(wxStringFromUTF8(s));
			}
		}

	}


	void AnalyzerWizard::fileMapping(FileMapping* m)
	{
		pFileMapping = m;

		wxCommandEvent evt(wxEVT_COMMAND_MENU_SELECTED, mnIDUpdateFileMapping);
		AddPendingEvent(evt);
	}


	void AnalyzerWizard::refreshGrid()
	{
		wxCommandEvent evt(wxEVT_COMMAND_MENU_SELECTED, mnIDGridUpdate);
		AddPendingEvent(evt);
	}


	void AnalyzerWizard::enableAnalyzer(bool v)
	{
		wxCommandEvent evt(wxEVT_COMMAND_MENU_SELECTED, mnIDCanRunAnalyzer);
		evt.SetInt(v ? 1 : 0);
		AddPendingEvent(evt);
	}


	void AnalyzerWizard::evtUpdateFileMapping(wxCommandEvent&)
	{
		pRefreshTimer->Stop();

		auto fmapptr = pFileMapping;
		FileMapping& map = *fmapptr;

		// clear the filesearch component
		onFileSearchClear();

		if (!(&map) or map.empty())
		{
			enableAll(false);
		}
		else
		{
			auto end = map.end();
			for (auto i = map.begin(); i != end; ++i)
				onFileSearchAdd(i->first);

			enableAll(true);
		}

		pRenderer->resetStatusWaiting();
		pGrid->forceRefresh();

		delete pCheckRelationship;

		auto* relship = new AnalyzeRelationship(*this, fmapptr, pRenderer->record());
		onUpdateMaxTimeseries.connect(relship, &AnalyzeRelationship::updateMaxTimeseries);
		pCheckRelationship = relship;
		updateMaxTimeseries();

		pRefreshTimer->Start(100, true);

		if (pFileSearch)
			pFileSearch->redoResearch();
		Dispatcher::GUI::Refresh(this);

		pUpdating = false;
	}


	void AnalyzerWizard::evtGridUpdate(wxCommandEvent&)
	{
		pLblInfo->SetLabel(wxEmptyString);
		Dispatcher::GUI::Refresh(pLblInfo);
		Dispatcher::GUI::Refresh(pGrid);
	}


	void AnalyzerWizard::evtCanRunAnalyzer(wxCommandEvent& evt)
	{
		pBtnRun->Enable(evt.GetInt() != 0);
		Dispatcher::GUI::Refresh(pBtnRun);
	}


	void AnalyzerWizard::recheckEachArea()
	{
		if (pCheckRelationship)
		{
			pCheckRelationship->stop();
			if (pLblInfo)
				pLblInfo->SetLabel(wxEmptyString);
			Dispatcher::GUI::Refresh(pLblInfo);
			pCheckRelationship->start();
		}
	}


	void AnalyzerWizard::beforeUpdate(int, int)
	{
		if (pRefreshTimer)
			pRefreshTimer->Stop();

		if (pBtnRun)
		{
			pBtnRun->Enable(false);
			Dispatcher::GUI::Refresh(pBtnRun);
		}
	}


	void AnalyzerWizard::afterUpdate(int, int)
	{
		if (pRefreshTimer)
			pRefreshTimer->Start(200, true);
	}


	void AnalyzerWizard::updateMaxTimeseries()
	{
		if (pBtnRun)
			pBtnRun->Enable(false);

		uint maxTS;
		bool useCustomTS = pCbMaxTimeseries->GetValue();
		String tmp;

		wxStringToString(pEdMaxTimeseries->GetValue(), tmp);

		if (not tmp.to(maxTS))
		{
			useCustomTS = false;
		}
		else
		{
			if (maxTS > 5000)
				useCustomTS = false;
		}

		onUpdateMaxTimeseries(useCustomTS, maxTS);
		if (pRenderer)
			pRenderer->resetStatusWaiting(false);
	}


	void AnalyzerWizard::evtLimitsChanged(wxCommandEvent& evt)
	{
		bool enabled = pCbMaxTimeseries->IsEnabled();
		pEdMaxTimeseries->Enable(enabled and pCbMaxTimeseries->GetValue());
		pEdLowerBound->Enable(enabled and pCbLowerBound->GetValue());
		pEdUpperBound->Enable(enabled and pCbUpperBound->GetValue());

		updateMaxTimeseries();
		recheckEachArea();

		evt.Skip();
	}



	bool AnalyzerWizard::saveToFile(const String& filename) const
	{
		if (not Data::Study::Current::Valid())
			return false;
		String tmp;
		auto& study = *Data::Study::Current::Get();

		IniFile ini;
		auto* mainSection = ini.addSection(".general");

		// Study
		mainSection->add("study", study.folder);
		// Tmp
		wxStringToString(pPathTemp->GetValue(), tmp);
		mainSection->add("temporary", tmp);

		// Target generator
		switch (pTSSelected)
		{
			case Data::timeSeriesLoad:  mainSection->add("target", "load"); break;
			case Data::timeSeriesSolar: mainSection->add("target", "solar"); break;
			case Data::timeSeriesWind:  mainSection->add("target", "wind"); break;
			default: mainSection->add("target", "load");
		}

		uint size[2];

		uint maxTS;
		bool useCustomTS = pCbMaxTimeseries->GetValue();
		wxStringToString(pEdMaxTimeseries->GetValue(), tmp);
		if (!tmp.to(maxTS))
			useCustomTS = false;
		else
		{
			if (maxTS > 5000)
				useCustomTS = false;
		}

		// aliases
		typedef Component::Datagrid::Renderer::Analyzer::Areas::Record Record;
		auto& mapping = *pFileMapping;
		auto& pRecord = pRenderer->record();

		pRecord.mutex.lock();
		uint recordCount = (uint) pRecord.array.size();
		for (uint i = 0; i != recordCount; ++i)
		{
			const Record& r = pRecord.array[i];
			if (!r.enabled or r.status != Record::stReady or !r.fileToSearch)
				continue;

			auto it = mapping.find(r.fileToSearch);
			if (it == mapping.end())
				continue;

			auto& area = *(study.areas.byIndex[r.areaIndex]);
			switch (pTSSelected)
			{
				case Data::timeSeriesLoad:
					if (area.load.prepro)
						area.load.prepro->xcast.distribution = r.distribution;
					break;
				case Data::timeSeriesSolar:
					if (area.solar.prepro)
						area.solar.prepro->xcast.distribution = r.distribution;
					break;
				case Data::timeSeriesWind:
					if (area.wind.prepro)
						area.wind.prepro->xcast.distribution = r.distribution;
					break;
				default:
					break;
			}

			auto* s = ini.addSection(area.id);
			s->add("file", it->second);
			size[0] = (useCustomTS) ? maxTS : r.mWidth;
			size[1] = r.mHeight;
			s->add("distribution", Data::XCast::DistributionToCString(r.distribution));
			s->add("data", r.absolute ? "raw" : "detrended");
		}

		pRecord.mutex.unlock();

		mainSection->add("width",  size[0]);
		mainSection->add("height", size[1]);
		mainSection->add("short-term-autocorrelation",   wxStringToDouble(pEdShortTermAutoCorr->GetValue()));
		mainSection->add("medium-term-autocorrelation",  wxStringToDouble(pEdMediumTermAutoCorr->GetValue()));
		mainSection->add("trimming",             wxStringToDouble(pEdTrim->GetValue()));
		mainSection->add("upperbound-enable",    pCbUpperBound->GetValue());
		mainSection->add("upperbound-value",     wxStringToDouble(pEdUpperBound->GetValue()));
		mainSection->add("lowerbound-enable",    pCbLowerBound->GetValue());
		mainSection->add("lowerboundr-value",    wxStringToDouble(pEdLowerBound->GetValue()));
		mainSection->add("memory-cache",         200);
		mainSection->add("clean",                pAutoClean->GetValue());

		return ini.save(filename);
	}


	void AnalyzerWizard::onBrowseMenu(Component::Button&, wxMenu& menu, void*)
	{
		if (pUpdating)
			return;
		wxMenuItem* it;

		it = Menu::CreateItem(&menu, wxID_ANY, wxT("Reset to the default user's folder"),
			"images/16x16/undo.png");
		menu.Connect(it->GetId(), wxEVT_COMMAND_MENU_SELECTED,
			wxCommandEventHandler(AnalyzerWizard::onBrowseReset), nullptr, this);

		menu.AppendSeparator();

		if (System::windows)
			it = Menu::CreateItem(&menu, wxID_ANY, wxT("Open in Windows Explorer"));
		else
			it = Menu::CreateItem(&menu, wxID_ANY, wxT("Open in Gnome Nautilus"));
		menu.Connect(it->GetId(), wxEVT_COMMAND_MENU_SELECTED,
			wxCommandEventHandler(AnalyzerWizard::onBrowseOpenInExplorer), nullptr, this);

		it = Menu::CreateItem(&menu, wxID_ANY, wxT("Refresh"));
		menu.Connect(it->GetId(), wxEVT_COMMAND_MENU_SELECTED,
			wxCommandEventHandler(AnalyzerWizard::onBrowseRefresh), nullptr, this);
	}


	void AnalyzerWizard::onSelectTimeseries(Component::Button&, wxMenu& menu, void*)
	{
		if (pUpdating)
			return;
		wxMenuItem* it;

		it = Menu::CreateItem(&menu, wxID_ANY, wxT("Load"));
		menu.Connect(it->GetId(), wxEVT_COMMAND_MENU_SELECTED,
			wxCommandEventHandler(AnalyzerWizard::onSelectTSLoad), nullptr, this);

		it = Menu::CreateItem(&menu, wxID_ANY, wxT("Wind"));
		menu.Connect(it->GetId(), wxEVT_COMMAND_MENU_SELECTED,
			wxCommandEventHandler(AnalyzerWizard::onSelectTSWind), nullptr, this);

		it = Menu::CreateItem(&menu, wxID_ANY, wxT("Solar"));
		menu.Connect(it->GetId(), wxEVT_COMMAND_MENU_SELECTED,
			wxCommandEventHandler(AnalyzerWizard::onSelectTSSolar), nullptr, this);
	}


	void AnalyzerWizard::onSelectTSLoad(wxCommandEvent&)
	{
		pTSSelected = Data::timeSeriesLoad;
		pTSSelector->caption(wxT("Load"));
		onUpdateTimeseriesType(pTSSelected);
		Dispatcher::GUI::Refresh(pTSSelector);
		if (pGrid)
			pGrid->forceRefresh();
	}


	void AnalyzerWizard::onSelectTSWind(wxCommandEvent&)
	{
		pTSSelected = Data::timeSeriesWind;
		pTSSelector->caption(wxT("Wind"));
		onUpdateTimeseriesType(pTSSelected);
		Dispatcher::GUI::Refresh(pTSSelector);
		if (pGrid)
			pGrid->forceRefresh();
	}


	void AnalyzerWizard::onSelectTSSolar(wxCommandEvent&)
	{
		pTSSelected = Data::timeSeriesSolar;
		pTSSelector->caption(wxT("Solar"));
		onUpdateTimeseriesType(pTSSelected);
		Dispatcher::GUI::Refresh(pTSSelector);
		if (pGrid)
			pGrid->forceRefresh();
	}


	void AnalyzerWizard::onBrowseRefresh(wxCommandEvent&)
	{
		browseDataFolder(wxEmptyString);
	}


	void AnalyzerWizard::onBrowseOpenInExplorer(wxCommandEvent&)
	{
		const wxString& path = pPath->GetValue();
		if (System::windows)
			wxExecute(wxString(wxT("explorer.exe \"")) << path << wxT("\""));
		else
			wxExecute(wxString(wxT("gnome-open \"")) << path << wxT("\""));
	}


	void AnalyzerWizard::onBrowseReset(wxCommandEvent&)
	{
		if (pUpdating or not Data::Study::Current::Valid())
			return;
		auto& study = *Data::Study::Current::Get();
		wxString path = wxStringFromUTF8(study.folder);
		path << IO::Constant<wchar_t>::Separator << wxT("user");
		browseDataFolder(path);
	}


	void AnalyzerWizard::info(const AnyString& text)
	{
		const wxString wxS = wxStringFromUTF8(text);
		wxCommandEvent evt(wxEVT_COMMAND_MENU_SELECTED, mnIDUpdateInfo);
		evt.SetString(wxS);
		AddPendingEvent(evt);
	}




} // namespace Window
} // namespace Antares

