// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_LIBS_ARRAY_MATRIX_FILL_MTX_H__
#define __ANTARES_LIBS_ARRAY_MATRIX_FILL_MTX_H__

#include <vector>

#include <boost/test/unit_test.hpp>

#include <antares/array/matrix.h>

using namespace std;
using namespace Antares;

template<class T = double>
class Matrix_easy_to_fill: public Matrix<T>
{
public:
    Matrix_easy_to_fill():
        Matrix<T>()
    {
    }

    Matrix_easy_to_fill(unsigned int height, unsigned int width):
        Matrix<T>(height, width)
    {
    }

    Matrix_easy_to_fill(unsigned int height, unsigned int width, const std::vector<T>& vec):
        Matrix<T>()
    {
        BOOST_REQUIRE_EQUAL(height * width, vec.size());
        this->reset(width, height);
        unsigned int count = 0;
        for (unsigned int j = 0; j < height; j++)
        {
            for (unsigned int i = 0; i < width; i++)
            {
                (*this)[i][j] = vec[count];
                count++;
            }
        }
    }

    mutable std::string data;
};

#endif // __ANTARES_LIBS_ARRAY_MATRIX_FILL_MTX_H__
