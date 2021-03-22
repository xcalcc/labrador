/*
 * Copyright (C) 2020-2021 Xcalibyte Limited, Inc.  All Rights Reserved.
 */

//
// ====================================================================
// xsca_stmt_rule.h
// ====================================================================
//
// template to integrate all stmt rules for all standards
//

#include "xsca_defs.h"
#include "stmt_null_handler.h"
#include "stmt_list_handler.h"

// add all stmt/expr rules into macro below
#define ALL_STMT_RULES(R)                      \
  R(GJB5369StmtRule,   "GJB5369StmtRule"),     \
  R(GJB8114StmtRule,   "GJB8114StmtRule"),     \
  R(MISRAStmtRule,     "MISRAStmtRule"),       \
  R(SJT11682StmtRule,  "SJT11682StmtRule")



// include all rules from sub directories
#include "GJB5369/GJB5369_stmt_rule.h"
#include "GJB8114/GJB8114_stmt_rule.h"
#include "MISRA/MISRA_stmt_rule.h"
#include "SJT11682/SJT11682_stmt_rule.h"

namespace xsca {

#define GetClass(CLASS, ...) CLASS
using RuleStmtHandler = StmtListHandler<ALL_STMT_RULES(GetClass)>;

}  // namespace xsca

