/*
 * Copyright (C) 2020-2021 Xcalibyte Limited, Inc.  All Rights Reserved.
 */

//
// ====================================================================
// xsca_decl_rule.h
// ====================================================================
//
// template to integrate all decl rules for all standards.
//

#include "xsca_defs.h"
#include "decl_null_handler.h"
#include "decl_list_handler.h"

// add all decl rules into macro below
#define ALL_DECL_RULES(R)                      \
  R(IdentifierBuilder, "IdentifierBuilder"),   \
  R(GJB5369DeclRule,   "GJB5369DeclRule"),     \
  R(GJB8114DeclRule,   "GJB8114DeclRule"),     \
  R(MISRADeclRule,     "MISRADeclRule"),       \
  R(SJT11682DeclRule,  "SJT11682DeclRule")


// include all rules from sub directories
#include "common/identifier_builder.h"
#include "GJB5369/GJB5369_decl_rule.h"
#include "GJB8114/GJB8114_decl_rule.h"
#include "MISRA/MISRA_decl_rule.h"
#include "SJT11682/SJT11682_decl_rule.h"

namespace xsca {
using namespace xsca::rule;

#define GetClass(CLASS, ...) CLASS
using RuleDeclHandler = DeclListHandler<ALL_DECL_RULES(GetClass)>;
}  // namespace xsca


