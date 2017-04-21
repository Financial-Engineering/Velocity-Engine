lexer grammar RazorLexer;

TEMPLATE:	'template'
	;
USING	:	'using'
	;
EXTERN	:	'extern'
	;
GLOBAL	:	'global'
	;
FUNCTION:	'function'
	;
IF	:	'if'
	;
ELSE	:	'else'
	;
END	:	'end'
	;
WHILE	:	'while'
	;
FOR	:	'for'
	;
RETURN	:	'return'
	;
BREAK	:	'break'
	;
CONTINUE:	'continue'
	;

ID	:	CHAR(LCHAR|DIGIT)*	
;

fragment LCHAR
	:	CHAR|'_'
;

fragment CHAR
	:	LC|UC
;

fragment LC
	:	'a'..'z'
	;

fragment UC
	:	'A'..'Z'
	;


STRING_LITERAL
    :  '"' ( EscapeSequence | ~('\\'|'"') )* '"' {setText(getText().substring(1, getText().length()-1));}
    ;

fragment
EscapeSequence
    :   '\\' ('b'|'t'|'n'|'f'|'r'|'\"'|'\''|'\\')
    ;

INT	:	'0'|('1'..'9' DIGIT*)
	;

FLOAT
    :   ('0'..'9')+ '.' ('0'..'9')* EXPONENT?
    |   '.' ('0'..'9')+ EXPONENT?
	;

fragment 
EXPONENT 
	:	('E'|'e')('+'|'-')? DIGIT+
	;

fragment 
DIGIT
	:	'0'..'9'
;

WS	:	('\t'|' '|NEWLINE)+ {$channel=HIDDEN;}
;

fragment 
NEWLINE
	:	'\r'|'\n'
;

COMMENT	:	'%{' (options {greedy=false;}:.)* '%}' {$channel=HIDDEN;}
;
		
LINECOMMENT
	:	'%' ~(NEWLINE)* NEWLINE {$channel=HIDDEN;}
;

EOS
	:	';'
	;

