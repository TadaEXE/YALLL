#pragma once

#include "YALLLParser.h"
#include "typesafety.h"
namespace typesafety {

static std::map<size_t, size_t> type_size = {
    {YALLLParser::I8_T, 8},   {YALLLParser::I16_T, 16},
    {YALLLParser::I32_T, 32}, {YALLLParser::I64_T, 64},
    {YALLLParser::U8_T, 8},   {YALLLParser::U16_T, 16},
    {YALLLParser::U32_T, 32}, {YALLLParser::U64_T, 64},
    {YALLLParser::BOOL_T, 1}, {YALLLParser::D32_T, 32},
    {YALLLParser::D64_T, 64}, {YALLLParser::TBD_T, 64},
    {INTAUTO_T_ID, 32},       {DECAUTO_T_ID, 32},
};
}
