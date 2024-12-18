grammar YALLL;


// General:
program: (interface | class)* EOF;

interface: terminal_interface_kw terminal_name interface_block;

class: terminal_class_kw terminal_name (size)? (terminal_colon_sym terminal_name)? class_block;

statement:
    expression
  | control_structure;

expression:
(
    declaration
  | definition
  | assignment
  | operation
  | terminal_break_kw
  | terminal_continue_kw
  | terminal_return_kw operation?
) terminal_semicolon_sym;

assignment: terminal_name terminal_equal_sym operation;


// Decs:
declaration:
    function_dec
  | var_dec;

function_dec: terminal_function_kw terminal_onerr_kw? terminal_name argument_list terminal_colon_sym type;

var_dec: type terminal_name;


// Defs:
definition:
    function_def
  | error_def
  | var_def;

function_def: terminal_function_kw terminal_noerr_kw? terminal_name argument_list terminal_colon_sym type block;

error_def: terminal_lbrack_sym terminal_name terminal_comma_sym terminal_string terminal_rbrack_sym;

var_def: type terminal_name terminal_equal_sym operation;


// Blocks:
block: terminal_lcurl_sym statement* terminal_rcurl_sym;

interface_block: terminal_lcurl_sym function_dec* terminal_rcurl_sym;

class_block: ((terminal_public_kw pp_block) | (terminal_private_kw pp_block) | (terminal_error_kw error_block))*;

pp_block: terminal_lcurl_sym (function_def | declaration)* terminal_rcurl_sym;

error_block: terminal_lcurl_sym error_def* terminal_rcurl_sym;

onerr_block: terminal_lcurl_sym switch terminal_rcurl_sym;

// Control structures:
control_structure:
    loop
  | switch
  | if_else;


  // Switch
  switch: (terminal_name terminal_colon_sym statement*)* terminal_default_kw terminal_colon_sym statement*;

  // Loops:
  loop:
      while_loop
    | for_loop
    | foreach_loop;

  while_loop: terminal_loop_kw terminal_lparen_sym operation terminal_rparen_sym loop_body;

  for_loop: terminal_loop_kw terminal_lparen_sym (definition | assignment)? terminal_semicolon_sym operation? terminal_semicolon_sym operation? terminal_rparen_sym loop_body;

  foreach_loop: terminal_loop_kw terminal_lparen_sym declaration terminal_colon_sym operation terminal_rparen_sym loop_body;

  loop_body: block;


  // If_else:
  if_else: if else_if* else?;

  if: terminal_if_kw terminal_lparen_sym operation terminal_rbrack_sym block;

  else_if: terminal_else_kw terminal_lparen_sym operation terminal_rbrack_sym block;

  else: terminal_else_kw block;


// Precedence climb:
operation: reterr_op;

reterr_op: terminal_reterr_kw? iserr_op;

iserr_op: terminal_iserr_kw? onerr_op;

onerr_op: bool_or_op (terminal_onerr_kw onerr_block);

bool_or_op: bool_and_op (terminal_or_sym bool_and_op)*;

bool_and_op: compare_op (terminal_and_sym compare_op)*;

compare_op: addition_op (compare_sym addition_op)*;

addition_op: multiplication_op ((terminal_plus_sym | terminal_minus_sym) multiplication_op)*;

multiplication_op: unary_op ((terminal_mul_sym | terminal_div_sym | terminal_mod_sym) unary_op)*;

unary_op: (terminal_not_sym | terminal_minus_sym)? primary_op;

primary_op:
    terminal_lparen_sym operation terminal_rparen_sym
  | function_call
  | terminal_op;

terminal_op:
    terminal_name
  | terminal_integer
  | terminal_decimal
  | terminal_string
  | terminal_null_value
  | terminal_bool_true
  | terminal_bool_false;


// Misc:
size: terminal_lbrack_sym terminal_integer terminal_rbrack_sym;

terminal_bool_true: 'true';
terminal_bool_false: 'false';
terminal_null_value: 'null';

function_call: terminal_name terminal_lparen_sym parameter_list terminal_rparen_sym;

parameter_list: (operation (terminal_comma_sym operation)*)?;


// Keywords:
terminal_interface_kw: 'interface';
terminal_class_kw: 'class';
terminal_function_kw: 'function';
terminal_reterr_kw: 'reterr';
terminal_onerr_kw: 'onerr';
terminal_iserr_kw: 'iserr';
terminal_noerr_kw: 'noerr';
terminal_error_kw: 'error';
terminal_loop_kw: 'loop';
terminal_if_kw: 'if';
terminal_else_kw: 'else';
terminal_new_kw: 'new';
terminal_default_kw: 'default';
terminal_break_kw: 'break';
terminal_continue_kw: 'continue';
terminal_return_kw: 'return';


// Types:
type:
    null_t
  | mut_t
  | base_t
  | array_type;
 
array_type: base_t size;
null_t: terminal_questionmark_sym base_t;
mut_t: terminal_not_sym base_t;
  // Base types:
  base_t:
      terminal_i64_t
    | terminal_i32_t
    | terminal_i16_t
    | terminal_i8_t
    | terminal_u64_t
    | terminal_u32_t
    | terminal_u16_t
    | terminal_u8_t
    | terminal_d64_t
    | terminal_d32_t
    | terminal_str_t
    | terminal_bool_t;

terminal_i64_t: 'i64';
terminal_i32_t: 'i32';
terminal_i16_t: 'i16';
terminal_i8_t: 'i8';
terminal_u64_t: 'u64';
terminal_u32_t: 'u32';
terminal_u16_t: 'u16';
terminal_u8_t: 'u8';
terminal_d64_t: 'd64';
terminal_d32_t: 'd32';
terminal_str_t: 'str';
terminal_bool_t: 'bool';


// Regex types:
terminal_integer: [0-9]+;
terminal_decimal: [0-9]* '.' [0-9]+;
terminal_name: [a-zA-Z_][a-zA-Z0-9_]*;
terminal_string: '"' ( ~["])* '"';


// Symbols:
terminal_colon_sym: ':';
terminal_semicolon_sym: ';';
terminal_lparen_sym: '(';
terminal_rparen_sym: ')';
terminal_lbrack_sym: '[';
terminal_rbrack_sym: ']';
terminal_lcurl_sym: '{';
terminal_rcurl_sym: '}';
terminal_or_sym: '||';
terminal_and_sym: '&&';
terminal_plus_sym: '+';
terminal_minus_sym: '-';
terminal_mul_sym: '*';
terminal_div_sym: '/';
terminal_mod_sym: '%';
terminal_not_sym: '!';
terminal_comma_sym: ',';
terminal_dot_sym: '.';


// Misc:
WS: [ \t\r\n\u000C]+ -> skip;
COMMENT: '/*' .*? '*/' -> skip;
LINE_COMMENT: '//' ~[\r\n]* -> skip;

