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
#ifndef __ANTARES_TOOLBOX_FILTER_PARAMETER_H__
# define __ANTARES_TOOLBOX_FILTER_PARAMETER_H__

# include <antares/wx-wrapper.h>
# include <wx/sizer.h>
# include <vector>


namespace Antares
{
namespace Toolbox
{
namespace Filter
{
namespace Operator
{

	// Forward declaration
	class AOperator;

	struct DataType
	{
		enum Value
		{
			dtNone,
			dtInt,
			dtFloat,
			dtString,
			dtList
		};
	};



	/*!
	** \brief Single parameter
	**
	** A parameter is most of the time a text field filled by the
	** user. All parameters are located after the operator of the
	** current filter.
	*/
	class Parameter : public wxEvtHandler
	{
	public:
		// List of parameters
		typedef std::vector<Parameter> List;

		/*!
		** \brief Values in differents types
		**
		** All values should be identical, since they should come
		** from a the same value of a wxTextCtrl.
		** (note: *should* and not *must*)
		*/
		struct Value
		{
			Value()
				:asInt(0), asDouble(0.), asString()
			{}
			Value(const Value& copy)
				:asInt(copy.asInt), asDouble(copy.asDouble), asString(copy.asString)
			{}

			//! Value as Int
			int asInt;
			//! Value as Double
			double asDouble;
			//! Value as String (raw value, should come from a wxTextCtrl)
			wxString asString;

			void reset(const int a)
			{
				asInt = a;
				asDouble = double(a);
				asString.Clear();
				asString << a;
			}

			void reset(const double a)
			{
				asInt = (int)floor(a);
				asDouble = a;
				asString.Clear();
				asString << a;
			}

			void reset(const wxString& s)
			{
				asString = s;
				long i;
				s.ToLong(&i);
				asInt = i;
				s.ToDouble(&asDouble);
			}

			Value& operator = (const Value& copy)
			{
				asInt = copy.asInt;
				asDouble = copy.asDouble;
				asString = copy.asString;
				return *this;
			}

		}; // struct Value

	public:
		//! \name Constructors && Destructor
		//@{
		//! Default constructor
		Parameter(AOperator& parent);
		//! Copy constructor
		Parameter(const Parameter& copy);
		//! Destructor
		virtual ~Parameter();
		//@}

		/*!
		** \brief Create if needed then get the sizer
		*/
		wxSizer* sizer(wxWindow* parent);

		//! \name Presets
		//@{
		//! Standard int
		Parameter& presetInt();
		//! Preset: WeekDay
		Parameter& presetWeekDay();
		//! Preset: Hour year
		Parameter& presetHourYear();
		//! Preset: Hour day
		Parameter& presetHourDay();
		//! Preset: Week number
		Parameter& presetWeek();
		//! Preset: Modulo
		Parameter& presetModuloAddon();
		//@}

		void refreshAttachedGrid();

	public:
		//! Parent operator
		AOperator& pOperator;
		//! Caption
		wxString caption;
		//! Post caption
		wxString postCaption;
		//! Data type
		DataType::Value dataType;
		//! Presets
		std::vector<wxString> defaultValues;

		//! Values in different types
		Value value;

		//! Operator =
		Parameter& operator = (const Parameter& copy);

	private:
		void onChange(wxCommandEvent& evt);
		void onListChanged(wxCommandEvent& evt);

	private:
		wxSizer* pSizer;

	}; // class Parameter






} // namespace Operator
} // namespace Filter
} // namespace Toolbox
} // namespace Antares

#endif // __ANTARES_TOOLBOX_FILTER_PARAMETER_H__
