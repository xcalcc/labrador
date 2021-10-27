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
// GJB5369_pp_rule.cpp
// ====================================================================
//
// implement preprocess related rules in GJB5369
//

#include "GJB5369_pp_rule.h"
#include "GJB5369_enum.inc"

namespace xsca {
namespace rule {

/* GJB5369: 4.1.1.11
 * Using '#' and '##' in the same macro is forbidden
 * GJB5369: 4.15.2.2
 * using ## and # carefully in macro
 */
void GJB5369PPRule::CheckMultipleSharp(const clang::MacroDirective *MD) {
  auto src_mgr = XcalCheckerManager::GetSourceManager();
  auto macro_info = MD->getMacroInfo();
  auto macro_loc = macro_info->getDefinitionLoc();
  auto end_loc = macro_info->getDefinitionEndLoc();

  // return if encounter builtin marco
  if (src_mgr->isWrittenInBuiltinFile(macro_loc)) {
    return;
  }

  const char *start = src_mgr->getCharacterData(macro_loc);
  const char *end = src_mgr->getCharacterData(end_loc);

  std::vector<std::string> tokens;
  while (start != end) {
    if ((*start) != '#') {
      start++;
      continue;
    } else if (*(start + 1) == '#') {
      tokens.push_back(std::string("##"));
      start += 2;
    } else {
      tokens.push_back(std::string("#"));
      start++;
    }
  }

  XcalIssue *issue = nullptr;
  XcalReport *report = XcalCheckerManager::GetReport();

  if (tokens.size() >= 2) {
    issue = report->ReportIssue(GJB5369, G4_1_1_11, macro_loc);
    std::string ref_msg = "Using '#' and '##' in the same macro is forbidden: ";
    ref_msg += macro_loc.printToString(*src_mgr);
    issue->SetRefMsg(ref_msg);
  }

  if (!tokens.empty()) {
    issue = report->ReportIssue(GJB5369, G4_15_2_2, macro_loc);
    std::string ref_msg = "Using ## and # carefully in macro: ";
    ref_msg += macro_loc.printToString(*src_mgr);
    issue->SetRefMsg(ref_msg);
  }
}

/* GJB5369: 4.1.1.12
 * Macro which is unlike a function is forbidden
 */
void GJB5369PPRule::CheckUnFunctionLike(const clang::MacroDirective *MD) {
  auto macro_info = MD->getMacroInfo();
  auto src_mgr = XcalCheckerManager::GetSourceManager();
  auto macro_loc = macro_info->getDefinitionLoc();

  // return if encounter builtin marco
  if (src_mgr->isWrittenInBuiltinFile(macro_loc)) {
    return;
  }

  if (macro_info->isFunctionLike()) {

    // check if the marco is start with '{'
    auto begin = macro_info->tokens_begin();
    if (!begin->is(clang::tok::TokenKind::l_brace)) {
      XcalIssue *issue = nullptr;
      XcalReport *report = XcalCheckerManager::GetReport();
      issue = report->ReportIssue(GJB5369, G4_1_1_12, macro_loc);
      std::string ref_msg = "Macro which is unlike a function is forbidden:" + macro_loc.printToString(*src_mgr);
      issue->SetRefMsg(ref_msg);
      return;
    }

    // Check if the macro's braces are paired
    int match = 0;
    for (const auto &it : macro_info->tokens()) {
      if (it.is(clang::tok::TokenKind::l_brace)) {
        match++;
      } else if (it.is(clang::tok::TokenKind::r_brace)) {
        match--;
      } else {
        continue;
      }
    }

    if (match != 0) {
      XcalIssue *issue = nullptr;
      XcalReport *report = XcalCheckerManager::GetReport();
      issue = report->ReportIssue(GJB5369, G4_1_1_12, macro_loc);
      std::string ref_msg = "Macro which is unlike a function is forbidden: " + macro_loc.printToString(*src_mgr);
      issue->SetRefMsg(ref_msg);
    }
  }
}

/*
 * GJB5369: 4.1.1.13
 * keywords in macro is forbidden
 */
void GJB5369PPRule::CheckMacroKeywords(const clang::MacroDirective *MD) {
  auto macro_info = MD->getMacroInfo();
  auto src_mgr = XcalCheckerManager::GetSourceManager();
  auto macro_loc = macro_info->getDefinitionLoc();
  auto conf_mgr = XcalCheckerManager::GetConfigureManager();

  // return if encounter builtin marco
  if (src_mgr->isWrittenInBuiltinFile(macro_loc)) {
    return;
  }

  for (const auto &it : macro_info->tokens()) {
    auto token_name = it.getIdentifierInfo();
    if (token_name != nullptr) {
      const std::string token = token_name->getName().str();
      auto isKeyword = conf_mgr->FindCAndCXXKeyword(token);
      if (isKeyword) {
        XcalIssue *issue = nullptr;
        XcalReport *report = XcalCheckerManager::GetReport();
        issue = report->ReportIssue(GJB5369, G4_1_1_13, macro_loc);
        std::string ref_msg = "Keywords in macro is forbidden: " + macro_loc.printToString(*src_mgr);
        issue->SetRefMsg(ref_msg);
      }
    }
  }
}

/*
 * GJB5369: 4.1.1.14
 * Redefining reserved words is forbidden
 */
void GJB5369PPRule::CheckReservedWordRedefine(const clang::MacroDirective *MD) {
  auto macro_info = MD->getMacroInfo();
  auto src_mgr = XcalCheckerManager::GetSourceManager();
  auto macro_loc = macro_info->getDefinitionLoc();
  auto end_loc = macro_info->getDefinitionEndLoc();
  auto conf_mgr = XcalCheckerManager::GetConfigureManager();

  // return if encounter builtin marco
  if (src_mgr->isWrittenInBuiltinFile(macro_loc)) {
    return;
  }

  if (macro_info->isFunctionLike()) {
    return;
  }

  auto start = src_mgr->getCharacterData(macro_loc);
  auto end = src_mgr->getCharacterData(end_loc);
  std::string token = "";
  while (*start != ' ' && start != end) {
    token += *start;
    start++;
  }

  if (conf_mgr->FindCAndCXXKeyword(token)) {
    XcalIssue *issue = nullptr;
    XcalReport *report = XcalCheckerManager::GetReport();
    issue = report->ReportIssue(GJB5369, G4_1_1_14, macro_loc);
    std::string ref_msg = "Redefining reserved words is forbidden: " + macro_loc.printToString(*src_mgr);
    issue->SetRefMsg(ref_msg);
  }
}

/*
 * GJB5369: 4.1.1.18
 * with "#if" but no "#endif" in the same file is forbidden
 * TODO: Clang will catch this mistake. We need to collect clang's error report.
 */
void GJB5369PPRule::CheckUnterminatedIf(clang::SourceLocation &Loc, clang::SourceLocation &IfLoc) {
  auto src_mgr = XcalCheckerManager::GetSourceManager();
  auto res = src_mgr->isWrittenInSameFile(Loc, IfLoc);
  if (!res) {
    XcalIssue *issue = nullptr;
    XcalReport *report = XcalCheckerManager::GetReport();
    issue = report->ReportIssue(GJB5369, G4_1_1_18, Loc);
    std::string ref_msg = "With \"#if\" but no \"#endif\" in the same file is forbidden:line: If: ";
    ref_msg += IfLoc.printToString(*src_mgr) + " -> Endif ";
    ref_msg += Loc.printToString(*src_mgr);
    issue->SetRefMsg(ref_msg);
  }
}

/*
 * GJB5369: 4.1.1.20
 * Using absolute path in the "#include <...>" is forbidden
 */
void GJB5369PPRule::CheckAbsolutePathInclude(clang::SourceLocation Loc, llvm::StringRef IncludedFilename) {
  auto filename = IncludedFilename.str();
  XcalIssue *issue = nullptr;
  XcalReport *report = XcalCheckerManager::GetReport();

  // unix
  if (filename[0] == '/') {
    issue = report->ReportIssue(GJB5369, G4_1_1_20, Loc);
    std::string ref_msg = "Using absolute path in the \"#include <...>\" is forbidden: " + filename;
    issue->SetRefMsg(ref_msg);
  }

  if ((filename.find(":\\") != std::string::npos) ||
      (filename.find(":\\") != std::string::npos) ||
      (filename.find(":/") != std::string::npos)) {
    issue = report->ReportIssue(GJB5369, G4_1_1_20, Loc);
    std::string ref_msg = "Using absolute path in the \"#include <...>\" is forbidden: " + filename;
    issue->SetRefMsg(ref_msg);
  }
}

#if 0
/*
   * GJB5369: 4.1.2.5
   * using "#define" in functions is forbidden
   * TODO: function decls have not been collected yet
   * TODO: the same as 4.1.2.6
   */
  void CheckDefineInFunction(const clang::MacroDirective *MD) {
    auto def_loc = MD->getLocation();
    auto src_mgr = XcalCheckerManager::GetSourceManager();
    auto scope_mgr = XcalCheckerManager::GetScopeManager();
    auto top_scope = scope_mgr->GlobalScope();

    bool in_func = top_scope->InFunctionRange(def_loc);
    if (in_func) {
      REPORT("GJB5396:4.1.2.5: Using \"#define\" in functions is forbidden: %s\n",
             def_loc.printToString(*src_mgr).c_str());
    }
  }
#endif

/*
 * GJB5369: 4.1.2.7
 * using "#pragma" carefully
 */
void GJB5369PPRule::CheckProgram(clang::SourceLocation Loc,
                                 clang::PragmaIntroducerKind &Introducer) {
  auto src_mgr = XcalCheckerManager::GetSourceManager();
  XcalIssue *issue = nullptr;
  XcalReport *report = XcalCheckerManager::GetReport();
  issue = report->ReportIssue(GJB5369, G4_1_2_7, Loc);
  std::string ref_msg = "Using \"#pragma\" carefully: %s\n" + Loc.printToString(*src_mgr);
  issue->SetRefMsg(ref_msg);
}

/*
 * GJB5369: 4.2.1.6
 * the macro parameters should be enclosed in parentheses
 */
void GJB5369PPRule::CheckParamWithParentheses(const clang::MacroDirective *MD) {
  auto macro_info = MD->getMacroInfo();
  auto src_mgr = XcalCheckerManager::GetSourceManager();
  auto macro_loc = macro_info->getDefinitionLoc();
  auto end_loc = macro_info->getDefinitionEndLoc();

  // return if encounter builtin marco
  if (src_mgr->isWrittenInBuiltinFile(macro_loc)) {
    return;
  }

  std::vector<std::string> params;
  std::string param_name;
  for (const auto &it : macro_info->params()) {
    param_name = it->getName().str();
    params.push_back(param_name);
  }

  bool is_previous_paren = false, is_next_paren = false, end = false;
  for (auto it = macro_info->tokens_begin(); it != macro_info->tokens_end(); it++) {
    if (it->is(clang::tok::l_paren)) {
      is_previous_paren = true;
      continue;
    }

    if (it->is(clang::tok::identifier)) {
      auto ident = it->getIdentifierInfo()->getName();
      if (std::find(params.begin(), params.end(), ident) != params.end()) {
        auto Loc = it->getLocation();
        if (++it == macro_info->tokens_end()) {
          is_next_paren = false;
          end = true;
        }
        if (!end && it->is(clang::tok::r_paren)) {
          is_next_paren = true;
        }
        if (!is_previous_paren || !is_next_paren) {
          XcalIssue *issue = nullptr;
          XcalReport *report = XcalCheckerManager::GetReport();
          issue = report->ReportIssue(GJB5369, G4_2_1_6, Loc);
          std::string ref_msg = "The macro parameters should be enclosed in parentheses: ";
          ref_msg += Loc.printToString(*src_mgr);
          issue->SetRefMsg(ref_msg);
        }
      }
    }
    if (end) break;
    is_previous_paren = false;
    is_next_paren = false;
  }
}

/*
 * GJB5369: 4.2.1.8
 * header file name contain ' \ /* is forbidden
 */
void GJB5369PPRule::CheckIncludeName(clang::SourceLocation Loc, llvm::StringRef IncludedFilename) {
  std::string filename = IncludedFilename.str();
  XcalIssue *issue = nullptr;
  XcalReport *report = XcalCheckerManager::GetReport();
  if (filename.find("'") != std::string::npos ||
      filename.find("\\") != std::string::npos ||
      filename.find("/") != std::string::npos ||
      filename.find("*") != std::string::npos) {
    issue = report->ReportIssue(GJB5369, G4_2_1_8, Loc);
    std::string ref_msg = "Header file name contain ' \\ /* is forbidden is forbidden: " + filename;
    issue->SetRefMsg(ref_msg);
  }
}

} // rule
} // xsca
