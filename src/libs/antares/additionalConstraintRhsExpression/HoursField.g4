grammar HoursField;

hoursField : group (COMMA group)* EOF ;
group      : LBRACK WS* hour (COMMA WS* hour)* WS* RBRACK ;
hour       : INT ;

LBRACK     : '[' ;
RBRACK     : ']' ;
COMMA      : ',' ;
INT        : [0-9]+ ;
WS : [\u0020\u0009\u000A\u000B\u000C\u000D]+ -> skip ;

