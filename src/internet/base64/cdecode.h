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

#ifndef BASE64_CDECODE_H
#define BASE64_CDECODE_H

#ifdef __cplusplus /* Only with a C++ Compiler */
extern "C"
{
#endif

    typedef enum
    {
        step_a,
        step_b,
        step_c,
        step_d
    } base64_decodestep;

    typedef struct
    {
        base64_decodestep step;
        char plainchar;
    } base64_decodestate;

    void base64_init_decodestate(base64_decodestate* state_in);

    int base64_decode_value(char value_in);

    int base64_decode_block(const char* code_in,
                            const int length_in,
                            char* plaintext_out,
                            base64_decodestate* state_in);

#ifdef __cplusplus
}
#endif
#endif /* BASE64_CDECODE_H */
