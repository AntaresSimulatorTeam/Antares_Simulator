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

#include "spotlight.h"
#include <wx/dcbuffer.h>
#include <wx/settings.h>
#include "listbox-panel.h"


using namespace Yuni;

#define NODE_DRAW_COLOR_VARIATION_LIGHT   30
#define NODE_DRAW_COLOR_VARIATION_DARK    15



namespace Antares
{
namespace Component
{

	//! Convenient typedef for unsigned char
	typedef unsigned char uchar;

	//! Highlight color
	static wxColour  gColorHighlight;
	//! The default background color
	static wxColour  gColorBackgroundDefault;



	/*!
	** \brief Draw a text in highlighting matching sub-strings
	**
	** The sub-strings are provided by @tokens
	**
	** \param text   Raw text
	** \param dc     Device Context
	** \param tokens List of matching sub-strings
	** \param x      An X-Coordinate
	** \param y      An Y-Coordinate
	*/
	template<bool BoldT, bool ReverseColorT>
	static void DrawTextWithSelection(const AnyString& text, const wxString& wxtext, wxDC& dc,
		const Spotlight::SearchToken::VectorPtr& tokens, uint itemHeight,
		uint x, uint y, bool canBeSelected)
	{
		enum
		{
			decalTextY = System::windows ? 1 : 0
		};
		if (!text)
			return;

		if (BoldT)
		{
			wxFont font = dc.GetFont();
			font.SetWeight(wxFONTWEIGHT_BOLD);
			dc.SetFont(font);
		}
		static const wxColour black(30, 30, 30);
		static const wxColour gray(90, 90, 90);
		static const wxColour white(255, 255, 255);
		const wxSize size = dc.GetTextExtent(wxtext);

		if (!tokens || tokens->empty() || !canBeSelected)
		{
			// Text, with shadow
			if (!ReverseColorT)
			{
				dc.SetTextForeground(white);
				dc.DrawText(wxtext, 1 + x, 1 + y + itemHeight / 2 - size.GetHeight() / 2 - decalTextY);
				dc.SetTextForeground(black);
				dc.DrawText(wxtext, x, y + itemHeight / 2 - size.GetHeight() / 2 - decalTextY);
			}
			else
			{
				dc.SetTextForeground(gray);
				dc.DrawText(wxtext, 1 + x, 1 + y + itemHeight / 2 - size.GetHeight() / 2 - decalTextY);
				dc.SetTextForeground(white);
				dc.DrawText(wxtext, x, y + itemHeight / 2 - size.GetHeight() / 2 - decalTextY);
			}
		}
		else
		{
			// restore default foreground color
			dc.SetTextForeground(black);

			bool* regions = new bool[1 + text.size()];
			(void)::memset(regions, 0, sizeof(bool) * (text.size() + 1));

			// Building regions
			{
				const Spotlight::SearchToken::Vector::const_iterator end = tokens->end();
				Spotlight::SearchToken::Vector::const_iterator i = tokens->begin();
				for (; i != end; ++i)
				{
					const Spotlight::SearchToken& token = *(*i);
					String::Size offset = 0;
					do
					{
						offset = text.ifind(token.text, offset);
						if (offset < text.size())
						{
							for (uint j = offset; j < token.text.size() + offset; ++j)
								regions[j] = true;
							++offset;
						}
						else
							break;
					}
					while (true);
				}
			}

			// To force the drawing of the last part
			regions[text.size()] = !regions[text.size() - 1];

			// The first flag
			bool flag = regions[0];
			uint offset = 0;
			for (uint i = 1; i != text.size() + 1; ++i)
			{
				if (regions[i] == flag)
					continue;

				const wxString t = wxStringFromUTF8(text.c_str() + offset, i - offset);
				const wxSize sizet = dc.GetTextExtent(t);

				if (flag)
				{
					dc.SetPen(wxPen(wxColour(230, 230, 0), 1, wxPENSTYLE_SOLID));
					dc.SetBrush(wxBrush(wxColour(255, 255, 0), wxBRUSHSTYLE_SOLID));
					dc.DrawRectangle(x, y + 1, sizet.GetWidth() + 4, itemHeight - 2);

					dc.SetTextForeground(gray);
					dc.DrawText(t, x + 3, y  + itemHeight / 2 - size.GetHeight() / 2 - decalTextY);
					x += sizet.GetWidth() + 6;
					flag = false;
				}
				else
				{
					if (!ReverseColorT)
					{
						dc.SetTextForeground(white);
						dc.DrawText(t, 1 + x, 1 + y  + itemHeight / 2 - size.GetHeight() / 2 - decalTextY);
						dc.SetTextForeground(black);
						dc.DrawText(t, x, y  + itemHeight / 2 - size.GetHeight() / 2 - 1);
					}
					else
					{
						dc.SetTextForeground(gray);
						dc.DrawText(t, 1 + x, 1 + y  + itemHeight / 2 - size.GetHeight() / 2 - decalTextY);
						dc.SetTextForeground(white);
						dc.DrawText(t, x, y  + itemHeight / 2 - size.GetHeight() / 2 - decalTextY);
					}
					x += sizet.GetWidth();
					flag = true;
				}
				offset = i;
			}

			delete[] regions;
		}

		if (BoldT)
		{
			// restore old style
			wxFont font = dc.GetFont();
			font.SetWeight(wxFONTWEIGHT_NORMAL);
			dc.SetFont(font);
		}
	}


	template<bool LeftToRightT>
	static void DrawTag(wxDC& dc, const Spotlight::IItem::Tag& tag, wxRect& bounds, uint itemHeight)
	{
		enum { spaceX = 3 };

		wxSize size = dc.GetTextExtent(tag.text);
		if (!LeftToRightT)
			bounds.x -= size.GetWidth() + 2 + spaceX * 2 + 4;

		float y = tag.color.red * 0.299f + tag.color.green * 0.587f + tag.color.blue * 0.114f;
		float u = (tag.color.blue - y) * 0.565f;
		float v = (tag.color.red - y)  * 0.713f;
		if (y < 50.f)
			y = 50.f;
		else
		{
			if (y > 225.f)
				y = 225.f;
		}

		const int r = Math::MinMax<int>((int)(y + 1.403f * v), 0, 255);
		const int g = Math::MinMax<int>((int)(y - 0.344f * u - 0.714f * v), 0, 255);
		const int b = Math::MinMax<int>((int)(y + 1.770f * u), 0, 255);

		wxColour pCachedColorGradientStart;
		wxColour pCachedColorGradientEnd;
		wxColour pCachedColorGradientStart2;
		wxColour pCachedColorGradientEnd2;
		wxColour pCachedColorText;

		pCachedColorGradientStart.Set(
			(uchar) Math::MinMax<int>(r + NODE_DRAW_COLOR_VARIATION_LIGHT / 2, 0, 255),
			(uchar) Math::MinMax<int>(g + NODE_DRAW_COLOR_VARIATION_LIGHT / 2, 0, 255),
			(uchar) Math::MinMax<int>(b + NODE_DRAW_COLOR_VARIATION_LIGHT / 2, 0, 255) );
		pCachedColorGradientEnd.Set(
			(uchar) Math::MinMax<int>(r - NODE_DRAW_COLOR_VARIATION_DARK / 2, 0, 255),
			(uchar) Math::MinMax<int>(g - NODE_DRAW_COLOR_VARIATION_DARK / 2, 0, 255),
			(uchar) Math::MinMax<int>(b - NODE_DRAW_COLOR_VARIATION_DARK / 2, 0, 255) );
		pCachedColorGradientStart2.Set(
			(uchar) Math::MinMax<int>(r + NODE_DRAW_COLOR_VARIATION_LIGHT * 2, 0, 255),
			(uchar) Math::MinMax<int>(g + NODE_DRAW_COLOR_VARIATION_LIGHT * 2, 0, 255),
			(uchar) Math::MinMax<int>(b + NODE_DRAW_COLOR_VARIATION_LIGHT * 2, 0, 255) );
		pCachedColorGradientEnd2.Set(
			(uchar) Math::MinMax<int>(r - NODE_DRAW_COLOR_VARIATION_DARK, 0, 255),
			(uchar) Math::MinMax<int>(g - NODE_DRAW_COLOR_VARIATION_DARK, 0, 255),
			(uchar) Math::MinMax<int>(b - NODE_DRAW_COLOR_VARIATION_DARK, 0, 255) );

		float yy = (y <= 147.f) ? 255.f : 60.f;
		pCachedColorText.Set(
			(uchar) Math::MinMax<int>((int)(yy + 1.403f * v), 0, 255),
			(uchar) Math::MinMax<int>((int)(yy - 0.344f * u - 0.714f * v), 0, 255),
			(uchar) Math::MinMax<int>((int)(yy + 1.770f * u), 0, 255) );

		// Border color
		wxColour border;
		border.Set(
			(uchar) Math::MinMax<int>(r - NODE_DRAW_COLOR_VARIATION_DARK * 2, 0, 255),
			(uchar) Math::MinMax<int>(g - NODE_DRAW_COLOR_VARIATION_DARK * 2, 0, 255),
			(uchar) Math::MinMax<int>(b - NODE_DRAW_COLOR_VARIATION_DARK * 2, 0, 255) );

		dc.SetPen(wxPen(border, 1, wxPENSTYLE_SOLID));
		dc.DrawRectangle(bounds.x, bounds.y + 2, size.GetWidth() + 2 + spaceX * 2, itemHeight - 4);
		// Gradient inside
		dc.GradientFillLinear(wxRect(bounds.x + 1, 1 + bounds.y + 2, size.GetWidth() + spaceX * 2,
			(itemHeight - 4 - 2) / 2),
			pCachedColorGradientStart, pCachedColorGradientEnd, wxSOUTH);
		dc.GradientFillLinear(wxRect(bounds.x + 1, bounds.y + itemHeight / 2, size.GetWidth() + spaceX * 2,
			(itemHeight - 4 - 2) / 2 ),
			pCachedColorGradientEnd2, pCachedColorGradientStart2, wxSOUTH);

		dc.SetTextForeground(pCachedColorText);
		enum
		{
			// Y offset on Windows for the beauty of alignment
			decalY = System::windows ? 1 : 0,
		};
		dc.DrawText(tag.text, bounds.x + spaceX + 1,
			bounds.y + itemHeight / 2 - size.GetHeight() / 2 - decalY);

		if (LeftToRightT)
			bounds.x += size.GetWidth() + 2 + spaceX * 2 + 4;
	}



	static uint Brightness(const wxColour& c)
	{
		return (uint) Math::SquareRoot(c.Red() * c.Red() * 0.241 + c.Green() * c.Green() * 0.691
			+ c.Blue() * c.Blue() * 0.068);
	}







	Spotlight::IItem::IItem() :
		tag(0),
		pSelected(false),
		pCountedAsResult(true)
	{
		// Global variable initialization
		{
			if (not gColorHighlight.IsOk())
				gColorHighlight = wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHT);
			if (not gColorBackgroundDefault.IsOk())
				gColorBackgroundDefault = wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW);
		}
	}


	Spotlight::IItem::~IItem()
	{
		// destructor
	}


	void Spotlight::IItem::caption(const wxString& caption)
	{
		wxStringToString(caption, pCaption);
		pCacheCaption = caption;
	}



	void Spotlight::IItem::draw(wxDC& dc, uint itemHeight, wxRect& bounds, bool selected,
		const SearchToken::VectorPtr& tokens) const
	{
		enum
		{
			tagHeight = 18,
		};

		// True to reverse text/shadow colors
		// When the background is dark, white should be the main color
		bool reverseTextColor = false;
		// Get if the item can be selected
		bool mayHasSelection = canBeSelected();

		if ((selected || pSelected) && (mayHasSelection))
		{
			const wxColour& selectionColor = gColorHighlight;
			wxColour border = selectionColor;
			wxColour a;
			if (pSelected)
			{
				a.Set(
					(uchar) Math::MinMax<int>(selectionColor.Red()   + 40, 0, 255),
					(uchar) Math::MinMax<int>(selectionColor.Green() + 40, 0, 255),
					(uchar) Math::MinMax<int>(selectionColor.Blue()  + 40, 0, 255), 120);
			}
			else
			{
				a.Set(
					(uchar) Math::MinMax<int>(gColorBackgroundDefault.Red()   - 25, 0, 255),
					(uchar) Math::MinMax<int>(gColorBackgroundDefault.Green() - 25, 0, 255),
					(uchar) Math::MinMax<int>(gColorBackgroundDefault.Blue()  - 25, 0, 255));
				border.Set(
					(uchar) Math::MinMax<int>(gColorBackgroundDefault.Red()   - 45, 0, 255),
					(uchar) Math::MinMax<int>(gColorBackgroundDefault.Green() - 45, 0, 255),
					(uchar) Math::MinMax<int>(gColorBackgroundDefault.Blue()  - 45, 0, 255));
			}

			if (Brightness(a) <= 180)
				reverseTextColor = true;

			dc.SetPen(wxPen(border, 1, wxPENSTYLE_SOLID));
			dc.SetBrush(wxBrush(a, wxBRUSHSTYLE_SOLID));
			dc.DrawRectangle(bounds.x, bounds.y, bounds.width - bounds.x - 1, itemHeight);
		}

		// Bitmap drawing
		if (pBitmap.IsOk())
		{
			dc.DrawBitmap(pBitmap, bounds.x + 2, bounds.y + itemHeight / 2 - pBitmap.GetHeight() / 2, true);
			if (pBitmap.GetWidth() > 20)
				bounds.x += pBitmap.GetWidth() + 5 + 3;
			else
				bounds.x += 25;
		}
		else
			bounds.x += 5;

		// All tags
		if (not pLeftTags.empty())
		{
			auto end = pLeftTags.end();
			for (auto i = pLeftTags.begin(); i != end; ++i)
				DrawTag<true>(dc, *(*i), bounds, itemHeight);

			// For beauty (space before caption)
			bounds.x += 3;
		}

		if (not pSubCaption.empty())
		{
			// Sub caption, wx format
			wxString subCaption = wxStringFromUTF8(pSubCaption);
			bool b = pCaption.equals("W2");
			int length = subCaption.Length();
			// display the main label
			// display the sub label, with shadow
			if (!reverseTextColor)
			{
				DrawTextWithSelection<true, false>(pCaption, pCacheCaption, dc, tokens, tagHeight,
					bounds.x, bounds.y, mayHasSelection);
				if (pSubCaption.length() * 5 < bounds.width - bounds.x - 1)
				{
					dc.SetTextForeground(wxColour(255, 255, 255));
					dc.DrawText(subCaption, bounds.x + 1, bounds.y + 16 + 1);
					dc.SetTextForeground(wxColour(70, 70, 90));
					dc.DrawText(subCaption, bounds.x, bounds.y + 16);
				}
				else
					dc.DrawText(subCaption(0, subCaption.Find(')') +1) + " ...", bounds.x, bounds.y + 16);
			}
			else
			{
				DrawTextWithSelection<true, true>(pCaption, pCacheCaption, dc, tokens, tagHeight,
					bounds.x, bounds.y, mayHasSelection);
				if (pSubCaption.length() * 5 < bounds.width - bounds.x - 1)
				{
					dc.SetTextForeground(wxColour(70, 70, 90));
					dc.DrawText(subCaption, bounds.x + 1, bounds.y + 16 + 1);
					dc.SetTextForeground(wxColour(255, 255, 255));
					dc.DrawText(subCaption, bounds.x, bounds.y + 16);
				}
				else
					dc.DrawText(subCaption(0, subCaption.Find(')') + 1) + " ...", bounds.x, bounds.y + 16);
			}
		}
		else
		{
			// display main label
			if (!reverseTextColor)
			{
				DrawTextWithSelection<false, false>(pCaption, pCacheCaption, dc, tokens, tagHeight,
					bounds.x, bounds.y, mayHasSelection);
			}
			else
			{
				DrawTextWithSelection<false, true>(pCaption, pCacheCaption, dc, tokens, tagHeight,
					bounds.x, bounds.y, mayHasSelection);
			}
		}

		// All tags
		if (!pRightTags.empty())
		{
			bounds.x = bounds.width;
			Tag::Vector::const_iterator end = pRightTags.end();
			for (Tag::Vector::const_iterator i = pRightTags.begin(); i != end; ++i)
				DrawTag<false>(dc, *(*i), bounds, itemHeight);
		}

		// increasing the Y offset
		bounds.y += itemHeight;
	}






} // namespace Component
} // namespace Antares

