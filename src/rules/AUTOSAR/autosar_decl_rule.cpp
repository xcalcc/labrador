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


#include "autosar_enum.inc"
#include "autosar_decl_rule.h"

namespace xsca {
namespace rule {

void AUTOSARDeclRule::CheckEnumUnderlyingType(const clang::EnumDecl *decl) {
  if (decl->getIntegerTypeSourceInfo()) return;
  XcalIssue *issue = nullptr;
  XcalReport *report = XcalCheckerManager::GetReport();
  issue = report->ReportIssue(AUTOSAR, A7_2_2, decl);
  std::string ref_msg = "Enumeration underlying base type shall be explicitly defined.";
  issue->SetRefMsg(ref_msg);
}

void AUTOSARDeclRule::CheckEnumScoped(const clang::EnumDecl *decl) {
  if (decl->isScoped()) return;
  XcalIssue *issue = nullptr;
  XcalReport *report = XcalCheckerManager::GetReport();
  issue = report->ReportIssue(AUTOSAR, A7_2_3, decl);
  std::string ref_msg = "Enumerations shall be declared as scoped enum classes.";
  issue->SetRefMsg(ref_msg);
}

void AUTOSARDeclRule::CheckUsingDirective(const clang::UsingDirectiveDecl *decl) {
  XcalIssue *issue = nullptr;
  XcalReport *report = XcalCheckerManager::GetReport();
  issue = report->ReportIssue(AUTOSAR, A7_3_4, decl);
  std::string ref_msg = "Using-directives shall not be used.";
  issue->SetRefMsg(ref_msg);
}

}
}