/*
** Copyright 2007-2024, RTE (https://www.rte-france.com)
** See AUTHORS.txt
** SPDX-License-Identifier: MPL-2.0
** This file is part of Antares-Simulator,
** Adequacy and Performance assessment for interconnected energy networks.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the Mozilla Public Licence 2.0 as published by
** the Mozilla Foundation, either version 2 of the License, or
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
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/
#ifndef __ANTARES_WINDOWS_OUTPUT_LAYER_H__
#define __ANTARES_WINDOWS_OUTPUT_LAYER_H__

#include "../../application/study.h"
#include "fwd.h"

namespace Antares
{
namespace Window
{
namespace OutputViewer
{
class Layer
{
public:
    //! Array
    using Vector = std::vector<Layer*>;

public:
    //! \name Constructor & Destructor
    //@{
    /*!
    ** \brief Default constructor
    */
    Layer(LayerType t = ltOutput);
    /*!
    ** \brief Copy constructor
    */
    Layer(const Layer& rhs);
    //! Destructor
    ~Layer();
    //@}

    /*!
    ** \brief Get the alternative caption (for virtual layers)
    */
    const wxString& alternativeCaption() const;

    //! Get if the layer is virtual
    bool isVirtual() const;

public:
    //! Index of the output
    uint index;
    //! The output
    Data::Output::Ptr selection;
    //! Type of the layer
    const LayerType type;

    //! Flag to know if the selection of the layer is detached from the global selection
    bool detached;
    //! Custom selection type (only valid if detached)
    SelectionType customSelectionType;
    //! Custom area / link selection (only valid if detached)
    Yuni::String customAreaOrLink;

private:
    void resetAlternativeCaption();

private:
    //! Alternative Caption (for virtual layer)
    wxString pAlternativeCaption;

}; // class Layer

} // namespace OutputViewer
} // namespace Window
} // namespace Antares

#include "layer.hxx"

#endif // __ANTARES_WINDOWS_OUTPUT_LAYER_H__
