// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_WINDOWS_OUTPUT_LAYER_H__
#define __ANTARES_WINDOWS_OUTPUT_LAYER_H__

#include "../../application/study.h"
#include "fwd.h"

namespace Antares::Window::OutputViewer
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

} // namespace Antares::Window::OutputViewer

#include "layer.hxx"

#endif // __ANTARES_WINDOWS_OUTPUT_LAYER_H__
