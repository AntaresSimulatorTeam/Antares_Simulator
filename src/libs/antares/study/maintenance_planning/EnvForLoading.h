//
// Created by milos on 1/11/23.
//

#pragma once

#include "yuni/core/fwd.h"
#include <antares/inifile/inifile.h>
#include <antares/array/matrix.h>

namespace Antares::Data {

class AreaList;
class EnvForLoading final
{
public:
    explicit EnvForLoading(AreaList& l, unsigned v) : areaList(l), version(v)
    {
    }
    //! INI file
    Yuni::Clob iniFilename;
    //! Current section
    IniFile::Section* section;

    //! List of areas
    AreaList& areaList;

    unsigned version;
};

} // Data
