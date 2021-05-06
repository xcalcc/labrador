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
    REPORT("GJB9114:5.1.1.1: Redefining reserved words is forbidden: %s -> "
           "%s\n",
           token.c_str(), macro_loc.printToString(*src_mgr).c_str());
  }
}

/*
 * GJB5111: 5.1.1.2
 * Define other something as keywords is forbidden
 */
void GJB8114PPRule::CheckDefineAsKeywords(const clang::MacroDirective *MD) {
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
        REPORT("GJB8114:5.1.1.2: Define other something as keywords is forbidden: %s -> "
               "%s\n",
               token.c_str(), macro_loc.printToString(*src_mgr).c_str());
      }
    }
  }
}

}
}