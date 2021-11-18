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
//#include <clang/Basic/FileManager.h>

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

void AUTOSARDeclRule::CheckUnnamedNamespaceInHeaderFile(const clang::NamespaceDecl *decl) {
  if (!decl->isAnonymousNamespace()) return;
  auto loc = decl->getLocation();
  auto src_mgr = XcalCheckerManager::GetSourceManager();
  auto filename = src_mgr->getFilename(loc);
  if (filename.find(".h") != std::string::npos) {
    XcalIssue *issue = nullptr;
    XcalReport *report = XcalCheckerManager::GetReport();
    issue = report->ReportIssue(AUTOSAR, A7_3_3, decl);
    std::string ref_msg = "There shall be no unnamed namespaces in header files.";
    issue->SetRefMsg(ref_msg);
  }
}

void AUTOSARDeclRule::CheckUsingDirective(const clang::UsingDirectiveDecl *decl) {
  XcalIssue *issue = nullptr;
  XcalReport *report = XcalCheckerManager::GetReport();
  issue = report->ReportIssue(AUTOSAR, A7_3_4, decl);
  std::string ref_msg = "Using-directives shall not be used.";
  issue->SetRefMsg(ref_msg);
}

void AUTOSARDeclRule::CheckUsingDirectiveInHeaderFile(const clang::UsingDirectiveDecl *decl) {
  auto loc = decl->getLocation();
  auto src_mgr = XcalCheckerManager::GetSourceManager();
  auto filename = src_mgr->getFilename(loc);
  if (filename.find(".h") != std::string::npos) {
    XcalIssue *issue = nullptr;
    XcalReport *report = XcalCheckerManager::GetReport();
    issue = report->ReportIssue(AUTOSAR, A7_3_6, decl);
    std::string ref_msg = "Using-directives and using-declarations shall not be used in header files.";
    issue->SetRefMsg(ref_msg);
  }
}

void AUTOSARDeclRule::CheckUsingDeclInHeaderFile(const clang::UsingDecl *decl) {
  clang::Decl *sink = nullptr;
  for (const auto &it : decl->shadows()) {
    auto target = it->getTargetDecl();
    if (clang::isa<clang::FunctionDecl>(target) ||
        clang::isa<clang::CXXRecordDecl>(target)) {
      continue;
    }
    sink = it;
    break;
  }

  if (sink != nullptr) {
    XcalIssue *issue = nullptr;
    XcalReport *report = XcalCheckerManager::GetReport();
    issue = report->ReportIssue(AUTOSAR, A7_3_6, decl);
    std::string ref_msg = "Using-directives and using-declarations shall not be used in header files.";
    issue->SetRefMsg(ref_msg);
  }
}

/*
 * AUTOSAR: A7-1-6
 * The typedef specifier shall not be used.
 */
void AUTOSARDeclRule::CheckTypedefDecl(const clang::TypedefDecl *decl) {
  XcalIssue *issue = nullptr;
  XcalReport *report = XcalCheckerManager::GetReport();
  issue = report->ReportIssue(AUTOSAR, A7_1_6, decl);
  std::string ref_msg = "The typedef specifier shall not be used.";
  issue->SetRefMsg(ref_msg);
}

/*
 * AUTOSAR: A8-2-1
 * When declaring function templates, the trailing return type syntax shall
 * be used if the return type depends on the type of parameters.
 */
void AUTOSARDeclRule::CheckTrailingReturnWhenDependTypeParameter(const clang::FunctionTemplateDecl *decl) {
  auto func_decl = decl->getAsFunction();
  auto ret_type = func_decl->getReturnType();
  if (ret_type->isDependentType()) {
    if (!func_decl->getType()->hasAutoForTrailingReturnType()) {
      XcalIssue *issue = nullptr;
      XcalReport *report = XcalCheckerManager::GetReport();
      issue = report->ReportIssue(AUTOSAR, A8_2_1, decl);
      std::string ref_msg = "When declaring function templates, the trailing return type syntax shall be used if the return "
                            "type depends on the type of parameters.";
      issue->SetRefMsg(ref_msg);
    }
  }
}

}
}