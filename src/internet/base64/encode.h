/*
** Copyright 2007-2018 RTE
** Authors: Antares_Simulator Team
**
** This file is part of Antares_Simulator.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
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
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Antares_Simulator. If not, see <http://www.gnu.org/licenses/>.
**
** SPDX-License-Identifier: licenceRef-GPL3_WITH_RTE-Exceptions
*/
// :mode=c++:
#ifndef BASE64_ENCODE_H
#define BASE64_ENCODE_H

#include <iostream>

namespace base64
{
extern "C"
{
#include "cencode.h"
}

struct encoder
{
    base64_encodestate _state;
    int _buffersize;

    encoder(int buffersize_in = BUFFERSIZE) : _buffersize(buffersize_in)
    {
    }

    int encode(char value_in)
    {
        return base64_encode_value(value_in);
    }

    int encode(const char* code_in, const int length_in, char* plaintext_out)
    {
        return base64_encode_block(code_in, length_in, plaintext_out, &_state);
    }

    int encode_end(char* plaintext_out)
    {
        return base64_encode_blockend(plaintext_out, &_state);
    }

    void encode(std::istream& istream_in, std::ostream& ostream_in)
    {
        base64_init_encodestate(&_state);
        //
        const int N = _buffersize;
        char* plaintext = new char[N];
        char* code = new char[2 * N];
        int plainlength;
        int codelength;

        do
        {
            istream_in.read(plaintext, N);
            plainlength = istream_in.gcount();
            //
            codelength = encode(plaintext, plainlength, code);
            ostream_in.write(code, codelength);
        } while (istream_in.good() && plainlength > 0);

        codelength = encode_end(code);
        ostream_in.write(code, codelength);
        //
        base64_init_encodestate(&_state);

        delete[] code;
        delete[] plaintext;
    }
};

} // namespace base64

#endif // BASE64_ENCODE_H
