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

