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
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/

#ifndef __ANTARES_LIBS_ARRAY_MATRIX_FILL_MTX_H__
#define __ANTARES_LIBS_ARRAY_MATRIX_FILL_MTX_H__

#include <vector>

#include <boost/test/unit_test.hpp>

#include <antares/array/matrix.h>

using namespace std;
using namespace Antares;

template<class T = double, class ReadWriteT = T>
class Matrix_easy_to_fill: public Matrix<T, ReadWriteT>
{
public:
    Matrix_easy_to_fill():
        Matrix<T, ReadWriteT>()
    {
    }

    Matrix_easy_to_fill(uint height, uint width):
        Matrix<T, ReadWriteT>(height, width)
    {
    }

    Matrix_easy_to_fill(uint height, uint width, const vector<T>& vec):
        Matrix<T, ReadWriteT>()
    {
        BOOST_REQUIRE_EQUAL(height * width, vec.size());
        this->reset(width, height, true);
        uint count = 0;
        for (uint j = 0; j < height; j++)
        {
            for (uint i = 0; i < width; i++)
            {
                this->entry[i][j] = vec[count];
                count++;
            }
        }
    }

    bool openFile(Yuni::IO::File::Stream& /* file */,
                  const AnyString& /* filename */) const override
    {
        return true;
    }

    void saveBufferToFile(std::string& buffer, Yuni::IO::File::Stream& /* f */) const override
    {
        data = buffer;
    }

public:
    mutable std::string data;
};

#endif // __ANTARES_LIBS_ARRAY_MATRIX_FILL_MTX_H__
