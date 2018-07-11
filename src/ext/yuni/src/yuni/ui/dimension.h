/*
** This file is part of libyuni, a cross-platform C++ framework (http://libyuni.org).
**
** This Source Code Form is subject to the terms of the Mozilla Public License
** v.2.0. If a copy of the MPL was not distributed with this file, You can
** obtain one at http://mozilla.org/MPL/2.0/.
**
** github: https://github.com/libyuni/libyuni/
** gitlab: https://gitlab.com/libyuni/libyuni/ (mirror)
*/
#ifndef __YUNI_UI_DIMENSION_H__
# define __YUNI_UI_DIMENSION_H__

# include <cassert>

namespace Yuni
{
namespace UI
{

	class ConversionData
	{
	public:
		float unitPerPercentParent;
		float unitPerPercentScreen;
		float unitPerPixel = 1.0f;
		float unitPerPoint = 4.0f / 3.0f; // = 96dpi / 72ppi
		float unitPerEm;
		float unitPerEn;
		float unitPerCentimeters = 243.84f; // = 96 dpi * 2.54 (inches in a cm)

	}; // class ConversionData



	/*!
	** \brief A dimension with a unit, used for placement and sizing of graphical elements
	*/
	class Dimension
	{
	public:
		enum Unit
		{
			uPercentParent,
			uPercentScreen,
			uPixel,
			uPoint,
			uEm,
			uEn,
			uCentimeters

		};

	public:
		Dimension(float value, Unit unit):
			pUnit(unit),
			pValue(value)
		{}

		Dimension(const Dimension& other):
			pUnit(other.pUnit),
			pValue(other.pValue)
		{}

		Dimension& operator = (const Dimension& other)
		{
			pUnit = other.pUnit;
			pValue = other.pValue;
			return *this;
		}

		Dimension& operator = (float value)
		{
			pValue = value;
			return *this;
		}

		void reset(float value, Unit unit)
		{
			pValue = value;
			pUnit = unit;
		}

		float operator () () const { return pValue; }
		float operator () (const ConversionData& conversion) const { return convert(conversion); }
		operator float () const { return pValue; }

		float convert(const ConversionData& conversion) const
		{
			switch (pUnit)
			{
				case uPercentParent:
					return pValue * conversion.unitPerPercentParent;
				case uPercentScreen:
					return pValue * conversion.unitPerPercentScreen;
				case uPixel:
					return pValue * conversion.unitPerPixel;
				case uPoint:
					return pValue * conversion.unitPerPoint;
				case uEm:
					return pValue * conversion.unitPerEm;
				case uEn:
					return pValue * conversion.unitPerEn;
				case uCentimeters:
					return pValue * conversion.unitPerCentimeters;
				default:
					assert(false && "Invalid enum value !");
					return pValue;
			}
		}

	private:
		Unit pUnit;

		float pValue;

	}; // class Dimension



} // namespace UI
} // namespace Yuni



inline Yuni::UI::Dimension operator "" _pcs(long double value)
{
	return Yuni::UI::Dimension((float)value, Yuni::UI::Dimension::uPercentScreen);
}

inline Yuni::UI::Dimension operator "" _pcs(unsigned long long value)
{
	return Yuni::UI::Dimension((float)value, Yuni::UI::Dimension::uPercentScreen);
}


inline Yuni::UI::Dimension operator "" _pcp(long double value)
{
	return Yuni::UI::Dimension((float)value, Yuni::UI::Dimension::uPercentParent);
}

inline Yuni::UI::Dimension operator "" _pcp(unsigned long long value)
{
	return Yuni::UI::Dimension((float)value, Yuni::UI::Dimension::uPercentParent);
}


inline Yuni::UI::Dimension operator "" _px(long double value)
{
	return Yuni::UI::Dimension((float)value, Yuni::UI::Dimension::uPixel);
}

inline Yuni::UI::Dimension operator "" _px(unsigned long long value)
{
	return Yuni::UI::Dimension((float)value, Yuni::UI::Dimension::uPixel);
}


inline Yuni::UI::Dimension operator "" _pt(long double value)
{
	return Yuni::UI::Dimension((float)value, Yuni::UI::Dimension::uPoint);
}

inline Yuni::UI::Dimension operator "" _pt(unsigned long long value)
{
	return Yuni::UI::Dimension((float)value, Yuni::UI::Dimension::uPoint);
}


inline Yuni::UI::Dimension operator "" _em(long double value)
{
	return Yuni::UI::Dimension((float)value, Yuni::UI::Dimension::uEm);
}

inline Yuni::UI::Dimension operator "" _em(unsigned long long value)
{
	return Yuni::UI::Dimension((float)value, Yuni::UI::Dimension::uEm);
}


inline Yuni::UI::Dimension operator "" _en(long double value)
{
	return Yuni::UI::Dimension((float)value, Yuni::UI::Dimension::uEn);
}

inline Yuni::UI::Dimension operator "" _en(unsigned long long value)
{
	return Yuni::UI::Dimension((float)value, Yuni::UI::Dimension::uEn);
}


inline Yuni::UI::Dimension operator "" _cm(long double value)
{
	return Yuni::UI::Dimension((float)value, Yuni::UI::Dimension::uCentimeters);
}

inline Yuni::UI::Dimension operator "" _cm(unsigned long long value)
{
	return Yuni::UI::Dimension((float)value, Yuni::UI::Dimension::uCentimeters);
}


#endif // __YUNI_UI_DIMENSION_H__
