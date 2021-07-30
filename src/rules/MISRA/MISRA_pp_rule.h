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
private:

  /* MISRA
   * Rule: 17.1
   * The features of <stdarg.h> shall not be used
   */
  void CheckStdArgHeaderFile(clang::SourceLocation Loc, llvm::StringRef IncludedFilename);

public:
  void MacroDefined(const clang::Token &MacroNameTok,
                    const clang::MacroDirective *MD) {
  }

  void MacroExpands(const clang::Token &MacroNameTok,
                    const clang::MacroDefinition &MD,
                    clang::SourceRange Range,
                    const clang::MacroArgs *Args
  ) {
  }

  void InclusionDirective(clang::SourceLocation DirectiveLoc,
                          const clang::Token &IncludeToken, llvm::StringRef IncludedFilename,
                          bool IsAngled, clang::CharSourceRange FilenameRange,
                          const clang::FileEntry *IncludedFile, llvm::StringRef SearchPath,
                          llvm::StringRef RelativePath, const clang::Module *Imported,
                          clang::SrcMgr::CharacteristicKind FileType) {
    CheckStdArgHeaderFile(DirectiveLoc, IncludedFilename);
  }

}; // MISRAPPRule

}
}