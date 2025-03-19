/*
Copyright (c) 2024, RTE (https://www.rte-france.com)

See AUTHORS.txt

This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at http://mozilla.org/MPL/2.0/.

SPDX-License-Identifier: MPL-2.0

This file is part of the Antares project.
*/

grammar Expr;

/* To match the whole input */
fullexpr: expr EOF;

expr
    : atom                                     # unsignedAtom
    | IDENTIFIER '.' IDENTIFIER                # portField
    | '-' expr                                 # negation
    | '(' expr ')'                             # expression
    | expr op=('/' | '*') expr                 # muldiv
    | expr op=('+' | '-') expr                 # addsub
    | expr COMPARISON expr                     # comparison
    | 'sum' '(' expr ')'                    # allTimeSum
    | 'sum' '(' from=shift '..' to=shift ',' expr ')'  # timeSum
    | IDENTIFIER '(' expr ')'                  # function
    | IDENTIFIER '[' shift ']'                 # timeShift
    | IDENTIFIER '[' expr  ']'                 # timeIndex
    | '(' expr ')' '[' shift ']'               # timeShiftExpr
    | '(' expr ')' '[' expr ']'               # timeIndexExpr
    ;

atom
    : NUMBER                                   # number
    | IDENTIFIER                               # identifier
    ;

// a shift is required to be either "t" or "t + ..." or "t - ..."
// Note: simply defining it as "shift: TIME ('+' | '-') expr" won't work
//       because the minus sign will not have the expected precedence:
//       "t - d + 1" would be equivalent to "t - (d + 1)"
shift: TIME shift_expr?;

// Because the shift MUST start with + or -, we need
// to differentiate it from generic "expr".
// A shift expression can only be extended to the right by a
// "right_expr" which cannot start with a + or -,
// unlike shift_expr itself.
// TODO: the grammar is still a little weird, because we
//       allow more things in the "expr" parts of those
//       shift expressions than on their left-most part
//       (port fields, nested time shifts and so on).
shift_expr
    : shift_expr op=('*' | '/') right_expr     # shiftMuldiv
    | shift_expr op=('+' | '-') right_expr     # shiftAddsub
    | op=('+' | '-') atom                      # signedAtom
    | op=('+' | '-') '(' expr ')'              # signedExpression
    ;

right_expr
    : right_expr op=('/' | '*') right_expr     # rightMuldiv
    | '(' expr ')'                             # rightExpression
    | atom                                     # rightAtom
    ;


fragment DIGIT         : [0-9] ;
fragment CHAR          : [a-zA-Z_];
fragment CHAR_OR_DIGIT : (CHAR | DIGIT);

NUMBER        : DIGIT+ ('.' DIGIT+)?;
TIME          : 't';
IDENTIFIER    : CHAR CHAR_OR_DIGIT*;
COMPARISON    : ( '=' | '>=' | '<=' );

WS: (' ' | '\t' | '\r'| '\n') -> skip;
