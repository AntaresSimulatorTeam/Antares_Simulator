// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

namespace Antares::Logs
{
enum class Color
{
    none,
    red,
    yellow,
    green,
    white
};

struct LevelInfo
{
    int level;
    const char* tag;
    bool toStderr;
    bool notifyCallback;
    Color tagColor;
    Color messageColor;
};
}
