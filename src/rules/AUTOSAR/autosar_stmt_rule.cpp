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

#include "autosar_stmt_rule.h"

namespace xsca {
namespace rule {


void AUTOSARStmtRule::CheckLambdaImplicitlyCaptured(const clang::LambdaExpr *stmt) {
  if (stmt->explicit_captures().empty()) {
    XcalIssue *issue = nullptr;
    XcalReport *report = XcalCheckerManager::GetReport();
    issue = report->ReportIssue(AUTOSAR, A5_1_2, stmt);
    std::string ref_msg = "Variables shall not be implicitly captured in a lambda expression.";
    issue->SetRefMsg(ref_msg);
  }
}

void AUTOSARStmtRule::CheckLambdaParameterList(const clang::LambdaExpr *stmt) {
  if (stmt->hasExplicitParameters()) return;
  XcalIssue *issue = nullptr;
  XcalReport *report = XcalCheckerManager::GetReport();
  issue = report->ReportIssue(AUTOSAR, A5_1_3, stmt);
  std::string ref_msg = "Parameter list (possibly empty) shall be included in every lambda expression.";
  issue->SetRefMsg(ref_msg);
}

}
}
