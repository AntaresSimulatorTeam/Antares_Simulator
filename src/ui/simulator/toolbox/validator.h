// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_TOOLBOX_VALIDATOR_H__
#define __ANTARES_TOOLBOX_VALIDATOR_H__

#include <wx/valtext.h>

namespace Antares::Toolbox::Validator
{
/*!
** \brief Get a default text validator for wx controls
**
** This validator validates these characters :
**  - Alphanumeric
**  - space
**  - '-'
**  - '_'
**  - '.'
*/
wxTextValidator Default();

/*!
** \brief Get a text validator for numerical content
**
** This validator validates these characters :
**  - Alphanumeric
**  - space
**  - '-'
**  - '_'
**  - '.'
*/
wxTextValidator Numeric();

} // namespace Antares::Toolbox::Validator

#endif // __ANTARES_TOOLBOX_VALIDATOR_H__
