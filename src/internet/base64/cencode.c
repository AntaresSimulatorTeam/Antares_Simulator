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

#include "cencode.h"

/* const int CHARS_PER_LINE = ANTARES  we do not want to justify per line 72;*/

void base64_init_encodestate(base64_encodestate* state_in)
{
    state_in->step = step_A;
    state_in->result = 0;
    state_in->stepcount = 0;
}

char base64_encode_value(char value_in)
{
    static const char* encoding
      = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    if (value_in > 63)
        return '=';
    return encoding[(int)value_in];
}

int base64_encode_block(const char* plaintext_in,
                        int length_in,
                        char* code_out,
                        base64_encodestate* state_in)
{
    const char* plainchar = plaintext_in;
    const char* const plaintextend = plaintext_in + length_in;
    char* codechar = code_out;
    char result;
    char fragment;

    result = state_in->result;

    switch (state_in->step)
    {
        while (1)
        {
        case step_A:
            if (plainchar == plaintextend)
            {
                state_in->result = result;
                state_in->step = step_A;
                return (int)(codechar - code_out);
            }
            fragment = *plainchar++;
            result = (char)((fragment & 0x0fc) >> 2);
            *codechar++ = base64_encode_value(result);
            result = (char)((fragment & 0x003) << 4);
        case step_B:
            if (plainchar == plaintextend)
            {
                state_in->result = result;
                state_in->step = step_B;
                return (int)(codechar - code_out);
            }
            fragment = *plainchar++;
            result |= (char)((fragment & 0x0f0) >> 4);
            *codechar++ = base64_encode_value(result);
            result = (char)((fragment & 0x00f) << 2);
        case step_C:
            if (plainchar == plaintextend)
            {
                state_in->result = result;
                state_in->step = step_C;
                return (int)(codechar - code_out);
            }
            fragment = *plainchar++;
            result |= (char)((fragment & 0x0c0) >> 6);
            *codechar++ = base64_encode_value(result);
            result = (fragment & 0x03f) >> 0;
            *codechar++ = base64_encode_value(result);

            ++(state_in->stepcount);
            /*if (state_in->stepcount == CHARS_PER_LINE/4)
            {
                    *codechar++ = '\n';
                    state_in->stepcount = 0;
            }*/
        }
    }
    /* control should not reach here */
    return (int)(codechar - code_out);
}

int base64_encode_blockend(char* code_out, base64_encodestate* state_in)
{
    char* codechar = code_out;

    switch (state_in->step)
    {
    case step_B:
        *codechar++ = base64_encode_value(state_in->result);
        *codechar++ = '=';
        *codechar++ = '=';
        break;
    case step_C:
        *codechar++ = base64_encode_value(state_in->result);
        *codechar++ = '=';
        break;
    case step_A:
        break;
    }
    /**codechar++ = '\n';*/

    return (int)(codechar - code_out);
}
