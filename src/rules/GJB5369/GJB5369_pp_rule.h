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

#include "pp_null_handler.h"
#include "xsca_checker_manager.h"

namespace xsca {
namespace rule {

class GJB5369PPRule : public PPNullHandler {
public:
  ~GJB5369PPRule() {}

private:
  /* GJB5369: 4.1.1.11
   * Using '#' and '##' in the same macro is forbidden
   * GJB5369: 4.15.2.2
   * using ## and # carefully in macro
   */
  void CheckMultipleSharp(const clang::MacroDirective *MD);

  /* GJB5369: 4.1.1.12
   * Macro which is unlike a function is forbidden
   */
  void CheckUnFunctionLike(const clang::MacroDirective *MD);

  /*
   * GJB5369: 4.1.1.13
   * keywords in macro is forbidden
   */
  void CheckMacroKeywords(const clang::MacroDirective *MD);

  /*
   * GJB5369: 4.1.1.14
   * Redefining reserved words is forbidden
   */
  void CheckReservedWordRedefine(const clang::MacroDirective *MD);

  /*
   * GJB5369: 4.1.1.18
   * with "#if" but no "#endif" in the same file is forbidden
   * TODO: Clang will catch this mistake. We need to collect clang's error report.
   */
  void CheckUnterminatedIf(clang::SourceLocation &Loc, clang::SourceLocation &IfLoc);

  /*
   * GJB5369: 4.1.1.20
   * Using absolute path in the "#include <...>" is forbidden
   */
  void CheckAbsolutePathInclude(clang::SourceLocation Loc, llvm::StringRef IncludedFilename);

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
                    clang::PragmaIntroducerKind &Introducer);

  /*
   * GJB5369: 4.2.1.6
   * the macro parameters should be enclosed in parentheses
   */
  void CheckParamWithParentheses(const clang::MacroDirective *MD);

  /*
   * GJB5369: 4.2.1.8
   * header file name contain ' \ /* is forbidden
   */
  void CheckIncludeName(clang::SourceLocation, llvm::StringRef IncludedFilename);
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
    CheckAbsolutePathInclude(DirectiveLoc, IncludedFilename);
    CheckIncludeName(DirectiveLoc, IncludedFilename);
  }

  void PragmaDirective(clang::SourceLocation Loc,
                       clang::PragmaIntroducerKind Introducer) {
    CheckProgram(Loc, Introducer);
  }

  void FileChanged(clang::SourceLocation Loc,
                   clang::PPCallbacks::FileChangeReason Reason,
                   clang::SrcMgr::CharacteristicKind FileType,
                   clang::FileID PrevFID) {

  }

  void MacroExpands(const clang::Token &MacroNameTok,
                    const clang::MacroDefinition &MD,
                    clang::SourceRange Range,
                    const clang::MacroArgs *Args) {
  }

}; // GJB5369PPRule

} // rule
} // xsca