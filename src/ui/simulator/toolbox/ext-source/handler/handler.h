// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_TOOLBOX_EXT_SOURCE_HANDLER_HANDLER_H__
#define __ANTARES_TOOLBOX_EXT_SOURCE_HANDLER_HANDLER_H__

#include <yuni/yuni.h>
#include <yuni/core/string.h>
#include <antares/study/study.h>
#include <map>

namespace Antares::ExtSource::Handler
{
//! A single key
using Key = Yuni::CString<50, false>;
//! A single value
using Value = Yuni::String;

//! Map of properties
using PropertyMap = std::map<Key, Value>;

/*!
** \brief Paste from the clipboard items from an Antares Study
*/
void AntaresStudy(Data::Study::Ptr target,
                  const Yuni::String& content,
                  uint offset,
                  PropertyMap& map,
                  bool forceDialog);

} // namespace Antares::ExtSource::Handler

#endif // __ANTARES_TOOLBOX_EXT_SOURCE_HANDLER_HANDLER_H__
