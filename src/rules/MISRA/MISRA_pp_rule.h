/*
 * Copyright (C) 2020-2021 Xcalibyte Limited, Inc.  All Rights Reserved.
 */

//
// ====================================================================
// MISRA_pp_rule.h
// ====================================================================
//
// implement preprocess related rules in MISRA-C-2012
//

#include <set>
#include "pp_null_handler.h"
#include "xsca_checker_manager.h"
#include <clang/Lex/Preprocessor.h>


namespace xsca {
namespace rule {
class MISRAPPRule : public PPNullHandler {
public:
  MISRAPPRule() {
    _disabled = XcalCheckerManager::GetDisableOption().getValue().find("MISRA") != std::string::npos;
  }

private:

  /* MISRA
   * Rule: 17.1
   * The features of <stdarg.h> shall not be used
   * Rule: 21.5
   * The standard header file <signal.h> shall not be used
   */
  void CheckFidHeaderFile(clang::SourceLocation Loc, llvm::StringRef IncludedFilename);

  /* MISRA
   * Rule: 21.6
   * The Standard Library input/output functions shall not be used
   */
  void CheckIOFunctionInStdio(const clang::Token &MacroNameTok, const clang::MacroDefinition &MD);

public:
  void MacroDefined(const clang::Token &MacroNameTok,
                    const clang::MacroDirective *MD) {
  }

  void MacroExpands(const clang::Token &MacroNameTok,
                    const clang::MacroDefinition &MD,
                    clang::SourceRange Range,
                    const clang::MacroArgs *Args
  ) {
    CheckIOFunctionInStdio(MacroNameTok, MD);
  }

  void InclusionDirective(clang::SourceLocation DirectiveLoc,
                          const clang::Token &IncludeToken, llvm::StringRef IncludedFilename,
                          bool IsAngled, clang::CharSourceRange FilenameRange,
                          const clang::FileEntry *IncludedFile, llvm::StringRef SearchPath,
                          llvm::StringRef RelativePath, const clang::Module *Imported,
                          clang::SrcMgr::CharacteristicKind FileType) {
    CheckFidHeaderFile(DirectiveLoc, IncludedFilename);
  }

}; // MISRAPPRule

}
}