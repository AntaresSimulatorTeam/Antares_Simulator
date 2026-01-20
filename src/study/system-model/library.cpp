// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/study/system-model/library.h"

#include <algorithm>
#include <iterator>
#include <utility>
#include <vector>

namespace Antares::ModelerStudy::SystemModel
{

/**
 * \brief Sets the ID of the library.
 *
 * \param id The ID to set.
 * \return Reference to the LibraryBuilder object.
 */
LibraryBuilder& LibraryBuilder::withId(const std::string& id)
{
    library_.id_ = id;
    return *this;
}

/**
 * \brief Sets the description of the library.
 *
 * \param description The description to set.
 * \return Reference to the LibraryBuilder object.
 */
LibraryBuilder& LibraryBuilder::withDescription(const std::string& description)
{
    library_.description_ = description;
    return *this;
}

/**
 * \brief Sets the port types of the library.
 *
 * \param portTypes A vector of PortType objects to set.
 * \return Reference to the LibraryBuilder object.
 *
 * inputs it not garanteed to be valid after the call
 */
LibraryBuilder& LibraryBuilder::withPortTypes(std::vector<PortType>&& portTypes)
{
    std::transform(portTypes.begin(),
                   portTypes.end(),
                   std::inserter(library_.portTypes_, library_.portTypes_.end()),
                   [](/*Non const to prevent copy*/ PortType& portType)
                   { return std::make_pair(portType.Id(), std::move(portType)); });
    return *this;
}

/**
 * \brief Sets the models of the library.
 *
 * \param models A vector of Model objects to set.
 * \return Reference to the LibraryBuilder object.
 *
 * inputs it not garanteed to be valid after the call
 */
LibraryBuilder& LibraryBuilder::withModels(std::vector<Model>&& models)
{
    std::transform(models.begin(),
                   models.end(),
                   std::inserter(*library_.models_, library_.models_->end()),
                   [](/*Non const to prevent copy*/ Model& model)
                   { return std::make_pair(model.Id(), std::move(model)); });
    return *this;
}

/**
 * \brief Returns the Library object.
 *
 * \return The constructed Library object.
 */
const Library& LibraryBuilder::build() const
{
    return library_;
}
} // namespace Antares::ModelerStudy::SystemModel
