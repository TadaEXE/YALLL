grammar YALLL;


// General:
program: (interface | class)* EOF;

interface: _interface_kw _name interface_block;

class: _class_kw _name (size)? (_colon_sym _name)? class_block;

statement:
    expression
  | control_structure;

expression:
(
    declaration
  | definition
  | assignment
  | operation
  | _break_kw
  | _continue_kw
  | _return_kw operation?
) _semicolon_sym;

assignment: _name _equal_sym operation;


// Decs:
declaration:
    function_dec
  | var_dec;

function_dec: _function_kw _onerr_kw? _name argument_list _colon_sym type;

var_dec: type _name;


// Defs:
definition:
    function_def
  | error_def
  | var_def;

function_def: _function_kw _noerr_kw? _name argument_list _colon_sym type block;

error_def: _lbrack_sym _name _comma_sym _string _rbrack_sym;

var_def: type _name _equal_sym operation;


// Blocks:
block: _lcurl_sym statement* _rcurl_sym;

interface_block: _lcurl_sym function_dec* _rcurl_sym;

class_block: ((_public_kw pp_block) | (_private_kw pp_block) | (_error_kw error_block))*

pp_block: _lcurl_sym (function_def | declaration)* _rcurl_sym;

error_block: _lcurl_sym error_def* _rcurl_sym;

onerr_block: _lcurl_sym switch _rcurl_sym;

// Control structures:
control_structure:
    loop
  | switch
  | if_else;


  // Switch
  switch: (_name _colon_sym statement*)* _default_kw _colon_sym statement*;

  // Loops:
  loop:
      while_loop
    | for_loop
    | foreach_loop;

  while_loop: _loop_kw _lparen_sym operation _rparen_sym loop_body;

  for_loop: _loop_kw _lparen_sym (definition | assignment)? _semicolon_sym operation? _semicolon_sym operation? _rparen_sym loop_body;

  foreach_loop: _loop_kw _lparen_sym declaration _colon_sym operation _rparen_sym loop_body;

  loop_body: block;


  // If_else:
  if_else: if else_if* else?;

  if: _if_kw _lparen_sym operation _rbrack_sym block;

  else_if: _else_kw _lparen_sym operation _rbrack_sym block;

  else: _else_kw block;


// Precedence climb:
operation: reterr_op;

reterr_op: _reterr_kw? iserr_op;

iserr_op: _iserr_kw? onerr_op;

onerr_op: bool_or_op (_onerr_kw onerr_block)

bool_or_op: bool_and_op (_or_sym bool_and_op)*;

bool_and_op: compare_op (_and_sym compare_op)*;

compare_op: addition_op (compare_sym addition_op)*;

addition_op: multiplication_op ((_plus_sym | _minus_sym) multiplication_op)*;

multiplication_op: unary_op ((_mul_sym | _div_sym | _mod_sym) unary_op)*;

unary_op: (_not_sym | _minus_sym)? primary_op;

primary_op:
    _lparen_sym operation _rparen_sym
  | function_call
  | terminal_op;

terminal_op:
    _name
  | _integer
  | _decimal
  | _string
  | _null_value
  | _bool_true
  | _bool_false;


// Misc:
size: _lbrack_sym _integer _rbrack_sym;

_bool_true = 'true';
_bool_false = 'false';
_null_value = 'null';

function_call: _name _lparen_sym parameter_list _rparen_sym;

parameter_list: (operation (_comma_sym operation)*)?;


// Keywords:
_interface_kw: 'interface';
_class_kw: 'class';
_function_kw: 'function';
_reterr_kw: 'reterr';
_onerr_kw: 'onerr';
_iserr_kw: 'iserr';
_noerr_kw: 'noerr';
_error_kw: 'error';
_loop_kw: 'loop';
_if_kw: 'if';
_else_kw: 'else';
_new_kw: 'new';
_default_kw: 'default';
_break_kw: 'break';
_continue_kw: 'continue';
_return_kw: 'return';


// Types:
type:
    null_t
  | mut_t
  | base_t
  | array_type;
 
array_type: base_t size;
null_t: _questionmark_sym base_t;
mut_t: _not_sym base_t;
  // Base types:
  base_t:
      _i64_t
    | _i32_t
    | _i16_t
    | _i8_t
    | _u64_t
    | _u32_t
    | _u16_t
    | _u8_t
    | _d64_t
    | _d32_t
    | _str_t
    | _bool_t;

_i64_t: 'i64';
_i32_t: 'i32';
_i16_t: 'i16';
_i8_t: 'i8';
_u64_t: 'u64';
_u32_t: 'u32';
_u16_t: 'u16';
_u8_t: 'u8';
_d64_t: 'd64';
_d32_t: 'd32';
_str_t: 'str';
_bool_t: 'bool'


// Regex types:
_integer: [0-9]+;
_decimal: [0-9]* '.' [0-9]+;
_name: [a-zA-Z_][a-zA-Z0-9_]*;
_string: '"' ( ~["])* '"';


// Symbols:
_colon_sym: ':';
_semicolon_sym: ';';
_lparen_sym: '(';
_rparen_sym: ')';
_lbrack_sym: '[';
_rbrack_sym: ']';
_lcurl_sym: '{';
_rcurl_sym: '}';
_or_sym: '||';
_and_sym: '&&';
_plus_sym: '+';
_minus_sym: '-';
_mul_sym: '*';
_div_sym: '/';
_mod_sym: '%';
_not_sym: '!';
_comma_sym: ',';
_dot_sym: '.';


// Misc:
WS: [ \t\r\n\u000C]+ -> skip;
COMMENT: '/*' .*? '*/' -> skip;
LINE_COMMENT: '//' ~[\r\n]* -> skip;
