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
#ifndef __ANTARES_TOOLBOX_COMPONENT_DATAGRID_MODIFIERS_HXX__
#define __ANTARES_TOOLBOX_COMPONENT_DATAGRID_MODIFIERS_HXX__

namespace Antares
{
namespace Component
{
namespace Datagrid
{
namespace // anonymous
{
enum ModifierSet
{
    modifierValues,
    modifierDataset,
    modifierMax
};

enum OpInputType
{
    opInputText = 0, // default
    opInputVoid,     // nothing
    opInputDate,
};

// Forward declaration
template<enum ModifierSet ModifierT>
struct ModifierOperatorsData;

template<enum ModifierSet ModifierT>
struct ModifierOperatorsData
{
    // nothing
};

template<>
struct ModifierOperatorsData<modifierValues>
{
    enum Operator
    {
        opPlus,
        opAssign,
        opMinus,
        opMult,
        opDiv,
        opAbs,
        opMax
    };

    static uint OperatorCount()
    {
        return (uint)opMax;
    }

    static const wchar_t* Name()
    {
        return L"Values";
    }

    static const wchar_t* ApplyName(uint)
    {
        return L"Apply";
    }
    static const wchar_t* OperatorToCString(uint op)
    {
        if (op < (uint)opMax)
        {
            switch ((Operator)op)
            {
            case opPlus:
                return L"+";
            case opAssign:
                return L"=";
            case opMinus:
                return L"-";
            case opMult:
                return L"*";
            case opDiv:
                return L"/";
            case opAbs:
                return L"ABS";
            case opMax:
                break;
            }
        }
        return L"?";
    }

    static OpInputType OperatorInputType(uint op)
    {
        if (op == (uint)opAbs)
            return opInputVoid;
        return opInputText;
    }

    static void ApplyChanges(uint op, const YString& value, Renderer::IRenderer*, VGridHelper* grid)
    {
        if (op >= (uint)opMax)
            return;
        double input;
        if ((Operator)op != opAbs && not value.to(input))
        {
            logs.error() << "The value does not seem a valid number";
            return;
        }
        int width = grid->GetNumberCols();
        int height = grid->GetNumberRows();
        wxString text;

        switch ((Operator)op)
        {
        case opPlus:
        {
            if (not Yuni::Math::Zero(input))
            {
                for (int w = 0; w < width; ++w)
                {
                    for (int h = 0; h < height; ++h)
                    {
                        text.clear();
                        double d = grid->GetNumericValue(h, w);
                        grid->SetValue(h, w, text << (d + input));
                    }
                }
            }
            break;
        }
        case opAssign:
        {
            text.clear();
            text << input;
            for (int w = 0; w < width; ++w)
            {
                for (int h = 0; h < height; ++h)
                    grid->SetValue(h, w, text);
            }
            break;
        }
        case opMinus:
        {
            if (not Yuni::Math::Zero(input))
            {
                for (int w = 0; w < width; ++w)
                {
                    for (int h = 0; h < height; ++h)
                    {
                        text.clear();
                        double d = grid->GetNumericValue(h, w);
                        grid->SetValue(h, w, text << (d - input));
                    }
                }
            }
            break;
        }
        case opMult:
        {
            if (Yuni::Math::Zero(input))
            {
                text = wxT("0");
                for (int w = 0; w < width; ++w)
                {
                    for (int h = 0; h < height; ++h)
                        grid->SetValue(h, w, text);
                }
            }
            else
            {
                if (not Yuni::Math::Equals(input, 1.))
                {
                    for (int w = 0; w < width; ++w)
                    {
                        for (int h = 0; h < height; ++h)
                        {
                            text.clear();
                            double d = grid->GetNumericValue(h, w);
                            grid->SetValue(h, w, text << (d * input));
                        }
                    }
                }
            }
            break;
        }
        case opDiv:
        {
            if (Yuni::Math::Zero(input))
            {
                logs.error() << "divide by zero. aborting.";
                return;
            }
            if (not Yuni::Math::Equals(input, 1.))
            {
                for (int w = 0; w < width; ++w)
                {
                    for (int h = 0; h < height; ++h)
                    {
                        text.clear();
                        double d = grid->GetNumericValue(h, w);
                        grid->SetValue(h, w, text << (d / input));
                    }
                }
            }
            break;
        }
        case opAbs:
        {
            for (int w = 0; w < width; ++w)
            {
                for (int h = 0; h < height; ++h)
                {
                    double d = grid->GetNumericValue(h, w);
                    if (d < 0) // avoid as much as possible grid->SetValue
                    {
                        text.clear();
                        grid->SetValue(h, w, text << (-d));
                    }
                }
            }
            break;
        }
        case opMax:
            break;
        }
    }
};

template<>
struct ModifierOperatorsData<modifierDataset>
{
    enum Operator
    {
        opShiftRows,
        opResizeColumns,
        opMax
    };

    static uint OperatorCount()
    {
        return (uint)opMax;
    }

    static const wchar_t* Name()
    {
        return L"Dataset";
    }

    static const wchar_t* ApplyName(uint)
    {
        return L"Apply";
    }

    static const wchar_t* OperatorToCString(uint op)
    {
        if (op < (uint)opMax)
        {
            switch ((Operator)op)
            {
            case opShiftRows:
                return L"Shift rows until";
            case opResizeColumns:
                return L"Resize columns to";
            case opMax:
                break;
            }
        }
        return L"?";
    }

    static OpInputType OperatorInputType(uint op)
    {
        if (op == (uint)opShiftRows)
            return opInputDate;
        return opInputText;
    }

    static void ApplyChanges(uint op,
                             const YString& value,
                             Renderer::IRenderer* renderer,
                             VGridHelper*)
    {
        if (op >= (uint)opMax)
            return;

        switch ((Operator)op)
        {
        case opShiftRows:
        {
            uint index = 0;
            uint month = (uint)-1;
            uint day = 1;

            value.words(" /-.", [&](AnyString& word) -> bool {
                switch (index)
                {
                case 0:
                    month = word.to<uint>();
                    if (month < 1 || month > 12)
                    {
                        logs.error() << "invalid month: got '" << word << "' => " << month;
                        month = (uint)-1;
                    }
                    break;
                case 1:
                    if (not word.to(day) || day < 1 || day > 31)
                    {
                        logs.error() << "invalid day";
                        day = (uint)-1;
                    }
                }
                ++index;
                return true;
            });

            --month;
            --day;
            if (month < 12 && day < 31)
            {
                auto monthname = (MonthName)month;
                if (not renderer->circularShiftRowsUntilDate(monthname, day))
                    logs.error() << "impossible to perform the row shifting in this dataset";
            }
            break;
        }
        case opResizeColumns:
        {
            uint width = (uint)renderer->width();
            uint maxwidth = (uint)renderer->maxWidthResize();
            uint newwidth = value.to<uint>();

            if (newwidth == width)
                break; // nothing to do

            if (newwidth > 0 && newwidth <= maxwidth)
            {
                logs.info() << "Resizing the matrix to " << newwidth << " columns";
                renderer->resizeMatrixToXColumns(newwidth);
            }
            else
            {
                if (maxwidth > 0)
                {
                    switch (newwidth)
                    {
                    case 0:
                        logs.error() << "impossible to resize the matrix";
                        break;
                    case 1:
                        logs.error() << "impossible to resize the matrix to 1 column";
                        break;
                    default:
                        logs.error()
                          << "impossible to resize the matrix to " << newwidth << " columns";
                    }
                }
                else
                    logs.error() << "impossible to resize the matrix";
            }
            break;
        }
        case opMax:
            break;
        }
    }
};

struct ModifierOperators
{
    static uint OperatorCount(ModifierSet modifier)
    {
        switch (modifier)
        {
        case modifierValues:
            return ModifierOperatorsData<modifierValues>::OperatorCount();
        case modifierDataset:
            return ModifierOperatorsData<modifierDataset>::OperatorCount();
        case modifierMax:
            break;
        }
        return 0;
    }

    static const wchar_t* Name(ModifierSet modifier)
    {
        switch (modifier)
        {
        case modifierValues:
            return ModifierOperatorsData<modifierValues>::Name();
        case modifierDataset:
            return ModifierOperatorsData<modifierDataset>::Name();
        case modifierMax:
            break;
        }
        return L"?";
    }

    static const wchar_t* ApplyName(ModifierSet modifier, uint op)
    {
        switch (modifier)
        {
        case modifierValues:
            return ModifierOperatorsData<modifierValues>::ApplyName(op);
        case modifierDataset:
            return ModifierOperatorsData<modifierDataset>::ApplyName(op);
        case modifierMax:
            break;
        }
        return L"?";
    }

    static const wchar_t* OperatorToCString(ModifierSet modifier, uint op)
    {
        switch (modifier)
        {
        case modifierValues:
            return ModifierOperatorsData<modifierValues>::OperatorToCString(op);
        case modifierDataset:
            return ModifierOperatorsData<modifierDataset>::OperatorToCString(op);
        case modifierMax:
            break;
        }
        return L"?";
    }

    static OpInputType OperatorInputType(ModifierSet modifier, uint op)
    {
        switch (modifier)
        {
        case modifierValues:
            return ModifierOperatorsData<modifierValues>::OperatorInputType(op);
        case modifierDataset:
            return ModifierOperatorsData<modifierDataset>::OperatorInputType(op);
        case modifierMax:
            break;
        }
        return opInputText;
    }

    static void ApplyChanges(ModifierSet modifier,
                             uint op,
                             const YString& value,
                             Renderer::IRenderer* renderer,
                             VGridHelper* grid)
    {
        if (!renderer)
            return;
        switch (modifier)
        {
        case modifierValues:
            ModifierOperatorsData<modifierValues>::ApplyChanges(op, value, renderer, grid);
            break;
        case modifierDataset:
            ModifierOperatorsData<modifierDataset>::ApplyChanges(op, value, renderer, grid);
            break;
        case modifierMax:
            break;
        }
    }

}; // class ModifierOperators

} // anonymous namespace
} // namespace Datagrid
} // namespace Component
} // namespace Antares

#endif // __ANTARES_TOOLBOX_COMPONENT_DATAGRID_MODIFIERS_HXX__
