/*
 * Copyright (C) 2020-2021 Xcalibyte Limited, Inc.  All Rights Reserved.
 */

//
// ====================================================================
// GJB8114_pp_rule.cpp
// ====================================================================
//
// implement preprocess related rules in GJB8114
//

#include "GJB8114_enum.inc"
#include "GJB8114_pp_rule.h"

namespace xsca {
namespace rule {

/*
 * GJB5111: 5.1.1.1
 * Changing the definition of basic type or keywords by macro is forbidden
 */
void GJB8114PPRule::CheckRedefineKeywordsByMacro(const clang::MacroDirective *MD) {
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
  std::string token;
  while (*start != ' ' && start != end) {
    token += *start;
    start++;
  }

  if (conf_mgr->FindCAndCXXKeyword(token)) {
    XcalIssue *issue = nullptr;
    XcalReport *report = XcalCheckerManager::GetReport();
    issue = report->ReportIssue(GJB8114, G5_1_1_1, macro_loc);
    std::string ref_msg = "Redefining reserved words is forbidden: ";
    ref_msg += token + " -> " + macro_loc.printToString(*src_mgr);
    issue->SetRefMsg(ref_msg);
  }
}

/*
 * GJB8114: 5.1.1.22
 * Head file being re-included is forbidden
 */
void GJB8114PPRule::CheckReIncludeHeadFile(clang::SourceLocation Loc, llvm::StringRef IncludedFilename) {
  auto filename = IncludedFilename.str();
  if (_included_file.find(filename) != _included_file.end()) {
    XcalIssue *issue = nullptr;
    XcalReport *report = XcalCheckerManager::GetReport();
    issue = report->ReportIssue(GJB8114, G5_1_1_22, Loc);
    std::string ref_msg = "Head file being re-included is forbidden: " + filename;
    issue->SetRefMsg(ref_msg);
  } else {
    _included_file.insert(filename);
  }
}

/*
 * GJB8114: 6.9.2.4
 * Don't use a header file with .h as suffix
 */
void GJB8114PPRule::CheckHeadSuffix(clang::SourceLocation Loc, llvm::StringRef IncludedFilename) {
  auto filename = IncludedFilename.str();

  auto endWith = [](const char *str, const char *suffix) -> bool {
    if (!str || !suffix)
      return 0;
    size_t lenstr = strlen(str);
    size_t lensuffix = strlen(suffix);
    if (lensuffix > lenstr)
      return 0;
    return std::strncmp(str + lenstr - lensuffix, suffix, lensuffix) == 0;
  };

  if (endWith(filename.c_str(), ".h")) {
    XcalIssue *issue = nullptr;
    XcalReport *report = XcalCheckerManager::GetReport();
    issue = report->ReportIssue(GJB8114, G6_9_2_4, Loc);
    std::string ref_msg = "Don't use a header file with .h as suffix: " + filename;
    issue->SetRefMsg(ref_msg);
  }
}


}
}