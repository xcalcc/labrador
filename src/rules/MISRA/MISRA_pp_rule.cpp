/*
 * Copyright (C) 2020-2021 Xcalibyte Limited, Inc.  All Rights Reserved.
 */

//
// ====================================================================
// MISRA_pp_rule.cpp
// ====================================================================
//
// implement all pp related rules in MISRA
//

#include "MISRA_pp_rule.h"

namespace xsca {
namespace rule {

#if 0
/* MISRA
 * Rule: 2.5
 * A project should not contain unused type declarations
 */
void MISRAPPRule::RecordMacro(const clang::Token &MacroNameTok, const clang::MacroDirective *MD) {
  auto src_mgr = XcalCheckerManager::GetSourceManager();
  auto macro_info = MD->getMacroInfo();
  auto macro_loc = macro_info->getDefinitionLoc();


  // return if encounter builtin marco
  if (src_mgr->isWrittenInBuiltinFile(macro_loc)) {
    return;
  }

  unsigned long hash_value = _hash_gen(MD);
  _all_macros.insert(hash_value);
  printf("%s: %d\n", MacroNameTok.getIdentifierInfo()->getNameStart(), macro_info->isUsed());
}

void MISRAPPRule::RecordUsedMacro(const clang::Token &MacroNameTok, const clang::MacroDirective *MD) {
  TRACE0();
//  for (const auto &it : _all_macros) printf("%lud\n", it);
//  _used_macros.push_back(_hash_gen(MD));
}
#endif

}
}
