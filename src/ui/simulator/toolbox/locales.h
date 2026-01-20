// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_LOCALES_H__
#define __ANTARES_LOCALES_H__

namespace Antares::Locale
{
/*!
** \brief Get informations about the current locale
*/
void Init();

/*!
** \brief Get the decimal point
*/
extern char DecimalPoint;

} // namespace Antares::Locale

#endif // __ANTARES_LOCALES_H__
