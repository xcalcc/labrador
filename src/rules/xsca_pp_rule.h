/*
   Copyright (C) 2021 Xcalibyte (Shenzhen) Limited.

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

     http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
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
  R(SJT11682PPRule,      "SJT11682PPRule"),    \
  R(AUTOSARPPRule,       "AUTOSARPPRule")



// include all rules from sub directories
#include "GJB5369/GJB5369_pp_rule.h"
#include "GJB8114/GJB8114_pp_rule.h"
#include "MISRA/MISRA_pp_rule.h"
#include "SJT11682/SJT11682_pp_rule.h"
#include "AUTOSAR/autosar_pp_rule.h"

namespace xsca {

#define GetClass(CLASS, ...) CLASS
using RulePPHandler = PPListHandler<ALL_PP_RULES(GetClass)>;

}  // namespace xsca

