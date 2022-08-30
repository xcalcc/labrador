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
// MISRA_pp_rule.cpp
// ====================================================================
//
// implement all pp related rules in MISRA
//

#include "MISRA_enum.inc"
#include "MISRA_pp_rule.h"

namespace xsca {
namespace rule {

/* MISRA
 * Rule: 5.5
 * Identifiers shall be distinct from macro names
 */
void MISRAPPRule::CheckRecordMacroName(const clang::Token &MacroNameTok, const clang::MacroDirective *MD) {
  auto scope_mgr = XcalCheckerManager::GetScopeManager();
  auto src_mgr = XcalCheckerManager::GetSourceManager();
  auto macro_info = MD->getMacroInfo();
  auto macro_loc = macro_info->getDefinitionLoc();
  auto end_loc = macro_info->getDefinitionEndLoc();
  const char *start = src_mgr->getCharacterData(macro_loc);
  const char *end = src_mgr->getCharacterData(end_loc);

  std::string name;
  while (start != end) {
    if (*start == '(') break;
    name += *start;
    start++;
  }
  if (!name.empty()) {
    scope_mgr->AddMacro(name, MD);
  }
}

/* MISRA
  * Rule: 7.1
  * Octal constants shall not be used
  */
void MISRAPPRule::CheckIfValue(clang::SourceLocation Loc, clang::SourceRange ConditionalRange,
                               clang::PPCallbacks::ConditionValueKind ConditionalValue) {
  auto src_mgr = XcalCheckerManager::GetSourceManager();
  clang::LangOptions langOps;
  clang::SmallString<256> buffer;
  llvm::StringRef val = clang::Lexer::getSpelling(ConditionalRange.getEnd(), buffer, *src_mgr, langOps);
  if (val.size() > 1 && val[0] == '0') {
    if ('0' <= val[1] && val[1] <= '7') {
      XcalIssue *issue = nullptr;
      XcalReport *report = XcalCheckerManager::GetReport();
      issue = report->ReportIssue(MISRA, M_R_7_1, Loc);
      std::string ref_msg = "Octal constants shall not be used";
      issue->SetRefMsg(ref_msg);
    }
  }
}

/* MISRA
 * Rule: 17.1
 * The features of <stdarg.h> shall not be used
 * Rule: 21.5
 * The standard header file <signal.h> shall not be used
 */
void MISRAPPRule::CheckFidHeaderFile(clang::SourceLocation Loc, llvm::StringRef IncludedFilename) {
  auto conf_mgr = XcalCheckerManager::GetConfigureManager();
  if (conf_mgr->IsForbidHeaderFile(IncludedFilename.str())) {
    XcalIssue *issue = nullptr;
    XcalReport *report = XcalCheckerManager::GetReport();
    if (IncludedFilename.str() == "stdarg.h")
      issue = report->ReportIssue(MISRA, M_R_17_1, Loc);
    else if (IncludedFilename.str() == "signal.h")
      issue = report->ReportIssue(MISRA, M_R_21_5, Loc);
    else if (IncludedFilename.str() == "tgmath.h")
      issue = report->ReportIssue(MISRA, M_R_21_11, Loc);
    else if (IncludedFilename.str() == "setjmp.h")
      issue = report->ReportIssue(MISRA, M_R_21_4, Loc);
    else if (IncludedFilename.str() == "time.h")
      issue = report->ReportIssue(MISRA, M_R_21_10, Loc);

    std::string ref_msg = "The features of " + IncludedFilename.str() + " shall not be used";
    issue->SetRefMsg(ref_msg);
  }
}

/* MISRA
 * Rule: 21.6
 * The Standard Library input/output functions shall not be used
 */
void MISRAPPRule::CheckIOFunctionInStdio(const clang::Token &MacroNameTok, const clang::MacroDefinition &MD) {
  std::vector<std::string> fid_macro{
      "BUFSIZ",
      "EOF",
      "FILENAME_MAX",
      "FOPEN_MAX",
      "L_tmpnam",
      "SEEK_CUR",
      "SEEK_END",
      "SEEK_SET",
      "TMP_MAX",
      "_IOFBF",
      "_IOLBF",
      "_IONBF",
      "stderr",
      "stdin",
      "stdout"
  };
  auto name = MacroNameTok.getIdentifierInfo()->getName();
  if (std::find(fid_macro.begin(), fid_macro.end(), name) != fid_macro.end()) {
    XcalIssue *issue = nullptr;
    XcalReport *report = XcalCheckerManager::GetReport();
    issue = report->ReportIssue(MISRA, M_R_21_6, MacroNameTok.getLocation());
    std::string ref_msg = "The Standard Library input/output functions shall not be used";
    issue->SetRefMsg(ref_msg);
  }
}

}
}
