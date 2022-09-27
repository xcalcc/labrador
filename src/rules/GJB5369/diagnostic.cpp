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

#include "diagnostic.h"
#include "xsca_report.h"
#include "GJB5369_enum.inc"
#include "xsca_checker_manager.h"
#include <clang/Basic/DiagnosticSema.h>

namespace xsca {
namespace rule {

void GJB5369Diagnostic::HandleDiagnostic(clang::DiagnosticsEngine::Level diagnosticLevel,
                                         const clang::Diagnostic &diagnosticInfo) {
  clang::SmallString<100> diagnosticMessage;
  diagnosticInfo.FormatDiagnostic(diagnosticMessage);
  clang::SourceLocation location = diagnosticInfo.getLocation();

  switch (diagnosticInfo.getID()) {
    case clang::diag::err_array_size_non_int: {
      AddIssue(G4_15_1_6, "size of array has non-integer type 'float'", location);
      break;
    }
    case clang::diag::note_ovl_candidate_arity: {
      AddIssue(G4_7_1_7, "real and formal parameters' number should be the same", location);
      break;
    }
    case clang::diag::err_bad_cxx_cast_generic: {
      // TODO: 4.7.1.7
//      AddIssue(G4_7_1_7, "")
    }
    case clang::diag::err_typecheck_convert_int_pointer: {
      // TODO: int * p = (float) a;
      AddIssue(G4_12_2_1, "assign to pointer with other types is dangerous", location);
    }
    default: {
      printf("%d\n",diagnosticInfo.getID());
      break;
    }
  }
}

void GJB5369Diagnostic::AddIssue(const char *rule,
                                 const std::string &ref_msg,
                                 clang::SourceLocation location) {
  XcalIssue *issue;
  XcalReport *report = XcalCheckerManager::GetReport();
  issue = report->ReportIssue(GJB5369, rule, location);
  issue->SetRefMsg(ref_msg);
}

}
}
