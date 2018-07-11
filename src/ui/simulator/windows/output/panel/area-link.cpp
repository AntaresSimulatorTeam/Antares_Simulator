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

#include "panel.h"
#include <yuni/io/file.h>
#include "../../../application/study.h"
#include "../../../application/wait.h"
#include "../output.h"
#include "../../../toolbox/components/datagrid/component.h"
#include <yuni/job/job.h>
#include "area-link-renderer.h"
#include <ui/common/lock.h>
#include <limits>


using namespace Yuni;

#define SEP Yuni::IO::Separator

# ifdef YUNI_OS_MSVC
#	define ANTARES_MATRIX_SNPRINTF  sprintf_s
# else
#	define ANTARES_MATRIX_SNPRINTF  snprintf
# endif



namespace Antares
{
namespace Private
{
namespace OutputViewerData
{


	namespace // anonymous
	{
		static const char* const sfmt[] =
		{
			"%.0f",  "%.1f",  "%.2f", "%.3f",  "%.4f",  "%.5f",  "%.6f",
			"%.7f",	 "%.8f",  "%.9f", "%.10f", "%.11f", "%.12f", "%.13f",
			"%.14f", "%.15f", "%.16f",
		};



		class JobMatrix : public Yuni::Job::IJob, public Yuni::IEventObserver<JobMatrix>
		{
		public:
			//! Smart pointer
			typedef SmartPtr<JobMatrix>  Ptr;
			//! Matrix type
			typedef ::Antares::Private::OutputViewerData::Panel::MatrixType MatrixType;

		public:
			JobMatrix(Panel& panel) :
				Yuni::Job::IJob(),
				shouldAbort(0),
				pPanel(panel),
				pMatrix(nullptr)
			{
			}

			virtual ~JobMatrix()
			{
				delete pMatrix;
				IEventObserver<JobMatrix>::destroyBoundEvents();
			}

			void stopLoading()
			{
				shouldAbort = 1;
			}

		public:
			//! Filename to load
			String filename;
			//! Loading options
			uint options;
			//!
			Atomic::Int<> shouldAbort;


		protected:
			virtual void onExecute()
			{
				if (shouldAbort)
					return;

				logs.info() << "[output-viewer] loading " << filename;
				bool success = false;

				// The swap memory MUST not be flushed. This can happen since we are not
				// in the main thread
				MemoryFlushLocker memoryLocker;

				pMatrix = new MatrixType();

				// try to open the file
				{
					// file lock
					auto mutex = ProvideLockingForFileLocking(filename);
					Yuni::MutexLocker locker(*mutex);
					if (not shouldAbort)
						success = pMatrix->loadFromCSVFile(filename, 1, 0, options);
				}
				# ifndef NDEBUG
				if (success)
					logs.debug() << "success " << filename;
				else
					logs.debug() << "failed " << filename;
				# endif

				if (not shouldAbort)
				{
					// Callback for updating the GUI
					Bind<void ()> callback;
					// note : The call to loadDataFromMatrix should be delayed to let the gui
					// perform some refresh
					// However, we should not delay with a timeout, which may cause
					// race condition for the GUI (hard to debug)
					if (success)
					{
						// dispatch matrix loading (for taking in the main thread)
						callback.bind(&pPanel, &Panel::loadDataFromMatrix, pMatrix);
						if (not shouldAbort)
							Antares::Dispatcher::GUI::Post(callback, 50);
						// avoid matrix being deleted
						pMatrix = nullptr;
						return;
					}

					// The loading of the matrix had failed.
					// The GUI will be updated with NULL
					callback.bind(&pPanel, &Panel::loadDataFromMatrix, (MatrixType*) nullptr);
					if (not shouldAbort)
						Antares::Dispatcher::GUI::Post(callback, 50);
				}

				// release
				delete pMatrix;
				pMatrix = nullptr;
				// early destruction, to reduce wait contention
				IEventObserver<JobMatrix>::destroyBoundEvents();
			}

		private:
			//! Reference to the real GUI component
			Panel& pPanel;
			GUILocker pGUILocker;
			WIP::Locker pWIPLocker;
			MatrixType* pMatrix;

		}; // class JobMatrix





		class JobAggregator : public Yuni::Job::IJob, public Yuni::IEventObserver<JobAggregator>
		{
		public:
			//! Smart pointer
			typedef SmartPtr<JobAggregator>  Ptr;
			//! Matrix type
			typedef Panel::MatrixType  MatrixType;

		public:
			JobAggregator(Panel& panel) :
				Yuni::Job::IJob(),
				shouldAbort(0),
				pPanel(panel),
				result(nullptr)
			{
				// Getting the layer type
				::Antares::Window::OutputViewer::Layer* layer = pPanel.layer();
				pLayerType = (layer) ? layer->type : Antares::Window::OutputViewer::ltOutput;
			}

			virtual ~JobAggregator()
			{
				IEventObserver<JobAggregator>::destroyBoundEvents();

				// Destroying the matrix result, if not already done
				delete result;
			}

			void stopLoading()
			{
				shouldAbort = 1;
			}

		public:
			//! Filename to load
			String::Vector filenames;
			//!
			Atomic::Int<> shouldAbort;

		protected:
			enum
			{
				maxHeaderWidth = 10,
				maxHeaderHeight = 7,
			};
			enum
			{
				options = Matrix<>::optImmediate | Matrix<>::optNoWarnIfEmpty
					| Matrix<>::optQuiet | Matrix<>::optNeverFails
			};

		protected:
			virtual void onExecute()
			{
				if (not shouldAbort)
				{
					if (pLayerType != Antares::Window::OutputViewer::ltOutput)
					{
						// The swap memory MUST not be flushed. This can happen since we are not
						// in the main thread
						MemoryFlushLocker memoryLocker;
						// Go ahead !
						performAggregation();
					}
				}
				// early destruction, to reduce wait contention
				IEventObserver<JobAggregator>::destroyBoundEvents();

				// Destroying the matrix result here (in a thread pool)
				// instead of the destructor (we have no idea in which thread
				// this costly operation will be performed)
				MatrixType* toDelete = result;
				if (toDelete)
				{
					// dereferencing the pointer (in case the thread is killed)
					result = nullptr;
					delete toDelete;
				}
				ops.clear();
				cellStatus.clear();
				headerColumns.clear();
				headerRows.clear();
				precision.clear();
			}


			void performAggregation()
			{
				using namespace Antares::Window::OutputViewer;
				
				uint count = 0;
				for (uint i = 0; i != (uint) filenames.size(); ++i)
				{
					if (shouldAbort || IsGUIAboutToQuit())
						return;
					auto& filename = filenames[i];
					logs.info() << "[output-viewer][virtual] loading " << filename;

					// Allocating a new matrix, that will be used to view
					// the file as raw data
					MatrixType  rawdata;

					bool success = false;
					{
						// file lock
						auto mutex = ProvideLockingForFileLocking(filename);
						Yuni::MutexLocker locker(*mutex);
						if (shouldAbort)
							return;
						success = rawdata.loadFromCSVFile(filename, 1, 0, options);
					}

					if (not success || rawdata.empty())
						continue;
					if (shouldAbort || IsGUIAboutToQuit())
						return;

					++count;

					// Readjust the size of all matrices / vectors
					extendMatrices(rawdata);

					uint mW = rawdata.width;
					uint mH = rawdata.height;

					// calculate the caption info with the first matrix
					if (!i)
						retrieveColRowCaption(rawdata);

					double v;
					for (uint x = 0; x != mW; ++x)
					{
						auto& rawcol = rawdata[x];
						auto&  opsColumn = ops[x];

						for (uint y = 0; y < mH; ++y)
						{
							// If the cell is already marked as "string", there is nothing
							// at this point
							if (cellStatus[x][y] == stString)
								continue;
							// If the cell is marked as "indeterminate" and the cell
							// is empty, it will remain "indeterminate"
							if (cellStatus[x][y] == stIndeterminate && rawcol[y].empty())
								continue;
														
							if (rawcol[y].empty() || y < maxHeaderHeight || not rawcol[y].to(v))
							{
								if (y < maxHeaderHeight)
								{
									if (!i) // first matrix
										headerColumns[x][y] = rawcol[y];
									else
									{
										if (not rawcol[y].empty())
										{
											if (not rawcol[y].equalsInsensitive(headerColumns[x][y]))
												headerColumns[x][y] = "??";
										}
									}
								}
								else if (x < maxHeaderWidth)
								{
									if (!i)
										headerRows[x][y] = rawcol[y];
									else
									{
										if (not rawcol[y].empty())
										{
											if (!rawcol[y].equalsInsensitive(headerRows[x][y]))
												headerRows[x][y] = "??";
										}
									}
								}
								// The conversion into a double has failed.
								cellStatus[x][y] = stString;
							}
							else
							{
								cellStatus[x][y] = stDouble;
								//if it is the first matrix and the element is a caption
								if (!i || colRowCaption[x])
									opsColumn[y] = v;
								else
								{
									switch (pLayerType)
									{
										case ltDiff:
											opsColumn[y] -= v;
											break;
										case ltAverage:
										case ltSum:
											opsColumn[y] += v;
											break;
										case ltMin:
											if (v < opsColumn[y])
												opsColumn[y] = v;
											break;
										case ltMax:
											if (v > opsColumn[y])
												opsColumn[y] = v;
											break;
										case ltOutput:
											assert(false);
											break; // should never happen
									}
								}

								// Detecting the precision
								String::Size offset = rawcol[y].rfind('.');
								if (offset < rawcol[y].size())
								{
									uint p = rawcol[y].size() - offset - 1;
									if (p > precision[x])
										precision[x] = (p > 16) ? 16 : p;
								}
							}
						}

						# ifdef ANTARES_SWAP_SUPPORT
						rawdata.flush();
						ops.flush();
						# endif
					}
				} // all filenames


				if (shouldAbort || IsGUIAboutToQuit())
					return;

				if (count && NULL != (result = new MatrixType()))
				{
					char conversionBuffer[128];
					result->reset(ops.width, ops.height);
					for (uint x = 0; x != ops.width; ++x)
					{
						if (!(x % 2) && (shouldAbort || IsGUIAboutToQuit()))
							return;
						assert(x < precision.size());

						auto& opscol = ops[x];
						auto& invcol = cellStatus[x];
						auto& rescol = result->entry[x];
						const char* format = sfmt[precision[x]];

						for (uint y = 0; y != ops.height; ++y)
						{			
							if (y < maxHeaderHeight)
								rescol[y] = headerColumns[x][y];
							else
							{
								if (invcol[y] != stDouble)
								{
									if (x < maxHeaderWidth)
										rescol[y] = headerRows[x][y];
									else
										continue;
								}
								else
								{
									// Post-processing
									if (pLayerType == ltAverage)
										opscol[y] /= (double) count;

									const int sizePrintf = ANTARES_MATRIX_SNPRINTF(conversionBuffer, sizeof(conversionBuffer), format, opscol[y]);
									if (sizePrintf >= 0 && sizePrintf < (int)(sizeof(conversionBuffer)))
										rescol[y].write((const char*) conversionBuffer, sizePrintf);
									else
										rescol[y] = "ERR";
								}
							}
						}
						# ifdef ANTARES_SWAP_SUPPORT
						opscol.flush();
						invcol.flush();
						rescol.flush();
						# endif
					}
				}

				// Early cleanup
				ops.clear();
				cellStatus.clear();

				if (shouldAbort || IsGUIAboutToQuit())
					return;

				// Early cleanup
				ops.clear();
				cellStatus.clear();

				// Callback for updating the GUI
				Bind<void ()> callback;

				if (count && result)
				{
					callback.bind(&pPanel, &Panel::loadDataFromMatrix, result);
					if (shouldAbort || IsGUIAboutToQuit())
						return;
					result = nullptr;
					Antares::Dispatcher::GUI::Post(callback, 50);
					return;
				}

				// The loading of the matrix had failed.
				// The GUI will be updated with NULL
				callback.bind(&pPanel, &Panel::loadDataFromMatrix, (MatrixType*) nullptr);
				if (shouldAbort || IsGUIAboutToQuit()) // double-check
					return;
				Antares::Dispatcher::GUI::Post(callback, 50);
			}


			void extendMatrices(const MatrixType& rawdata)
			{
				uint mW = rawdata.width;
				uint mH = rawdata.height;

				if (rawdata.width > ops.width || rawdata.height > ops.height)
				{
					if (!ops.width || !ops.height)
					{
						ops.reset(mW, mH);
						cellStatus.reset(mW, mH);
						headerColumns.reset(mW, maxHeaderHeight);
						headerRows.reset(maxHeaderWidth, mH);
						precision.resize(mW);
					}
					else
					{
						if (mW > ops.width && mH > ops.height)
							ops.resizeWithoutDataLost(mW, mH, 0.);
						else
						{
							if (rawdata.width > ops.width)
								ops.resizeWithoutDataLost(mW, ops.height, 0.);
							else
								ops.resizeWithoutDataLost(ops.width, mH, 0.);
						}
						cellStatus.resizeWithoutDataLost(ops.width, ops.height, stIndeterminate);
						headerColumns.resizeWithoutDataLost(ops.width, maxHeaderHeight);
						headerRows.resizeWithoutDataLost(maxHeaderWidth, ops.height);
						precision.resize(ops.width);
					}
				}
			}


			void retrieveColRowCaption(const MatrixType& matrix)
			{
				uint mW = matrix.width;
				uint mH = matrix.height;

				colRowCaption.clear();
				colRowCaption.resize(mW, false);

				//! The header for houly from the fourth row likes below:
				// AreaName  hourly  null  null   null  OV.COST
				// null      null    null  null   null  Euro
				// null      index   day   month  hour  EXP

				// for 'index' header
				colRowCaption[1] = true;

				if (mW > maxHeaderWidth && mH > maxHeaderHeight)
				{
					//from the thrid element
					for (uint x = 2; x != maxHeaderWidth; ++x)
					{
						uint countNonEmpty = 0;
						// 1srt line
						if (not matrix[x][4].empty())
							++countNonEmpty;
						// 2nd line
						if (not matrix[x][5].empty())
							++countNonEmpty;
						// 3nd line
						if (not matrix[x][6].empty())
							++countNonEmpty;

						// The check on "year" is required for the view variable-per-variable
						colRowCaption[x] = (countNonEmpty <= 1);
					}
				}
			}


		private:
			enum State
			{
				stIndeterminate = 0,
				stDouble,
				stString,
			};

		private:
			Antares::Window::OutputViewer::LayerType pLayerType;
			//! Reference to the real GUI component
			Panel& pPanel;
			GUILocker pGUILocker;
			WIP::Locker pWIPLocker;
			MatrixType* result;
			std::vector<bool> colRowCaption;
			// The result matrix
			Matrix<double> ops;
			Matrix<uint8> cellStatus;
			MatrixType headerColumns;
			MatrixType headerRows;
			std::vector<uint8> precision;
		}; // class JobAggregator




		class DataAreaOrLink : public Panel::IData
		{
		public:
			DataAreaOrLink()
			{
			}

			virtual ~DataAreaOrLink()
			{
				assert(wxIsMainThread() == true && "Must be ran from the main thread");
				stopLoading();
			}

		public:
			Event<void ()> stopLoading;
		};



	} // anonymous namespace


	void Panel::loadVirtualLayer()
	{
		assert(this && "Invalid this");

		if (!pComponent || IsGUIAboutToQuit())
			return;

		// waiting for all other standard panels
		if (pPanelsInCallingLoadDataFromFile)
		{
			logs.debug() << "[output-viewer] waiting for other panels";
			Antares::Dispatcher::GUI::Post(this, &Panel::loadVirtualLayer);
			return;
		}

		GUILocker locker;

		delete pData;
		JobAggregator* job = new JobAggregator(*this);
		DataAreaOrLink* data = new DataAreaOrLink();
		pData = data;

		data->stopLoading.connect(job, &JobAggregator::stopLoading);

		uint count = (uint) pComponent->pSelections.size();
		for (uint i = 0; i != count; ++i)
		{
			assert(i < (uint) pComponent->pPanelAllOutputs.size());
			auto* panel = pComponent->pPanelAllOutputs[i];
			if (panel && not panel->filename().empty())
				job->filenames.push_back(panel->filename());
		}
		Antares::Dispatcher::Post((const Yuni::Job::IJob::Ptr&) job);
	}


	void Panel::loadDataFromFile()
	{
		using namespace Antares::Window::OutputViewer;
		// asserts
		assert(this && "Invalid this");
		assert(pLayer && "invalid layer");

		if (!pComponent)
		{
			--pPanelsInCallingLoadDataFromFile;
			return;
		}
		if (IsGUIAboutToQuit())
			return;

		delete pData;
		pData = nullptr;

		GUILocker locker;

		// The current output
		auto& output = *(pLayer->selection);

		// Getting the current selection type
		const SelectionType selectionType = ((pLayer->detached)
			? pLayer->customSelectionType : pComponent->pCurrentSelectionType);
		// The area / link name
		const String& areaOrLinkName = ((pLayer->detached)
			? pLayer->customAreaOrLink : pComponent->pCurrentAreaOrLink);


		auto* data = new DataAreaOrLink();
		auto* job  = new JobMatrix(*this);

		data->stopLoading.connect(job, &JobMatrix::stopLoading);
		pData = data;

		String& filename = job->filename;
		filename.reserve(output.path.size() + 40 + areaOrLinkName.size());

		// Economy
		switch (output.mode)
		{
			case Data::stdmEconomy:
				filename << output.path << SEP << "economy" << SEP;
				break;
			case Data::stdmAdequacy:
				filename << output.path << SEP << "adequacy" << SEP;
				break;
			case Data::stdmAdequacyDraft:
			case Data::stdmUnknown:
			case Data::stdmMax:
				filename << output.path << SEP << "unknown" << SEP;
				break;
		}

		// LOD
		switch (pComponent->pCurrentLOD)
		{
			case lodAllMCYears:
				filename << "mc-all";
				break;
			case lodDetailledResults:
				{
					if (output.version <= (uint)Data::version360)
						filename << "mc-i";
					else
						filename << "mc-ind" << SEP;

					CString<8,false> tmp;
					tmp.resize(5, "0");
					CString<12,false>  stryear(pComponent->pCurrentYear);
					tmp.overwriteRight(stryear);
					filename << tmp;
					break;
				}
			case lodDetailedResultsWithConcatenation:
				filename << "mc-var";
				break;
		}
		filename << SEP;


		if (selectionType == stArea)
			filename << "areas";
		else
			filename << "links";

		filename << SEP << areaOrLinkName << SEP;

		if (pComponent->pBtnValues->pushed())
			filename << "values";
		else if (pComponent->pBtnDetails->pushed())
			filename << "details";
		else if (pComponent->pBtnID->pushed())
			filename << "id";

		filename << '-';

		if (pComponent->pBtnHourly->pushed())
			filename << "hourly";
		else if (pComponent->pBtnDaily->pushed())
			filename << "daily";
		else if (pComponent->pBtnWeekly->pushed())
			filename << "weekly";
		else if (pComponent->pBtnMonthly->pushed())
			filename << "monthly";
		else if (pComponent->pBtnAnnual->pushed())
			filename << "annual";

		if (pComponent->pCurrentLOD == lodDetailedResultsWithConcatenation)
			filename << '-' << pComponent->pCurrentVariableID;

		filename << ".txt";

		if (!IO::File::Exists(filename))
		{
			// Before v3.8, the output folder was named `Economy` instead of `economy`
			// It is obviously an issue on linux
			// (adequacy was not available for the output viewer before 3.8)
			String newfilename = filename;
			String torepl;
			torepl << SEP << "economy" << SEP;
			String replacement;
			replacement << SEP << "Economy" << SEP;
			newfilename.replace(torepl, replacement);

			if (IO::File::Exists(newfilename))
				filename = newfilename;
		}

		// The filename to open
		pFilename = filename;

		job->options = Matrix<>::optImmediate | Matrix<>::optNoWarnIfEmpty
			| Matrix<>::optQuiet | Matrix<>::optNeverFails;


		// Checking if actually there is no data
		{
			String checkFilename;
			checkFilename = filename;
			checkFilename << ".nodata";

			if (IO::File::Exists(checkFilename))
			{
				delete job;
				noData();
				--pPanelsInCallingLoadDataFromFile;
				return;
			}
		}
		--pPanelsInCallingLoadDataFromFile;
		Antares::Dispatcher::Post((const Yuni::Job::IJob::Ptr&) job);
	}


	void Panel::loadDataFromMatrix(MatrixType* matrix)
	{
		assert(this && "Invalid this");
		assert(pLayer && "invalid layer");

		if (!pComponent)
			return;

		GUILocker locker;
		if (not matrix)
		{
			if (pComponent->pCurrentLOD == Antares::Window::OutputViewer::lodDetailedResultsWithConcatenation
				&& pComponent->pHasYearByYear)
			{
				// Checking if actually there is no data
				String filename = pFilename;
				filename << ".nodata";
				if (IO::File::Exists(filename))
					noData();
				else
					messageMergeYbY();
			}
			else
				noData();
		}
		else
		{
			typedef Antares::Component::Datagrid::Component  DatagridType;
			clearAllComponents();

			auto* renderer = new AreaLinkRenderer(pComponent, matrix);
			DatagridType* grid = new DatagridType(this, renderer,
				wxEmptyString, false, true, true, true);

			renderer->grid(grid);
			pSizer->Add(grid, 1, wxALL|wxEXPAND);
			grid->forceRefresh();

			// Scroll event
			pComponent->pOnScrollAllPanels.connect(grid, &DatagridType::scroll);
			pComponent->pOnSelectCellAllPanels.connect(renderer, &AreaLinkRenderer::onExternalSelectCell);
		}

		auto* sizer = GetSizer();
		if (sizer)
			sizer->Layout();
	}


	void Panel::noData()
	{
		message(wxT("no data available"), "images/64x64/file-not-found.png");
		auto* sizer = GetSizer();
		if (sizer)
			sizer->Layout();
	}





} // namespace OutputViewerData
} // namespace Private
} // namespace Antares

