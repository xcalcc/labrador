/*
 * Copyright (C) 2020-2021 Xcalibyte Limited, Inc.  All Rights Reserved.
 */

//
// ====================================================================
// example_pp_rule.h
// ====================================================================
//
// this is an example to describe how to write a preprocessor callbacks
// based checker for lexer related rules.
//

#include "xsca_defs.h"
#include "pp_null_handler.h"
#include "pp_list_handler.h"

// CHANGE HERE: add all preprocess rules into macro below
#define ALL_PP_RULES(R) \
  R(CheckPPExample, "CheckPPExample")

// force class/types defined in xcsa::example
namespace xsca {
namespace example {

// CHANGE HERE: add include file below
#include "check_pp_example.inc"

#define GetClass(CLASS, ...) CLASS
using ExamplePPHandler = PPListHandler<ALL_PP_RULES(GetClass)>;

}  // namespace example
}  // namespace xsca
