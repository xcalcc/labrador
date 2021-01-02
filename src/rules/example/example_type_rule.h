/*
 * Copyright (C) 2020-2021 Xcalibyte Limited, Inc.  All Rights Reserved.
 */

//
// ====================================================================
// example_type_rule.h
// ====================================================================
//
// this is an example to describe how to write a type match based
// checker for type related rules.
//

#include "xsca_defs.h"
#include "type_null_handler.h"
#include "type_list_handler.h"

// CHANGE HERE: add all type rules into macro below
#define ALL_TYPE_RULES(R) \
  R(CheckTypeExample, "CheckTypeExample")

namespace xsca {
namespace example {

// CHANGE HERE: include type rules file below
#include "check_type_example.inc"

#define GetClass(CLASS, ...) CLASS
using ExampleTypeHandler = TypeListHandler<ALL_TYPE_RULES(GetClass)>;

}  // namespace example
}  // namespace xsca
