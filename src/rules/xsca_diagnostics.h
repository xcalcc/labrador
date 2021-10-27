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

#ifndef LABRADOR_XSCA_DIAGNOSTICS_H
#define LABRADOR_XSCA_DIAGNOSTICS_H

#include "xsca_defs.h"
#include "diagnostic_dispatcher.h"

// add all decl rules into macro below
#define ALL_Diagnostic(R)                      \
  R(GJB5369Diagnostic,   "GJB5369Diagnostic"), \
  R(GJB8114Diagnostic,   "GJB8114Diagnostic"), \
  R(MISRADiagnostic,     "MISRADiagnostic")
//  R(SJT11682DeclRule,  "SJT11682DeclRule")

#include "GJB5369/diagnostic.h"
#include "GJB8114/diagnostic.h"
#include "MISRA/diagnostic.h"

namespace xsca {
using namespace rule;

#define GetClass(CLASS, ...) CLASS

using RuleDiagnosticDispatcher = DiagnosticDispatcher<ALL_Diagnostic(GetClass)>;
using RuleDiagnosticManager = DiagnosticManager<RuleDiagnosticDispatcher>;
}

#endif //LABRADOR_XSCA_DIAGNOSTICS_H
