/*
 * Copyright (C) 2020-2021 Xcalibyte Limited, Inc.  All Rights Reserved.
 */

//
// ====================================================================
// xsca_type_rule.h
// ====================================================================
//
// template to integrate all type rules for all standards.
//

#include "xsca_defs.h"
#include "type_null_handler.h"
#include "type_list_handler.h"

// add all type rules into macro below
#define ALL_TYPE_RULES(R)                      \
  R(GJB5369TypeRule,   "GJB5369TypeRule"),     \
  R(GJB8114TypeRule,   "GJB8114TypeRule"),     \
  R(MISRATypeRule,     "MISRATypeRule"),       \
  R(SJT11682TypeRule,  "SJT11682TypeRule")


namespace xsca {
namespace rule {

// include all rules from sub directories
#include "GJB5369/GJB5369_type_rule.h"
#include "GJB8114/GJB8114_type_rule.h"
#include "MISRA/MISRA_type_rule.h"
#include "SJT11682/SJT11682_type_rule.h"

#define GetClass(CLASS, ...) CLASS
using RuleTypeHandler = TypeListHandler<ALL_TYPE_RULES(GetClass)>;

}  // namespace rule
}  // namespace xsca

