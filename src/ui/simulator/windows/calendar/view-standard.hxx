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

#include <ui/common/component/panel.h>
#include <wx/dcbuffer.h>
#include <yuni/core/math/math.h>
#include <yuni/core/bind.h>



namespace Antares
{
namespace Window
{
	class CalendarSelect;


	class CalendarViewStandard : public Component::Panel
	{
	public:
		enum
		{
			nbMonthPerRow = 4,
			spaceBetweenMonth = 10,
			dayWidth = 18,
			decalX = 10,
			decalY = 10,
			weekNumbersWidth = dayWidth * 2,
			recommendedWindowWidth = nbMonthPerRow * (dayWidth * 7 + spaceBetweenMonth) + decalX * 2
				+ weekNumbersWidth * nbMonthPerRow /* week number*/,
			recommendedWindowHeight = (12 / nbMonthPerRow) * (dayWidth * 8 + spaceBetweenMonth) + decalY * 2 + 4 + dayWidth * 2,
			borderWidth = 4
		};

	public:
		CalendarViewStandard(wxWindow* parent, CalendarSelect& dialog);
		virtual ~CalendarViewStandard() {}

		void selectWholeYear();
		void selectNone();

		//! Event: draw the panel
		void onDraw(wxPaintEvent&);
		//! UI: Erase background, empty to avoid flickering
		void onEraseBackground(wxEraseEvent&) {}
		//! wxEvent : onMouseMove
		virtual void onMouseMoved(int x, int y);

		//! Click down
		virtual void onMouseDown(wxMouseEvent&);

		//! Click up
		virtual void onMouseUp(wxMouseEvent&);


	public:
		//! Current calendar
		Date::Calendar& calendar;
		//! Event : update text
		Yuni::Bind<void (const YString&)> onUpdateSelectionText;


	private:
		inline void updateSelectionDayRange();
		void prepareGrid(wxDC& dc, const Date::Calendar& calendar);
		void updateGridCells(int x, int y);
		void updateSelectionText();
		void updateSelectionText(YString& out, uint from, uint to);

	private:
		//! Mouse position
		wxPoint pMousePosition;

		//! The index of the current day year where the mouse is hover
		uint pCurrentDayYearHover;
		//! The index of the day year where the range selection starts
		uint pDayYearRangeStart;
		//! Current mouse Selection from the user
		uint pCurrentSelectionDayRange[2];

		//! Memory Device Context for pre-drawing the calendar
		wxBitmap pCacheBackgroundImage;
		//! Flag to know if the background cache image is ready
		bool pCacheBkgReady;

		enum DayType
		{
			dtInvalid,
			dtNormal,
			dtHighlight,
			dtHighlightRange,
			dtHighlightRangeSimulationOut,
			dtSelection,
			dtWeek,
			dtWeekNormal,
			dtWeekHighlight,
			dtMax
		};

		struct CellInfo
		{
			//! Day type
			DayType type;
			//! X-Coordinate
			int x;
			//! Y-Coordinate
			int y;
			//! Text X-Coordinate
			int textX;
			//! Text Y-Coordinate
			int textY;
			//! Text
			wxString text;
		};

		CellInfo pCacheDay[Date::Calendar::maxDaysInYear];

		CellInfo pCacheWeek[Date::Calendar::maxWeeksInYear * 2];
		uint pCacheWeekObjectID[Date::Calendar::maxWeeksInYear];

		//! Parent window
		CalendarSelect& pDialog;

		// Event table
		DECLARE_EVENT_TABLE()

	}; // class CalendarViewStandard



	BEGIN_EVENT_TABLE(CalendarViewStandard, Panel)
		EVT_PAINT(CalendarViewStandard::onDraw)
		EVT_ERASE_BACKGROUND(CalendarViewStandard::onEraseBackground)
	END_EVENT_TABLE()


	// The font re-used for each drawing
	enum
	{
		fontSize = 8,

		textDrawOffsetY = Yuni::System::windows ? -1 : 0,
	};
	static const wxFont font(wxFontInfo(fontSize).Family(wxFONTFAMILY_SWISS).FaceName("Tahoma"));
	static const wxFont fontBold(wxFontInfo(fontSize).Family(wxFONTFAMILY_SWISS).Bold().FaceName("Tahoma"));



	CalendarViewStandard::CalendarViewStandard(wxWindow* parent, CalendarSelect& dialog) :
		Panel(parent),
		calendar(Data::Study::Current::Get()->calendar),
		pDialog(dialog)
	{
		SetBackgroundStyle(wxBG_STYLE_CUSTOM); // Required by both GTK and Windows
		SetSize(recommendedWindowWidth, recommendedWindowHeight);
		assert(parent != NULL);

		pCurrentDayYearHover         = (uint) -1;
		pDayYearRangeStart           = (uint) -1;
		pCurrentSelectionDayRange[0] = pDialog.selectionDayRange[0];
		pCurrentSelectionDayRange[1] = pDialog.selectionDayRange[1];
		pCacheBkgReady = false;

		for (uint i = 0; i != calendar.maxDaysInYear; ++i)
		{
			pCacheDay[i].x = 100000; // arbitrary - far far away
			pCacheDay[i].type = dtNormal;
		}
		for (uint i = 0; i != calendar.maxWeeksInYear * 2; ++i)
		{
			pCacheWeek[i].x = 100000; // arbitrary - far far away
			pCacheWeek[i].type = dtInvalid;
		}

		wxMemoryDC dc;
		prepareGrid(dc, calendar);
		updateGridCells(-1, -1);
		Dispatcher::GUI::Post(this, &CalendarViewStandard::updateSelectionText);
		Dispatcher::GUI::Refresh(this);
	}


	void CalendarViewStandard::selectWholeYear()
	{
		pDialog.pHasBeenModified = true;
		pDialog.selectionDayRange[0] = 0;
		pDialog.selectionDayRange[1] = 365;
		pCurrentDayYearHover         = (uint) -1;
		pDayYearRangeStart           = (uint) -1;

		updateGridCells(-1, -1);
		updateSelectionText();
		Refresh();
	}


	void CalendarViewStandard::selectNone()
	{
		pDialog.pHasBeenModified     = true;
		pDialog.selectionDayRange[0] = (uint) -1;
		pDialog.selectionDayRange[1] = (uint) -1;
		pCurrentDayYearHover         = (uint) -1;
		pDayYearRangeStart           = (uint) -1;

		updateGridCells(-1, -1);
		updateSelectionText();
		Refresh();
	}


	void CalendarViewStandard::updateGridCells(int x, int y)
	{
		// note : prepareGrid must be called at least once before this routine
		// looking for the day hover
		for (uint d = 0; d != calendar.maxDaysInYear; ++d)
		{
			auto& day = pCacheDay[d];
			if (x >= day.x && y >= day.y && x < day.x + dayWidth && y < day.y + dayWidth)
			{
				// Do never take into consideration the last day in leap year
				if (d < 365)
					pCurrentDayYearHover = d;
				break;
			}
		}
		updateSelectionDayRange();

		for (uint w = 0; w != calendar.maxWeeksInYear; ++w)
			pCacheWeek[pCacheWeekObjectID[w]].type = dtWeekNormal;

		// updating all cells
		for (uint d = 0; d != calendar.maxDaysInYear; ++d)
		{
			auto& day = pCacheDay[d];

			// hover
			if (d == pCurrentDayYearHover)
			{
				for (uint i = d + 1; i < calendar.maxDaysInYear; ++i)
				{
					pCacheDay[i].type =
						(pDialog.selectionDayRange[1] != (uint) -1 && i < pDialog.selectionDayRange[1]
						&& i >= pDialog.selectionDayRange[0])
							? dtSelection : dtNormal;
				}

				// highlighting the week
				uint week = calendar.days[d].week;
				pCacheWeek[pCacheWeekObjectID[week]].type = dtWeekHighlight;
				auto& range = calendar.weeks[week].daysYear;
				if (d < pDayYearRangeStart)
				{
					for (uint wd = range.first; wd < range.end; ++wd)
						pCacheDay[wd].type = dtWeek;
				}
				else
				{
					for (uint wd = d + 1; wd < range.end; ++wd)
						pCacheDay[wd].type = dtWeek;
				}

				// hightlighting the day where the mouse is hover
				day.type = dtHighlight;
				break;
			}

			day.type = (d >= pDayYearRangeStart)
				? ((d == pDayYearRangeStart) ? dtHighlight : dtHighlightRange)
				: (d >= pDialog.selectionDayRange[0] && d < pDialog.selectionDayRange[1] ? dtSelection : dtNormal);
		}

		if (pDayYearRangeStart == (uint) -1)
		{
			// highlight (error) partial weeks for user selection
			if (pDialog.selectionDayRange[0] != (uint) -1 && pDialog.selectionDayRange[1] != (uint) -1)
			{
				uint count = pDialog.selectionDayRange[1] - pDialog.selectionDayRange[0];
				if (count < 7)
				{
					for (uint i = pDialog.selectionDayRange[0]; i < pDialog.selectionDayRange[1]; ++i)
						pCacheDay[i].type = dtHighlightRangeSimulationOut;
				}
				else
				{
					uint partial = count % 7;
					for (uint i = pDialog.selectionDayRange[1] - partial; i < pDialog.selectionDayRange[1]; ++i)
						pCacheDay[i].type = dtHighlightRangeSimulationOut;
				}
			}
		}
		else
		{
			if (pCurrentDayYearHover != (uint) -1)
			{
				// highlight (error) partial weeks for the current in range selection
				uint count = pCurrentSelectionDayRange[1] - pCurrentSelectionDayRange[0] + 1;
				if (count < 7)
				{
					for (uint i = pCurrentSelectionDayRange[0]; i <= pCurrentSelectionDayRange[1]; ++i)
						pCacheDay[i].type = dtHighlightRangeSimulationOut;
				}
				else
				{
					uint partial = count % 7;
					for (uint i = pCurrentSelectionDayRange[1] - partial + 1; i <= pCurrentSelectionDayRange[1]; ++i)
						pCacheDay[i].type = dtHighlightRangeSimulationOut;
				}
			}
		}

		// highlight the begining of the week for the selection
		if (pDayYearRangeStart < calendar.maxDaysInYear && pDayYearRangeStart > 0)
		{
			uint week = calendar.days[pDayYearRangeStart].week;
			auto& range = calendar.weeks[week].daysYear;
			for (uint wd = range.first; wd < pDayYearRangeStart; ++wd)
				pCacheDay[wd].type = dtWeek;
		}
	}


	void CalendarViewStandard::onMouseMoved(int x, int y)
	{
		// Notify other components as well
		Antares::Component::Panel::OnMouseMoveFromExternalComponent();
		// Update informations about the mouse position
		pMousePosition.x = x;
		pMousePosition.y = y;
		if (pDayYearRangeStart == (uint) -1)
			pCurrentDayYearHover = (uint) -1;

		updateGridCells(x, y);

		if (pDayYearRangeStart != (uint) -1)
			updateSelectionText();
		// refresh the component itself
		Refresh();
	}


	void CalendarViewStandard::onMouseDown(wxMouseEvent& evt)
	{
		if (pDialog.allowRangeSelection)
			pDayYearRangeStart = pCurrentDayYearHover;

		pCurrentDayYearHover = (uint) -1;
		const auto& position = evt.GetPosition();
		updateGridCells(position.x, position.y);

		Dispatcher::GUI::Refresh(this);
	}


	void CalendarViewStandard::onMouseUp(wxMouseEvent&)
	{
		// keeping somewhere the current mouse selection
		if (pDialog.allowRangeSelection)
		{
			pDialog.selectionDayRange[0] = pCurrentSelectionDayRange[0];
			pDialog.selectionDayRange[1] = pCurrentSelectionDayRange[1] + 1;
		}
		else
		{
			pDialog.selectionDayRange[0] = pCurrentDayYearHover;
			pDialog.selectionDayRange[1] = pCurrentDayYearHover + 1;
		}
		pDialog.pHasBeenModified = true;

		// invalidating the current mouse selection
		if (pDialog.allowQuickSelect && pDialog.selectionDayRange[0] != (uint) -1)
		{
			pDialog.pCanceled = false;
			onUpdateSelectionText.unbind();
			Dispatcher::GUI::Close(&pDialog, 100);
		}
		else
		{
			pCurrentDayYearHover = (uint) -1;
			pDayYearRangeStart   = (uint) -1;

			updateSelectionDayRange();
			updateGridCells(10000, 0);
			Dispatcher::GUI::Refresh(this);
			updateSelectionText();
		}
	}


	void CalendarViewStandard::onDraw(wxPaintEvent&)
	{
		// The DC
		wxAutoBufferedPaintDC dc(this);
		// Shifts the device origin so we don't have to worry
		// about the current scroll position ourselves
		PrepareDC(dc);
		// Cute font
		dc.SetFont(font);

		const wxColour bkgColor(255, 255, 255);

		if (not pCacheBkgReady)
		{
			pCacheBackgroundImage.Create(recommendedWindowWidth, recommendedWindowHeight);
			wxMemoryDC memdc;
			memdc.SelectObject(pCacheBackgroundImage);
			prepareGrid(memdc, calendar);
			pCacheBkgReady = true;
		}

		dc.DrawBitmap(pCacheBackgroundImage, 0, 0, false);

		// redrawing all cells
		struct
		{
			wxColour background;
			wxColour text;
		} colorSet[dtMax];

		colorSet[dtNormal].text.Set(60, 60, 60);
		colorSet[dtNormal].background.Set(255, 255, 255);

		colorSet[dtWeekNormal].text.Set(170, 170, 170);
		colorSet[dtWeekNormal].background.Set(255, 255, 255);

		colorSet[dtWeekHighlight].text.Set(0, 0, 0);
		colorSet[dtWeekHighlight].background.Set(235, 235, 235);

		colorSet[dtHighlight].text.Set(250, 250, 250);
		colorSet[dtHighlight].background.Set(10, 10, 10);

		colorSet[dtHighlightRange].text.Set(230, 230, 230);
		colorSet[dtHighlightRange].background.Set(80, 80, 80);

		colorSet[dtHighlightRangeSimulationOut].text.Set(230, 230, 230);
		colorSet[dtHighlightRangeSimulationOut].background.Set(200, 30, 30);

		colorSet[dtWeek].text.Set(50, 40, 40);
		colorSet[dtWeek].background.Set(210, 210, 215);

		colorSet[dtSelection].text.Set(30, 30, 30);
		colorSet[dtSelection].background.Set(157, 205, 255);


		for (uint day = 0; day != calendar.maxDaysInYear; ++day)
		{
			auto& cache = pCacheDay[day];
			auto& colors = colorSet[cache.type];

			dc.SetBrush(wxBrush(colors.background, wxBRUSHSTYLE_SOLID));
			dc.SetPen(wxPen(colors.background, 1, wxPENSTYLE_SOLID));
			dc.SetTextForeground(colors.text);

			dc.DrawRectangle(wxRect(cache.x, cache.y, dayWidth, dayWidth));
			dc.DrawText(cache.text, cache.textX, cache.textY);
		}

		for (uint week = 0; week != calendar.maxWeeksInYear * 2; ++week)
		{
			auto& cache = pCacheWeek[week];
			if (cache.type == dtInvalid)
				break;
			auto& colors = colorSet[cache.type];

			dc.SetBrush(wxBrush(colors.background, wxBRUSHSTYLE_SOLID));
			dc.SetPen(wxPen(colors.background, 1, wxPENSTYLE_SOLID));
			dc.SetTextForeground(colors.text);

			dc.DrawRectangle(wxRect(cache.x, cache.y, weekNumbersWidth, dayWidth));
			dc.DrawText(cache.text, cache.textX, cache.textY);
		}
	}



	inline void CalendarViewStandard::updateSelectionDayRange()
	{
		pCurrentSelectionDayRange[0] = Math::Min(pDayYearRangeStart, pCurrentDayYearHover);
		pCurrentSelectionDayRange[1] = Math::Max(pDayYearRangeStart, pCurrentDayYearHover);
	}


	void CalendarViewStandard::prepareGrid(wxDC& dc, const Date::Calendar& calendar)
	{
		// Cute font
		dc.SetFont(font);

		wxRect rect = GetRect();
		const wxColour bkgColor(255, 255, 255);

		// Redraw the background
		dc.SetPen(wxPen(bkgColor, 1, wxPENSTYLE_SOLID));
		dc.SetBrush(wxBrush(bkgColor, wxBRUSHSTYLE_SOLID));
		dc.DrawRectangle(0, 0, rect.GetWidth(), rect.GetHeight());

		wxRect monthRect;
		monthRect.x = weekNumbersWidth;
		monthRect.y = 0;
		monthRect.width  = dayWidth * 7;
		monthRect.height = dayWidth * 6 + dayWidth/*weekdays*/ + borderWidth * 4;
		uint dayYear = 0;

		wxColour dayTextColor(60, 60, 60);
		wxColour dayTextColorOdd(100, 50, 50);
		wxColour dayBkgColor(255, 255, 255);
		wxColour dayUserSelectTextColor(20, 20, 20);
		wxColour dayUserSelectBkgColor(255, 230, 230);

		wxColour weekdaysBkgColor(250, 250, 250);
		wxColour weekdaysTextColor(40, 40, 40);
//		wxColour monthBkgColor(230, 230, 250);
		wxColour monthBkgColor(89, 124, 145);
		wxColour monthTextColor(250, 250, 250);

		wxColour selectionBkgColor(175, 186, 209);
		wxColour selectionTextColor(40, 40, 40);
		wxColour selectionDayBkgColor(131, 139, 157);
		wxColour selectionDayTextColor(250, 250, 250);

		uint weekIndex = 0;

		wxString text;
		uint maxY = 0;

		for (uint month = 0; month != 12; ++month)
		{
			if (month > 0)
			{
				if (0 == (month % nbMonthPerRow))
				{
					monthRect.x = weekNumbersWidth;
					monthRect.y = maxY + dayWidth + borderWidth;
					maxY = 0;
				}
				else
					monthRect.x += weekNumbersWidth + monthRect.width + spaceBetweenMonth;
			}

			uint wx = 0;
			uint wy = 0;

			// month name
			dc.SetFont(fontBold);
			{
				uint x = decalX + monthRect.x + wx;
				uint y = decalY + monthRect.y + wy;
				enum { w = dayWidth * 7, };

				dc.SetPen(wxPen(monthBkgColor, 1, wxPENSTYLE_SOLID));
				dc.SetBrush(wxBrush(monthBkgColor, wxBRUSHSTYLE_SOLID));
				dc.DrawRectangle(wxRect(x, y, w, dayWidth));

				text = wxStringFromUTF8(calendar.text.months[month].name);
				auto extend = dc.GetTextExtent(text);
				dc.SetTextForeground(monthTextColor);
				dc.DrawText(text,
					x + w / 2 - extend.GetWidth() / 2,
					y + dayWidth / 2 - extend.GetHeight() / 2);
			}
			wy += dayWidth;

			// days of the week
			dc.SetFont(font);
			dc.SetTextForeground(weekdaysTextColor);
			dc.SetPen(wxPen(weekdaysBkgColor, 1, wxPENSTYLE_SOLID));
			dc.SetBrush(wxBrush(weekdaysBkgColor, wxBRUSHSTYLE_SOLID));
			for (uint wd = 0; wd != 7; ++wd)
			{
				uint x = decalX + monthRect.x + wx + (wd * dayWidth);
				uint y = decalY + monthRect.y + wy;

				dc.DrawRectangle(wxRect(x, y, dayWidth, dayWidth));

				text = Date::WeekdayToLShortString(wd);
				auto extend = dc.GetTextExtent(text);
				dc.DrawText(text,
					x + dayWidth / 2 - extend.GetWidth() / 2,
					y + dayWidth / 2 - extend.GetHeight() / 2);
			}
			dc.SetPen(wxPen(monthBkgColor, 1, wxPENSTYLE_SOLID));
			wy += dayWidth;
			{
				uint x = decalX + monthRect.x + wx;
				uint y = decalY + monthRect.y + wy;
				dc.DrawLine(x, y, x + dayWidth * 7, y);
			}
			wy += borderWidth;

			// week numbers positions
			uint weekNumbersX = decalX + monthRect.x + wx - weekNumbersWidth;
			uint weekNumbersY = decalY + monthRect.y + wy;

			// Current day of the week to display
			uint graphicalWeekday = (uint) calendar.months[month].firstWeekday;
			wx += graphicalWeekday * dayWidth;
			// How many days in the current month
			uint daysPerMonth = calendar.months[month].days;

			// registering the first week number
			{
				auto& cacheweek = pCacheWeek[weekIndex];
				cacheweek.x = weekNumbersX;
				cacheweek.y = weekNumbersY;
				cacheweek.type = dtWeekNormal;
			}

			for (uint day = 0; day < daysPerMonth; ++day, ++dayYear)
			{
				uint x = decalX + monthRect.x + wx;
				uint y = decalY + monthRect.y + wy;

				if (not graphicalWeekday)
				{
					if (day != 0)
					{
						// registering the first week number
						++weekIndex;
						auto& cacheweek = pCacheWeek[weekIndex];
						cacheweek.x = weekNumbersX;
						cacheweek.y = weekNumbersY;
						cacheweek.type = dtWeekNormal;
					}
				}

				pCacheWeek[weekIndex].textX = dayYear;

				auto& cacheday = pCacheDay[dayYear];
				cacheday.x = x;
				cacheday.y = y;

				cacheday.text.clear();
				cacheday.text << (day + 1);

				auto extend = dc.GetTextExtent(cacheday.text);
				cacheday.textX = x + dayWidth / 2 - extend.GetWidth() / 2;
				cacheday.textY = y + dayWidth / 2 - extend.GetHeight() / 2 + textDrawOffsetY;

				if (y > maxY)
					maxY = y;
				if (++graphicalWeekday >= 7)
				{
					graphicalWeekday = 0;
					wx = 0;
					wy += dayWidth;
					weekNumbersY += dayWidth;
				}
				else
					wx += dayWidth;
			}
			++weekIndex;
		}

		// invalidating the last pseudo week
		pCacheWeek[++weekIndex].type = dtInvalid;

		// initializing textextend for all weeks
		for (uint week = 0; week != calendar.maxWeeksInYear; ++week)
			pCacheWeekObjectID[week] = calendar.maxWeeksInYear * 2 - 1; // invalid and never seen

		for (uint pseudoweek = 0; pseudoweek != calendar.maxWeeksInYear * 2; ++pseudoweek)
		{
			auto& cache = pCacheWeek[pseudoweek];
			if (cache.type == dtInvalid)
				break;

			cache.text.clear();
			if (cache.textX < calendar.maxDaysInYear)
			{
				uint week = calendar.days[cache.textX].week;
				week = calendar.weeks[week].userweek;
				//if (pCacheWeekObjectID[week] > pseudoweek)
				//	pCacheWeekObjectID[week] = pseudoweek;
				cache.text << week;
			}

			auto extend = dc.GetTextExtent(cache.text);
			cache.textX = cache.x + weekNumbersWidth - extend.GetWidth() - 6;
			cache.textY = cache.y + dayWidth / 2 - extend.GetHeight() / 2 + textDrawOffsetY;
		}
	}


	void CalendarViewStandard::updateSelectionText(YString& text, uint from, uint to)
	{
		auto& start = calendar.days[from];

		text << ' ';
		if (to != (uint) -1)
			text << "From  ";

		auto realmonth = calendar.months[start.month].realmonth;
		text << Date::WeekdayToString((int) start.weekday);
		text << ", ";
		text << (start.dayMonth + 1) << ' ' << Date::MonthToString((int)realmonth);

		if (to != (uint) -1)
		{
			auto& end = calendar.days[to];
			realmonth = calendar.months[end.month].realmonth;
			text << "  to  ";
			text << Date::WeekdayToString((int) end.weekday);
			text << ", ";
			text << (end.dayMonth + 1) << ' ' << Date::MonthToString((int)realmonth);

			uint nbdays = to - from + 1;
			if (nbdays > 1 && nbdays < 400)
			{
				text << "  (" << nbdays;
				if (nbdays >= 7)
				{
					text << " days, ";
					uint nbweeks = nbdays / 7;
					if (nbweeks == 1)
						text << "1 week)";
					else
						text << nbweeks << " weeks)";
				}
				else
					text << "days)";
			}
		}
	}


	void CalendarViewStandard::updateSelectionText()
	{
		YString text;

		if (pDayYearRangeStart != (uint) -1)
		{
			text = " Selecting  ";
			uint from  = pCurrentSelectionDayRange[0];
			uint to    = pCurrentSelectionDayRange[1];
			if (to == from)
				to = (uint) -1;
			updateSelectionText(text, from, to);
		}
		else
		{
			uint from = pDialog.selectionDayRange[0];
			uint to   = (pDialog.selectionDayRange[1] == (uint) -1) ? (uint) -1 : pDialog.selectionDayRange[1] -1;
			if (from != (uint) -1)
			{
				if (to == from)
					to = (uint) -1;
				updateSelectionText(text, from, to);
			}
		}

		if (text.empty())
			text = "(no selection)";
		onUpdateSelectionText(text);
	}




} // namespace Window
} // namespace Antares
