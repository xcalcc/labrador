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

#include <vector>
#include <clang/Lex/Preprocessor.h>

class GJB5369PPRule : public PPNullHandler {
public:
  ~GJB5369PPRule() {}

private:
  /* GJB5369: 4.1.1.11
   * Using '#' and '##' in the same macro is forbidden
   */
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

    if (tokens.size() >= 2) {
      REPORT("GJB5396:4.1.1.11: Using '#' and '##' in the same macro is "
             "forbidden: %s\n",
             macro_loc.printToString(*src_mgr).c_str());
    }
  }

  /* GJB5369: 4.1.1.12
   * Macro which is unlike a function is forbidden
   */
  void CheckUnFunctionLike(const clang::MacroDirective *MD) {
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
        REPORT(
            "GJB5396:4.1.1.12: Macro which is unlike a function is forbidden: "
            "%s\n",
            macro_loc.printToString(*src_mgr).c_str());
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
        REPORT(
            "GJB5396:4.1.1.12: Macro which is unlike a function is forbidden: "
            "%s\n",
            macro_loc.printToString(*src_mgr).c_str());
      }
    }
  }

  /*
   * GJB5369: 4.1.1.13
   * keywords in macro is forbidden
   */
  void CheckMacroKeywords(const clang::MacroDirective *MD) {
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
          REPORT("GJB5396:4.1.1.13: keywords in macro is forbidden: %s -> "
                 "%s\n",
                 token.c_str(), macro_loc.printToString(*src_mgr).c_str());
        }
      }
    }
  }

  /*
   * GJB5369: 4.1.1.14
   * Redefining reserved words is forbidden
   */
  void CheckReservedWordRedefine(const clang::MacroDirective *MD) {
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
      REPORT("GJB5396:4.1.1.14: Redefining reserved words is forbidden: %s -> "
             "%s\n",
             token.c_str(), macro_loc.printToString(*src_mgr).c_str());
    }
  }

  /*
   * GJB5369: 4.1.1.18
   * with "#if" but no "#endif" in the same file is forbidden
   * TODO: Clang will catch this mistake. We need to collect clang's error report.
   */
  void CheckUnterminatedIf(clang::SourceLocation &Loc, clang::SourceLocation &IfLoc) {
    auto src_mgr = XcalCheckerManager::GetSourceManager();
    auto res = src_mgr->isWrittenInSameFile(Loc, IfLoc);
    if (!res) {
      REPORT("GJB5396:4.1.1.18: with \"#if\" but no \"#endif\" in the same "
             "file is forbidden:line: If: %s -> EndIf %s\n",
             IfLoc.printToString(*src_mgr).c_str(), Loc.printToString(*src_mgr).c_str());
    }
  }

  /*
   * GJB5369: 4.1.1.20
   * Using absolute path in the "#include <...>" is forbidden
   */
  void CheckAbsolutePathInclude(llvm::StringRef IncludedFilename) {
    auto filename = IncludedFilename.str();

    // unix
    if (filename[0] == '/') {
      REPORT("GJB5396:4.1.1.20: Using absolute path in the \"#include <...>\" "
             "is forbidden: %s\n",
             filename.c_str());
    }

    if ((filename.find(":\\") != std::string::npos) ||
        (filename.find(":\\") != std::string::npos) ||
        (filename.find(":/") != std::string::npos)) {
      REPORT("GJB5396:4.1.1.20: Using absolute path in the \"#include <...>\" "
             "is forbidden: %s\n",
             filename.c_str());
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
  void CheckProgram(clang::SourceLocation Loc,
                    clang::PragmaIntroducerKind &Introducer) {
    auto src_mgr = XcalCheckerManager::GetSourceManager();
    REPORT("GJB5369: 4.1.2.7: using \"#pragma\" carefully: %s\n",
           Loc.printToString(*src_mgr).c_str());
  }

  /*
   * GJB5369: 4.2.1.6
   * the macro parameters should be enclosed in parentheses
   */
  void CheckParamWithParentheses(const clang::MacroDirective *MD) {
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
            REPORT("GJB5369: 4.2.1.6: the macro parameters should be "
                   "enclosed in parentheses: %s\n",
                   Loc.printToString(*src_mgr).c_str());
          }
        }
      }
      if (end) break;
      is_previous_paren = false;
      is_next_paren = false;
    }
  }

public:
  void MacroDefined(const clang::Token &MacroNameTok,
                    const clang::MacroDirective *MD) {
    CheckMultipleSharp(MD);
    CheckUnFunctionLike(MD);
    CheckMacroKeywords(MD);
    CheckReservedWordRedefine(MD);
    CheckParamWithParentheses(MD);
  }

  void Endif(clang::SourceLocation Loc, clang::SourceLocation IfLoc) {
    CheckUnterminatedIf(Loc, IfLoc);
  }

  void InclusionDirective(clang::SourceLocation DirectiveLoc,
                          const clang::Token &IncludeToken, llvm::StringRef IncludedFilename,
                          bool IsAngled, clang::CharSourceRange FilenameRange,
                          const clang::FileEntry *IncludedFile, llvm::StringRef SearchPath,
                          llvm::StringRef RelativePath, const clang::Module *Imported,
                          clang::SrcMgr::CharacteristicKind FileType) {
    CheckAbsolutePathInclude(IncludedFilename);
  }

  void PragmaDirective(clang::SourceLocation Loc,
                       clang::PragmaIntroducerKind Introducer) {
    CheckProgram(Loc, Introducer);
  }

}; // GJB5369PPRule
