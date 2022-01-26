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
#include "MISRA_enum.inc"
#include "xsca_checker_manager.h"
#include <clang/Basic/DiagnosticSema.h>

namespace xsca {
namespace rule {


void MISRADiagnostic::HandleDiagnostic(clang::DiagnosticsEngine::Level diagnosticLevel,
                                       const clang::Diagnostic &diagnosticInfo) {
  clang::SmallString<100> diagnosticMessage;
  diagnosticInfo.FormatDiagnostic(diagnosticMessage);
  clang::SourceLocation location = diagnosticInfo.getLocation();
  auto msg = diagnosticMessage.c_str();
  auto src_mgr = XcalCheckerManager::GetSourceManager();

#ifdef Is_True_On
  printf("%u\n", diagnosticInfo.getID());
#endif
  switch (diagnosticInfo.getID()) {
    case 1067:  // MISRA 4.2
      AddIssue(M_R_4_2, "Trigraphs should not be used", location);
      break;
    case 2594:  // MISRA 11.4
      AddIssue(M_R_11_4, msg, location);
      break;
    case 2937:  // MISRA-2008 15-3-7
      AddIssue(M_R_15_3_7, msg, location);
      break;
    case 4030:  // MISRA-2008 5-2-2
    case 5942:
      AddIssue(M_R_5_2_2, msg, location);
      break;
    case 4518:
    case 5705:  // MISRA 9.4
      AddIssue(M_R_9_4, msg, location);
      break;
    case clang::diag::ext_designated_init_array: {
      break;
    }
    case 5418:  // MISRA 13.4
      AddIssue(M_R_13_4, msg, location);
      break;
    case 5382:  // MISRA 16.7
      AddIssue(M_R_16_7, msg, location);
      break;
    default:
      break;
  }
}

void MISRADiagnostic::AddIssue(const char *rule, const std::string &ref_msg, clang::SourceLocation location) {
  XcalIssue *issue;
  XcalReport *report = XcalCheckerManager::GetReport();
  issue = report->ReportIssue(MISRA, rule, location);
  issue->SetRefMsg(ref_msg);
}

}
}
