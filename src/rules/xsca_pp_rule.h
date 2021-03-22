/*
 * Copyright (C) 2020-2021 Xcalibyte Limited, Inc.  All Rights Reserved.
 */

//
// ====================================================================
// xsca_pp_rule.h
// ====================================================================
//
// template to integrate all decl rules for all standards
//

#include "xsca_defs.h"
#include "pp_null_handler.h"
#include "pp_list_handler.h"

// add all preprocess rules into macro below
#define ALL_PP_RULES(R)                        \
  R(GJB5369PPRule,       "GJB5369PPRule"),     \
  R(GJB8114PPRule,       "GJB8114PPRule"),     \
  R(MISRAPPRule,         "MISRAPPRule"),       \
  R(SJT11682PPRule,      "SJT11682PPRule")



// include all rules from sub directories
#include "GJB5369/GJB5369_pp_rule.h"
#include "GJB8114/GJB8114_pp_rule.h"
#include "MISRA/MISRA_pp_rule.h"
#include "SJT11682/SJT11682_pp_rule.h"

namespace xsca {

#define GetClass(CLASS, ...) CLASS
using RulePPHandler = PPListHandler<ALL_PP_RULES(GetClass)>;

}  // namespace xsca

