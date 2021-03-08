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
      printf("GJB5396:4.1.1.11: Using '#' and '##' in the same macro is "
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
        printf(
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
        printf(
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
          printf("GJB5396:4.1.1.13: keywords in macro is forbidden: %s -> "
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
      printf("GJB5396:4.1.1.14: Redefining reserved words is forbidden: %s -> "
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
      printf("GJB5396:4.1.1.18: with \"#if\" but no \"#endif\" in the same "
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
      printf("Using absolute path in the \"#include <...>\" is forbidden %s\n",
             filename.c_str());
    }

    if ((filename.find(":\\") != std::string::npos) ||
        (filename.find(":\\") != std::string::npos) ||
        (filename.find(":/") != std::string::npos)) {
      printf("Using absolute path in the \"#include <...>\" is forbidden %s\n",
             filename.c_str());
    }

  }

public:
  void MacroDefined(const clang::Token &MacroNameTok,
                    const clang::MacroDirective *MD) {
    CheckMultipleSharp(MD);
    CheckUnFunctionLike(MD);
    CheckMacroKeywords(MD);
    CheckReservedWordRedefine(MD);
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

}; // GJB5369PPRule
