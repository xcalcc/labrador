/*
 * Copyright (C) 2020-2021 Xcalibyte Limited, Inc.  All Rights Reserved.
 */

//
// ====================================================================
// example_decl_rule.h
// ====================================================================
//
// this is an example to describe how to write a clang decl checker
//

#include "xsca_defs.h"
#include "decl_null_handler.h"
#include "decl_list_handler.h"

// add all decl rules into macro below
#define ALL_DECL_RULES(R) \
  R(CheckDeclExample, "CheckDeclExample")

// force below class/types defined in xsca::example
namespace xsca {
namespace example {
// decl based rules
#include "check_decl_example.inc"


#define GetClass(CLASS, ...) CLASS
using ExampleDeclHandler = DeclListHandler<ALL_DECL_RULES(GetClass)>;

}  // namespace example
}  // namespace xsca

