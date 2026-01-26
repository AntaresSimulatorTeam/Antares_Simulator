// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_TOOLBOX_FILTER_PARAMETER_H__
#define __ANTARES_TOOLBOX_FILTER_PARAMETER_H__

#include <wx/sizer.h>
#include <vector>

namespace Antares::Toolbox::Filter::Operator
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
class Parameter: public wxEvtHandler
{
public:
    // List of parameters
    using List = std::vector<Parameter>;

    /*!
    ** \brief Values in differents types
    **
    ** All values should be identical, since they should come
    ** from a the same value of a wxTextCtrl.
    ** (note: *should* and not *must*)
    */
    struct Value
    {
        Value():
            asInt(0),
            asDouble(0.),
            asString()
        {
        }

        Value(const Value& copy):
            asInt(copy.asInt),
            asDouble(copy.asDouble),
            asString(copy.asString)
        {
        }

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

        Value& operator=(const Value& copy)
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
    Parameter& operator=(const Parameter& copy);

private:
    void onChange(wxCommandEvent& evt);
    void onListChanged(wxCommandEvent& evt);

private:
    wxSizer* pSizer;

}; // class Parameter

} // namespace Antares::Toolbox::Filter::Operator

#endif // __ANTARES_TOOLBOX_FILTER_PARAMETER_H__
