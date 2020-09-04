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
#ifndef __ANTARES_TOOLBOX_COMPONENT_DATAGRID_RENDERER_H__
# define __ANTARES_TOOLBOX_COMPONENT_DATAGRID_RENDERER_H__

# include <antares/wx-wrapper.h>
# include <yuni/core/event.h>
# include <antares/date.h>
# include <wx/colour.h>
# include "default.h"
# include <antares/study/study.h>

# ifdef YUNI_OS_MSVC
// Disable warning C4250, inherits via dominance
#	pragma warning(disable : 4250)
# endif


namespace Antares
{
namespace Component
{
namespace Datagrid
{
namespace Renderer
{


	/*!
	** \brief Data provider for Datagrids
	**
	** A renderer is merely an interface between the datagrid component and the real data.
	** Because the displayed data are not necessarily the same than in memory, some
	** conversion might be required.
	** This class provided all informations needed by a Datagrid (and more actually).
	*/
	class IRenderer : public Yuni::IEventObserver<IRenderer>
	{
	public:
		enum CellStyle
		{
			cellStyleDefault = 0,
			cellStyleCustom,
			cellStyleDefaultAlternate,
			cellStyleDefaultDisabled,
			cellStyleDefaultAlternateDisabled,
			cellStyleDefaultCenter,
			cellStyleDefaultCenterAlternate,
			cellStyleDefaultCenterDisabled,
			cellStyleDefaultCenterAlternateDisabled,
			cellStyleWarning,
			cellStyleError,
			cellStyleDisabled,
			cellStyleAverage,
			cellStyleSum,
			cellStyleMinMax,
			cellStyleConstraintEnabled,
			cellStyleConstraintDisabled,
			cellStyleConstraintNoWeight,
			cellStyleConstraintWeight,
			cellStyleConstraintWeightCount,
			cellStyleConstraintOperator,
			cellStyleConstraintType,
			cellStyleLayerDisabled,
			cellStyleFilterYearByYearOn,
			cellStyleFilterYearByYearOff,
			cellStyleFilterSynthesisOn,
			cellStyleFilterSynthesisOff,
			cellStyleFilterUndefined,

			cellStyleMax /* Max items in the list */
		};

		static const wxChar* CellStyleToCSSClass(CellStyle s);
		static void CellStyleToCSSClass(CellStyle s, Yuni::String& str);

	public:
		//! \name Constructor & Destructor
		//@{
		/*!
		** \brief Default constructor
		*/
		IRenderer();
		//! Destructor
		virtual ~IRenderer();
		//@}

		//! \name Valid
		//@{
		virtual bool valid() const = 0;
		//@}


		//! \name Sizes
		//@{
		/*!
		** \brief The effective width of the grid
		*/
		virtual int width() const = 0;

		/*!
		** \brief The effective height of the grid
		*/
		virtual int height() const = 0;

		/*!
		** \brief The real width of the matrix in memory
		**
		** This method is used for example to know how the matrix could be resized
		*/
		virtual int internalWidth() const;
		/*!
		** \brief The real height of the matrix in memory
		**
		** This method is used for example to know how the matrix could be resized
		*/
		virtual int internalHeight() const;
		//@}

		//! \name Captions
		//@{
		/*!
		** \brief Get the caption of a column
		*/
		virtual wxString columnCaption(int colIndx) const = 0;

		/*!
		** \brief Get the caption of a row
		*/
		virtual wxString rowCaption(int rowIndx) const = 0;
		//@}

		//! \name Values
		//@{
		/*!
		** \brief Read a cell as a string
		*/
		virtual wxString asString(int x, int y) const;
		/*!
		** \brief Read a cell as a double
		*/
		virtual double asDouble(int x, int y) const;

		/*!
		** \brief Get the string representation of a Cell
		*/
		virtual wxString cellValue(int x, int y) const = 0;

		void appendCellValue(int x, int y, Yuni::String& s) const;

		/*!
		** \brief Get the floating value of a Cell
		*/
		virtual double cellNumericValue(int x, int y) const = 0;

		/*!
		** \brief Try to modify a cell value
		** \return True if the operation succeeded, False otherwise
		*/
		virtual bool cellValue(int x, int y, const Yuni::String& value) = 0;
		//@}

		//! \name Display
		//@{
		virtual void resetColors(int x, int y, wxColour& background, wxColour& textForeground) const = 0;

		//! \name Apparence
		//@{
		virtual IRenderer::CellStyle cellStyle(int, int y) const;
		IRenderer::CellStyle cellStyleWithNumericCheck(int x, int y) const;

		virtual wxColour cellBackgroundColor(int, int) const;
		virtual wxColour cellTextColor(int, int) const;

		//! Get the color of the vertical right border
		virtual wxColour verticalBorderColor(int x, int y) const;

		//! Get the color of the horizontal bottom border
		virtual wxColour horizontalBorderColor(int x, int y) const;

		//! Get the alignment of a cell (-1: left, 0: center, +1: right)
		virtual int cellAlignment(int x, int y) const;

		//! Get how the a column must resize (0: auto, >0: specific width, <0: use custom string)
		virtual int columnWidthCustom(int x) const;
		//! String to use
		virtual void hintForColumnWidth(int x, wxString& out) const;
		//@}

		/*!
		** \brief Time Precision
		*/
		virtual Date::Precision  precision();

		/*!
		** \brief Make sure all data are effectively loaded
		*/
		virtual bool ensureDataAreLoaded();
		//@}

		//! \name Resizes
		//@{
		/*!
		** \brief The most suitable column count
		** \return The most suitable column count, 0 if can not be resized
		*/
		virtual uint maxWidthResize() const;
		/*!
		** \brief The most suitable column count
		** \return The most suitable column count, 0 if can not be resized
		*/
		virtual uint maxHeightResize() const;

		/*!
		** \brief Resize the internal matrix with a given col count
		*/
		virtual void resizeMatrixToXColumns(uint /*m*/) {}

		/*!
		** \brief Event trigerred before resizing a matrix
		**
		** \param oldX The previous width
		** \param oldY The previous height
		** \param newX THe new width of the matrix
		** \param newY THe new height of the matrix
		** \return True to perform the resize
		*/
		virtual bool onMatrixResize(uint oldX, uint oldY, uint& newX, uint& newY);

		/*!
		** \brief Event triggered right after a matrix was loaded by the renderer
		*/
		virtual void onMatrixLoad() {}

		/*!
		** \brief Rotate all rows until a given date
		**
		** \param month A month
		** \param daymonth Day in this month (zero-based)
		*/
		virtual bool circularShiftRowsUntilDate(MonthName month, uint daymonth);
		//@}


		//! \name Events
		//@{
		//! A cell has been selected
		virtual void onSelectCell(uint, uint) {}
		//! A scroll event has been triggered
		virtual void onScroll() {}
		//@}

		virtual void applyLayerFiltering(size_t layerID, VGridHelper* gridHelper);


	public:
		//! Event on refresh
		Yuni::Event<void()> onRefresh;
		//! The real precision asked by the datagrid
		// This property is set by VGridHelper
		Date::Precision dataGridPrecision;
		//! Attached study (if any)
		Data::Study::Ptr study;

		//! True to invalidate the current data and force a refresh for the next call to DBGrid:onDraw
		bool invalidate;

	protected:
		//! Event: the study has been closed
		virtual void onStudyClosed();
		//! Event: the study has been loaded
		virtual void onStudyLoaded();

	}; // class IRenderer





} // namespace Renderer
} // namespace Datagrid
} // namespace Component
} // namespace Antares

# include "renderer.hxx"

#endif // __ANTARES_TOOLBOX_COMPONENT_DATAGRID_RENDERER_H__
