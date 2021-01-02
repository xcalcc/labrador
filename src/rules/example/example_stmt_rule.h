/*
 * Copyright (C) 2020-2021 Xcalibyte Limited, Inc.  All Rights Reserved.
 */

//
// ====================================================================
// example_stmt_rule.h
// ====================================================================
//
// this is an example to describe how to write a stmt match based
// checker for statement/expression related rules.
//

#include "xsca_defs.h"
#include "stmt_null_handler.h"
#include "stmt_list_handler.h"

// CHANGE HERE: add all stmt/expr rules into macro below
#define ALL_STMT_RULES(R) \
  R(CheckStmtExample, "CheckStmtExample")

namespace xsca {
namespace example {

// CHANGE HERE: include rules file below
#include "check_stmt_example.inc"

#define GetClass(CLASS, ...) CLASS
using ExampleStmtHandler = StmtListHandler<ALL_STMT_RULES(GetClass)>;

}  // namespace example
}  // namespace xsca
