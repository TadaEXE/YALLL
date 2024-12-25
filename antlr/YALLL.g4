grammar YALLL;


// General:
program: (interface | class)* entry_point? (interface | class)* EOF;

interface: INTERFACE_KW NAME interface_block;

class: CLASS_KW NAME (size)? (COLON_SYM NAME)? class_block;

entry_point: FUNCTION_KW LPAREN_SYM RPAREN_SYM block;

statement:
    expression
  | control_structure;

expression:
(
    declaration
  | definition
  | assignment
  | operation
  | BREAK_KW
  | CONTINUE_KW
  | RETURN_KW operation?
) SEMICOLON_SYM;

assignment: NAME EQUAL_SYM operation;


// Decs:
declaration:
    function_dec
  | var_dec;

function_dec: FUNCTION_KW ONERR_KW? NAME parameter_list COLON_SYM type;

var_dec: type NAME;


// Defs:
definition:
    function_def
  | error_def
  | var_def
  | lazy_var_def;

function_def: FUNCTION_KW NOERR_KW? NAME parameter_list COLON_SYM type block;

error_def: LBRACK_SYM NAME COMMA_SYM STRING RBRACK_SYM;

var_def: type NAME EQUAL_SYM operation;

lazy_var_def: LAZY_KW var_def;


parameter_list: LPAREN_SYM (type NAME (COLON_SYM type NAME)*)? RPAREN_SYM;


// Blocks:
block: LCURL_SYM statement* RCURL_SYM;

interface_block: LCURL_SYM function_dec* RCURL_SYM;

class_block: ((PUBLIC_KW pp_block) | (PRIVATE_KW pp_block) | (ERROR_KW error_block))*;

pp_block: LCURL_SYM (function_def | declaration)* RCURL_SYM;

error_block: LCURL_SYM error_def* RCURL_SYM;

onerr_block: LCURL_SYM switch RCURL_SYM;

// Control structures:
control_structure:
    loop
  | switch
  | if_else;


  // Switch
  switch: (NAME COLON_SYM statement*)* DEFAULT_KW COLON_SYM statement*;

  // Loops:
  loop:
      while_loop
    | for_loop
    | foreach_loop;

  while_loop: LOOP_KW LPAREN_SYM operation RPAREN_SYM loop_body;

  for_loop: LOOP_KW LPAREN_SYM (definition | assignment)? SEMICOLON_SYM operation? SEMICOLON_SYM operation? RPAREN_SYM loop_body;

  foreach_loop: LOOP_KW LPAREN_SYM declaration COLON_SYM operation RPAREN_SYM loop_body;

  loop_body: block;


  // If_else:
  if_else: if else_if* else?;

  if: IF_KW LPAREN_SYM operation RBRACK_SYM block;

  else_if: ELSE_KW LPAREN_SYM operation RBRACK_SYM block;

  else: ELSE_KW block;


// Precedence climb:
operation: reterr_op;

reterr_op: RETERR_KW? iserr_op;

iserr_op: ISERR_KW? onerr_op;

onerr_op: bool_or_op (ONERR_KW onerr_block);

bool_or_op: bool_and_op (OR_SYM bool_and_op)*;

bool_and_op: compare_op (AND_SYM compare_op)*;

compare_op: addition_op (compare_sym addition_op)*;

addition_op: multiplication_op ((PLUS_SYM | MINSU_SYM) multiplication_op)*;

multiplication_op: unary_op ((MUL_SYM | DIV_SYM | MOD_SYM) unary_op)*;

unary_op: (NOT_SYM | MINSU_SYM)? primary_op;

primary_op:
    LPAREN_SYM operation RPAREN_SYM
  | function_call
  | terminal_op;

terminal_op:
    NAME
  | INTEGER
  | DECIMAL
  | STRING
  | NULL_VALUE
  | BOOL_True
  | BOOL_FALSE;


// Misc:
size: LBRACK_SYM INTEGER RBRACK_SYM;

BOOL_True: 'true';
BOOL_FALSE: 'false';
NULL_VALUE: 'null';

function_call: NAME LPAREN_SYM argument_list RPAREN_SYM;

argument_list: (operation (COMMA_SYM operation)*)?;


// Keywords:
INTERFACE_KW: 'interface';
CLASS_KW: 'class';
FUNCTION_KW: 'function';
RETERR_KW: 'reterr';
ONERR_KW: 'onerr';
ISERR_KW: 'iserr';
NOERR_KW: 'noerr';
ERROR_KW: 'error';
LOOP_KW: 'loop';
IF_KW: 'if';
ELSE_KW: 'else';
NEW_KW: 'new';
DEFAULT_KW: 'default';
BREAK_KW: 'break';
CONTINUE_KW: 'continue';
RETURN_KW: 'return';
PUBLIC_KW: 'public';
PRIVATE_KW: 'private';
LAZY_KW: 'lazy';


// Types:
type:
    null_t
  | mut_t
  | base_t
  | array_type;
 
array_type: base_t size;
null_t: QUESETIONMARK_SYM base_t;
mut_t: NOT_SYM base_t;
  // Base types:
  base_t:
      I64_T
    | I32_T
    | I16_T
    | I8_T
    | U64_T
    | U32_T
    | U16_T
    | U8_T
    | D64_T
    | D32_T
    | STR_T
    | BOOL_T;

I64_T: 'i64';
I32_T: 'i32';
I16_T: 'i16';
I8_T: 'i8';
U64_T: 'u64';
U32_T: 'u32';
U16_T: 'u16';
U8_T: 'u8';
D64_T: 'd64';
D32_T: 'd32';
STR_T: 'str';
BOOL_T: 'bool';


// Regex types:
INTEGER: [0-9]+;
DECIMAL: [0-9]* '.' [0-9]+;
NAME: [a-zA-Z_][a-zA-Z0-9_]*;
STRING: '"' ( ~["])* '"';


// Symbols:
COLON_SYM: ':';
SEMICOLON_SYM: ';';
LPAREN_SYM: '(';
RPAREN_SYM: ')';
LBRACK_SYM: '[';
RBRACK_SYM: ']';
LCURL_SYM: '{';
RCURL_SYM: '}';
OR_SYM: '||';
AND_SYM: '&&';
PLUS_SYM: '+';
MINSU_SYM: '-';
MUL_SYM: '*';
DIV_SYM: '/';
MOD_SYM: '%';
NOT_SYM: '!';
COMMA_SYM: ',';
DOT_SYM: '.';
QUESETIONMARK_SYM: '?';
EQUAL_SYM: '=';

compare_sym:
    GREATER_SYM
  | LESS_SYM
  | GREATER_EQUAL_SYM
  | LESS_EQUAL_SYM
  | EQUAL_EQUAL_SYM
  | NOT_EQUAL_SYM;

GREATER_SYM: '>';
LESS_SYM: '<';
GREATER_EQUAL_SYM: '>=';
LESS_EQUAL_SYM: '<=';
EQUAL_EQUAL_SYM: '==';
NOT_EQUAL_SYM: '!=';


// Misc:
WS: [ \t\r\n\u000C]+ -> skip;
COMMENT: '/*' .*? '*/' -> skip;
LINE_COMMENT: '//' ~[\r\n]* -> skip;

