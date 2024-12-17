grammar YALLL;

program: (interface | class)* EOF;

interface: _interface_kw _name interface_block;

class: _class_kw _name (size)? (_colon_sym _name)? class_block;

statement:
    (expression | _semicolon_sym)
  | function_def
  | var_def
  | loop;


// Defs:
function_def: _function_kw _noerr_kw? _name argument_list _colon_sym type (block | _semicolon_sym);

error_def: _lbrack_sym _name _comma_sym _string _rbrack_sym;

var_def: type _name (_equal_sym expression)? _semicolon_sym;


// Blocks:
block: _lcurl_sym statement* _rcurl_sym;

interface_block: _lcurl_sym function_dec* _rcurl_sym;

class_block: ((_public_kw pp_block) | (_private_kw pp_block) | (_error_kw error_block))*

pp_block: _lcurl_sym (function_def | declaration)* _rcurl_sym;

error_block: _lcurl_sym error_def* _rcurl_sym;



// Loops:
loop: _loop_kw _lparen_sym
  (
      while_loop_type
    | for_loop_type
    | foreach_loop_type
  )
  _rparen_sym block;

while_loop_type: expression;

for_loop_type: assignment _semicolon_sym expression _semicolon_sym expression;

foreach_loop_type: declaration _colon_sym expression;

// If_else:
if_else: if else_if* else?;

if: _if_kw _lparen_sym expression _rbrack_sym block;

else_if: _else_kw _lparen_sym expression _rbrack_sym block;

else: _else_kw block;


// Precedence climb:
expression: reterr_exp;

reterr_exp: _reterr_kw? iserr_exp;

iserr_exp: _iserr_kw? onerr_exp;

onerr_exp: bool_or_exp (_onerr_kw onerr_block)

bool_or_exp: bool_and_exp (_or_sym bool_and_exp)*;

bool_and_exp: compare_exp (_and_sym compare_exp)*;

compare_exp: addition_exp (compare_sym addition_exp)*;

addition_exp: multiplication_exp ((_plus_sym | _minus_sym) multiplication_exp)*;

multiplication_exp: unary_exp ((_mul_sym | _div_sym | _mod_sym) unary_exp)*;

unary_exp: (_not_sym | _minus_sym)? primary_exp;

primary_exp:
  _lparen_sym expression _rparen_sym |
  terminal_exp;

terminal_exp:
    _name
  | _integer
  | _decimal
  | _string
  | _null_value
  | _bool_true
  | _bool_false;


size: _lbrack_sym _integer _rbrack_sym;

_bool_true = 'true';
_bool_false = 'false';
_null_value = 'null';

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
