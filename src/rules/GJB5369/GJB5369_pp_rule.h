/*
 * Copyright (C) 2020-2021 Xcalibyte Limited, Inc.  All Rights Reserved.
 */

//
// ====================================================================
// GJB5369_pp_rule.h
// ====================================================================
//
// implement preprocess related rules in GJB5369
//

#include <clang/Lex/Preprocessor.h>
#include <vector>

class GJB5369PPRule : public PPNullHandler {
public:
  ~GJB5369PPRule() {}

private:
  void CheckMultipleSharp(const clang::MacroDirective *MD) {
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
    while(start != end) {
      if ((*start) != '#') {
        start++;
        continue;
      } else if (*(start+1) == '#') {
        tokens.push_back(std::string("##"));
        start += 2;
      } else {
        tokens.push_back(std::string("#"));
        start++;
      }
    }

    if (tokens.size() >= 2) {
      printf("GJB5396:4.1.1.11: Using '#' and '##' in the same macro is "
             "forbidden: %s\n",
             macro_loc.printToString(*src_mgr).c_str());
    }
  }

public:
  void MacroDefined(const clang::Token &MacroNameTok,
                    const clang::MacroDirective *MD) {
    CheckMultipleSharp(MD);
  }

}; // GJB5369PPRule

