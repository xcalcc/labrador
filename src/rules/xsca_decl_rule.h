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
  R(SJT11682DeclRule,  "SJT11682DeclRule"),    \
  R(AUTOSARDeclRule,   "AUTOSARDeclRule")

// include all rules from sub directories
#include "common/identifier_builder.h"
#include "GJB5369/GJB5369_decl_rule.h"
#include "GJB8114/GJB8114_decl_rule.h"
#include "MISRA/MISRA_decl_rule.h"
#include "SJT11682/SJT11682_decl_rule.h"
#include "AUTOSAR/autosar_decl_rule.h"

namespace xsca {
using namespace xsca::rule;

#define GetClass(CLASS, ...) CLASS
using RuleDeclHandler = DeclListHandler<ALL_DECL_RULES(GetClass)>;
}  // namespace xsca


