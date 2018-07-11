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
#ifndef __ANTARES_TOOLBOX_COMPONENT_DATAGRID_RENDERER_MATRIX_HXX__
# define __ANTARES_TOOLBOX_COMPONENT_DATAGRID_RENDERER_MATRIX_HXX__

# include <yuni/core/math.h>
# include "../../../../application/main.h"
# include "../../../../application/wait.h"
# include <wx/statusbr.h>
# include <yuni/core/static/types.h>
# include <yuni/io/file.h>
# include <ui/common/lock.h>


namespace Antares
{
namespace Component
{
namespace Datagrid
{
namespace Renderer
{


	template<class T, class ReadWriteT, uint PrecisionT>
	inline Antares::Matrix<T,ReadWriteT>*
	Matrix<T,ReadWriteT,PrecisionT>::matrix() const
	{
		return pMatrix;
	}


	template<class T, class ReadWriteT, uint PrecisionT>
	inline wxString Matrix<T,ReadWriteT,PrecisionT>::rowCaption(int row) const
	{
		if (row < Date::Calendar::maxHoursInYear && !(!study))
		{
			return wxStringFromUTF8(study->calendar.text.hours[row]);
		}
		return wxString();
	}


	template<class T, class ReadWriteT, uint PrecisionT>
	inline wxString Matrix<T,ReadWriteT,PrecisionT>::columnCaption(int colIndx) const
	{
		return wxString() << wxT("     TS-") << (1 + colIndx) << wxT("     ");
	}


	template<class T, class ReadWriteT, uint PrecisionT>
	inline int Matrix<T,ReadWriteT,PrecisionT>::width() const
	{
		return (!pMatrix) ? 0 : (int) pMatrix->width;
	}


	template<class T, class ReadWriteT, uint PrecisionT>
	inline int Matrix<T,ReadWriteT,PrecisionT>::height() const
	{
		return !pMatrix ? 0 : (int)pMatrix->height;
	}



	template<class T, class ReadWriteT, uint PrecisionT>
	inline Matrix<T,ReadWriteT,PrecisionT>::Matrix(wxWindow* parent, Antares::Matrix<T,ReadWriteT>* matrix) :
		pControl(parent),
		pMatrix(matrix)
	{}


	template<class T, class ReadWriteT, uint PrecisionT>
	inline Matrix<T,ReadWriteT,PrecisionT>::~Matrix()
	{
		onRefresh.clear();
	}


	template<class T, class ReadWriteT, uint PrecisionT>
	inline IRenderer::CellStyle Matrix<T,ReadWriteT,PrecisionT>::cellStyle(int x, int y) const
	{
		return (Yuni::Math::Zero(cellNumericValue(x, y)))
			? ((y % 2) ? cellStyleDefaultAlternateDisabled : cellStyleDefaultDisabled)
			: ((y % 2) ? cellStyleDefaultAlternate : cellStyleDefault);
	}


	template<class T, class ReadWriteT, uint PrecisionT>
	inline wxString Matrix<T,ReadWriteT,PrecisionT>::cellValue(int x, int y) const
	{
		return (pMatrix && (uint) x < pMatrix->width && (uint) y < pMatrix->height)
			? DoubleToWxString(pMatrix->entry[x][y])
			: wxString();
	}


	template<class T, class ReadWriteT, uint PrecisionT>
	inline double Matrix<T,ReadWriteT,PrecisionT>::cellNumericValue(int x, int y) const
	{
		return (pMatrix && (uint) x < pMatrix->width && (uint) y < pMatrix->height)
			? pMatrix->entry[x][y]
			: 0.;
	}


	template<class T, class ReadWriteT, uint PrecisionT>
	bool Matrix<T,ReadWriteT,PrecisionT>::cellValue(int x, int y, const Yuni::String& value)
	{
		if (pMatrix && (uint) x < pMatrix->width && (uint) y < pMatrix->height)
		{
			double v;
			if (value.to(v))
			{
				// The value must be rounded if not a decimal value
				if (Yuni::Static::Type::IsDecimal<ReadWriteT>::No)
					v = Yuni::Math::Round(v);
				else
					v = Yuni::Math::Round(v, matrixPrecision);

				if (!Yuni::Math::Equals((double)pMatrix->entry[x][y], v))
				{
					pMatrix->entry[x][y] = static_cast<T>(static_cast<ReadWriteT>(v));
					pMatrix->markAsModified();
				}
				return true;
			}
		}
		return false;
	}


	template<class T, class ReadWriteT, uint PrecisionT>
	void Matrix<T,ReadWriteT,PrecisionT>::matrix(Antares::Matrix<T,ReadWriteT>* m)
	{
		pMatrix = !study ? nullptr : m;
		onRefresh();
	}


	template<class T, class ReadWriteT, uint PrecisionT>
	bool Matrix<T,ReadWriteT,PrecisionT>::ensureDataAreLoaded()
	{
		using namespace Yuni;

		GUIBeginUpdate();

		if (!(!study) && pMatrix && pMatrix->jit)
		{
			auto& jit = *pMatrix->jit;
			// Timestamp for last modification
			auto timestamp = IO::File::LastModificationTime(jit.sourceFilename);
			if (jit.lastModification != timestamp)
			{
				jit.lastModification = timestamp;
				if (!jit.modified)
					jit.alreadyLoaded = false;
			}
		}

		if (!(!study) && pMatrix && (!JIT::IsReady(pMatrix->jit) || invalidate))
		{
			if (!pMatrix->jit)
			{
				GUIEndUpdate();
				return true;
			}

			auto& jit = *pMatrix->jit;
			::wxBeginBusyCursor();

			auto& mainFrm = *Forms::ApplWnd::Instance();

			// Working...
			WIP::Locker wip;
			GUILocker locker;

			// The following instructions seem to seriously slow down
			// the execution of the program :
			// # wxBusyInfo info(wxT("Loading data..."), Forms::ApplWnd::Instance());
			// # wxTheApp->Yield();

			if (not JIT::IsReady(pMatrix->jit))
			{
				// We can call SetStatusText from here, it may produce some strange segv
				// on Linux. Nothing indicates that it has the same issue on Windows.
				//mainFrm.SetStatusText(wxString(wxT("  Loading "))
				//	<< wxStringFromUTF8(jit.sourceFilename) << wxT("..."));
				logs.info() << "loading '" << jit.sourceFilename << "'";
			}

			if (not pMatrix->invalidate(true))
			{
				if (pMatrix->jit)
					logs.error() << "I/O Error: Impossible to load '" << jit.sourceFilename << "'";
				else
					logs.error() << "I/O Error: Impossible to load the data";
			}
			// Notify
			onMatrixLoad();

			mainFrm.resetDefaultStatusBarText();

			// restore cursor
			::wxEndBusyCursor();
			GUIEndUpdate();
			return true;
		}

		GUIEndUpdate();
		return false;
	}



	template<class T, class ReadWriteT, uint PrecisionT>
	void Matrix<T,ReadWriteT,PrecisionT>::resizeMatrixToXColumns(uint m)
	{
		// TODO This method remains for compatibility only and should be removed
		if (!(!study) && pMatrix)
		{
			if (onMatrixResize(pMatrix->width, pMatrix->height, m, pMatrix->height))
			{
				GUIBeginUpdate();
				pMatrix->resizeWithoutDataLost(m, pMatrix->height);
				MarkTheStudyAsModified(study);
				GUIEndUpdate();
			}
		}
	}


	template<class T, class ReadWriteT, uint PrecisionT>
	inline bool Matrix<T,ReadWriteT,PrecisionT>::valid() const
	{
		return (!IsGUIAboutToQuit() && !(!study) && pMatrix != NULL);
	}


	template<class T, class ReadWriteT, uint PrecisionT>
	void Matrix<T,ReadWriteT,PrecisionT>::onStudyClosed()
	{
		IRenderer::onStudyClosed();
	}


	template<class T, class ReadWriteT, uint PrecisionT>
	void Matrix<T,ReadWriteT,PrecisionT>::onStudyLoaded()
	{
		IRenderer::onStudyLoaded();
	}


	template<class T, class ReadWriteT, uint PrecisionT>
	bool Matrix<T,ReadWriteT,PrecisionT>::circularShiftRowsUntilDate(MonthName month, uint daymonth)
	{
		if (!study || !pMatrix)
			return false;
		if (daymonth > 30)
		{
			# ifndef NDEBUG
			logs.warning() << "invalid internal parameter: daymonth is bteween 0..30";
			# endif
			return false;
		}

		auto& calendar = study->calendar;
		uint relativeMonth = calendar.mapping.months[(uint)month];
		uint daysForMonth  = calendar.months[relativeMonth].days;
		if (daymonth >= daysForMonth)
			return false;

		uint dayYear   = calendar.months[relativeMonth].daysYear.first + daymonth;
		if (dayYear >= calendar.maxDaysInYear)
			return false;
		uint hourindex = calendar.days[dayYear].hours.first;

		switch (this->precision())
		{
			case Date::hourly:
				{
					GUIBeginUpdate();
					pMatrix->circularShiftRows(hourindex);
					MarkTheStudyAsModified(study);
					GUIEndUpdate();
					return true;
				}
			case Date::daily:
				{
					GUIBeginUpdate();
					pMatrix->circularShiftRows(dayYear);
					MarkTheStudyAsModified(study);
					GUIEndUpdate();
					return true;
				}
			case Date::monthly:
				{
					GUIBeginUpdate();
					pMatrix->circularShiftRows(month);
					MarkTheStudyAsModified(study);
					GUIEndUpdate();
					return true;
				}
			//case Date::stepSecond:
			//case Date::stepMinute:
			case Date::weekly:
			case Date::annual:
			case Date::stepNone:
			case Date::stepAny:
				logs.error() << "row shifting not allowed here";
				break;
		}

		return false;
	}





} // namespace Renderer
} // namespace Datagrid
} // namespace Component
} // namespace Antares

#endif // __ANTARES_TOOLBOX_COMPONENT_DATAGRID_RENDERER_MATRIX_HXX__
